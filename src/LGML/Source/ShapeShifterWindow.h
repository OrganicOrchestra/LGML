/*
  ==============================================================================

    ShapeShifterWindow.h
    Created: 2 May 2016 4:10:48pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SHAPESHIFTERWINDOW_H_INCLUDED
#define SHAPESHIFTERWINDOW_H_INCLUDED

#include "ShapeShifterPanel.h"

class ShapeShifterWindow : public ResizableWindow
{
public:
	ShapeShifterWindow(ShapeShifterPanel * _panel, Rectangle<int> bounds);
	~ShapeShifterWindow();

	enum DragMode {NONE, TAB, PANEL};

	ShapeShifterPanel * panel;

	ComponentDragger dragger;
	void mouseDown(const MouseEvent& e) override;
	void mouseDrag(const MouseEvent& e) override;
	void mouseUp(const MouseEvent &e) override;

	DragMode dragMode;
	

	void userTriedToCloseWindow() override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShapeShifterWindow)
};


#endif  // SHAPESHIFTERWINDOW_H_INCLUDED
