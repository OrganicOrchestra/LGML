/*
  ==============================================================================

    AudioMixerNode.cpp
    Created: 3 Mar 2016 10:14:46am
    Author:  bkupe

  ==============================================================================
*/

#include "AudioMixerNode.h"
#include "AudioMixerNodeUI.h"

NodeBaseUI * AudioMixerNode::createUI()
{

	AudioMixerNodeUI * ui = new AudioMixerNodeUI(this);
	return ui;

}