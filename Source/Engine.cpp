/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#define DO_PRAGMA(x) _Pragma (#x)
#define TODO(x) DO_PRAGMA(message ("TODO - " #x))



#include "Engine.h"

#include "Controller/Impl/SerialManager.h"
#include "Logger/LGMLLogger.h"
#include "Utils/DebugHelpers.h"

#include "Node/NodeContainer/NodeContainer.h"
#include "Utils/AudioDebugPipe.h"
#include "Utils/AudioDebugCrack.h"
#include "Controllable/Parameter/ParameterFactory.h"
#include "Utils/NetworkUtils.h"

#include "Utils/StringUtil.hpp"
#include "Scripting/Js/JsGlobalEnvironment.h"


const char* const filenameSuffix = ".lgml";
const char* const filenameWildcard = "*.lgml";

void setDefault(PropertiesFile *f, const String & n, const var & d)
{
    if(!f->containsKey(n))
        f->setValue(n, d);
}

void initDefaultUserSettings(){
    auto settings = getAppProperties()->getUserSettings();
    setDefault(settings,"multiThreadedLoading",false);
    setDefault(settings,"check for updates",false);

    settings->saveIfNeeded();
}

Engine::Engine(): FileBasedDocument (filenameSuffix,
                                         filenameWildcard,
                                         "Load a filter graph",
                                         "Save a filter graph"),
    ParameterContainer ("root"),
    threadPool (4),
    isLoadingFile(false),
    engineStartTime(Time::currentTimeMillis())

{
    nameParam->isEditable = false;
    ControllableContainer::globalRoot = this;
    ParameterFactory::logAllTypes();
    
    loadingStartTime = 0;
    initAudio();
    Logger::setCurrentLogger (LGMLLogger::getInstance());

    MIDIManager::getInstance()->init();
    SerialManager::getInstance()->init();
    NodeManager::getInstance()->addNodeManagerListener (this);
    VSTManager::getInstance();


    addChildControllableContainer (NodeManager::getInstance());
    addChildControllableContainer (TimeManager::getInstance());
    addChildControllableContainer (ControllerManager::getInstance());
    addChildControllableContainer (FastMapper::getInstance());

    DBG ("max recording time : " << std::numeric_limits<sample_clk_t>().max() / (44100.0 * 60.0 * 60.0) << "hours @ 44.1kHz");
    initDefaultUserSettings();
    
}


Engine::~Engine()
{
    engineListeners.call (&EngineListener::stopEngine);
    engineListeners.clear();
    controllableContainerListeners.clear();


    closeAudio();

    NodeManager::deleteInstance();
    PresetManager::deleteInstance();
    FastMapper::deleteInstance();



    VSTManager::deleteInstance();
    TimeManager::deleteInstance();




    ControllerManager::deleteInstance();


    Logger::setCurrentLogger (nullptr);
    LGMLLogger::deleteInstance();


    JsGlobalEnvironment::deleteInstance();
    MIDIManager::deleteInstance();

    SerialManager::deleteInstance();

    NetworkUtils::deleteInstance();

    AudioDebugPipe::deleteInstanciated();
#if !FORCE_DISABLE_CRACK
    AudioDebugCrack::deleteInstanciated();
#endif


}

void Engine::parseCommandline (const CommandLineElements& commandLine)
{

    for (auto& c : commandLine)
    {
        if (c.command == "f" || c.command == "")
        {
            if (c.args.size() == 0)
            {
                LOG ("no file provided for command : " + c.command);
                jassertfalse;
                continue;
            }

            String fileArg = c.args[0];

            if (File::isAbsolutePath (fileArg))
            {
                File f (fileArg);

                if (f.existsAsFile()) loadDocument (f);
            }
            else
            {
                NLOG ("Engine", "File : " << fileArg << " not found.");
            }
        }
        else if(c.command=="p"){
            if (c.args.size() == 0)
            {
                LOG ("no preferences provided for command : " + c.command);
                jassertfalse;
                continue;
            }


            if(c.args.size()==0 || c.args.size()%2!=0){
                LOG("unable to parse parameter : " << c.args.joinIntoString(":"));
                jassertfalse;
                continue;
            }
            for( int i = 0 ; i < c.args.size()-1 ; i+=2){
                if (!getAppProperties()->getUserSettings()->containsKey(c.args[i])){
                    LOG("unknown parameter : " << c.args[i]);
                    jassertfalse;
                    continue;
                }
                getAppProperties()->getUserSettings()->setValue(c.args[i], c.args[i+1]);
            }
        }

    }


}


void Engine::initAudio()
{

    graphPlayer.setProcessor (NodeManager::getInstance()->getAudioGraph());
    ScopedPointer<XmlElement> savedAudioState (getAppProperties()->getUserSettings()->getXmlValue ("audioDeviceState"));
    getAudioDeviceManager().initialise (64, 64, savedAudioState, true);
    getAudioDeviceManager().addChangeListener (&audioSettingsHandler);
    // timeManager should be the first audio Callback added to ensure that time is updated each new block
    getAudioDeviceManager().addAudioCallback (TimeManager::getInstance());
    getAudioDeviceManager().addAudioCallback (&graphPlayer);

    DBG ("init audio");

}


