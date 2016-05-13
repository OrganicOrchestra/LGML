/*
  ==============================================================================

    ControlVariableReferenceUI.cpp
    Created: 12 May 2016 4:05:58pm
    Author:  bkupe

  ==============================================================================
*/

#include "ControlVariableReferenceUI.h"
#include "ControllerManagerUI.h"
#include "ControllerUIHelpers.h"

ControlVariableReferenceUI::ControlVariableReferenceUI(ControlVariableReference * _cvr) :
	cvr(_cvr),
	chooseBT("Choose"),
	currentVariableParamUI(nullptr)
{
	aliasUI = cvr->alias->createStringParameterUI();

	addAndMakeVisible(aliasUI);
	addAndMakeVisible(&chooseBT);
	addAndMakeVisible(&referenceLabel);
	referenceLabel.setColour(referenceLabel.textColourId, TEXTNAME_COLOR);

	aliasUI->setNameLabelVisible(false);
	chooseBT.addListener(this);

	cvr->addReferenceListener(this);
}

ControlVariableReferenceUI::~ControlVariableReferenceUI()
{
	cvr->removeReferenceListener(this);
}

void ControlVariableReferenceUI::buttonClicked(Button * b)
{
	if (b == &chooseBT)
	{
		ControlVariableChooser chooser;
		ControlVariable * cv = chooser.showAndGetVariable();
		if (cv != nullptr)
		{
			cvr->setCurrentVariable(cv);
		}
		
	}
}

void ControlVariableReferenceUI::resized()
{
	Rectangle<int> r = getLocalBounds();
	
	Rectangle<int> rUI = r.removeFromBottom(10);
	if (currentVariableParamUI != nullptr) currentVariableParamUI->setBounds(rUI);

	chooseBT.setBounds(r.removeFromLeft(40));
	r.removeFromLeft(2);
	aliasUI->setBounds(r.removeFromRight(50));
	r.reduce(5, 0);
	referenceLabel.setBounds(r);


}

void ControlVariableReferenceUI::currentReferenceChanged(ControlVariableReference *)
{
	if (currentVariableParamUI != nullptr)
	{
		removeChildComponent(currentVariableParamUI);
		currentVariableParamUI = nullptr;
	}

	if (cvr->currentVariable != nullptr)
	{
		referenceLabel.setText(cvr->currentVariable->parameter->niceName, NotificationType::dontSendNotification);
		currentVariableParamUI = (ParameterUI *)cvr->currentVariable->parameter->createDefaultUI();
		addAndMakeVisible(currentVariableParamUI);
	}
	else
	{
		referenceLabel.setText("[None]",NotificationType::dontSendNotification);
	}

	resized();
}
