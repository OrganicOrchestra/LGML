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
#include "ParameterUIFactory.h"

ParameterProxyUI::ParameterProxyUI(ParameterProxy * proxy) :
  linkedParamUI(nullptr),
	ParameterUI(proxy),
	paramProxy(proxy),
	chooser(proxy->parentContainer->parentContainer)

{
  setMappingDest(true);
	chooser.addControllableReferenceUIListener(this);
	addAndMakeVisible(&chooser);
	paramProxy->addParameterProxyListener(this);

	setLinkedParamUI(paramProxy->linkedParam);
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


	Rectangle<int> paramR = r.removeFromRight(50);
  if (linkedParamUI != nullptr){ linkedParamUI->setBounds(paramR);}
  else{chooser.setBounds(paramR);}

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

  linkedParamUI = p?ParameterUIFactory::createDefaultUI(p):nullptr;


	if (linkedParamUI != nullptr)
	{
		addAndMakeVisible(linkedParamUI);
		updateTooltip();
		linkedParamUI->setTooltip(getTooltip());
	}
  chooser.setVisible(linkedParamUI!=nullptr);
	resized();
}



void ParameterProxyUI::linkedParamChanged(ParameterProxy * p)
{
  jassert(p==paramProxy);
	setLinkedParamUI(p->linkedParam);
}

void ParameterProxyUI::choosedControllableChanged(ControllableReferenceUI*,Controllable * c)
{
  auto t = c->getParameter();
  paramProxy->setParamToReferTo(t);


}

void ParameterProxyUI::controllableNameChanged(Controllable * c)
{
	ParameterUI::controllableNameChanged(c);
	updateTooltip();
	if(linkedParamUI)linkedParamUI->setTooltip(getTooltip());
}
