/*
  ==============================================================================

    FastMapperUI.h
    Created: 17 May 2016 6:05:39pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef FASTMAPPERUI_H_INCLUDED
#define FASTMAPPERUI_H_INCLUDED

#include "ShapeShifterContent.h"
#include "FastMapUI.h"
#include "FastMapper.h"


class FastMapperUI;
class FastMapperUIListener
{
public:
    virtual ~FastMapperUIListener() {}
	virtual void fastMapperContentChanged(FastMapperUI *) {}
};

class FastMapperUI : 
	public Component,
	public FastMapperListener
{
public:
	FastMapperUI(FastMapper * fastMapper);
	virtual ~FastMapperUI();

	FastMapper * fastMapper;

	OwnedArray<FastMapUI> mapsUI;

	void addFastMapUI(FastMap *);
	void removeFastMapUI(FastMap *);

	FastMapUI * getUIForFastMap(FastMap *);
	 
	const int mapHeight = 35;
	const int gap = 5;
	int getContentHeight();

	void resized() override;
	void clear();

	void mouseDown(const MouseEvent &e) override;

	virtual void fastMapAdded(FastMap *) override;
	virtual void fastMapRemoved(FastMap *) override;

	ListenerList<FastMapperUIListener> fastMapperUIListeners;
	void addFastMapperUIListener(FastMapperUIListener* newListener) { fastMapperUIListeners.add(newListener); }
	void removeFastMapperUIListener(FastMapperUIListener* listener) { fastMapperUIListeners.remove(listener); }


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FastMapperUI)

};

class FastMapperViewport : 
	public ShapeShifterContent,
	public FastMapperUIListener
{
public:
	FastMapperViewport(FastMapperUI * _fastMapperUI) :
		fastMapperUI(_fastMapperUI), 
		ShapeShifterContent("FastMapper")
	{
		vp.setViewedComponent(fastMapperUI, true);
		vp.setScrollBarsShown(true, false);
		vp.setScrollOnDragEnabled(false);
		addAndMakeVisible(vp);
		vp.setScrollBarThickness(10);

		fastMapperUI->addFastMapperUIListener(this);
	}

	virtual ~FastMapperViewport()
	{
		fastMapperUI->removeFastMapperUIListener(this);
	}

	void resized() override {
		vp.setBounds(getLocalBounds());
		int th = jmax<int>(fastMapperUI->getContentHeight(), getHeight());
		Rectangle<int> targetBounds = getLocalBounds().withPosition(fastMapperUI->getPosition()).withHeight(th);
		targetBounds.removeFromRight(vp.getScrollBarThickness());
		fastMapperUI->setBounds(targetBounds);
	}

	void fastMapperContentChanged()
	{
		resized();
	}

	Viewport vp;
	FastMapperUI * fastMapperUI;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FastMapperViewport)
};


#endif  // FASTMAPPERUI_H_INCLUDED
