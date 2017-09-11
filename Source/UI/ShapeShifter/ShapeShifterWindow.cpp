/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#include "ShapeShifterWindow.h"
#include "ShapeShifterManager.h"
#include "../Style.h"
#include "../LookAndFeelOO.h"


extern ApplicationCommandManager& getCommandManager();

ShapeShifterWindow::ShapeShifterWindow(ShapeShifterPanel * _panel, Rectangle<int> bounds) :
	ResizableWindow(_panel->currentContent->contentName, true),
	dragMode(PANEL),
	panel(_panel),
	checking(false)
{
	setTopLeftPosition(bounds.getTopLeft());
	_panel->setBounds(bounds);

	panel->setPreferredWidth(getWidth());
	panel->setPreferredHeight(getHeight());

	//DBG("window -> addShapeShifterListener " << panel->header.tabs[0]->content->contentName);
	panel->addShapeShifterPanelListener(this); //is it necessary ?

	setContentNonOwned(_panel, true);

	setBackgroundColour(BG_COLOR.darker(.1f).withAlpha(.3f));

	setResizable(true, true);
	setDraggable(true);

	setVisible(true);
	toFront(true);
  static LookAndFeelOO lfOO;
  setLookAndFeel(&lfOO);
#if JUCE_OPENGL 
  OpenGLContext * context = OpenGLContext::getContextAttachedTo(*ShapeShifterManager::getInstance()->mainShifterContainer.getTopLevelComponent());
  if(context){
    openGLContext.setNativeSharedContext(context->getRawContext());
  }
  else{
    jassertfalse;
  }
  openGLContext.setContinuousRepainting(false);
  openGLContext.attachTo(*getTopLevelComponent());
#endif

	addMouseListener(this, true);

  addKeyListener ((&getCommandManager())->getKeyMappings());

}

ShapeShifterWindow::~ShapeShifterWindow()
{
#if JUCE_OPENGL 
  openGLContext.detach();
#endif
	removeMouseListener(this);
	clear();

}

void ShapeShifterWindow::paintOverChildren(Graphics & g)
{
//	g.setColour(BG_COLOR.brighter());
//	g.drawRect(getLocalBounds());
}

void ShapeShifterWindow::resized()
{
	ResizableWindow::resized();

	if (panel == nullptr) return;

	panel->setPreferredWidth(getWidth());
	panel->setPreferredHeight(getHeight());
}

void ShapeShifterWindow::mouseDown(const MouseEvent & e)
{
	if (e.eventComponent == &panel->header || dynamic_cast<ShapeShifterPanelTab *>(e.eventComponent) != nullptr)
	{
		dragMode = e.eventComponent == &panel->header ? PANEL : TAB;
		dragger.startDraggingComponent(this, e);
	} else
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

void ShapeShifterWindow::mouseUp(const MouseEvent &)
{
	panel->setTransparentBackground(false);

	checking = true;
	bool found = ShapeShifterManager::getInstance()->checkDropOnCandidateTarget(panel);
	checking = false;

	if (found)
	{
		clear();
		ShapeShifterManager::getInstance()->closePanelWindow(this, false);
	}
}


void ShapeShifterWindow::clear()
{
	if (panel != nullptr)
	{
		panel->removeShapeShifterPanelListener(this);
		panel = nullptr;
	}
}

void ShapeShifterWindow::userTriedToCloseWindow()
{
	ShapeShifterManager::getInstance()->closePanelWindow(this, true);
}

var ShapeShifterWindow::getCurrentLayout()
{
	var data(new DynamicObject());
	data.getDynamicObject()->setProperty("panel", panel->getCurrentLayout());
	data.getDynamicObject()->setProperty("x", getBounds().getPosition().x);
	data.getDynamicObject()->setProperty("y", getBounds().getPosition().y);
	data.getDynamicObject()->setProperty("width", getWidth());
	data.getDynamicObject()->setProperty("height", getHeight());
	return data;
}

void ShapeShifterWindow::panelEmptied(ShapeShifterPanel *)
{
	if (!checking) ShapeShifterManager::getInstance()->closePanelWindow(this, true);
}
