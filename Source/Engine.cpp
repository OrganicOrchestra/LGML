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

#include "Engine.h"

#include "Controller/Impl/SerialManager.h"
#include "Logger/LGMLLogger.h"
#include "Utils/DebugHelpers.h"

#include "Node/NodeContainer/NodeContainer.h"
#include "Node/Impl/AudioDeviceOutNode.h" //  for fade out on quit
#include "Utils/AudioDebugPipe.h"
#include "Utils/AudioDebugCrack.h"
#include "Controllable/Parameter/ParameterFactory.h"
#include "Utils/NetworkUtils.h"


#include "Scripting/Js/JsGlobalEnvironment.h"

#include "Version.h"

// this allow to avoid juce header dependency in all other source code
// calling Engine::versionNumber instead of ProjectInfo::versionNumber
int Engine::versionNumber( ProjectInfo::versionNumber);
const char* Engine::versionString ( ProjectInfo::versionString);
const char* Engine::projectName   ( ProjectInfo::projectName);


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
    setDefault(settings,"check for updates",true);
    setDefault(settings,"language","english");

    settings->saveIfNeeded();
}

Engine::Engine():
FileBasedDocument (filenameSuffix,
                                     filenameWildcard,
                                     "Load a filter graph",
                                     "Save a filter graph"),
ParameterContainer ("root"),
threadPool (4),
isLoadingFile(false),
engineStartTime(Time::currentTimeMillis()),
hasDefaultOSCControl(false)

{
    nameParam->isEditable = false;
    ControllableContainer::globalRoot = this;
    ParameterFactory::registerExtraTypes();
    ParameterFactory::logAllTypes();
    saveSession = addNewParameter<StringParameter>("save", "save current session");
    saveSession->alwaysNotify = true;
    saveSession->isHidenInEditor = true;
    loadSession = addNewParameter<StringParameter>("load", "load a given session");
    loadSession->alwaysNotify = true;
    loadSession->isHidenInEditor = true;
    closeEngine = addNewParameter<Trigger>("close","close engine");
    closeEngine->isHidenInEditor = true;
    addChildControllableContainer(engineStats=new EngineStats(this));

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
    setLanguage(getAppProperties()->getUserSettings()->getValue("language"));

}


Engine::~Engine()
{
    engineListeners.call (&EngineListener::stopEngine);
    engineListeners.clear();
    controllableContainerListeners.clear();

    jassert(fadeAudioOut());
    closeAudio();

    threadPool.removeAllJobs(true, -1);
    JsGlobalEnvironment::deleteInstance(); // need to be first to avoid memory leaks

    NodeManager::deleteInstance();
    PresetManager::deleteInstance();
    FastMapper::deleteInstance();



    VSTManager::deleteInstance();
    TimeManager::deleteInstance();




    ControllerManager::deleteInstance();


    Logger::setCurrentLogger (nullptr);
    LGMLLogger::deleteInstance();



    MIDIManager::deleteInstance();

    SerialManager::deleteInstance();

    NetworkUtils::deleteInstance();

    AudioDebugPipe::deleteInstanciated();
#if !FORCE_DISABLE_CRACK
    AudioDebugCrack::deleteInstanciated();
#endif


}

bool Engine::fadeAudioOut(){
    auto * nm =NodeManager::getInstanceWithoutCreating();
    if(!nm) return false;
    AudioDeviceOutNode * outNode = nullptr;
    for(auto n:nm->nodes){
        if(auto on = dynamic_cast<AudioDeviceOutNode*>(n)){
            outNode = on;
            outNode->globalFader.startFadeOut();
        }
    }
    if(!outNode){
        return false;
    }

    int maxCount = 100;
    while(outNode->globalFader.getLastFade()!=0){
        maxCount--;
        if(maxCount<=0){
            return false;
        }
        Thread::sleep(10);
    }
    return true;

}

void Engine::onContainerParameterChanged ( ParameterBase* p) {
    if(p==saveSession){
        File fileToLoad (loadSession->stringValue());
        MessageManager::callAsync([this,fileToLoad](){saveAs(File(saveSession->stringValue()),false, false,true);});
    }
    else if( p==loadSession){
        File fileToLoad (loadSession->stringValue());
        MessageManager::callAsync([this,fileToLoad](){loadFrom(fileToLoad,true);});
    }

};
void Engine::onContainerTriggerTriggered(Trigger *t){
    if(t==closeEngine){
        JUCEApplication::getInstance()->systemRequestedQuit();
    }
}

