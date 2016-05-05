/*
  ==============================================================================

    ShapeShifterManager.h
    Created: 2 May 2016 3:11:35pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SHAPESHIFTERMANAGER_H_INCLUDED
#define SHAPESHIFTERMANAGER_H_INCLUDED

#include "ShapeShifterContainer.h"
#include "ShapeShifterWindow.h"

class ShapeShifterContentDefinition
{
public:
	ShapeShifterContentDefinition(const String &_panelID, ShapeShifterContent* _content) :panelID(_panelID), content(_content) {}
	String panelID;
	ShapeShifterContent * content;
};

class ShapeShifterManager : 
	public ShapeShifterPanel::Listener,
	public MenuBarModel
{
public:

	
	juce_DeclareSingleton(ShapeShifterManager,true);
	ShapeShifterManager();
	virtual ~ShapeShifterManager();

	ShapeShifterContainer mainContainer;
	OwnedArray<ShapeShifterPanel> openedPanels;
	OwnedArray<ShapeShifterWindow> openedWindows;

	ShapeShifterPanel * currentCandidatePanel;
	void setCurrentCandidatePanel(ShapeShifterPanel *);

	

	ShapeShifterPanel * createPanel(ShapeShifterContent * content, ShapeShifterPanelTab * sourceTab = nullptr);
	void removePanel(ShapeShifterPanel * panel);

	ShapeShifterWindow * showPanelWindow(ShapeShifterPanel * _panel, Rectangle<int> bounds);
	void closePanelWindow(ShapeShifterWindow * window);


	ShapeShifterPanel * checkCandidateTargetForPanel(ShapeShifterPanel * panel);
	bool checkDropOnCandidateTarget(ShapeShifterPanel * panel);

	ShapeShifterWindow * getWindowForPanel(ShapeShifterPanel * panel);


	//Content registration & menu
	OwnedArray<ShapeShifterContentDefinition> definitions;
	void registerContent(const String &contentID, ShapeShifterContent * content);
	void generateMenu();

	const int baseCommandID = 0x31000;
	void getCommandInfo(int commandID, ApplicationCommandInfo &result);
	String getNameForCommandID(int commandID) const;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShapeShifterManager)


	// Inherited via MenuBarModel
	virtual StringArray getMenuBarNames() override;
	virtual PopupMenu getMenuForIndex(int topLevelMenuIndex, const String & menuName) override;
	virtual void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;
};

#endif  // SHAPESHIFTERMANAGER_H_INCLUDED
