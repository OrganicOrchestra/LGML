/*
  ==============================================================================

    MovablePanel.h
    Created: 2 May 2016 3:08:37pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef MOVABLEPANEL_H_INCLUDED
#define MOVABLEPANEL_H_INCLUDED

#include "ShapeShifter.h"

class ShapeShifterPanel : public ShapeShifter
{
public:
	ShapeShifterPanel(const String &_panelName, Component *innerComponent);
	virtual ~ShapeShifterPanel();

	class PanelHeader : public Component
	{
	public:
		PanelHeader(const String &_panelName);
		~PanelHeader();
		Label panelLabel;

		void paint(Graphics &g) override;
		void resized();
	};

	PanelHeader header;
	ScopedPointer<Component> innerComponent;

	const int headerHeight = 20;

	void paint(Graphics & g) override;
	void resized() override;

	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShapeShifterPanel)
};


#endif  // MOVABLEPANEL_H_INCLUDED
