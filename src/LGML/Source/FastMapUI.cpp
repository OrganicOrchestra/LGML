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
	refUI->setAliasVisible(false);
	refUI->setRemoveBTVisible(false);

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

	Image removeImage = ImageCache::getFromMemory(BinaryData::removeBT_png, BinaryData::removeBT_pngSize);

	removeBT.setImages(false, true, true, removeImage,
		0.7f, Colours::transparentBlack,
		removeImage, 1.0f, Colours::transparentBlack,
		removeImage, 1.0f, Colours::pink.withAlpha(0.8f),
		0.5f);
	removeBT.addListener(this);

	addAndMakeVisible(&removeBT);

	setSize(100, 40);
}

FastMapUI::~FastMapUI()
{
	fastMap->removeFastMapListener(this);
}

void FastMapUI::paint(Graphics & g)
{
	g.setColour(PANEL_COLOR);
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 2);
}

void FastMapUI::resized()
{
	Rectangle<int> r = getLocalBounds().reduced(3);
	removeBT.setBounds(r.removeFromRight(r.getHeight()).reduced(4));

	refUI->setBounds(r.removeFromLeft((int)(getWidth()*.3f)));
	chooseTargetBT.setBounds(r.removeFromRight((int)(getWidth()*.2f)).reduced(2,1));
	
	Rectangle<int> inR = r.removeFromLeft((int)(getWidth()*.2f));
	minInputUI->setBounds(inR.removeFromTop((int)(getHeight()*.35f)));
	maxInputUI->setBounds(inR.removeFromBottom((int)(getHeight()*.35f)));

	Rectangle<int> outR = r.removeFromRight((int)(getWidth()*.2f));
	minOutputUI->setBounds(outR.removeFromTop((int)(getHeight()*.35f)));
	maxOutputUI->setBounds(outR.removeFromBottom((int)(getHeight()*.35f)));

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

void FastMapUI::buttonClicked(Button * b)
{
	if(b == &removeBT) fastMap->remove();
}

 
