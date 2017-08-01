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
#include "Inspector.h"


class FastMapper;
class FastMapperUI;
class FastMapperUIListener
{
public:
    virtual ~FastMapperUIListener() {}
	virtual void fastMapperContentChanged(FastMapperUI *) {}
};

class FastMapperUI :
	public Component,
	private ControllableContainerListener,
private ButtonListener,
private Inspector::InspectorListener
{
public:
	FastMapperUI(FastMapper * fastMapper, ControllableContainer * viewFilterContainer = nullptr);
	virtual ~FastMapperUI();

	FastMapper * fastMapper;
  TextButton linkToSelection,addFastMapBt;
  ScopedPointer<Component> potentialIn,potentialOut;
	OwnedArray<FastMapUI> mapsUI;

	ControllableContainer * viewFilterContainer;

	void clear();

	void addFastMapUI(FastMap *);
	void removeFastMapUI(FastMapUI *);

	void resetAndUpdateView();
	void setViewFilter(ControllableContainer * filterContainer);
	bool mapPassViewFilter(FastMap *);

	FastMapUI * getUIForFastMap(FastMap *);

	const int mapHeight = 35;
	const int gap = 5;
	int getContentHeight();

	void resized() override;
	void mouseDown(const MouseEvent &e) override;

	virtual void controllableContainerAdded(ControllableContainer*,ControllableContainer *) override;
	virtual void controllableContainerRemoved(ControllableContainer*,ControllableContainer *) override;

	ListenerList<FastMapperUIListener> fastMapperUIListeners;
	void addFastMapperUIListener(FastMapperUIListener* newListener) { fastMapperUIListeners.add(newListener); }
	void removeFastMapperUIListener(FastMapperUIListener* listener) { fastMapperUIListeners.remove(listener); }
private:

  void buttonClicked (Button*) override;
  void currentComponentChanged(Inspector * ) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FastMapperUI)

};

class FastMapperViewport :
	public ShapeShifterContentComponent,
	public FastMapperUIListener
{
public:
	FastMapperViewport(const String &contentName, FastMapperUI * _fastMapperUI) :
		fastMapperUI(_fastMapperUI),
		ShapeShifterContentComponent(contentName)
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

	void fastMapperContentChanged(FastMapperUI *)override
	{
		resized();
	}

	Viewport vp;
	FastMapperUI * fastMapperUI;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FastMapperViewport)
};


#endif  // FASTMAPPERUI_H_INCLUDED
