/*
 ==============================================================================

 Engine.cpp
 Created: 2 Apr 2016 11:03:21am
 Author:  Martin Hermant

 ==============================================================================
 */

#include "Engine.h"
#include "SerialManager.h"
#include "LGMLLogger.h"
#include "MainComponent.h"
#include "DebugHelpers.h"

#include "NodeContainer.h"



const char* const filenameSuffix = ".lgml";
const char* const filenameWildcard = "*.lgml";

Engine::Engine():FileBasedDocument (filenameSuffix,
                                    filenameWildcard,
                                    "Load a filter graph",
                                    "Save a filter graph"),
	ControllableContainer("root")
{
	skipControllableNameInAddress = true;

  initAudio();
  Logger::setCurrentLogger (LGMLLogger::getInstance());

  MIDIManager::getInstance()->init();
  SerialManager::getInstance()->init();
  NodeManager::getInstance()->addNodeManagerListener(this);
  VSTManager::getInstance();

  addChildControllableContainer(NodeManager::getInstance());
  addChildControllableContainer(TimeManager::getInstance());
  addChildControllableContainer(ControllerManager::getInstance());
  addChildControllableContainer(FastMapper::getInstance());
  addChildControllableContainer(RuleManager::getInstance());
}


Engine::~Engine(){
  closeAudio();



  NodeManager::deleteInstance();
  PresetManager::deleteInstance();
  FastMapper::deleteInstance();



  VSTManager::deleteInstance();
  TimeManager::deleteInstance(); //TO PREVENT LEAK OF SINGLETON



  ControllerManager::deleteInstance();


  Logger::setCurrentLogger(nullptr);
  LGMLLogger::deleteInstance();
  RuleManager::deleteInstance();

  JsGlobalEnvironment::deleteInstance();
  MIDIManager::deleteInstance();

  SerialManager::deleteInstance();
  
}

void Engine::parseCommandline(const CommandLineElements & commandLine){

  for (auto & c:commandLine){
    if(c.command== "f"|| c.command==""){
      if(c.args.size()==0){
        LOG("no file provided for command : "+c.command);
        jassertfalse;
        continue;
      }
      String fileArg = c.args[0];
      if (File::isAbsolutePath(fileArg)) {
        File f(fileArg);
        if (f.existsAsFile()) loadDocument(f);
      }
      else {
        NLOG("Engine","File : " << fileArg << " not found.");
      }
    }

  }


}


void Engine::initAudio(){

  graphPlayer.setProcessor(NodeManager::getInstance()->mainContainer->getAudioGraph());
  ScopedPointer<XmlElement> savedAudioState (getAppProperties().getUserSettings()->getXmlValue ("audioDeviceState"));
  getAudioDeviceManager().initialise (64, 64, savedAudioState, true);
  getAudioDeviceManager().addChangeListener(&audioSettingsHandler);
  // timeManager should be the first audio Callback added to ensure that time is updated each new block
  getAudioDeviceManager().addAudioCallback(TimeManager::getInstance());
  getAudioDeviceManager().addAudioCallback (&graphPlayer);

  DBG("init audio");

}


void Engine::suspendAudio(bool shouldBeSuspended){


  if(AudioProcessor * ap =graphPlayer.getCurrentProcessor()){
    const ScopedLock lk(ap->getCallbackLock());
    ap->suspendProcessing (shouldBeSuspended);
    if(shouldBeSuspended)ap->releaseResources();
    else {
      if(AudioIODevice * dev = getAudioDeviceManager().getCurrentAudioDevice()){
      ap->prepareToPlay(dev->getCurrentSampleRate(), dev->getCurrentBufferSizeSamples());
      }
      else{
        // if no audio device are present initialize it (if not, inner graph is void)
        NLOG("Engine","!!! no audio device available !!!");
        ap->prepareToPlay(44100,1024);
      }
    }
  }

  TimeManager::getInstance()->lockTime(shouldBeSuspended);



}

