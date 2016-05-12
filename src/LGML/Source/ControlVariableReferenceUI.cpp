/*
  ==============================================================================

    ControlVariableReferenceUI.cpp
    Created: 12 May 2016 4:05:58pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControlVariableReferenceUI.h"
#include "ControllerManagerUI.h"
#include "DebugHelpers.h"

ControlVariableReferenceUI::ControlVariableReferenceUI(ControlVariableReference * _cvr) :
	cvr(_cvr),
	chooseBT("Choose")
{
	aliasUI = cvr->alias->createStringParameterUI();

	addAndMakeVisible(aliasUI);
	addAndMakeVisible(&chooseBT);
	addAndMakeVisible(&referenceLabel);
	referenceLabel.setColour(referenceLabel.textColourId, TEXTNAME_COLOR);

}

ControlVariableReferenceUI::~ControlVariableReferenceUI()
{
}

void ControlVariableReferenceUI::buttonClicked(Button * b)
{
	if (b == &chooseBT)
	{
		ControllableContainerPopupMenu chooser(ControllerManager::getInstance());
		int result = chooser.show();
		NLOG("CVR",String(result));
	}
}

void ControlVariableReferenceUI::resized()
{
	Rectangle<int> r = getLocalBounds();
	chooseBT.setBounds(r.removeFromLeft(40));
	r.removeFromLeft(2);
	referenceLabel.setBounds(r.removeFromLeft(50));
	r.removeFromLeft(10);
	aliasUI->setBounds(r);
}
