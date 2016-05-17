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

void ControlVariableReferenceUI::setAliasVisible(bool value)
{
	aliasUI->setVisible(value);
	resized();
}

void ControlVariableReferenceUI::setRemoveBTVisible(bool value)
{
	removeBT.setVisible(value);
	resized();
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
		chooseBT.setButtonText(cvr->currentVariable->parameter->niceName);
		currentVariableParamUI = (ParameterUI *)cvr->currentVariable->parameter->createDefaultUI();
		addAndMakeVisible(currentVariableParamUI);
	}
	else
	{
		chooseBT.setButtonText("[Source]");
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

	if (removeBT.isVisible()) removeBT.setBounds(r.removeFromRight(r.getHeight()).reduced(2));
	r.removeFromRight(2); 
	if (aliasUI->isVisible()) aliasUI->setBounds(r.removeFromRight(r.getWidth()-100));
	chooseBT.setBounds(r);	
}

void ControlVariableReferenceUI::referenceVariableChanged(ControlVariableReference *)
{
	updateCurrentReference();
}

void ControlVariableReferenceUI::referenceVariableNameChanged(ControlVariableReference *)
{
	chooseBT.setButtonText(cvr->currentVariable->parameter->niceName);
}

void ControlVariableReferenceUI::parameterValueChanged(Parameter *)
{
	resized();
}
