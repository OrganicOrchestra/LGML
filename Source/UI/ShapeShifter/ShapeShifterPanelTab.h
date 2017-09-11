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


#ifndef SHAPESHIFTERPANELTAB_H_INCLUDED
#define SHAPESHIFTERPANELTAB_H_INCLUDED

#include "ShapeShifterContent.h"//keep
class ShapeShifterPanelTab :
	public juce::Component,
	public ButtonListener
{
public:
	ShapeShifterPanelTab(ShapeShifterContent * _content);
	~ShapeShifterPanelTab();

	ShapeShifterContent * content;
	Label panelLabel;
	ImageButton closePanelBT;

	bool selected;
	void setSelected(bool value);

	void paint(Graphics &g) override;
	void resized()override;

	int getLabelWidth();

	void buttonClicked(Button * b) override;

	//Listener
	class TabListener
	{
	public:
		virtual ~TabListener() {};
		virtual void askForRemoveTab(ShapeShifterPanelTab *) {};
	};

	ListenerList<TabListener> tabListeners;
	void addShapeShifterTabListener(TabListener* newListener) { tabListeners.add(newListener); }
	void removeShapeShifterTabListener(TabListener* listener) { tabListeners.remove(listener); }


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShapeShifterPanelTab)
};


#endif  // SHAPESHIFTERPANELTAB_H_INCLUDED
