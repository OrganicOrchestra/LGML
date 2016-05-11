/*
  ==============================================================================

    GenericControllableContainerEditor.cpp
    Created: 9 May 2016 6:41:59pm
    Author:  bkupe

  ==============================================================================
*/

#include "GenericControllableContainerEditor.h"
#include "InspectableComponent.h"
#include "ControllableUI.h"

GenericControllableContainerEditor::GenericControllableContainerEditor(InspectableComponent * sourceComponent) :
	InspectorEditor(sourceComponent)
{
	sourceContainer = sourceComponent->relatedControllableContainer;

	innerContainer = new CCInnerContainer(sourceContainer, 0, sourceComponent->recursiveInspectionLevel);
	addAndMakeVisible(innerContainer);

	resized();
}

GenericControllableContainerEditor::~GenericControllableContainerEditor()
{
	innerContainer->clear();
}

void GenericControllableContainerEditor::resized()
{
	innerContainer->setBounds(getLocalBounds());
}

void GenericControllableContainerEditor::clear()
{
	innerContainer->clear();
}





//Inner Container

CCInnerContainer::CCInnerContainer(ControllableContainer * _container, int _level, int _maxLevel) :
	container(_container),
	level(_level),
	maxLevel(_maxLevel),
	containerLabel("containerLabel",_container->niceName)
{
	container->addControllableContainerListener(this);

	addAndMakeVisible(containerLabel);
	containerLabel.setFont(containerLabel.getFont().withHeight(10));
	containerLabel.setColour(containerLabel.backgroundColourId,BG_COLOR.brighter(.2f));
	containerLabel.setSize(containerLabel.getFont().getStringWidth(containerLabel.getText()) + 10,14);
	containerLabel.setColour(containerLabel.textColourId, TEXTNAME_COLOR);

	for (auto &c : container->controllables)
	{
		addControllableUI(c);
	}

	if (level < maxLevel)
	{
		for (auto &cc : container->controllableContainers)
		{
			addCCInnerUI(cc);
		}
	}	
}

CCInnerContainer::~CCInnerContainer()
{
	container->removeControllableContainerListener(this);
	clear();
}

void CCInnerContainer::addCCInnerUI(ControllableContainer * cc)
{
	DBG("Add CC Inner UI !");
	CCInnerContainer * ccui = new CCInnerContainer(cc, level + 1, maxLevel);
	innerContainers.add(ccui);
	addAndMakeVisible(ccui);
}

void CCInnerContainer::removeCCInnerUI(ControllableContainer * cc)
{
	CCInnerContainer * ccui = getInnerContainerForCC(cc);
	if (ccui == nullptr) return;

	removeChildComponent(ccui);
	innerContainers.removeObject(ccui);
}



void CCInnerContainer::addControllableUI(Controllable * c)
{
	if (c->isControllableFeedbackOnly || !c->isControllableExposed) return;

	ControllableUI * cui = c->createDefaultUI();
	controllablesUI.add(cui);
	addAndMakeVisible(cui);
}

void CCInnerContainer::removeControllableUI(Controllable * c)
{
	ControllableUI * cui = getUIForControllable(c);
	if (cui == nullptr) return;

	removeChildComponent(cui);
	controllablesUI.removeObject(cui);
	
}

ControllableUI * CCInnerContainer::getUIForControllable(Controllable * c)
{
	for (auto &cui : controllablesUI)
	{
		if (cui->controllable == c) return cui;
	}

	return nullptr;
}

CCInnerContainer * CCInnerContainer::getInnerContainerForCC(ControllableContainer * cc)
{
	for (auto &ccui : innerContainers)
	{
		if (ccui->container == cc) return ccui;
	}

	return nullptr;
}

int CCInnerContainer::getContentHeight()
{
	int gap = 2;
	int ccGap = 5;
	int controllableHeight = 15;
	int margin = 5;
	int h = controllablesUI.size() * (controllableHeight + gap);
	for (auto &ccui : innerContainers) h += ccui->getContentHeight() + ccGap;

	h += containerLabel.getHeight();
	h += margin * 2;

	return h;
}

void CCInnerContainer::paint(Graphics & g)
{
	//if (level == 0) return;
	g.setColour(BG_COLOR.brighter(.3f));
	g.drawRoundedRectangle(getLocalBounds().toFloat(),4,2);
}

void CCInnerContainer::resized()
{
	int gap = 2;
	int ccGap = 5;
	int controllableHeight = 15;
	int margin = 5;

	Rectangle<int> r = getLocalBounds();
	containerLabel.setBounds(r.removeFromTop(containerLabel.getHeight()).withSizeKeepingCentre(containerLabel.getWidth(), containerLabel.getHeight()));

	r.reduce(margin, margin);

	for (auto &cui : controllablesUI)
	{
		cui->setBounds(r.removeFromTop(controllableHeight));
		r.removeFromTop(gap);
	}
	r.removeFromTop(ccGap);

	for (auto &ccui : innerContainers)
	{
		ccui->setBounds(r.removeFromTop(ccui->getContentHeight()));
		r.removeFromTop(ccGap);
	}
}

void CCInnerContainer::clear()
{
	controllablesUI.clear();
	innerContainers.clear();
}

void CCInnerContainer::controllableAdded(Controllable * c)
{
	if (c->parentContainer != container) return;

	addControllableUI(c);
}

void CCInnerContainer::controllableRemoved(Controllable * c)
{
	removeControllableUI(c);
}

void CCInnerContainer::controllableContainerAdded(ControllableContainer * cc)
{
	if (level >= maxLevel) return;
	if (cc->parentContainer != container) return;
	
	addCCInnerUI(cc);

}

void CCInnerContainer::controllableContainerRemoved(ControllableContainer * cc)
{
	removeCCInnerUI(cc);
}


void CCInnerContainer::childStructureChanged(ControllableContainer *)
{
	resized();
}
