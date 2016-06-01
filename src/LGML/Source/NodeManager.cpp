/*
  ==============================================================================

    NodeManager.cpp
    Created: 2 Mar 2016 8:33:39pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeManager.h"
#include "DummyNode.h"


AudioDeviceManager& getAudioDeviceManager();

juce_ImplementSingleton(NodeManager);

NodeManager::NodeManager() :
    ControllableContainer("Node Manager")
{
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
	mainContainer->clear();
	audioGraph.clear();
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
	clear();
	mainContainer->loadJSONData(data.getDynamicObject()->getProperty("mainContainer"));
}

void NodeManager::updateAudioGraph() {
	AudioIODevice * ad = getAudioDeviceManager().getCurrentAudioDevice();
    if(ad == nullptr) return;
    ScopedLock lk( getAudioDeviceManager().getAudioCallbackLock());
    audioGraph.prepareToPlay(ad->getCurrentSampleRate(), ad->getDefaultBufferSize());

}