void Engine::closeAudio(){
  getAudioDeviceManager().removeAudioCallback (&graphPlayer);
  getAudioDeviceManager().removeAudioCallback(TimeManager::getInstance());
  getAudioDeviceManager().closeAudioDevice();
}


void Engine::clear(){
  //    do we need to stop audio?
  //stopAudio();


  TimeManager::getInstance()->playState->setValue(false);

  FastMapper::getInstance()->clear();
  RuleManager::getInstance()->clear();
  
  ControllerManager::getInstance()->clear();
//  JsGlobalEnvironment::getInstance()->getEnv()->clear();
//  graphPlayer.setProcessor(nullptr);
  
  NodeManager::getInstance()->clear();
//graphPlayer.setProcessor(NodeManager::getInstance()->mainContainer->getAudioGraph());


  PresetManager::getInstance()->clear();



  changed();    //fileDocument
}

void Engine::stimulateAudio( bool s){
  if(s){
    stimulator= new AudioFucker(&getAudioDeviceManager());
    getAudioDeviceManager().addAudioCallback(stimulator);

  }
  else{
    getAudioDeviceManager().removeAudioCallback(stimulator);
    stimulator=nullptr;
  }

}




void Engine::MultipleAudioSettingsHandler::changeListenerCallback(ChangeBroadcaster *){
  //    Trick allowing to defer all changes have the last word
  startTimer(1);
}
void Engine::MultipleAudioSettingsHandler::timerCallback(){

  String configName = getConfigName();
  if(lastConfigName == configName){return;}
  ScopedPointer<XmlElement> oldSetupXml = getAppProperties().getUserSettings()->getXmlValue(oldSettingsId);
  if(!oldSetupXml)return;

  XmlElement * xml = oldSetupXml->getChildByName(configName);
  lastConfigName = configName;


  if(xml!=nullptr){
    XmlElement * xmlSetup = xml->getChildElement(0);
    if(xmlSetup){
      AudioDeviceManager::AudioDeviceSetup setup ;
      getAudioDeviceManager().getAudioDeviceSetup(setup);
      setup.bufferSize = xmlSetup->getIntAttribute("audioDeviceBufferSize",setup.bufferSize);
      setup.sampleRate = xmlSetup->getDoubleAttribute("audioDeviceRate",setup.sampleRate);

      setup.inputChannels .parseString (xmlSetup->getStringAttribute ("audioDeviceInChans",  "11"), 2);
      setup.outputChannels.parseString (xmlSetup->getStringAttribute ("audioDeviceOutChans", "11"), 2);
      setup.useDefaultInputChannels=false;
      setup.useDefaultOutputChannels = false;

      getAudioDeviceManager().setAudioDeviceSetup(setup, true);
    }
  }

  stopTimer();

}

String Engine::MultipleAudioSettingsHandler::getConfigName(){
  AudioDeviceManager::AudioDeviceSetup setup ;
  getAudioDeviceManager().getAudioDeviceSetup(setup);
  String idealName = setup.inputDeviceName+"_"+setup.outputDeviceName;
  String escaped = StringUtil::toShortName(idealName);
  return escaped;


}


void Engine::MultipleAudioSettingsHandler::saveCurrent(){
  ScopedPointer<XmlElement> audioState (getAudioDeviceManager().createStateXml());
  getAppProperties().getUserSettings()->setValue ("audioDeviceState", audioState);
  ScopedPointer<XmlElement> oldXml = getAppProperties().getUserSettings()->getXmlValue(oldSettingsId);
  if(!oldXml){oldXml = new XmlElement(oldSettingsId);}

  String configName = getConfigName();
  XmlElement * oldConfig = oldXml->getChildByName(configName);

  if(oldConfig){oldXml->removeChildElement(oldConfig, true);}
  oldConfig = oldXml->createNewChildElement(configName) ;
  oldConfig->addChildElement(getAudioDeviceManager().createStateXml());


  getAppProperties().getUserSettings()->setValue(oldSettingsId.toString(), oldXml);
  getAppProperties().getUserSettings()->saveIfNeeded();

}


