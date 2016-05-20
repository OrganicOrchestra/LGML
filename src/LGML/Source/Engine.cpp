/*
 ==============================================================================

 Engine.cpp
 Created: 2 Apr 2016 11:03:21am
 Author:  Martin Hermant

 ==============================================================================
 */

#include "Engine.h"
#include "LGMLLogger.h"
#include "MainComponent.h"
#include "DebugHelpers.h"

const char* const filenameSuffix = ".lgml";
const char* const filenameWildcard = "*.lgml";

Engine::Engine():FileBasedDocument (filenameSuffix,
                                    filenameWildcard,
                                    "Load a filter graph",
                                    "Save a filter graph"){
    initAudio();
    Logger::setCurrentLogger (LGMLLogger::getInstance());

	MIDIManager::getInstance()->init();
}


Engine::~Engine(){
    stopAudio();

	FastMapper::deleteInstance();
	MIDIManager::deleteInstance();
    TimeManager::deleteInstance(); //TO PREVENT LEAK OF SINGLETON
    ControllerManager::deleteInstance();
    NodeManager::deleteInstance();
    VSTManager::deleteInstance();
    PresetManager::deleteInstance();
	ControllerManager::deleteInstance();
    JsGlobalEnvironment::deleteInstance();
    Logger::setCurrentLogger(nullptr);
    LGMLLogger::deleteInstance();
    RuleManager::deleteInstance();
}

void Engine::parseCommandline(const String & commandLine){

    StringArray args;
	args.addTokens (commandLine, true);
    args.trim();

	DBG("Parse commandline : " << args[0]);

    int parsingIdx=0;
    while(parsingIdx<args.size())
	{
        String command = "";
        bool isParameter = args[parsingIdx].startsWith("-");
        if(isParameter){
            command = args[parsingIdx].substring(1, args[parsingIdx].length());
            parsingIdx++;
            if(parsingIdx>=args.size()){break;}
        }

		String argument = args[parsingIdx].removeCharacters(juce::StringRef("\""));

        //DBG("parsing commandline, command : " << command << ", argument :" << argument << " / parsingIdx : " << parsingIdx);

        if(command== "f"|| parsingIdx==0){
			if (File::isAbsolutePath(argument)) {
				File f(argument);
				if (f.existsAsFile()) loadDocument(f);
			}
			else { 
				NLOG("Engine","File : " << argument << " not found."); 
			}
        }


        parsingIdx++;
    }

}


void Engine::initAudio(){

    graphPlayer.setProcessor(&NodeManager::getInstance()->audioGraph);
    ScopedPointer<XmlElement> savedAudioState (getAppProperties().getUserSettings()->getXmlValue ("audioDeviceState"));
    getAudioDeviceManager().initialise (256, 256, savedAudioState, true);

    getAudioDeviceManager().addAudioCallback (&graphPlayer);
    getAudioDeviceManager().addAudioCallback(TimeManager::getInstance());
    DBG("init audio");

}


void Engine::stopAudio(){
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
    NodeManager::getInstance()->clear();
    PresetManager::getInstance()->clear();


    changed();    //fileDocument
}

void Engine::createNewGraph(){
    clear();

    ConnectableNode * node = NodeManager::getInstance()->mainContainer->addNode(NodeType::AudioDeviceInType);
    node->xPosition->setValue(150);
    node->yPosition->setValue(100);
    node = NodeManager::getInstance()->mainContainer->addNode(NodeType::AudioDeviceOutType);
    node->xPosition->setValue(450);
    node->yPosition->setValue(100);
    changed();
}
