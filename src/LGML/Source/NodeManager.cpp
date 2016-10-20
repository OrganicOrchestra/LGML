/*
  ==============================================================================

    NodeManager.cpp
    Created: 2 Mar 2016 8:33:39pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeManager.h"
#include "DummyNode.h"
#include "NodeContainer.h"

AudioDeviceManager& getAudioDeviceManager();
bool isEngineLoadingFile();
juce_ImplementSingleton(NodeManager);

NodeManager::NodeManager() :
  ThreadPool(4),
    ControllableContainer("Node Manager")
{
  isLoading = false;
	saveAndLoadRecursiveData = false;
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

var NodeManager::getJSONData()
{
	var data = ControllableContainer::getJSONData();
	data.getDynamicObject()->setProperty("mainContainer", mainContainer->getJSONData());
	return data;
}

void NodeManager::loadJSONDataInternal(var data)
{
  jassert(isLoading ==false);
  jobsWatcher = new JobsWatcher(this);
  isLoading = true;
	clear();
	mainContainer->loadJSONData(data.getDynamicObject()->getProperty("mainContainer"));

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
