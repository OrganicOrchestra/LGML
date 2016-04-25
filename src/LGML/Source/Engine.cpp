/*
 ==============================================================================
 
 Engine.cpp
 Created: 2 Apr 2016 11:03:21am
 Author:  Martin Hermant
 
 ==============================================================================
 */

#include "Engine.h"

#include "NodeConnectionEditor.h"

const char* const filenameSuffix = ".lgml";
const char* const filenameWildcard = "*.lgml";

Engine::Engine():FileBasedDocument (filenameSuffix,
                                    filenameWildcard,
                                    "Load a filter graph",
                                    "Save a filter graph"){
	initAudio();
}


Engine::~Engine(){
    stopAudio();
    TimeManager::deleteInstance(); //TO PREVENT LEAK OF SINGLETON
    ControllerManager::deleteInstance();
    NodeManager::deleteInstance();
    VSTManager::deleteInstance();
}

void Engine::parseCommandline(const String & commandLine){
	
	StringArray args;

	args.addTokens (commandLine, false);
	args.trim();
	
	
	int parsingIdx=0;
	while(parsingIdx<args.size()){
		String command = "";
		bool isParameter = args[parsingIdx].startsWith("-");
		if(isParameter){
			command = args[parsingIdx].substring(1, args[parsingIdx].length());
			parsingIdx++;
			if(parsingIdx>=args.size()){break;}
		}
		String argument = File::createLegalPathName(args[parsingIdx]);
		
		DBG("parsing commandline :" << command << " " << argument);
		
		
		if(command== "f"|| parsingIdx==0){
			if (File::isAbsolutePath(argument)) {
				File f(argument);
				if (f.existsAsFile()) loadDocument(f);
			}
			else{DBG("not found file : " << argument << " please provide a valid absolute path");}
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
	
	
    TimeManager::getInstance()->stop();
    ControllerManager::getInstance()->clear();
    NodeManager::getInstance()->clear();
	
    changed();    //fileDocument
}

void Engine::createNewGraph(){
    clear();
	
    NodeBase * node = NodeManager::getInstance()->addNode(NodeFactory::NodeType::AudioIn);
    node->xPosition->setValue(150);
    node->yPosition->setValue(50);
    node = NodeManager::getInstance()->addNode(NodeFactory::NodeType::AudioOut);
    node->xPosition->setValue(450);
    node->yPosition->setValue(50);
    changed();
}
