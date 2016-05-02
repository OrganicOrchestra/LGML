/*
  ==============================================================================

    MovablePanel.h
    Created: 2 May 2016 3:08:37pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef MOVABLEPANEL_H_INCLUDED
#define MOVABLEPANEL_H_INCLUDED

#include "JuceHeader.h"

class MovablePanel : public Component
{
public:
	MovablePanel(const String &_panelName, Component *innerComponent);
	virtual ~MovablePanel();

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

	void resized() override;

	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MovablePanel)
};


#endif  // MOVABLEPANEL_H_INCLUDED
