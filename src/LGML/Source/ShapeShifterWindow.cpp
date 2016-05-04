/*
  ==============================================================================

    ShapeShifterWindow.cpp
    Created: 2 May 2016 4:10:48pm
    Author:  bkupe

  ==============================================================================
*/

#include "ShapeShifterWindow.h"
#include "ShapeShifterManager.h"
#include "Style.h"

ShapeShifterWindow::ShapeShifterWindow(ShapeShifterPanel * _panel, Rectangle<int> bounds) :
	panel(_panel),
	ResizableWindow(_panel->currentContent->getName(), true),
	dragMode(PANEL)
{
	_panel->setBounds(bounds);

	setContentNonOwned(_panel,true);

	setBackgroundColour(BG_COLOR.darker(.1f).withAlpha(.3f));
	//setAlwaysOnTop(true);

	setResizable(true, true);
	setDraggable(true);
	
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
	panel->setTransparentBackground(true);
	ShapeShifterManager::getInstance()->checkCandidateTargetForPanel(panel);
	dragger.dragComponent(this, e, 0);
}

void ShapeShifterWindow::mouseUp(const MouseEvent & e)
{
	DBG("mouse up");
	panel->setTransparentBackground(false);
	ShapeShifterManager::getInstance()->checkDropOnCandidateTarget(panel);
}


void ShapeShifterWindow::userTriedToCloseWindow()
{
	ShapeShifterManager::getInstance()->closePanelWindow(this);
}

