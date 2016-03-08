/*
  ==============================================================================

    AudioMixerNode.cpp
    Created: 3 Mar 2016 10:14:46am
    Author:  bkupe

  ==============================================================================
*/

#include "AudioMixerNode.h"
#include "NodeBaseUI.h"

NodeBaseUI * AudioMixerNode::createUI()
{

	NodeBaseUI * ui = new NodeBaseUI(this);
	return ui;

}