void Engine::parseCommandline (const CommandLineElements& commandLine)
{

    File fileToLoad = File();
    for (auto& c : commandLine)
    {
        if (c.command == "f" || c.command == "")
        {
            if (c.args.size() == 0)
            {
                LOGE(juce::translate("no file provided for command : ") + c.command);
                jassertfalse;
                continue;
            }

            String fileArg = c.args[0];
            fileToLoad = File (fileArg);

            if( !(File::isAbsolutePath (fileArg) && fileToLoad.existsAsFile())){

                NLOGE("Engine", juce::translate("File : 123 not found.").replace("123", fileArg));
            }
        }
        else if(c.command=="p"){
            if (c.args.size() == 0)
            {
                LOG (juce::translate("no preferences provided for command : 123").replace("123", c.command));
                jassertfalse;
                continue;
            }


            if(c.args.size()==0 || c.args.size()%2!=0){
                LOGE(juce::translate("unable to parse parameter : 123").replace("123", c.args.joinIntoString(":")));
                jassertfalse;
                continue;
            }
            for( int i = 0 ; i < c.args.size()-1 ; i+=2){
                if (!getAppProperties()->getUserSettings()->containsKey(c.args[i])){
                    LOGE(juce::translate("unknown parameter : ") << c.args[i]);
                    jassertfalse;
                    continue;
                }
                getAppProperties()->getUserSettings()->setValue(c.args[i], c.args[i+1]);
            }
        }
        else if (c.command=="remote"){
            getEngine()->hasDefaultOSCControl = true;
        }

    }

    if (fileToLoad.existsAsFile()) loadFrom (fileToLoad,true);

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


    if (auto* ap = dynamic_cast<AudioProcessorGraph*>(graphPlayer.getCurrentProcessor()))
    {
        ap->suspendProcessing (shouldBeSuspended);
        const ScopedLock lk (ap->getCallbackLock());


        if (shouldBeSuspended){
            ap->releaseResources();
        }
        else
        {
            if (AudioIODevice* dev = getAudioDeviceManager().getCurrentAudioDevice())
            {
                NodeManager::getInstance()->setRateAndBufferSizeDetails(dev->getCurrentSampleRate(), dev->getCurrentBufferSizeSamples());
                ap->releaseResources();
                ap->prepareToPlay (dev->getCurrentSampleRate(), dev->getCurrentBufferSizeSamples());
            }
            else
            {
                // if no audio device are present initialize it (if not, inner graph is void)
                NLOGE("Engine", juce::translate("no audio device available"));
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
//      graphPlayer.setProcessor(nullptr);

    PresetManager::getInstance()->clear();

    NodeManager::getInstance()->clear();

    JsGlobalEnvironment::getInstance()->clear();
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


File& getTranslationFolder(){
static File tf = File::getSpecialLocation(File::userDocumentsDirectory).getChildFile("LGML").getChildFile("translations");
    return tf;
}
void Engine::setLanguage(const String & s){

    if(s=="english"){
        juce::LocalisedStrings::setCurrentMappings(nullptr);
        return;
    }
    auto translationsFolder = getTranslationFolder();
    String fname = s.toLowerCase()+".txt";
    File curF = translationsFolder.getChildFile(fname);
    if(curF.exists()){
        juce::LocalisedStrings::setCurrentMappings(new LocalisedStrings(curF,true));
    }
    else{
        // no translation found
        // jassertfalse;
    }
}
StringArray  Engine::getAvailableLanguages(){
    auto tf = getTranslationFolder();
    Array<File> tfs ;
    tf.findChildFiles(tfs, File::findFiles, false,"*.txt");
    StringArray res = {"english"};
    for(auto& r:tfs){
        auto lname = r.getFileNameWithoutExtension();
        if(lname!="english"){
            res.add(juce::translate(lname));
        }
    }
    return res;


}






/////////////
//Engine Stats
/////////

Engine::EngineStats::EngineStats(Engine * e):
engine(e),
ParameterContainer("stats"),
isListeningGlobal(false),
timerTicks(0){
    setUserDefined(false);
    audioCpu = addNewParameter<Point2DParameter<floatParamType>>("audioCpu",
                                                        "cpu percentage used by Audio",
                                                        0,0);
    audioCpu->isEditable = false;
    audioCpu->isSavable = false;

    //startTimer(300);

//TODO implement UI Monitoring tool
//    activateGlobalStats(true);

}
void Engine::EngineStats::timerCallback(){
    timerTicks++;
    auto time = engine->getElapsedMillis();

    audioCpu->setPoint(getAudioDeviceManager().getCpuUsage() * 100.0f,time);
    if(isListeningGlobal){
        //        const ScopedLock lk(modCounts.getLock());
        CountMapType::Iterator it (modCounts);

        typedef std::pair<String, int> UsagePoint;
        Array<UsagePoint> paramUsage;
        while (it.next())
        {
            int usage = it.getValue().size();
            String pName = it.getKey();
            struct EComp{
                int compareElements( const UsagePoint & a,const UsagePoint & b){return a.second-b.second;};
            };
            static EComp eComp;
            paramUsage.addSorted(eComp,UsagePoint(pName,usage));

        }

        int toPrint = jmin(3,paramUsage.size());
        for (int i = 0 ; i < toPrint ; i++){
            auto u = paramUsage[i];
            DBG(u.first << ":" << String(u.second));
        }
//        int curtime = engine->getElapsedMillis();
        // clean old
        it.reset();
        Array<String> toRemove;
        while (it.next())
        {

            String addr = it.getKey();
            Array<int> tl =  it.getValue();
            int i = 0;
            while(i < tl.size()){
//                auto t = tl[i];
                //                if(t<curtime-3000){
                tl.remove(i);
                //                }
                //                else{
                //                    i++;
                //                }
            }
            if(tl.size()==0){
                toRemove.add(addr);
            }


        }

        for(auto a:toRemove){
            modCounts.remove(a);
        }
        //        modCounts.clear();


    }
}

float Engine::EngineStats::getAudioCPU() const{
    return audioCpu->getX();
}
template<>
void Engine::EngineStats::GlobalListener::controllableFeedbackUpdate(ControllableContainer * notif,Controllable * c ){
    if(c&& c->parentContainer!=owner){
        const int  t = getEngine()->getElapsedMillis();
        owner->modCounts.getReference(c->controlAddress).add(t);
    }
}

void Engine::EngineStats::activateGlobalStats(bool s){
    isListeningGlobal = s;
    if(s){
        globalListener = new GlobalListener(this);
        engine->addControllableContainerListener(globalListener);
    }
    else{
        engine->removeControllableContainerListener(globalListener);
        globalListener = nullptr;
    }
}




