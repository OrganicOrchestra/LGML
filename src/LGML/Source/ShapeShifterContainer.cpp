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
		GapGrabber * gg = new GapGrabber(direction == HORIZONTAL?GapGrabber::HORIZONTAL:GapGrabber::VERTICAL);
		grabbers.insert(index, gg);
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


void ShapeShifterContainer::removePanel(ShapeShifterPanel * panel)
{
	int shifterIndex = shifters.indexOf(panel);
	shifters.removeAllInstancesOf(panel);
	panel->setParentContainer(nullptr);
	panel->removeShapeShifterPanelListener(this);
	
	if (shifters.size() == 0)
	{
		//dispatch emptied container so parent container deletes it
		containerListeners.call(&ShapeShifterContainerListener::containerEmptied, this);
	}
	else
	{
		GapGrabber * gg = grabbers[(jmin<int>(shifterIndex, grabbers.size() - 1))];
		removeChildComponent(gg);
		grabbers.remove(shifterIndex, true);
	}

	removeChildComponent(panel);
	resized();
}

ShapeShifterContainer * ShapeShifterContainer::insertContainerAt(int index, Direction _direction)
{
	ShapeShifterContainer * ssc = new ShapeShifterContainer(_direction);
	shifters.insert(index,ssc);
	containers.add(ssc);
	ssc->addShapeShifterContainerListener(this);
	addAndMakeVisible(ssc);

	if (shifters.size() > 1)
	{
		GapGrabber * gg = new GapGrabber(direction == HORIZONTAL ? GapGrabber::HORIZONTAL : GapGrabber::VERTICAL);
		grabbers.insert(index, gg);
		addAndMakeVisible(gg);
		gg->addGrabberListener(this);
	}

	resized();

	return ssc;
}

void ShapeShifterContainer::removeContainer(ShapeShifterContainer * container)
{
	int shifterIndex = shifters.indexOf(container);
	shifters.removeAllInstancesOf(container);
	container->removeShapeShifterContainerListener(this);
	container->setParentContainer(nullptr);

	if (shifters.size() == 0)
	{
		containerListeners.call(&ShapeShifterContainerListener::containerEmptied, this);
	}
	else
	{
		GapGrabber * gg = grabbers.getUnchecked(jmin<int>(shifterIndex, grabbers.size() - 1));
		removeChildComponent(gg);
		grabbers.remove(shifterIndex, true);
	}

	removeChildComponent(container);
	resized();
}

void ShapeShifterContainer::movePanelsInContainer(ShapeShifterPanel * containedPanel, ShapeShifterPanel * newPanel, Direction _newDir, bool secondBeforeFirst)
{
	DBG("Move Panel In Container");
	ShapeShifterContainer * newContainer = insertContainerAt(shifters.indexOf(containedPanel), _newDir);
	//removePanel(containedPanel);
	//newContainer->insertPanelAt(containedPanel, 0);
	newContainer->insertPanelAt(newPanel, secondBeforeFirst ? 0 : 1);
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
            jassertfalse;
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
	DBG("callback container emptied");
	removeContainer(container);
}


