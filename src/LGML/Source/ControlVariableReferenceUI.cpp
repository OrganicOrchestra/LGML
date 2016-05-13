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

	aliasUI->parameter->addParameterListener(this);

	addAndMakeVisible(aliasUI);
	addAndMakeVisible(&chooseBT);
	addAndMakeVisible(&referenceLabel);
	referenceLabel.setColour(referenceLabel.textColourId, TEXTNAME_COLOR);

	aliasUI->setNameLabelVisible(false);
	chooseBT.addListener(this);

	cvr->addReferenceListener(this);

	Image removeImage = ImageCache::getFromMemory(BinaryData::removeBT_png, BinaryData::removeBT_pngSize);

	removeBT.setImages(false, true, true, removeImage,
		0.7f, Colours::transparentBlack,
		removeImage, 1.0f, Colours::transparentBlack,
		removeImage, 1.0f, Colours::pink.withAlpha(0.8f),
		0.5f);
	removeBT.addListener(this);

	addAndMakeVisible(&removeBT);

	updateCurrentReference();
}

ControlVariableReferenceUI::~ControlVariableReferenceUI()
{
	aliasUI->parameter->removeParameterListener(this);
	cvr->removeReferenceListener(this);
}

void ControlVariableReferenceUI::updateCurrentReference()
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
		referenceLabel.setText("[None]", NotificationType::dontSendNotification);
	}

	resized();
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
	else if (b == &removeBT)
	{
		cvr->remove();
	}
}

void ControlVariableReferenceUI::resized()
{
	Rectangle<int> r = getLocalBounds();
	
	Rectangle<int> rUI = r.removeFromBottom(10);
	if (currentVariableParamUI != nullptr) currentVariableParamUI->setBounds(rUI);

	removeBT.setBounds(r.removeFromRight(r.getHeight()).reduced(2));
	chooseBT.setBounds(r.removeFromLeft(40));
	r.removeFromLeft(2);
	aliasUI->setBounds(r.removeFromRight(100));
	r.reduce(5, 0);
	referenceLabel.setBounds(r);
}

void ControlVariableReferenceUI::currentReferenceChanged(ControlVariableReference *)
{
	updateCurrentReference();
}

void ControlVariableReferenceUI::parameterValueChanged(Parameter *)
{
	DBG("Resized here !");
	resized();
}