void Engine::suspendAudio (bool shouldBeSuspended)
{


    if (AudioProcessor* ap = graphPlayer.getCurrentProcessor())
    {
        ap->suspendProcessing (shouldBeSuspended);
        const ScopedLock lk (ap->getCallbackLock());


        if (shouldBeSuspended)ap->releaseResources();
        else
        {
            if (AudioIODevice* dev = getAudioDeviceManager().getCurrentAudioDevice())
            {
                NodeManager::getInstance()->setRateAndBufferSizeDetails(dev->getCurrentSampleRate(), dev->getCurrentBufferSizeSamples());
                ap->prepareToPlay (dev->getCurrentSampleRate(), dev->getCurrentBufferSizeSamples());
            }
            else
            {
                // if no audio device are present initialize it (if not, inner graph is void)
                NLOG ("Engine", "!!! no audio device available !!!");
                ap->prepareToPlay (44100, 1024);
            }
        }
    }
    else
    {
        jassertfalse;
    }

    TimeManager::getInstance()->lockTime (shouldBeSuspended);



}

void Engine::closeAudio()
{
    getAudioDeviceManager().removeAudioCallback (&graphPlayer);
    getAudioDeviceManager().removeAudioCallback (TimeManager::getInstance());
    getAudioDeviceManager().closeAudioDevice();
}


void Engine::clear()
{
    //    do we need to stop audio?
    //  suspendAudio(true);


    TimeManager::getInstance()->playState->setValue (false);

    FastMapper::getInstance()->clear();

    ControllerManager::getInstance()->clear();
    //  JsGlobalEnvironment::getInstance()->getEnv()->clear();
    //  graphPlayer.setProcessor(nullptr);

    PresetManager::getInstance()->clear();

    NodeManager::getInstance()->clear();

    
    //graphPlayer.setProcessor(NodeManager::getInstance()->getAudioGraph());




    //  suspendAudio(false);

    changed();    //fileDocument
}

void Engine::stimulateAudio ( bool s)
{
    if (s)
    {
        stimulator = new AudioFucker (&getAudioDeviceManager());
        getAudioDeviceManager().addAudioCallback (stimulator);

    }
    else
    {
        getAudioDeviceManager().removeAudioCallback (stimulator);
        stimulator = nullptr;
    }

}




void Engine::MultipleAudioSettingsHandler::changeListenerCallback (ChangeBroadcaster*)
{
    //    Trick allowing to defer all changes have the last word
    startTimer (300);
}
void Engine::MultipleAudioSettingsHandler::timerCallback()
{

    String configName = getConfigName();

    if (lastConfigName == configName) {return;}

    ScopedPointer<XmlElement> oldSetupXml = getAppProperties()->getUserSettings()->getXmlValue (oldSettingsId);

    if (!oldSetupXml)return;

    XmlElement* xml = oldSetupXml->getChildByName (configName);
    lastConfigName = configName;


    if (xml != nullptr)
    {
        XmlElement* xmlSetup = xml->getChildElement (0);

        if (xmlSetup)
        {
            AudioDeviceManager::AudioDeviceSetup setup ;
            getAudioDeviceManager().getAudioDeviceSetup (setup);
            setup.bufferSize = xmlSetup->getIntAttribute ("audioDeviceBufferSize", setup.bufferSize);
            setup.sampleRate = xmlSetup->getDoubleAttribute ("audioDeviceRate", setup.sampleRate);

            setup.inputChannels .parseString (xmlSetup->getStringAttribute ("audioDeviceInChans",  "11"), 2);
            setup.outputChannels.parseString (xmlSetup->getStringAttribute ("audioDeviceOutChans", "11"), 2);
            setup.useDefaultInputChannels = false;
            setup.useDefaultOutputChannels = false;

            getAudioDeviceManager().setAudioDeviceSetup (setup, true);
        }
    }

    stopTimer();

}

String Engine::MultipleAudioSettingsHandler::getConfigName()
{
    AudioDeviceManager::AudioDeviceSetup setup ;
    getAudioDeviceManager().getAudioDeviceSetup (setup);
    String idealName = setup.inputDeviceName + "_" + setup.outputDeviceName;
    String escaped = Controllable::toShortName (idealName);
    return escaped;


}


void Engine::MultipleAudioSettingsHandler::saveCurrent()
{
    ScopedPointer<XmlElement> audioState (getAudioDeviceManager().createStateXml());
    getAppProperties()->getUserSettings()->setValue ("audioDeviceState", audioState);
    ScopedPointer<XmlElement> oldXml = getAppProperties()->getUserSettings()->getXmlValue (oldSettingsId);

    if (!oldXml) {oldXml = new XmlElement (oldSettingsId);}

    String configName = getConfigName();
    XmlElement* oldConfig = oldXml->getChildByName (configName);

    if (oldConfig) {oldXml->removeChildElement (oldConfig, true);}

    oldConfig = oldXml->createNewChildElement (configName) ;
    oldConfig->addChildElement (getAudioDeviceManager().createStateXml());


    getAppProperties()->getUserSettings()->setValue (oldSettingsId.toString(), oldXml);
    getAppProperties()->getUserSettings()->saveIfNeeded();

}

const int Engine::getElapsedMillis()const {
    int64 res =Time::currentTimeMillis() -engineStartTime;
    jassert(res>=0);
    return (int)res;
}


