/*
  ==============================================================================

    GenericControllableContainerEditor.h
    Created: 9 May 2016 6:41:59pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef GENERICCONTROLLABLECONTAINEREDITOR_H_INCLUDED
#define GENERICCONTROLLABLECONTAINEREDITOR_H_INCLUDED

#include "InspectorEditor.h"
#include "ControllableContainer.h"

class GenericControllableContainerEditor : public InspectorEditor
{
public :
	GenericControllableContainerEditor(InspectableComponent * sourceComponent);
	virtual ~GenericControllableContainerEditor();

	ControllableContainer * sourceContainer;

	OwnedArray<ControllableUI> controllablesUI;

	void createUI();
	void addUIForContainer(ControllableContainer * cc, int recursiveLevel);

	void resized() override;


	void clear() override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenericControllableContainerEditor)
};


#endif  // GENERICCONTROLLABLECONTAINEREDITOR_H_INCLUDED
