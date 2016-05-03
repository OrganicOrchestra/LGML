/*
  ==============================================================================

    ShapeShifterManager.cpp
    Created: 2 May 2016 3:11:35pm
    Author:  bkupe

  ==============================================================================
*/

#include "ShapeShifterManager.h"

juce_ImplementSingleton(ShapeShifterManager);

ShapeShifterManager::ShapeShifterManager() : 
	mainContainer(ShapeShifterContainer::ContentType::CONTAINERS,ShapeShifterContainer::Direction::VERTICAL)
{

}

ShapeShifterManager::~ShapeShifterManager()
{
	openedWindows.clear();
}

ShapeShifterPanel * ShapeShifterManager::createPanel(ShapeShifterContent * content, ShapeShifterPanelTab * sourceTab)
{
	ShapeShifterPanel * panel = new ShapeShifterPanel(content,sourceTab);
	panel->setSize(content->getWidth(), content->getHeight());
	panel->addShapeShifterPanelListener(this);
	openedPanels.add(panel);
	return panel;
}

void ShapeShifterManager::removePanel(ShapeShifterPanel * panel)
{
	panel->removeShapeShifterPanelListener(this); 
	openedPanels.removeObject(panel, true);
}

ShapeShifterWindow * ShapeShifterManager::showPanelWindow(ShapeShifterPanel * _panel, Rectangle<int> bounds)
{
	ShapeShifterWindow * w = new ShapeShifterWindow(_panel,bounds);

	openedWindows.add(w);
	return w;
}

void ShapeShifterManager::closePanelWindow(ShapeShifterWindow * window)
{
	openedWindows.removeObject(window, true);
}

void ShapeShifterManager::panelEmptied(ShapeShifterPanel * panel)
{
	removePanel(panel);
}
