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


#include "AudioMixerNodeUI.h"
#include "../UI/ConnectableNodeUI.h"

void AudioMixerNodeUI::resized() {
	if(outputBusUIs.size()==0)return;
	Rectangle<int> area = getLocalBounds();
	if(mixerNode->oneToOne->boolValue()){
		int diagoNum = jmin(mixerNode->numberOfInput->intValue(),mixerNode->numberOfOutput->intValue());
		float step = (float)(area.getWidth()/ diagoNum);
		const int pad = 3;
		for(int i = 0 ; i < outputBusUIs.size() ; i ++){
			OutputBusUI * o = outputBusUIs.getUnchecked(i);
			if(i<diagoNum){
				o->setOneVisible(i);
				o->setVisible(true);
				o->setBounds(area.removeFromLeft((int)step).reduced(pad));
			}
			else{
				o->setVisible(false);
			}
		}
	}
	else{
		
		float step = area.getHeight()/ (float)outputBusUIs.size();
		const int pad = 3;
		for(auto & o:outputBusUIs){
			o->setVisible(true);
			o->setBounds(area.removeFromTop((int)step).reduced(pad));
			o->setAllVisible();
		}
	}
}

AudioMixerNodeUI::~AudioMixerNodeUI() {
	mixerNode->removeConnectableNodeListener(this);
	mixerNode->oneToOne->removeParameterListener(this);
}

void AudioMixerNodeUI::init() {
	mixerNode = (AudioMixerNode*)node.get();
	postCommandMessage(0);
	mixerNode->addConnectableNodeListener(this);
	mixerNode->oneToOne->addParameterListener(this);
  setDefaultSize(250,150);

	
}
void AudioMixerNodeUI::handleCommandMessage(int /*commandId*/){
	numAudioOutputChangedUI(mixerNode, mixerNode->numberOfOutput->intValue());
	numAudioInputChangedUI(mixerNode, mixerNode->numberOfInput->intValue());
	
}
void AudioMixerNodeUI::numAudioInputChanged(ConnectableNode * c , int /*numInput*/){
	mixerNode = (AudioMixerNode*)c;
	postCommandMessage(0);
}
void AudioMixerNodeUI::numAudioOutputChanged(ConnectableNode * c , int /*numInput*/){
		mixerNode = (AudioMixerNode*)c;
	postCommandMessage(0);
}

void AudioMixerNodeUI::numAudioInputChangedUI(ConnectableNode * c , int numInput){
  if(c->getAudioProcessor()->getTotalNumOutputChannels()!=outputBusUIs.size()){
    numAudioOutputChanged(c, c->getAudioProcessor()->getTotalNumOutputChannels());
  }
	int i =0;
	for(auto & b:outputBusUIs){
		b->setNumInput(numInput);
		i++;
	}
	resized();
};
void AudioMixerNodeUI::numAudioOutputChangedUI(ConnectableNode *, int newNum){
	int lastNum = outputBusUIs.size();
newNum=	mixerNode->outBuses.size();
	if(newNum>lastNum){
		for(int i = lastNum ; i < newNum ; i++){
			OutputBusUI * oo =new OutputBusUI(mixerNode->outBuses[i]);
			outputBusUIs.add(oo);
			addAndMakeVisible(oo);
		}
	}
	else if(newNum<lastNum){
		outputBusUIs.removeRange(newNum, lastNum-newNum);
	}
	resized();
	
};


void AudioMixerNodeUI::parameterValueChanged(Parameter * p) {
	if(p==mixerNode->oneToOne)postCommandMessage(0);
};





//==============================================================================
// OutputBusUI


void AudioMixerNodeUI::OutputBusUI::setOneVisible(int num){
	visibleChanels.clear();
	visibleChanels.setBit(num);
	updateVisibleChannels();
}

void AudioMixerNodeUI::OutputBusUI::setAllVisible(){
	visibleChanels.setRange(0, inputVolumes.size(), true);
	updateVisibleChannels();
}

int AudioMixerNodeUI::OutputBusUI::getNumOfVisibleChannels(){
	return visibleChanels.countNumberOfSetBits();
}
void AudioMixerNodeUI::OutputBusUI::updateVisibleChannels(){
	for(int i = 0 ; i < inputVolumes.size() ; i++){
		inputVolumes.getUnchecked(i)->setVisible(visibleChanels[i]);
	}
	postCommandMessage(0);
	
}

void AudioMixerNodeUI::OutputBusUI::setNumInput(int numInput){
	
	int lastSize = inputVolumes.size();
	if(numInput>lastSize){
		for(int  i = lastSize ; i < numInput;i++ ){
			FloatSliderUI *v = new FloatSliderUI(owner->volumes[i]);
			v->orientation = FloatSliderUI::Direction::VERTICAL;
			inputVolumes.add(v);
			addAndMakeVisible(v);
			visibleChanels.setBit(i);
			
		}
		
	}
	else if(numInput<lastSize){
		inputVolumes.removeLast(lastSize-numInput,true);
    visibleChanels.setRange(numInput, lastSize - numInput, false);

	}
	updateVisibleChannels();

	
}

void AudioMixerNodeUI::OutputBusUI::handleCommandMessage(int /*id*/){
  resized();
	repaint();
}


void AudioMixerNodeUI::OutputBusUI::resized() {
	if(inputVolumes.size()==0)return;
	Rectangle<int> area = getLocalBounds();
	int numToBeDisplayed = getNumOfVisibleChannels();
	float step = area.getWidth()*1.0f/numToBeDisplayed;
	const int pad = 2;
	int idx = 0;
	for(auto & o:inputVolumes){
    if(visibleChanels[idx]){
			o->setBounds(area.removeFromLeft((int)step).reduced(pad));
    }
		idx++;
	}
}
