/*
  ==============================================================================

    NodeBase.cpp
    Created: 2 Mar 2016 8:36:17pm
    Author:  bkupe

  ==============================================================================
*/

#include "NodeBase.h"


NodeBase::NodeBase(NodeAudioProcessor * _audioProcessor, NodeDataProcessor * _dataProcessor) :
	audioProcessor(_audioProcessor),
	dataProcessor(_dataProcessor)
{
	hasAudioInputs = audioProcessor != nullptr?audioProcessor->getTotalNumInputChannels()>0:false;
	hasAudioOutputs = audioProcessor != nullptr ? audioProcessor->getTotalNumOutputChannels()>0:false;
	hasDataInputs = dataProcessor != nullptr ? dataProcessor->getTotalNumInputChannels()>0:false;
	hasDataOutputs = dataProcessor != nullptr ? dataProcessor->getTotalNumInputChannels()>0:false;


}