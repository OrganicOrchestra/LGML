/*
  ==============================================================================

    ShapeShifterContainer.cpp
    Created: 2 May 2016 3:11:25pm
    Author:  bkupe

  ==============================================================================
*/

#include "ShapeShifterContainer.h"
#include "Style.h"
#include "ShapeShifterManager.h"


ShapeShifterContainer::ShapeShifterContainer(Direction _direction) : 
	ShapeShifter(ShapeShifter::CONTAINER),
	direction(_direction)
{

}

ShapeShifterContainer::~ShapeShifterContainer()
{
}

void ShapeShifterContainer::insertPanelAt(ShapeShifterPanel * panel, int index)
{
	shifters.insert(index, panel);
	addAndMakeVisible(panel);
	panel->setParentContainer(this);
	panel->addShapeShifterPanelListener(this);

	if (shifters.size() > 1)
	{
		GapGrabber * gg = new GapGrabber(direction == HORIZONTAL ? GapGrabber::HORIZONTAL : GapGrabber::VERTICAL);
		grabbers.add(gg);
		addAndMakeVisible(gg);
		gg->addGrabberListener(this);
	}

	resized();
}

void ShapeShifterContainer::insertPanelRelative(ShapeShifterPanel * panel, ShapeShifterPanel * relativeTo, ShapeShifterPanel::AttachZone zone)
{
	//TODO check direction and create sub container if needed
	switch (zone)
	{
	case  ShapeShifterPanel::AttachZone::LEFT:
		if (direction == HORIZONTAL) insertPanelAt(panel, shifters.indexOf(relativeTo));
		else movePanelsInContainer(panel, relativeTo, HORIZONTAL,false);
	break;

	case  ShapeShifterPanel::AttachZone::RIGHT:
		if (direction == HORIZONTAL) insertPanelAt(panel, shifters.indexOf(relativeTo)+1);
		else movePanelsInContainer(panel, relativeTo, HORIZONTAL,true);
	break; 
	
	case  ShapeShifterPanel::AttachZone::TOP:
		if (direction == VERTICAL) insertPanelAt(panel, shifters.indexOf(relativeTo));
		else movePanelsInContainer(panel, relativeTo, VERTICAL,false);
	break; 
	
	case  ShapeShifterPanel::AttachZone::BOTTOM:
		if (direction == VERTICAL) insertPanelAt(panel, shifters.indexOf(relativeTo)+1);
		else movePanelsInContainer(panel, relativeTo, VERTICAL,true);
	break;

	}
}


void ShapeShifterContainer::removePanel(ShapeShifterPanel * panel, bool deletePanel,  bool silent)
{
	int shifterIndex = shifters.indexOf(panel);
	shifters.removeAllInstancesOf(panel);
	panel->setParentContainer(nullptr);
	panel->removeShapeShifterPanelListener(this);
	
	DBG("Remove panel, new shifter size : " << shifters.size());
	if (shifters.size() == 0)
	{
		//dispatch emptied container so parent container deletes it
		if(!silent) containerListeners.call(&ShapeShifterContainerListener::containerEmptied, this);
	}else if (shifters.size() == 1)
	{
		if(!silent) containerListeners.call(&ShapeShifterContainerListener::oneShifterRemaining, this,shifters[0]);
	}else
	{
		GapGrabber * gg = grabbers[(jmin<int>(shifterIndex, grabbers.size() - 1))];
		removeChildComponent(gg);
		grabbers.remove(grabbers.indexOf(gg), true);
	}

	removeChildComponent(panel);
	if (deletePanel) ShapeShifterManager::getInstance()->removePanel(panel);
	resized();
}

ShapeShifterContainer * ShapeShifterContainer::insertContainerAt(int index, Direction _direction)
{
	ShapeShifterContainer * ssc = new ShapeShifterContainer(_direction);
	return insertContainerAt(index, ssc);
}

ShapeShifterContainer * ShapeShifterContainer::insertContainerAt(int index, ShapeShifterContainer * container)
{
	shifters.insert(index, container);
	containers.add(container);
	container->addShapeShifterContainerListener(this);
	addAndMakeVisible(container);

	if (shifters.size() > 1)
	{
		GapGrabber * gg = new GapGrabber(direction == HORIZONTAL ? GapGrabber::HORIZONTAL : GapGrabber::VERTICAL);
		grabbers.add(gg);
		addAndMakeVisible(gg);
		gg->addGrabberListener(this);
	}

	resized();

	return container;
}

void ShapeShifterContainer::removeContainer(ShapeShifterContainer * container, bool silent)
{
	int shifterIndex = shifters.indexOf(container);
	shifters.removeAllInstancesOf(container);
	container->removeShapeShifterContainerListener(this);
	container->setParentContainer(nullptr);

	if (shifters.size() == 0)
	{
		if(!silent) containerListeners.call(&ShapeShifterContainerListener::containerEmptied, this);
	}
	else if (shifters.size() == 1)
	{
		if(!silent) containerListeners.call(&ShapeShifterContainerListener::oneShifterRemaining, this, shifters[0]);
	}else
	{
		GapGrabber * gg = grabbers.getUnchecked(jmin<int>(shifterIndex, grabbers.size() - 1));
		removeChildComponent(gg);
		grabbers.remove(grabbers.indexOf(gg), true);
	}

	removeChildComponent(container);
	resized();
}

