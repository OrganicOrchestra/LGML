/*
  ==============================================================================

    NodeManager.cpp
    Created: 2 Mar 2016 8:33:39pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeManager.h"
#include "DummyNode.h"


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

void NodeManager::updateAudioGraph() {
	AudioIODevice * ad = getAudioDeviceManager().getCurrentAudioDevice();
	audioGraph.prepareToPlay(ad->getCurrentSampleRate(), ad->getDefaultBufferSize());
}


