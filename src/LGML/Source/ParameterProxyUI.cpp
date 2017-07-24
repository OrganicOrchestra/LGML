/*
  ==============================================================================

    ParameterProxyUI.cpp
    Created: 31 May 2016 4:40:08pm
    Author:  bkupe

  ==============================================================================
*/

#include "ParameterProxyUI.h"
#include "StringParameterUI.h"
#include "ControllableContainer.h"

ParameterProxyUI::ParameterProxyUI(ParameterProxy * proxy) :
	linkedParamUI(nullptr),
	ParameterUI(proxy),
	paramProxy(proxy),
	chooser(proxy->parentContainer->parentContainer)
{
	chooser.addControllableReferenceUIListener(this);
	addAndMakeVisible(&chooser);
	paramProxy->addParameterProxyListener(this);

	setLinkedParamUI(paramProxy->linkedParam);

	aliasParam = paramProxy->proxyAlias.createStringParameterUI();
	addAndMakeVisible(aliasParam);

	Image removeImage = ImageCache::getFromMemory(BinaryData::removeBT_png, BinaryData::removeBT_pngSize);

	removeBT.setImages(false, true, true, removeImage,
		0.7f, Colours::transparentBlack,
		removeImage, 1.0f, Colours::transparentBlack,
		removeImage, 1.0f, Colours::white.withAlpha(.7f),
		0.5f);
	removeBT.addListener(this);

	addAndMakeVisible(&removeBT);
}

ParameterProxyUI::~ParameterProxyUI()
{
	paramProxy->removeParameterProxyListener(this);
	setLinkedParamUI(nullptr);
}

void ParameterProxyUI::resized()
{
	Rectangle<int> r = getLocalBounds();
	if (r.getWidth() == 0 || r.getHeight() == 0) return;

	removeBT.setBounds(r.removeFromRight(r.getHeight()));
	Rectangle<int> paramR = r.removeFromRight(50);
	if (linkedParamUI != nullptr) linkedParamUI->setBounds(paramR);
	r.removeFromRight(5);
	chooser.setBounds(r.removeFromLeft(50));
	aliasParam->setBounds(r);
}

void ParameterProxyUI::setLinkedParamUI(Parameter * p)
{
	if (p == nullptr) return;

	if (linkedParamUI != nullptr)
	{
		if (linkedParamUI->parameter == p) return;

		removeChildComponent(linkedParamUI);
		linkedParamUI = nullptr;
	}

	linkedParamUI = dynamic_cast<ParameterUI *>(p->createDefaultUI(paramProxy));

	if (linkedParamUI != nullptr)
	{
		addAndMakeVisible(linkedParamUI);
		updateTooltip();
		linkedParamUI->setTooltip(getTooltip());
	}

	resized();
}

void ParameterProxyUI::buttonClicked(Button * b)
{
	if (b == &removeBT)
	{
		paramProxy->remove();
	}
}

void ParameterProxyUI::linkedParamChanged(Parameter * p)
{
	setLinkedParamUI(p);
}

void ParameterProxyUI::choosedControllableChanged(ControllableReferenceUI*,Controllable * c)
{
	paramProxy->setLinkedParam(dynamic_cast<Parameter *>(c));
}

void ParameterProxyUI::controllableNameChanged(Controllable * c)
{
	ParameterUI::controllableNameChanged(c);
	updateTooltip();
	if(linkedParamUI)linkedParamUI->setTooltip(getTooltip());
}