void ShapeShifterContainer::movePanelsInContainer(ShapeShifterPanel * newPanel, ShapeShifterPanel * containedPanel, Direction _newDir, bool secondBeforeFirst)
{
	DBG("Move panels in container : " << containedPanel->currentContent->getName() << " / " << newPanel->currentContent->getName() << " newDir " << _newDir);
	int targetIndex = shifters.indexOf(containedPanel);
	removePanel(containedPanel,false,true);
	ShapeShifterContainer * newContainer = insertContainerAt(targetIndex, _newDir);
	newContainer->insertPanelAt(containedPanel, 0);
	newContainer->insertPanelAt(newPanel, secondBeforeFirst ? 1:0);
	
	DBG("Move panels in container end, shifters size = " << shifters.size());
}


void ShapeShifterContainer::resized()
{
	
	Rectangle<int> r = getLocalBounds();
	int gap = 6;
	int totalSpace = (direction == HORIZONTAL) ? r.getWidth() : r.getHeight();

	int numShifters = shifters.size();

	int numDefaultSpace = numShifters;
	int reservedPreferredSpace = 0;

	

	for (auto &p : shifters)
	{
		int tp = (direction == HORIZONTAL) ? p->preferredWidth : p->preferredHeight;
		if (tp >= 0)
		{
			numDefaultSpace--;
			reservedPreferredSpace += tp;
		}
	}

	int defaultSpace =  numDefaultSpace == 0?0:(totalSpace-reservedPreferredSpace) / numDefaultSpace - gap*(numShifters - 1);

	int panelIndex = 0;
	for (auto &p : shifters)
	{
		int tp = (direction == HORIZONTAL) ? p->preferredWidth : p->preferredHeight;
		int targetSpace = tp >= 0 ? tp : defaultSpace;

		Rectangle<int> tr = (direction == HORIZONTAL) ? r.removeFromLeft(targetSpace) : r.removeFromTop(targetSpace);
		p->setBounds(tr);

		if (panelIndex < grabbers.size())
		{
			Rectangle<int> gr = (direction == HORIZONTAL)?r.removeFromLeft(gap):r.removeFromTop(gap);
			grabbers[panelIndex]->setBounds(gr);
		}

		panelIndex++;
	}
}

void ShapeShifterContainer::grabberGrabUpdate(GapGrabber * gg, int dist)
{
	ShapeShifter * firstShifter = shifters[grabbers.indexOf(gg)];
	ShapeShifter * secondShifter = shifters[grabbers.indexOf(gg) + 1];

	switch (direction)
	{
	case HORIZONTAL:
		if (firstShifter->preferredWidth >= 0) firstShifter->setPreferredWidth(firstShifter->preferredWidth + dist);
			else secondShifter->setPreferredWidth(secondShifter->preferredWidth - dist);
			break;

	case VERTICAL:
		if (firstShifter->preferredHeight >= 0) firstShifter->setPreferredHeight(firstShifter->preferredHeight + dist);
		else secondShifter->setPreferredHeight(secondShifter->preferredHeight - dist);
		break;

        case NONE:
            //@ben what to we do for that?
			//@martin never gonna happen :)
            jassertfalse;
			break;
	}

	resized();
}


void ShapeShifterContainer::panelDetach(ShapeShifterPanel * panel)
{
	Rectangle<int> panelBounds = panel->getScreenBounds();
	removePanel(panel);
	ShapeShifterManager::getInstance()->showPanelWindow(panel, panelBounds);
}

void ShapeShifterContainer::panelRemoved(ShapeShifterPanel * panel)
{
	removePanel(panel);
}

void ShapeShifterContainer::containerEmptied(ShapeShifterContainer * container)
{
	DBG("Container emptied");
	removeContainer(container);
}

void ShapeShifterContainer::oneShifterRemaining(ShapeShifterContainer * container, ShapeShifter * lastShifter)
{
	DBG("One Shifter remaining ! " << (lastShifter->shifterType ==1?"Container":"Panel"));
	int containerIndex = shifters.indexOf(container);

	if (lastShifter->shifterType == CONTAINER)
	{
		container->removeContainer((ShapeShifterContainer *)lastShifter, true);
		insertContainerAt(containerIndex, (ShapeShifterContainer *)lastShifter);
		
		
	}
	else
	{
		container->removePanel((ShapeShifterPanel *)lastShifter, false, true);
		insertPanelAt((ShapeShifterPanel *)lastShifter, containerIndex);
	}
	
	removeContainer(container);
}


