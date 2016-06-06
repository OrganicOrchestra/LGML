/*
  ==============================================================================

    ParameterProxyUI.cpp
    Created: 31 May 2016 4:40:08pm
    Author:  bkupe

  ==============================================================================
*/

#include "ParameterProxyUI.h"
#include "StringParameterUI.h"

ParameterProxyUI::ParameterProxyUI(ParameterProxy * proxy) :
	linkedParamUI(nullptr),
	ParameterUI(proxy),
	paramProxy(proxy),
	chooser(proxy->parentContainer)
{
	chooser.addControllableChooserListener(this);
	addAndMakeVisible(&chooser);
	paramProxy->addParameterProxyListener(this);

	setLinkedParamUI(paramProxy->linkedParam);

	aliasParam = paramProxy->proxyAlias.createStringParameterUI();
	addAndMakeVisible(aliasParam);
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
	Rectangle<int> firstLine = r.removeFromTop(20);
	chooser.setBounds(firstLine.removeFromLeft(50));
	firstLine.removeFromLeft(2);
	aliasParam->setBounds(firstLine);
	r.removeFromTop(2);
	if (linkedParamUI != nullptr) linkedParamUI->setBounds(r);
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

void ParameterProxyUI::linkedParamChanged(Parameter * p)
{
	setLinkedParamUI(p);
}

void ParameterProxyUI::choosedControllableChanged(Controllable * c)
{
	paramProxy->setLinkedParam(dynamic_cast<Parameter *>(c));
}

void ParameterProxyUI::controllableNameChanged(Controllable * c)
{
	ParameterUI::controllableNameChanged(c);
	updateTooltip();
	linkedParamUI->setTooltip(getTooltip());
}
