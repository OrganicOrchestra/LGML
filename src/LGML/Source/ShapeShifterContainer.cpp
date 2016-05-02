/*
  ==============================================================================

    ShapeShifterContainer.cpp
    Created: 2 May 2016 3:11:25pm
    Author:  bkupe

  ==============================================================================
*/

#include "ShapeShifterContainer.h"
#include "Style.h"
#include "ShapeShifter.h"

ShapeShifterContainer::ShapeShifterContainer(ContentType _contentType, Direction _direction) : 
	direction(_direction), contentType(_contentType)
{

}

ShapeShifterContainer::~ShapeShifterContainer()
{
}

void ShapeShifterContainer::insertPanelAt(ShapeShifterPanel * panel, int index)
{
	shifters.insert(index, panel);
	addAndMakeVisible(panel);
	if (shifters.size() > 1)
	{
		GapGrabber * gg = new GapGrabber(direction == HORIZONTAL?GapGrabber::HORIZONTAL:GapGrabber::VERTICAL);
		grabbers.insert(index, gg);
		addAndMakeVisible(gg);
		gg->addGrabberListener(this);
	}

	resized();
}


void ShapeShifterContainer::removePanel(ShapeShifterPanel * panel)
{
	shifters.removeAllInstancesOf(panel);
	removeChildComponent(panel);
}

ShapeShifterContainer * ShapeShifterContainer::insertContainerAt(int index, ContentType _contentType, Direction _direction)
{
	ShapeShifterContainer * ssc = new ShapeShifterContainer(_contentType, _direction);
	shifters.add(ssc);
	containers.add(ssc);
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
}

void ShapeShifterContainer::paint(Graphics & g)
{

}

void ShapeShifterContainer::resized()
{
	Rectangle<int> r = getLocalBounds();
	int gap = 10;
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

void ShapeShifterContainer::grabberGrabUpdate(GapGrabber * gg, float dist)
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
	}

	resized();
}


