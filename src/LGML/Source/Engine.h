/*
 ==============================================================================

 Engine.h
 Created: 2 Apr 2016 11:03:21am
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED


#include "MIDIManager.h"//keep
#include "ControlManager.h"//keep
#include "NodeManager.h"//keep
#include "PresetManager.h"//keep
#include "AudioFucker.h"//keep
#include "TimeManager.h"//keep
#include "RuleManager.h"//keep
#include "FastMapper.h"//keep
#include "VSTManager.h"//keep
class AudioFucker;

class Engine:public FileBasedDocument,NodeManager::NodeManagerListener{
public:
    Engine();
    ~Engine();

    // Audio
    AudioProcessorPlayer graphPlayer;

    void createNewGraph();
    void clear();
    void initAudio();
    void closeAudio();


    void suspendAudio(bool);

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
    var getJSONData();
    void loadJSONData(var data);

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
  File fileBeingLoaded;
  void managerEndedLoading()override;

  bool isLoadingFile;
    
};


static String lastFileListKey = "recentNodeGraphFiles";


#endif  // ENGINE_H_INCLUDED
