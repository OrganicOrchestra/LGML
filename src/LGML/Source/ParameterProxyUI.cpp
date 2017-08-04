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
	chooser(proxy->getRoot())

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


	Rectangle<int> paramR = r;
  if (linkedParamUI != nullptr){
    linkedParamUI->setBounds(paramR);

  }
  else{chooser.setBounds(paramR);}

}

void ParameterProxyUI::setLinkedParamUI(Parameter * p)
{
//	if (p == nullptr) return;

	if (linkedParamUI != nullptr)
	{
//    auto * cUI = dynamic_cast<ParameterUI*>(linkedParamUI->ownedControllableUI.get());
    auto * cUI = dynamic_cast<ParameterUI*>(linkedParamUI.get());
		if (cUI && p && cUI->parameter == p) return;

		removeChildComponent(linkedParamUI);
		linkedParamUI = nullptr;
	}

//  linkedParamUI = p?new NamedControllableUI(ParameterUIFactory::createDefaultUI(p),100,true):nullptr;
  linkedParamUI = p?ParameterUIFactory::createDefaultUI(p):nullptr;


	if (linkedParamUI != nullptr)
	{
		addAndMakeVisible(linkedParamUI);
		updateTooltip();
    auto * cUI = dynamic_cast<ParameterUI*>(linkedParamUI.get());
		cUI->setTooltip(getTooltip());
	}
  chooser.setVisible(linkedParamUI==nullptr);
	resized();
}



void ParameterProxyUI::linkedParamChanged(ParameterProxy * p)
{
  jassert(p==paramProxy);
  if(!MessageManager::getInstance()->isThisTheMessageThread()){
    MessageManager::getInstance()->callAsync([this,p](){linkedParamChanged(p);});
  }
  else{
	setLinkedParamUI(p->linkedParam);
  }
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
  if(auto * cUI = dynamic_cast<ParameterUI*>(linkedParamUI.get()))
		cUI->setTooltip(getTooltip());
	
}
