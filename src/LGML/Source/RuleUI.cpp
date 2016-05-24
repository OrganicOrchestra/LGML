/*
  ==============================================================================

    RuleUI.cpp
    Created: 4 May 2016 5:07:18pm
    Author:  bkupe

  ==============================================================================
*/

#include "RuleUI.h"
#include "Style.h"
#include "MainComponent.h"
#include "RuleEditor.h"

RuleUI::RuleUI(Rule * _rule) :
	InspectableComponent( _rule, "rule"),
	rule(_rule)
{
	rule->addRuleListener(this);

	enabledBT = rule->enabledParam->createToggle();
	nameTF = rule->nameParam->createStringParameterUI();

	addAndMakeVisible(enabledBT);
	addAndMakeVisible(nameTF);

	Image removeImage = ImageCache::getFromMemory(BinaryData::removeBT_png, BinaryData::removeBT_pngSize);

	removeBT.setImages(false, true, true, removeImage,
		0.7f, Colours::transparentBlack,
		removeImage, 1.0f, Colours::transparentBlack,
		removeImage, 1.0f, Colours::white.withAlpha(.7f),
		0.5f);
	removeBT.addListener(this);

	addAndMakeVisible(&removeBT);

	addMouseListener(this, true);
}

RuleUI::~RuleUI()
{
	rule->removeRuleListener(this);
}

void RuleUI::paint(Graphics & g)
{
	Rectangle<int> r = getLocalBounds();

	g.setColour(NORMAL_COLOR.darker());
	g.fillRoundedRectangle(r.toFloat(), 2);

	if (isSelected)
	{
		g.setColour(HIGHLIGHT_COLOR);
		g.drawRoundedRectangle(r.toFloat(), 2, 2);
	}

	g.setColour(rule->isActiveParam->boolValue() ? (rule->enabledParam->boolValue()?Colours::lightgreen:Colours::yellow) : NORMAL_COLOR);
	g.fillEllipse(r.removeFromRight(r.getHeight()).reduced(4).toFloat());
}

void RuleUI::resized()
{
	Rectangle<int> r = getLocalBounds().reduced(2);
	r.removeFromRight(15);
	removeBT.setBounds(r.removeFromRight(20));
	enabledBT->setBounds(r.removeFromLeft(r.getHeight()));
	r.removeFromLeft(5);
	nameTF->setBounds(r);
}

void RuleUI::mouseDown(const MouseEvent &)
{
	selectThis();
}

void RuleUI::buttonClicked(Button * b)
{
	if (b == &removeBT)
	{
		rule->remove();
	}
}

bool RuleUI::keyPressed(const KeyPress & key)
{
	if (!isSelected) return false;

	if (key.getKeyCode() == KeyPress::deleteKey || key.getKeyCode() == KeyPress::backspaceKey)
	{
		rule->remove();
		return true;
	}

	return false;
}

void RuleUI::ruleActivationChanged(Rule *)
{
	repaint();
}

InspectorEditor * RuleUI::getEditor()
{
	return new RuleEditor(this);
}
