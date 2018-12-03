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

static HashMap<FileType, StringArray> fileTypes;



int initBaseFileTypes(){
    fileTypes.set(Js,StringArray {"js"});
    fileTypes.set(Audio, StringArray {"wav","aif","aiff","mp3"});
    fileTypes.set(PdPatch, StringArray {"pd"});
    fileTypes.set(Preset,StringArray {"json"});


    return 0;

}
static const int watchInterval = 500;

class FileWatcher : private Timer{
public:
    FileWatcher(FileParameter * _fp):fp(_fp){
        startTimer(watchInterval);
        lastFileModTime = fp->getFile().getLastModificationTime();
    }

    ~FileWatcher(){
        stopTimer();
    }
    void timerCallback() override{
        if (isEngineLoadingFile())return;
        if(!fp.get())stopTimer();
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


FileParameter::FileParameter (const String& niceName, const String& description, const String& initialValue,const FileType type,LoaderFunctionType _loaderFunction):StringParameter(niceName,description,initialValue)
,fileType(type)
,isWatchable(false)
,isReloadable(false)
,isWatching(false)
,loaderFunction(_loaderFunction)
,loadingState(EMPTY)
{
    

}

FileParameter::~FileParameter(){
    masterReference.clear();
}
const File  FileParameter::getFile() const{
    return getEngine()->getFileAtNormalizedPath(stringValue());
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



void FileParameter::startLoading(){
    jassert(loadingState!=LOADING);
    if(!loaderFunction){
        jassertfalse;
        return;
    }
    
    errorMsg = "";

    if(stringValue().isEmpty()){
        loadingState = EMPTY;
        return;
    }
    loadingState = LOADING;
    fileListeners.call(&Listener::loadingStarted,this);
    Result r=Result::fail("file not processed");
    if(!hasValidPath(true)){
        r = Result::fail("path not valid");
    }
    else{
        const File f = getFile();
        r = loaderFunction(f);
    }


    if(r){
        loadingState = LOADED;
    }
    else{
        loadingState = LOADED_WITH_ERROR;
        errorMsg = r.getErrorMessage();
    }
    fileListeners.call(&Listener::loadingEnded,this);
}


void  FileParameter::setValueInternal (const var& newVal)
{
    String path = newVal.toString();
    if(File::isAbsolutePath(path)){
        value = path;
    }
    else{
        value = getEngine()->getNormalizedFilePath(path);
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
    fileWatcher = isWatching? new FileWatcher(this):nullptr;


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
        for(auto & s: fileTypes[fileType]){
            allowed.add(String(includeWildCards?"*":"")+"."+s);
        }

        return allowed.joinIntoString(";");
    }



}

bool FileParameter::fileHasValidExtension(const File & f){

    if(fileType==Any){return true;}
    else if(!f.exists()){return false;}
    else{
        return f.hasFileExtension(getAllowedExtensionsFilter(false));
    }

}




// static initialization of filetypes hopefully safe enough
int dummy = initBaseFileTypes();
