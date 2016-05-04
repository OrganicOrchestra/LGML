/*
  ==============================================================================

    ShapeShifterPanelTab.h
    Created: 3 May 2016 12:43:01pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SHAPESHIFTERPANELTAB_H_INCLUDED
#define SHAPESHIFTERPANELTAB_H_INCLUDED

#include "ShapeShifterContent.h"

class ShapeShifterPanelTab : public Component
{
public:
	ShapeShifterPanelTab(ShapeShifterContent * _content);
	~ShapeShifterPanelTab();

	ShapeShifterContent * content;
	Label panelLabel;

	bool selected;
	void setSelected(bool value);

	void paint(Graphics &g) override;
	void resized();

	int getLabelWidth();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShapeShifterPanelTab)
};


#endif  // SHAPESHIFTERPANELTAB_H_INCLUDED
