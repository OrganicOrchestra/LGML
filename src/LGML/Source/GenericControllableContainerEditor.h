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


class GenericControllableContainerEditor;

class CCInnerContainer : public Component, public ControllableContainer::Listener, public ButtonListener
{
public:
	class CCLinkBT : public TextButton
	{
	public:
		CCLinkBT(ControllableContainer * targetContainer);
		ControllableContainer * targetContainer;
	};


	CCInnerContainer(GenericControllableContainerEditor * editor,ControllableContainer * container, int level, int maxLevel, bool canAccessLowerContainers);
	virtual ~CCInnerContainer();

	Label containerLabel;

	ControllableContainer * container;
	OwnedArray<ControllableUI> controllablesUI;
	OwnedArray<CCInnerContainer> innerContainers;
	OwnedArray<CCLinkBT> lowerContainerLinks;

	GenericControllableContainerEditor * editor;

	int level;
	int maxLevel;
	bool canAccessLowerContainers;

	void paint(Graphics &g) override;
	void resized() override;
	void clear();


	void addControllableUI(Controllable * c);
	void removeControllableUI(Controllable *c);

	void addCCInnerUI(ControllableContainer * cc);
	void removeCCInnerUI(ControllableContainer * cc);

	void addCCLink(ControllableContainer * cc);
	void removeCCLink(ControllableContainer * cc);

	int getContentHeight();

	ControllableUI * getUIForControllable(Controllable * c);
	CCInnerContainer * getInnerContainerForCC(ControllableContainer * cc);
	CCLinkBT * getCCLinkForCC(ControllableContainer * cc);

	void controllableAdded(Controllable *);
	void controllableRemoved(Controllable *);
	void controllableContainerAdded(ControllableContainer *);
	void controllableContainerRemoved(ControllableContainer *);
	void childStructureChanged(ControllableContainer *);

	void buttonClicked(Button * b);
};


class GenericControllableContainerEditor : public InspectorEditor, public ButtonListener
{
public :
	GenericControllableContainerEditor(InspectableComponent * sourceComponent);
	virtual ~GenericControllableContainerEditor();

	TextButton parentBT;

	ControllableContainer * sourceContainer;
	ScopedPointer<CCInnerContainer> innerContainer;

	void setCurrentInspectedContainer(ControllableContainer *);
	
	void resized() override;
	void clear() override;

	void buttonClicked(Button  *b);
	

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GenericControllableContainerEditor)
};


#endif  // GENERICCONTROLLABLECONTAINEREDITOR_H_INCLUDED
