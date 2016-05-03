/*
  ==============================================================================

    ShapeShifterWindow.cpp
    Created: 2 May 2016 4:10:48pm
    Author:  bkupe

  ==============================================================================
*/

#include "ShapeShifterWindow.h"
#include "ShapeShifterManager.h"

ShapeShifterWindow::ShapeShifterWindow(ShapeShifterPanel * _panel, Rectangle<int> bounds) :
	panel(_panel),
	ResizableWindow(_panel->currentContent->getName(), true),
	dragMode(PANEL)
{
	_panel->setBounds(bounds);
	setContentNonOwned(_panel,true);

	setAlwaysOnTop(true);

	setResizable(true, true);
	setDraggable(true);
	
	//setBounds(bounds);
	setVisible(true);
	toFront(true);

	addMouseListener(this,true);

}

ShapeShifterWindow::~ShapeShifterWindow()
{
	removeMouseListener(this);
}

void ShapeShifterWindow::mouseDown(const MouseEvent & e)
{
	if (e.eventComponent == &panel->header || dynamic_cast<ShapeShifterPanelTab *>(e.eventComponent) != nullptr)
	{
		dragMode = e.eventComponent == &panel->header ? PANEL : TAB;
		dragger.startDraggingComponent(this, e);
	}else
	{
		dragMode = NONE;
	}

}

void ShapeShifterWindow::mouseDrag(const MouseEvent & e)
{
	if (dragMode == NONE) return;
	dragger.dragComponent(this, e, 0);
}


void ShapeShifterWindow::userTriedToCloseWindow()
{
	ShapeShifterManager::getInstance()->closePanelWindow(this);
}

