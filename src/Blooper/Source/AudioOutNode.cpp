/*
  ==============================================================================

    AudioOutNode.cpp
    Created: 7 Mar 2016 8:04:38pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "AudioOutNode.h"

#include "DummyNodeUI.h"


NodeBaseUI * AudioOutNode::createUI() {
	DummyNodeUI* ui = new DummyNodeUI(this);
    return ui;
    
}