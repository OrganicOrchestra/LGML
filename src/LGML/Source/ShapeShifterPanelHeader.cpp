/*
  ==============================================================================

    ShapeShifterPanelHeader.cpp
    Created: 3 May 2016 2:25:55pm
    Author:  bkupe

  ==============================================================================
*/

#include "ShapeShifterPanelHeader.h"
#include "Style.h"

ShapeShifterPanelHeader::ShapeShifterPanelHeader()
{
	addMouseListener(this, true);
}

ShapeShifterPanelHeader::~ShapeShifterPanelHeader()
{
	
}

void ShapeShifterPanelHeader::addTab(ShapeShifterContent * content)
{
	ShapeShifterPanelTab * tab = new ShapeShifterPanelTab(content);
	addAndMakeVisible(tab);
	tabs.add(tab);
	resized();
}

void ShapeShifterPanelHeader::removeTab(ShapeShifterPanelTab * tab, bool doRemove)
{
	removeChildComponent(tab);
	tabs.removeObject(tab,doRemove);
	resized();
}

void ShapeShifterPanelHeader::attachTab(ShapeShifterPanelTab * tab)
{
	addAndMakeVisible(tab);
	tabs.add(tab);
	resized();
}

void ShapeShifterPanelHeader::mouseDown(const MouseEvent & e)
{
	ShapeShifterPanelTab * tab = dynamic_cast<ShapeShifterPanelTab *>(e.eventComponent);
	if (tab != nullptr)
	{
		listeners.call(&Listener::tabSelect, tab);
	}
		
}

void ShapeShifterPanelHeader::mouseDrag(const MouseEvent & e)
{
	int minDetachDistance = 20;

	if (e.eventComponent == this)
	{
		if (e.getDistanceFromDragStart() > minDetachDistance) listeners.call(&Listener::headerDrag);
	}else
	{
		ShapeShifterPanelTab * tab = dynamic_cast<ShapeShifterPanelTab *>(e.eventComponent);
		if (e.getDistanceFromDragStart() > minDetachDistance && tab != nullptr) listeners.call(&Listener::tabDrag,tab);
	}
	
}

void ShapeShifterPanelHeader::paint(Graphics & g)
{
	g.fillAll(BG_COLOR.brighter(.1f));
}

void ShapeShifterPanelHeader::resized()
{
	//re arrange tabs
	Rectangle<int> r = getLocalBounds();
	if (r.getWidth() == 0 || r.getHeight() == 0) return;

	DBG("Header local bounds " << r.toString());
	for (auto &t : tabs)
	{
		DBG("get tab width from header " << t->getWidth());
		t->setBounds(r.removeFromLeft(t->getWidth()));
	}
}
