/*
  ==============================================================================

    InspectorEditor.h
    Created: 9 May 2016 7:08:50pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef INSPECTOREDITOR_H_INCLUDED
#define INSPECTOREDITOR_H_INCLUDED

#include "JuceHeader.h"
class InspectableComponent;

class InspectorEditor : public Component
{
public:
	InspectorEditor(InspectableComponent * sourceComponent);
	virtual ~InspectorEditor();


	InspectableComponent * sourceComponent;

	void resized() override;

	virtual int getContentHeight();

	virtual void clear();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InspectorEditor)
};

#endif  // INSPECTOREDITOR_H_INCLUDED
