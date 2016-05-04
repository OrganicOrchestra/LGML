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
#include "ShapeShifterPanelTab.h"
#include "ShapeShifterContent.h"
#include "ShapeShifterPanelHeader.h"

class ShapeShifterContainer;

class ShapeShifterPanel : public ShapeShifter, public ShapeShifterPanelHeader::Listener
{
public:
	enum AttachZone {NONE, TOP, BOTTOM, LEFT, RIGHT, CENTER };

	ShapeShifterPanel(ShapeShifterContent *innerComponent, ShapeShifterPanelTab * sourceTab = nullptr);
	virtual ~ShapeShifterPanel();

	const int headerHeight = 20;
	ShapeShifterPanelHeader header;

	Array<ShapeShifterContent *> contents;

	ShapeShifterContent * currentContent;
	void setCurrentContent(ShapeShifterContent * content);
	
	bool transparentBackground;

	bool targetMode;
	void setTargetMode(bool value);

	Point<float> candidateTargetPoint;
	AttachZone candidateZone;

	void paint(Graphics & g) override;
	void paintOverChildren(Graphics & g) override;
	void resized() override;

	void setTransparentBackground(bool value);

	void attachTab(ShapeShifterPanelTab *);
	void detachTab(ShapeShifterPanelTab *, bool createNewPanel);
	void removeTab(ShapeShifterPanelTab *);

	bool attachPanel(ShapeShifterPanel *); //attach all tabs of a panel to this panel, or attach a panel to the parent container depending on attachZone

	void addContent(ShapeShifterContent * content, bool setCurrent = true);
	
	//Attach helpers

	AttachZone checkAttachZone(ShapeShifterPanel * source);
	void setCandidateZone(AttachZone zone);

	virtual void tabDrag(ShapeShifterPanelTab *) override;
	virtual void tabSelect(ShapeShifterPanelTab *) override;
	virtual void headerDrag() override;

	//Listener
	class Listener
	{
	public:
        virtual ~Listener(){};
		virtual void panelDetach(ShapeShifterPanel *) {}
		virtual void panelEmptied(ShapeShifterPanel *) {};
		virtual void panelRemoved(ShapeShifterPanel *) {};

		virtual void headerDrag(ShapeShifterPanel *) {}
		virtual void tabDrag(ShapeShifterPanel *) {};
	};
	
	ListenerList<Listener> listeners;
	void addShapeShifterPanelListener(Listener* newListener) { listeners.add(newListener); }
	void removeShapeShifterPanelListener(Listener* listener) { listeners.remove(listener); }

	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShapeShifterPanel)

		

		

};


#endif  // MOVABLEPANEL_H_INCLUDED
