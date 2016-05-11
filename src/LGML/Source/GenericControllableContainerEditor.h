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

class CCInnerContainer : public Component, public ControllableContainer::Listener
{
public:
	CCInnerContainer(ControllableContainer * container, int level, int maxLevel);
	virtual ~CCInnerContainer();

	Label containerLabel;

	ControllableContainer * container;
	OwnedArray<ControllableUI> controllablesUI;
	OwnedArray<CCInnerContainer> innerContainers;

	int level;
	int maxLevel;

	void paint(Graphics &g) override;
	void resized() override;
	void clear();


	void addControllableUI(Controllable * c);
	void removeControllableUI(Controllable *c);

	void addCCInnerUI(ControllableContainer * cc);
	void removeCCInnerUI(ControllableContainer * cc);

	int getContentHeight();

	ControllableUI * getUIForControllable(Controllable * c);
	CCInnerContainer * getInnerContainerForCC(ControllableContainer * cc);

	void controllableAdded(Controllable *);
	void controllableRemoved(Controllable *);
	void controllableContainerAdded(ControllableContainer *);
	void controllableContainerRemoved(ControllableContainer *);
	void childStructureChanged(ControllableContainer *);
};


class GenericControllableContainerEditor : public InspectorEditor
{
public :
	GenericControllableContainerEditor(InspectableComponent * sourceComponent);
	virtual ~GenericControllableContainerEditor();

	ControllableContainer * sourceContainer;
	ScopedPointer<CCInnerContainer> innerContainer;
	
	void resized() override;
	void clear() override;

	

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenericControllableContainerEditor)
};


#endif  // GENERICCONTROLLABLECONTAINEREDITOR_H_INCLUDED
