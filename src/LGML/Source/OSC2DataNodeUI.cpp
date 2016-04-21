/*
  ==============================================================================

    OSC2DataNodeUI.cpp
    Created: 19 Apr 2016 10:39:16am
    Author:  bkupe

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "OSC2DataNodeUI.h"
#include "FloatSliderUI.h"

//==============================================================================
OSC2DataNodeContentUI::OSC2DataNodeContentUI()
{
    
}

OSC2DataNodeContentUI::~OSC2DataNodeContentUI()
{
	o2dNode->removeO2DListener(this);
}

void OSC2DataNodeContentUI::paint(Graphics& g)
{
	
}

void OSC2DataNodeContentUI::resized()
{
	Rectangle<int> r = getLocalBounds().reduced(5);
	for (auto &pui : parameterUIs)
	{
		pui->setBounds(r.removeFromTop(20));
		r.removeFromTop(10);
	}
}

void OSC2DataNodeContentUI::init()
{
	setSize(150, 200);

	o2dNode = (OSC2DataNode *)node;
	o2dNode->addO2DListener(this);

	for (auto &p : o2dNode->dynamicParameters)
	{
		addUIForParameter(p);
	}
}

void OSC2DataNodeContentUI::addUIForParameter(Parameter * p)
{
	switch (p->type)
	{
	case Parameter::Type::FLOAT:
		addParameterUI(((FloatParameter *)p)->createSlider());
		break;
	}
	
}

void OSC2DataNodeContentUI::addParameterUI(ParameterUI * pui)
{
	parameterUIs.add(pui);
	addAndMakeVisible(pui);
	DBG("Add Parameter UI : "+getLocalBounds().toString());
	setBounds(getLocalBounds().withHeight(parameterUIs.size() * 30));
}

void OSC2DataNodeContentUI::parameterAdded(Parameter * p)
{
	addUIForParameter(p);
}

void OSC2DataNodeContentUI::parameterRemoved(Parameter * p)
{
	ParameterUI * pui = getUIForParameter(p);
	if (pui == nullptr) return;
	parameterUIs.removeObject(pui);
	resized();
}

