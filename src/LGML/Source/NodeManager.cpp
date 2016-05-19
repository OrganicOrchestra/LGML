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
    ControllableContainer("Node Manager"),
	mainContainer("mainContainer")
{
	saveAndLoadRecursiveData = false;
    setCustomShortName("node");
	mainContainer.skipControllableNameInAddress = true;
	//mainContainer.addNodeContainerListener(this);
	addChildControllableContainer(&mainContainer);
}

NodeManager::~NodeManager()
{
    clear();

}

void NodeManager::clear()
{
	mainContainer.clear();
	audioGraph.clear();
	dataGraph.clear();
}

var NodeManager::getJSONData()
{
	var data = ControllableContainer::getJSONData();
	data.getDynamicObject()->setProperty("mainContainer", mainContainer.getJSONData());
	return data;
}

void NodeManager::loadJSONDataInternal(var data)
{
	mainContainer.loadJSONData(data.getDynamicObject()->getProperty("mainContainer"));
}

void NodeManager::updateAudioGraph() {
	AudioIODevice * ad = getAudioDeviceManager().getCurrentAudioDevice();
	audioGraph.prepareToPlay(ad->getCurrentSampleRate(), ad->getDefaultBufferSize());
}


