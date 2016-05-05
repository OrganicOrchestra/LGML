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
	mainContainer(ShapeShifterContainer::Direction::VERTICAL),
	currentCandidatePanel(nullptr)
{

}

ShapeShifterManager::~ShapeShifterManager()
{
	openedWindows.clear();
}

void ShapeShifterManager::setCurrentCandidatePanel(ShapeShifterPanel * panel)
{
	if (currentCandidatePanel == panel) return;

	if (currentCandidatePanel != nullptr) currentCandidatePanel->setTargetMode(false);
	currentCandidatePanel = panel;
	if (currentCandidatePanel != nullptr) currentCandidatePanel->setTargetMode(true);

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
	window->removeFromDesktop();
	openedWindows.removeObject(window, true);
}

ShapeShifterPanel * ShapeShifterManager::checkCandidateTargetForPanel(ShapeShifterPanel * panel)
{
	ShapeShifterPanel * candidate = nullptr;

	for (auto &p : openedPanels)
	{
		if (p == panel) continue;

		if (p->getLocalBounds().contains(p->getLocalPoint(panel, Point<float>()).toInt()))
		{
			candidate = p;
		}
	}
	setCurrentCandidatePanel(candidate);

	if(currentCandidatePanel != nullptr) currentCandidatePanel->checkAttachZone(panel);


	return candidate;
}

bool ShapeShifterManager::checkDropOnCandidateTarget(ShapeShifterPanel * panel)
{
	if (currentCandidatePanel == nullptr) return false;

	bool result = currentCandidatePanel->attachPanel(panel);
	if (result) closePanelWindow(getWindowForPanel(panel));

	setCurrentCandidatePanel(nullptr);
	return true;
}

ShapeShifterWindow * ShapeShifterManager::getWindowForPanel(ShapeShifterPanel * panel)
{
	for (auto &w : openedWindows)
	{
		if (w->panel == panel) return w;
	}

	return nullptr;
}




void ShapeShifterManager::registerContent(const String &contentID, ShapeShifterContent * content)
{
	definitions.add(new ShapeShifterContentDefinition(contentID, content));
}

void ShapeShifterManager::generateMenu()
{
	//How to ?
}