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
	DBG("Container destroy !");
	clear();
}

void ShapeShifterContainer::insertShifterAt(ShapeShifter * shifter, int index)
{
	shifters.insert(index, shifter);
	addAndMakeVisible(shifter);
	shifter->setParentContainer(this);
	
	if (shifters.size() > 1)
	{
		GapGrabber * gg = new GapGrabber(direction == HORIZONTAL ? GapGrabber::HORIZONTAL : GapGrabber::VERTICAL);
		grabbers.add(gg);
		addAndMakeVisible(gg);
		gg->addGrabberListener(this);
	}

	resized();
}

void ShapeShifterContainer::removeShifter(ShapeShifter * shifter, bool deleteShifter, bool silent)
{
	DBG("Remove Shifter, deleteShifter ? " << String(deleteShifter) << ", silent " << String(silent));
	int shifterIndex = shifters.indexOf(shifter);
	shifters.removeAllInstancesOf(shifter);
	shifter->setParentContainer(nullptr);

	int cIndex = getIndexOfChildComponent(shifter);
	DBG("REMOVE Shifter :: is in children ? " << cIndex);
	removeChildComponent(shifter);

	

	switch (shifter->shifterType)
	{
	case CONTAINER:
		((ShapeShifterContainer *)shifter)->removeShapeShifterContainerListener(this);
		if (deleteShifter) delete shifter;
		break;

	case PANEL:
		((ShapeShifterPanel *)shifter)->removeShapeShifterPanelListener(this);
		if (deleteShifter) ShapeShifterManager::getInstance()->removePanel(((ShapeShifterPanel *)shifter));
		break;
	}

	

	if (shifters.size() == 0)
	{
		//dispatch emptied container so parent container deletes it
		if (!silent) containerListeners.call(&ShapeShifterContainerListener::containerEmptied, this);
	}
	else if (shifters.size() == 1)
	{
		if (!silent) containerListeners.call(&ShapeShifterContainerListener::oneShifterRemaining, this, shifters[0]);
	}else
	{
		GapGrabber * gg = grabbers[(jmin<int>(shifterIndex, grabbers.size() - 1))];
		removeChildComponent(gg);
		grabbers.remove(grabbers.indexOf(gg), true);
		resized();
	}
	
	
}

void ShapeShifterContainer::insertPanelAt(ShapeShifterPanel * panel, int index)
{
	insertShifterAt(panel, index);
	panel->addShapeShifterPanelListener(this);
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


ShapeShifterContainer * ShapeShifterContainer::insertContainerAt(Direction _direction, int index)
{
	ShapeShifterContainer * ssc = new ShapeShifterContainer(_direction);
	return insertContainerAt(ssc,index);
}

ShapeShifterContainer * ShapeShifterContainer::insertContainerAt(ShapeShifterContainer * container, int index)
{
	insertShifterAt(container, index);
	container->addShapeShifterContainerListener(this);
	resized();

	return container;
}


void ShapeShifterContainer::movePanelsInContainer(ShapeShifterPanel * newPanel, ShapeShifterPanel * containedPanel, Direction _newDir, bool secondBeforeFirst)
{
	int targetIndex = shifters.indexOf(containedPanel);
	removeShifter(containedPanel,false,true);
	ShapeShifterContainer * newContainer = insertContainerAt(_newDir, targetIndex);
	newContainer->insertPanelAt(containedPanel, 0);
	newContainer->insertPanelAt(newPanel, secondBeforeFirst ? 1:0);
	
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

void ShapeShifterContainer::clear()
{
	int numShiftersToClear = shifters.size() > 0; 

	DBG("Container clear, num shifters to clear " << numShiftersToClear);
	
	while (numShiftersToClear > 0)
	{
		DBG(">> Remove shifter in clear");
		removeShifter(shifters[0], true, true);
		numShiftersToClear--;
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
	DBG("Panel Detach");
	Rectangle<int> panelBounds = panel->getScreenBounds();
	removeShifter(panel,false);
	ShapeShifterManager::getInstance()->showPanelWindow(panel, panelBounds);
}

void ShapeShifterContainer::panelEmptied(ShapeShifterPanel * panel)
{
	DBG("Panel emptied");
	removeShifter(panel, true, false);
}

void ShapeShifterContainer::panelDestroyed(ShapeShifterPanel * panel)
{
	DBG("Panel destroyed");
	removeShifter(panel, false, true);
}

void ShapeShifterContainer::containerEmptied(ShapeShifterContainer * container)
{
	DBG("CONTAINER EMPTIED");
	removeShifter(container,true);
}

void ShapeShifterContainer::oneShifterRemaining(ShapeShifterContainer * container, ShapeShifter * lastShifter)
{
	DBG("ONE SHIFTER REMAINING");
	int containerIndex = shifters.indexOf(container);
	
	DBG("Remove shifter from child container");
	container->removeShifter((ShapeShifterContainer *)lastShifter, false, true);

	DBG("Insert last shifter in parent container");
	if (lastShifter->shifterType == PANEL) insertPanelAt((ShapeShifterPanel *)lastShifter, containerIndex);
	else insertContainerAt((ShapeShifterContainer *)lastShifter, containerIndex);

	DBG("Remove useless container");
	removeShifter(container,true,true);
	
}


