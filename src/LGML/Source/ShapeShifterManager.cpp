/*
  ==============================================================================

    ShapeShifterManager.cpp
    Created: 2 May 2016 3:11:35pm
    Author:  bkupe

  ==============================================================================
*/

#include "ShapeShifterManager.h"
#include "ShapeShifterFactory.h"
#include "DebugHelpers.h"

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


ShapeShifterPanel * ShapeShifterManager::getPanelForContent(ShapeShifterContent * content)
{
	for (auto &p : openedPanels)
	{
		if (p->hasContent(content)) return p;
	}

	return nullptr;
}

ShapeShifterPanel * ShapeShifterManager::getPanelForContentName(const String & name)
{
	for (auto &p : openedPanels)
	{
		if (p->hasContent(name)) return p;
	}

	return nullptr;
}

ShapeShifterPanel * ShapeShifterManager::createPanel(ShapeShifterContent * content, ShapeShifterPanelTab * sourceTab)
{
	ShapeShifterPanel * panel = new ShapeShifterPanel(content,sourceTab);

	//if(content != nullptr) panel->setSize(content->getWidth(), content->getHeight());

	panel->addShapeShifterPanelListener(this);
	openedPanels.add(panel);
	return panel;
}

void ShapeShifterManager::removePanel(ShapeShifterPanel * panel)
{
	DBG("Remove Panel");
	panel->removeShapeShifterPanelListener(this);
	openedPanels.removeObject(panel, true);
}

ShapeShifterWindow * ShapeShifterManager::showPanelWindow(ShapeShifterPanel * _panel, Rectangle<int> bounds)
{
	ShapeShifterWindow * w = new ShapeShifterWindow(_panel,bounds);
	openedWindows.add(w);
	w->toFront(true);
	return w;
}

ShapeShifterWindow * ShapeShifterManager::showPanelWindowForContent(PanelName panelName)
{
	ShapeShifterContent * c = ShapeShifterFactory::createContentForIndex(panelName);

	if (c == nullptr) return nullptr;

	ShapeShifterPanel * newP = createPanel(c);
	Rectangle<int> r(300, 500);
	ShapeShifterWindow * w = showPanelWindow(newP, r);
	return w;
}

