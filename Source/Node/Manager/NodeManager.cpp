/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#include "NodeManager.h"
#include "../Impl/DummyNode.h"
#include "../NodeContainer/NodeContainer.h"

IMPL_OBJ_TYPE(NodeManager);
extern AudioDeviceManager& getAudioDeviceManager();
bool isEngineLoadingFile();
juce_ImplementSingleton(NodeManager);

NodeManager::NodeManager(StringRef name) :
  ThreadPool(4),
    ParameterContainer("Nodes")
{
  isLoading = false;
    setCustomShortName("node");

	mainContainer = new NodeContainer("mainContainer");
	addChildControllableContainer(mainContainer);
	mainContainer->skipControllableNameInAddress = true;
	mainContainer->enabledParam->isControllableExposed = false;
	mainContainer->miniMode->isControllableExposed = false;
	mainContainer->nameParam->isControllableExposed = false;
}

NodeManager::~NodeManager()
{
	clear();
}

void NodeManager::clear()
{
  // maincontainer will automaticly delete all nodes with him
//  removeChildControllableContainer(mainContainer);
//mainContainer = new NodeContainer("mainContainer");
//  addChildControllableContainer(mainContainer);
  mainContainer->clear();

	dataGraph.clear();

	nodeManagerListeners.call(&NodeManagerListener::managerCleared);
}



void NodeManager::configureFromObject(DynamicObject * data)
{
  jassert(isLoading ==false);
  jobsWatcher = new JobsWatcher(this);
  isLoading = true;
	clear();
  ParameterContainer::configureFromObject(data);

//	mainContainer->loadJSONData(data.getDynamicObject()->getProperty("mainContainer"));

}

void NodeManager::rebuildAudioGraph() {
  if(!isLoading && !isEngineLoadingFile()){
  mainContainer->updateAudioGraph();
  }
	
}


void NodeManager::notifiedJobsEnded(){
  isLoading = false;
  rebuildAudioGraph();
  nodeManagerListeners.call(&NodeManagerListener::managerEndedLoading);

}

void NodeManager::notifiedJobsProgressed(float progress)
{
	nodeManagerListeners.call(&NodeManagerListener::managerProgressedLoading, progress);
}
