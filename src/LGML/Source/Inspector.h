/*
  ==============================================================================

    Inspector.h
    Created: 9 May 2016 6:41:38pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef INSPECTOR_H_INCLUDED
#define INSPECTOR_H_INCLUDED

#include "ShapeShifterContent.h"
#include "InspectableComponent.h"
#include "InspectorEditor.h"

class Inspector : public Component, public InspectableComponent::InspectableListener, public InspectorEditor::InspectorEditorListener
{
public:
	Inspector();
	virtual ~Inspector();

	InspectableComponent * currentComponent;

	ScopedPointer<InspectorEditor> currentEditor;

	bool isEnabled;
	void setEnabled(bool value);

	void setCurrentComponent(InspectableComponent * component);

	void resized() override;

	void clear();
	void inspectCurrentComponent();

	void inspectableRemoved(InspectableComponent * component) override;

	void contentSizeChanged(InspectorEditor *) override;
	//Listener
	class  InspectorListener
	{
	public:
		/** Destructor. */
		virtual ~InspectorListener() {}
		virtual void currentComponentChanged(Inspector * ) {};
		virtual void contentSizeChanged(Inspector *) {};
	};

	ListenerList<InspectorListener> listeners;
	void addInspectorListener(InspectorListener* newListener) { listeners.add(newListener); }
	void removeInspectorListener(InspectorListener* listener) { listeners.remove(listener); }


};

class InspectorViewport : public ShapeShifterContent, public Inspector::InspectorListener {
public:
	InspectorViewport(Inspector * _inspector) :inspector(_inspector), ShapeShifterContent("Inspector")
	{
		vp.setViewedComponent(inspector, true);
		vp.setScrollBarsShown(true, false);
		vp.setScrollOnDragEnabled(false);
		contentIsFlexible = true;
		addAndMakeVisible(vp);
		vp.setScrollBarThickness(10);

		inspector->addInspectorListener(this);

	}

	virtual ~InspectorViewport()
	{
		inspector->clear();
		inspector->removeInspectorListener(this);
	}

	void resized() override {
		Rectangle<int> r = getLocalBounds();
		r.removeFromRight(vp.getScrollBarThickness());

		vp.setBounds(r);
		if(inspector->currentEditor == nullptr) inspector->setBounds(r);
		else inspector->setBounds(r.withHeight(inspector->currentEditor->getContentHeight()));
	}
	Viewport vp;
	Inspector * inspector;

	void currentComponentChanged(Inspector *) override { resized(); }
	void contentSizeChanged(Inspector *) override { resized(); }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InspectorViewport)
};

#endif  // INSPECTOR_H_INCLUDED
