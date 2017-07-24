/*
  ==============================================================================

    ControllableHelpers.h
    Created: 12 May 2016 4:21:18pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLABLEHELPERS_H_INCLUDED
#define CONTROLLABLEHELPERS_H_INCLUDED

#include "JuceHeader.h"//keep
class ControllableContainer;
class Controllable;


class ControllableContainerPopupMenu : public PopupMenu
{
public:
	ControllableContainerPopupMenu(ControllableContainer * rootContainer);
	virtual ~ControllableContainerPopupMenu();

	Array<Controllable *> controllableList;
	void populateMenu(PopupMenu *subMenu, ControllableContainer * container, int &currentId);


	Controllable * showAndGetControllable();

};


class ControllableReferenceUI : public TextButton, public ButtonListener
{
public :
	ControllableReferenceUI(ControllableContainer * rootContainer = nullptr);
	virtual ~ControllableReferenceUI();

	WeakReference<ControllableContainer> rootContainer;

	WeakReference<Controllable> currentControllable;
	void setCurrentControllale(Controllable * c);

  void paintOverChildren(Graphics &g)override;

  void setHovered(bool b);

  bool isDragCandidate;

	class  Listener
	{
	public:
		/** Destructor. */
		virtual ~Listener() {}
		virtual void choosedControllableChanged(ControllableReferenceUI*,Controllable *) {};
	};

	ListenerList<Listener> listeners;
	void addControllableReferenceUIListener(Listener* newListener) { listeners.add(newListener); }
	void removeControllableReferenceUIListener(Listener* listener) { listeners.remove(listener); }

	virtual void buttonClicked(Button * b) override;
};

#endif  // CONTROLLABLEHELPERS_H_INCLUDED
