/*
  ==============================================================================

    FileParameter.h
    Created: 2 Dec 2018 3:21:17pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once
#include "StringParameter.h"

typedef enum{
    Any,
    Js,
    Audio,
    PdPatch,
    PresetFile

} FileType;

typedef enum {
    EMPTY,
    LOADING,
    LOADED,
    LOADED_WITH_ERROR
}LoadingState;

class FileWatcher;
class FileLoaderJob;


struct FilePostponedLoader;


class FileParameter : public StringParameter{
public:
    typedef std::function<Result(const File & f)> LoaderFunctionType;
    static LoaderFunctionType dummyLoader;
    FileParameter (const String& niceName, const String& description = "", const String& initialValue = "",const FileType t=Any,LoaderFunctionType loaderFuntion=dummyLoader,bool isAsync=false);
    ~FileParameter();
    static void addFileType(String Name,StringArray allowedExtensions);
    static void initBaseFileTypes();
    bool fileHasValidExtension(const File & f);
    bool hasValidPath(bool ignoreEmpty);
    FileType fileType;
    bool isWatchable;
    bool isReloadable;

    void setLoader(LoaderFunctionType & f);
    
    const File getFile() const;
    void setFile(const File & f);
    void triggerFileChange(ParameterBase::Listener * from=nullptr);
    LoadingState getLoadingState();
    String errorMsg;

    void setIsWatching(bool t);
    bool isWatching;


    class Listener{
    public:
        virtual ~Listener(){};
        virtual void watchStateChanged(FileParameter *){};
        virtual void loadingStarted(FileParameter * ){};
        virtual void loadingEnded(FileParameter * ){};

    };

    void addFileListener(Listener * l){fileListeners.add(l);}
    void removeFileListener(Listener * l){fileListeners.remove(l);}


    String  getAllowedExtensionsFilter(bool includeWildCards);
    String getFullTypeName();

    ListenerList<Listener> fileListeners;



    DECLARE_OBJ_TYPE (FileParameter,"File parameter");

protected:
    std::function<Result(const File &)>  loaderFunction;
    void setValueInternal (const var& newVal) override;
    void startLoading(LoaderFunctionType & loader);
    void startLoading();
    void endLoading(const Result & result);
private:
    bool isAsync;
    friend class FileLoaderJob;
    WeakReference<FileLoaderJob> fileLoaderJob;
    LoadingState loadingState;
    ScopedPointer<FileWatcher> fileWatcher;
    WeakReference<FileParameter>::Master masterReference;
    friend class WeakReference<FileParameter>;

    ScopedPointer<FilePostponedLoader> filePostponedLoader;
    friend class FilePostponedLoader;
};
