/*
 ==============================================================================

 Engine.h
 Created: 2 Apr 2016 11:03:21am
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

//#define MULTITHREADED_LOADING

#pragma warning (disable : 4100)

#include "MIDIManager.h"//keep
#include "ControlManager.h"//keep
#include "NodeManager.h"//keep
#include "PresetManager.h"//keep
#include "AudioFucker.h"//keep
#include "TimeManager.h"//keep
#include "RuleManager.h"//keep
#include "FastMapper.h"//keep
#include "VSTManager.h"//keep
#include "ProgressNotifier.h"
class AudioFucker;


class Engine:public FileBasedDocument,NodeManager::NodeManagerListener,AsyncUpdater,public ProgressNotifier,
	public ControllableContainer
{
public:
    Engine();
    ~Engine();

    // Audio
    AudioProcessorPlayer graphPlayer;

    void createNewGraph();
    void clear();
    void initAudio();
    void closeAudio();


    void suspendAudio(bool shouldSuspend);

    void parseCommandline(const String & );

    //==============================================================================
    // see EngineFileDocument.cpp

    //  inherited from FileBasedDocument
    String getDocumentTitle()override ;
    Result loadDocument (const File& file)override;
    Result saveDocument (const File& file)override;
    File getLastDocumentOpened() override;
    void setLastDocumentOpened (const File& file) override;

    //    #if JUCE_MODAL_LOOPS_PERMITTED
    //     File getSuggestedSaveAsFile (const File& defaultFile)override;
    //    #endif

    // our Saving methods
    var getJSONData() override;
    void loadJSONData(var data,ProgressTask * loadingTask);

    bool checkFileVersion(DynamicObject * metaData);
    int versionStringToInt(const String &version);
    String getMinimumRequiredFileVersion();

    void  stimulateAudio(bool);
    ScopedPointer<AudioFucker> stimulator;

    class MultipleAudioSettingsHandler : public ChangeListener,public Timer{
    public:
        MultipleAudioSettingsHandler():oldSettingsId("oldAudioSettings"){}
        Identifier oldSettingsId;
        void changeListenerCallback(ChangeBroadcaster * )override;
        void saveCurrent();
        String getConfigName();
        void load();
        String lastConfigName;
        void timerCallback()override;

    };
    MultipleAudioSettingsHandler audioSettingsHandler;

  int64 loadingStartTime;
  
  void managerEndedLoading() override;
  void managerProgressedLoading(float progress) override;


  void fileLoaderEnded();
  bool allLoadingThreadsAreEnded();
  void loadDocumentAsync(const File & file);

  class FileLoader : public Thread,public Timer{
  public:
    FileLoader(Engine * e,File f):Thread("EngineLoader"),owner(e),fileToLoad(f){
      //startTimerHz(4);
      //fakeProgress = 0;
      isEnded = false;
    }
    ~FileLoader(){
      
    }

    void timerCallback()override{
      //fakeProgress+=getTimerInterval()/5000.0;
      //fakeProgress = jmin(1.0f,fakeProgress);
      //owner->engineListeners.call(&EngineListener::fileProgress,fakeProgress, 0);
    }

    void run() override{
      owner->loadDocumentAsync(fileToLoad);
      isEnded = true;
      owner->fileLoaderEnded();
    }

	//float fakeProgress ;
    Engine * owner;
    File fileToLoad;
    bool isEnded;
    

  };

  ScopedPointer<FileLoader> fileLoader;



  class EngineListener{
  public:
    virtual ~EngineListener(){};

    virtual void startLoadFile(){};
    // TODO implement progression
    virtual void fileProgress(float percent,int state){};
    virtual void endLoadFile(){};
  };

  ListenerList<EngineListener> engineListeners;
  void addEngineListener(EngineListener* e){engineListeners.add(e);}
  void removeEngineListener(EngineListener* e){engineListeners.remove(e);}

  bool isLoadingFile;
  var jsonData;

  void handleAsyncUpdate()override;

  
};


static String lastFileListKey = "recentNodeGraphFiles";


#endif  // ENGINE_H_INCLUDED
