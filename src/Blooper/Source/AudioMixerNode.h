/*
  ==============================================================================

    AudioMixerNode.h
    Created: 3 Mar 2016 10:14:46am
    Author:  bkupe

  ==============================================================================
*/

#ifndef AUDIOMIXERNODE_H_INCLUDED
#define AUDIOMIXERNODE_H_INCLUDED


#include "NodeBase.h"
class NodeBaseUI;

class AudioMixerNode : public NodeBase
{

public:
	AudioMixerNode(uint32 nodeId) :NodeBase(nodeId) {}


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioMixerNode)
};

#endif  // AUDIOMIXERNODE_H_INCLUDED