void ShapeShifterManager::closePanelWindow(ShapeShifterWindow * window, bool doRemovePanel)
{
	DBG("Remove Window");
	window->removeFromDesktop();
	if (doRemovePanel) removePanel(window->panel);
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
	if (result) closePanelWindow(getWindowForPanel(panel),false);

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

void ShapeShifterManager::loadLayout(var layout)
{
	clearAllPanelsAndWindows();

	mainContainer.loadLayout(layout.getDynamicObject()->getProperty("mainLayout"));

	Array<var>* wData = layout.getDynamicObject()->getProperty("windows").getArray();

	if (wData != nullptr)
	{
		for (auto &wd : *wData)
		{
			DynamicObject * d = wd.getDynamicObject();
			ShapeShifterPanel * p = createPanel(nullptr);
			p->loadLayout(d->getProperty("panel"));
			Rectangle<int> bounds(d->getProperty("x"),d->getProperty("y"),d->getProperty("width"),d->getProperty("height"));
			DBG("Window bounds " << bounds.toString());
			showPanelWindow(p,bounds);
		}
	}
}

var ShapeShifterManager::getCurrentLayout()
{
	DynamicObject * d = new DynamicObject();
	var layout(d);
	d->setProperty("mainLayout", mainContainer.getCurrentLayout());

	var wData;
	for (auto &w : openedWindows)
	{
		wData.append(w->getCurrentLayout());
	}

	layout.getDynamicObject()->setProperty("windows", wData);
	return layout;
}

void ShapeShifterManager::loadLayoutFromFile(int fileIndexInLayoutFolder)
{

	File destDir = File::getSpecialLocation(File::SpecialLocationType::userDocumentsDirectory).getChildFile("LGML/layouts");
	if (!destDir.exists())
	{
		destDir.createDirectory();
	}

	File layoutFile;
	if (fileIndexInLayoutFolder == -1)
	{
		FileChooser fc("Load layout", destDir, "*.lgmllayout");
		if (!fc.browseForFileToOpen()) return;
		layoutFile = fc.getResult();
	}
	else
	{
		Array<File> layoutFiles = getLayoutFiles();
		layoutFile = layoutFiles[fileIndexInLayoutFolder];
	}


	ScopedPointer<InputStream> is(layoutFile.createInputStream());
	var data = JSON::parse(*is);
	loadLayout(data);

	NLOG("SS", "Layout loaded");
}

void ShapeShifterManager::loadDefaultLayoutFile()
{
	String defaultLayoutFileData = String::fromUTF8(BinaryData::default_lgmllayout);
	loadLayout(JSON::parse(defaultLayoutFileData));
}

void ShapeShifterManager::saveCurrentLayout()
{
	File destDir = File::getSpecialLocation(File::SpecialLocationType::userDocumentsDirectory).getChildFile("LGML/layouts");
	if (!destDir.exists()) destDir.createDirectory();

	FileChooser fc("Save layout", destDir, "*.lgmllayout");
	if (fc.browseForFileToSave(true))
	{
		File layoutFile = fc.getResult();
		if (layoutFile.exists())
		{
			layoutFile.deleteFile();
			layoutFile.create();
		}
		ScopedPointer<OutputStream> os(layoutFile.createOutputStream());
		JSON::writeToStream(*os, getCurrentLayout());
		os->flush();
	}

	NLOG("SS","Layout saved");
}

Array<File> ShapeShifterManager::getLayoutFiles()
{
	File layoutFolder = File::getSpecialLocation(File::SpecialLocationType::userDocumentsDirectory).getChildFile("LGML/layouts");
	if (!layoutFolder.exists())
	{
		layoutFolder.createDirectory();
	}

	Array<File> layoutFiles;
	layoutFolder.findChildFiles(layoutFiles, File::findFiles, false, "*.lgmllayout");

	return layoutFiles;
}

void ShapeShifterManager::clearAllPanelsAndWindows()
{
	while (openedWindows.size() > 0)
	{
		closePanelWindow(openedWindows[0], true);
	}
	while (openedPanels.size() > 0)
	{
		removePanel(openedPanels[0]);
	}
	mainContainer.clear();
}

PopupMenu ShapeShifterManager::getPanelsMenu()
{
	PopupMenu p;
	PopupMenu layoutP;
	layoutP.addItem(baseSpecialMenuCommandID + 1, "Save Current layout");
	layoutP.addItem(baseSpecialMenuCommandID + 2, "Load Default Layout");
	layoutP.addItem(baseSpecialMenuCommandID + 3, "Load Layout...");
	layoutP.addSeparator();

	Array<File> layoutFiles = getLayoutFiles();

	int specialIndex = layoutP.getNumItems()+1;
	for (auto &f : layoutFiles)
	{
		layoutP.addItem(baseSpecialMenuCommandID+specialIndex,f.getFileNameWithoutExtension());
		specialIndex++;
	}

	p.addSubMenu("Layout", layoutP);

	p.addSeparator();

	int currentID = 1;
	for (auto &n : globalPanelNames)
	{
		p.addItem(baseMenuCommandID + currentID, n, true);
		currentID++;
	}

	return p;
}

void ShapeShifterManager::handleMenuPanelCommand(int commandID)
{
	bool isSpecial = ((commandID & 0xff000) == 0x32000);

	if (isSpecial)
	{
		int sCommandID = commandID - baseSpecialMenuCommandID;
		switch (sCommandID)
		{
		case 1: //Save
			saveCurrentLayout();
			break;

		case 2 : //Load
			loadDefaultLayoutFile();
			break;

		case 3: //Load
			loadLayoutFromFile();
			break;

		default:
			loadLayoutFromFile(sCommandID - 4);
			break;
		}
		return;
	}

	int relCommandID = commandID - baseMenuCommandID - 1;

	String contentName = globalPanelNames[relCommandID];
	ShapeShifterPanel * p = getPanelForContentName(contentName);
	if (p != nullptr)
	{
		p->setCurrentContent(contentName);
		ShapeShifterWindow * w = getWindowForPanel(p);
		if (w != nullptr) w->toFront(true);
	}else
	{
		ShapeShifterContent * c = ShapeShifterFactory::createContentForIndex((PanelName)relCommandID);

		if (c == nullptr) return;

		ShapeShifterPanel * newP = createPanel(c);

		if (mainContainer.shifters.size() == 0)
		{
			mainContainer.insertPanelAt(newP, 0);
		}
		else
		{
			Rectangle<int> r(300, 500);
			showPanelWindow(newP, r);
		}
	}
}
