/*
  ==============================================================================

    FastMapUI.cpp
    Created: 17 May 2016 6:05:44pm
    Author:  bkupe

  ==============================================================================
*/

#include "FastMapUI.h"

FastMapUI::
FastMapUI(FastMap * f) :
	fastMap(f)
{
	fastMap->addFastMapListener(this);
	chooseTargetBT.addControllableChooserListener(this);

	refUI = new ControlVariableReferenceUI(f->reference);
	minInputUI = fastMap->minInputVal->createSlider();
	maxInputUI = fastMap->maxInputVal->createSlider();
	minOutputUI = fastMap->minOutputVal->createSlider();
	maxOutputUI = fastMap->maxOutputVal->createSlider();
	
	addAndMakeVisible(refUI);
	addAndMakeVisible(chooseTargetBT);
	addAndMakeVisible(minInputUI);
	addAndMakeVisible(maxInputUI);
	addAndMakeVisible(minOutputUI);
	addAndMakeVisible(maxOutputUI);

	setSize(100, 40);
}

FastMapUI::~FastMapUI()
{
	fastMap->removeFastMapListener(this);
}

void FastMapUI::resized()
{
	Rectangle<int> r = getLocalBounds();
	refUI->setBounds(r.removeFromLeft((int)(getWidth()*.3f)));
	chooseTargetBT.setBounds(r.removeFromRight((int)(getWidth()*.2f)));
	
	Rectangle<int> inR = r.removeFromLeft((int)(getWidth()*.2f));
	minInputUI->setBounds(inR.removeFromTop((int)(getHeight()*.4f)));
	maxInputUI->setBounds(inR.removeFromBottom((int)(getHeight()*.4f)));

	Rectangle<int> outR = r.removeFromRight((int)(getWidth()*.2f));
	minOutputUI->setBounds(outR.removeFromTop((int)(getHeight()*.4f)));
	maxOutputUI->setBounds(outR.removeFromBottom((int)(getHeight()*.4f)));

}

void FastMapUI::choosedControllableChanged(Controllable * c)
{
	fastMap->setTarget(c);

	if (c != nullptr)
	{
		minOutputUI->setVisible(c->type != Controllable::TRIGGER && c->type != Controllable::BOOL);
		maxOutputUI->setVisible(c->type != Controllable::TRIGGER && c->type != Controllable::BOOL);
	}
}

 
