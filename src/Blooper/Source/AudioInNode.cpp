/*
  ==============================================================================

    AudioInNode.cpp
    Created: 7 Mar 2016 8:03:48pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "AudioInNode.h"
#include "DummyNodeUI.h"


NodeBaseUI * AudioInNode::createUI() {
	DummyNodeUI* ui = new DummyNodeUI(this);
    return ui;
    
}