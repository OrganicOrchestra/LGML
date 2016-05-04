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

class ShapeShifterManager : public ShapeShifterPanel::Listener
{
public:
	juce_DeclareSingleton(ShapeShifterManager,true);
	ShapeShifterManager();
	~ShapeShifterManager();

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
	
	void panelEmptied(ShapeShifterPanel * panel) override;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShapeShifterManager)

};

#endif  // SHAPESHIFTERMANAGER_H_INCLUDED
