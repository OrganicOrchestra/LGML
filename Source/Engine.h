/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

#define MULTITHREADED_LOADING


#include "JuceHeaderAudio.h"
#include "Version.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/players/juce_AudioProcessorPlayer.h>
#include "MIDI/MIDIManager.h"

#include "Node/Manager/NodeManager.h"

#include "Utils/ProgressNotifier.h"
#include "Utils/CommandLineElements.hpp"

//#include "Utils/AudioFucker.h"
//#include "Controller/ControllerManager.h"
//#include "Preset/PresetManager.h"
//#include "Time/TimeManager.h"
//#include "FastMapper/FastMapper.h"
//#include "Audio/VSTManager.h"
class AudioFucker;



#if ENGINE_HEADLESS
#include "Utils/HeadlessWrappers.h"
#else
    #include "JuceHeaderUI.h"
#endif

class Engine:
    public juce::FileBasedDocument,
    NodeManager::NodeManagerListener,
    AsyncUpdater,
    public ProgressNotifier,
    public ParameterContainer
{
public:
    Engine();
    ~Engine();

    // Audio
    AudioProcessorPlayer graphPlayer;

    void createNewGraph();
    void clear();


    ParameterBase* saveSession,*loadSession,*closeEngine;

    class EngineStats : public ParameterContainer ,public Timer{
    public:
        EngineStats(Engine *);
        void activateGlobalStats(bool);
        float getAudioCPU() const;
        Point2DParameter<floatParamType> * audioCpu;
        bool isListeningGlobal;
        typedef OwnedFeedbackListener<EngineStats> GlobalListener;
        typedef HashMap<String, Array<int>,DefaultHashFunctions,CriticalSection> CountMapType;
         CountMapType modCounts;
    private:
        void timerCallback()override;
        Engine * engine;
        int timerTicks;
        std::unique_ptr< GlobalListener > globalListener;
    };


    //==============================================================================
    // see EngineFileDocument.cpp

    //  inherited from FileBasedDocument
    String getDocumentTitle()override ;
    // do not call this, call loadFrom instead (empowers FileBasedDocument behaviour)


    // helpers for file document
    File getCurrentProjectFolder();
    // return absolute Path if out of project directory
    String getNormalizedFilePath (const File& f);
    File getFileAtNormalizedPath (const String& path);

    File getLastDocumentOpened() override;

    VersionTriplet getSupportedVersionMask();

    void  stimulateAudio (bool);


    class MultipleAudioSettingsHandler : public ChangeListener, public Timer
    {
    public:
        MultipleAudioSettingsHandler(): oldSettingsId ("oldAudioSettings") {}
        void init();
        Identifier oldSettingsId;
        void changeListenerCallback (ChangeBroadcaster* )override;
        void saveCurrent();
        String getConfigName();

        String lastConfigName;
        void timerCallback()override;

    };

    MultipleAudioSettingsHandler audioSettingsHandler;


    const int getElapsedMillis()const ;

    void managerEndedLoading() override;
    void managerProgressedLoading (float progress) override;




    class EngineListener
    {
    public:
        EngineListener();
        virtual ~EngineListener();
        virtual void startEngine() {};
        virtual void stopEngine() {};
        virtual void startLoadFile() {};
        virtual void fileProgress (float percent, int state) {};
        virtual void endLoadFile() {};
    };

    class EngineFileSaver{
    public:
        EngineFileSaver(const String & name);
        virtual ~EngineFileSaver();
        virtual Result saveFiles(const File & baseFolder) = 0;
        virtual Result loadFiles(const File & baseFolder) = 0;
        virtual bool isDirty()=0;
        const String name;
    };




    void addEngineListener (EngineListener* e) {engineListeners.add (e);}
    void removeEngineListener (EngineListener* e) {engineListeners.remove (e);}

    bool isLoadingFile;
    
    static void setLanguage(const String & l);
    static StringArray getAvailableLanguages();
    static File& getTranslationFolder();



    RecentlyOpenedFilesList getLastOpenedFileList();




    std::unique_ptr< EngineStats> engineStats;


    ThreadPool threadPool;

private:

    std::unique_ptr<AudioFucker> stimulator;

    // our Saving methods
    DynamicObject* createObject() override;
    void loadJSONData (const var& data, ProgressTask* loadingTask);

    bool checkFileVersion (DynamicObject* metaData);


    Result loadDocument (const File& file)override;
    Result saveDocument (const File& file)override;

    void setLastDocumentOpened (const File& file) override;

    void parseCommandline (const CommandLineElements& );


    int64 engineStartTime;
    int loadingStartTime;


    void fileLoaderEnded();
    void loadDocumentAsync (const File& file);

    
    bool hasDefaultOSCControl;


    ListenerList<EngineListener> engineListeners;


    void initAudio();
    bool fadeAudioOut();
    void closeAudio();
    void suspendAudio (bool shouldSuspend);


    bool allLoadingThreadsAreEnded();

    void onContainerParameterChanged( ParameterBase*)final;
    void onContainerTriggerTriggered(Trigger *t)final;

    class FileLoader : public Thread, private Timer
    {
    public:
        FileLoader (Engine* e, File f): Thread ("EngineLoader"), owner (e), fileToLoad (f)
        {
            //startTimerHz(4);
            //fakeProgress = 0;
            isEnded = false;
        }
        ~FileLoader()
        {

        }

        void timerCallback()override
        {
            //fakeProgress+=getTimerInterval()/5000.0;
            //fakeProgress = jmin(1.0f,fakeProgress);
            //owner->engineListeners.call(&EngineListener::fileProgress,fakeProgress, 0);
        }

        void run() override
        {
            owner->loadDocumentAsync (fileToLoad);
            isEnded = true;
            owner->fileLoaderEnded();
        }

        //float fakeProgress ;
        Engine* owner;
        File fileToLoad;
        bool isEnded;

        
    };


    Array<EngineFileSaver*> fileSavers;
    void handleAsyncUpdate()override;

    std::unique_ptr<FileLoader> fileLoader;
    friend class FileLoader;


    friend class LGMLApplication;


};




extern Engine* getEngine();
extern bool isEngineLoadingFile();
extern AudioDeviceManager& getAudioDeviceManager();
extern ThreadPool* getEngineThreadPool();

#endif  // ENGINE_H_INCLUDED
