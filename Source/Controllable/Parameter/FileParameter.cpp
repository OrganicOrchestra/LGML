/*
  ==============================================================================

    FileParameter.cpp
    Created: 2 Dec 2018 3:21:17pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "FileParameter.h"

#include "ParameterFactory.h"
REGISTER_PARAM_TYPE (FileParameter)
#include "../../Engine.h" // for path resolving
                          //String getNormalizedFilePath (const File& f);
                          //File getFileAtNormalizedPath (const String& path);



FileParameter::LoaderFunctionType FileParameter::dummyLoader =[](const File &){return Result::fail("no loader");};

struct FileTypeInfo{
    StringArray extensions;
    String fullName;

};


class FileTypeMap:public HashMap<FileType, FileTypeInfo>,public DeletedAtShutdown{
public:
    FileTypeMap(){
        set(Js,{StringArray {"js"},"Javascript"});
        set(Audio, {StringArray {"wav","aif","aiff","mp3"},"Audio"});
        set(PdPatch, {StringArray {"pd"},"Pure-data"});
        set(PresetFile,{StringArray {"json"},"Preset"});
        set(Text,{StringArray {"txt"},"Text"});
    }
};

static FileTypeMap* fileTypes = new FileTypeMap(); // will be deleted at shutdown

// class deffering file loading once the whole session has been loaded
struct FilePostponedLoader : private Engine::EngineListener
{
    FilePostponedLoader(FileParameter * fp):owner(fp){}
    ~FilePostponedLoader(){}
    void endLoadFile() override {if(owner.get()){owner->startLoading();}}
    WeakReference<FileParameter> owner;
};

static const int watchInterval = 500;

class FileWatcher : private Timer{
public:
    FileWatcher(FileParameter * _fp):fp(_fp){
        startTimer(watchInterval);
        lastFileModTime = fp->getFile().getLastModificationTime();
    }

    ~FileWatcher(){stopTimer();}
    void timerCallback() override{
        if (isEngineLoadingFile())return;
        if(!fp.get())stopTimer();
        if(fp->stringValue().isEmpty()) return;
        Time newTime = fp->getFile().getLastModificationTime();

            if (newTime != lastFileModTime && !(fp->getLoadingState()==LOADING))//|| !_isInSyncWithLGML )
        {
            fp->triggerFileChange();
            lastFileModTime = newTime;
        }
    }

    
    
    Time lastFileModTime;
    WeakReference<FileParameter> fp;

};


FileParameter::FileParameter (const String& niceName, const String& description, const String& initialValue,const FileType type,LoaderFunctionType _loaderFunction,bool _isAsync):
StringParameter(niceName,description,initialValue)
,fileType(type)
,loaderFunction(_loaderFunction)
,isAsync( _isAsync) // TODO support Async generate random crash
{
    

}

FileParameter::~FileParameter(){
    jassert(fileLoaderJob.get()==nullptr);
    masterReference.clear();
}
const File  FileParameter::getFile() const{
    return getEngine()?getEngine()->getFileAtNormalizedPath(stringValue()):File();
}

void FileParameter::setFile(const File & f){
    setValue(f.getFullPathName());
}
void FileParameter::setLoader(std::function<Result(const File &)> & f){
    loaderFunction = f;
}

void FileParameter::startLoading(std::function<Result(const File & f)> & loader){
    setLoader(loader);
    startLoading();
}

LoadingState FileParameter::getLoadingState(){
    return loadingState;
}


class FileLoaderJob : public ThreadPoolJob,public AsyncUpdater{
public:
    FileLoaderJob(const WeakReference<FileParameter> _fp):ThreadPoolJob("FileLoader"),fp(_fp){

    }

    ~FileLoaderJob(){
        masterReference.clear();
    }
    ThreadPoolJob::JobStatus runJob() override{
        if(!shouldExit()){
        loadFileImpl();
        }
        return ThreadPoolJob::JobStatus::jobHasFinished;
    }
    void loadFileImpl(){
        Result r=Result::fail("file not processed");
        if(fp.get()){

            if(!fp.get()->hasValidPath(true)){
                r = Result::fail("path not valid");
            }
            else{
                const File f = fp->getFile();
                r = fp->loaderFunction(f);
            }


            if(r){
                fp.get()->loadingState = LOADED;
            }
            else{
                fp.get()->loadingState = LOADED_WITH_ERROR;
                fp.get()->errorMsg = r.getErrorMessage();
            }

            {
            MessageManagerLock ml;
            handleAsyncUpdate();
            }

        }
        
    }
    void handleAsyncUpdate()override{
        if(auto pfp = fp.get()){
            pfp->fileListeners.call(&FileParameter::Listener::loadingEnded,fp.get());
        if(!pfp->isAsync){ // clear job from here if not called in engine pool (auto deleted)
            pfp->fileLoaderJob = nullptr;
            delete this;
        }
        }
    }
    
    WeakReference<FileParameter> fp;

private:
    WeakReference<FileLoaderJob>::Master masterReference;
    friend class WeakReference<FileLoaderJob>;
    
};

void FileParameter::startLoading(){
    jassert(loadingState!=LOADING);
    if(!loaderFunction){
        jassertfalse;
        return;
    }
    
    errorMsg = "";

    if(stringValue().isEmpty()){
        loadingState = EMPTY;
        if(parentContainer){ // hack to avoid loading on param construction
            const File f {};
            if(loaderFunction){loaderFunction(f);}
            else{jassertfalse;}
        }
        return;
    }
    if(isEngineLoadingFile()){
        filePostponedLoader = std::make_unique<FilePostponedLoader>(this);
        return;
    }
    loadingState = LOADING;
    fileListeners.call(&Listener::loadingStarted,this);

    if(!isAsync && fileLoaderJob){
        fileLoaderJob->signalJobShouldExit();
    }
    fileLoaderJob=new FileLoaderJob(this);
    if(isAsync){
        getEngineThreadPool()->addJob(fileLoaderJob,true); // job will auto delete
    }
    else{
        fileLoaderJob->runJob();
//        fileLoaderJob = nullptr;
    }

}


void  FileParameter::setValueInternal (const var& newVal)
{
    String path = newVal.toString();
    if(File::isAbsolutePath(path)){
        value = getEngine()->getNormalizedFilePath(path);
    }
    else{
        value = path;
    }

    if(loaderFunction){
        startLoading();
    }
};


void FileParameter::triggerFileChange(ParameterBase::Listener * from){
    setValueFrom(from,value,false,true);
}


void FileParameter::setIsWatching(bool s){
    isWatching = s;
    fileWatcher = isWatching? std::make_unique<FileWatcher>(this):nullptr;


}
bool FileParameter::hasValidPath(bool ignoreEmpty){
    if(ignoreEmpty && stringValue().isEmpty())return true;
    auto f = getFile();
    return f.exists() && fileHasValidExtension(f);
}


String  FileParameter::getAllowedExtensionsFilter(bool includeWildCards){
    if(fileType==Any){
        return "*";
    }
    else{
        StringArray allowed;
        for(auto & s: fileTypes->getReference(fileType).extensions){
            allowed.add(String(includeWildCards?"*":"")+"."+s);
        }

        return allowed.joinIntoString(";");
    }



}
String FileParameter::getFullTypeName(){
    if(fileType==Any){
        return "";
    }
    else{
        return fileTypes->getReference(fileType).fullName;
    }
}

bool FileParameter::fileHasValidExtension(const File & f){

    if(fileType==Any){return true;}
    else if(!f.exists()){return false;}
    else{
        return f.hasFileExtension(getAllowedExtensionsFilter(false));
    }

}

String FileParameter::getWithFirstValidExtension(const String & name){
    auto fkFile = File::getCurrentWorkingDirectory().getChildFile (name);
    if(name.isNotEmpty() && !fileHasValidExtension(fkFile)){
        auto exts = fileTypes->getReference(fileType).extensions;
        auto ext = exts.size()>0?exts[0]:"";
        return fkFile.getFileNameWithoutExtension()+"."+ext;
    }
    return name;
}




