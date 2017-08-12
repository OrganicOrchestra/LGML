/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#ifndef INSPECTABLECOMPONENT_H_INCLUDED
#define INSPECTABLECOMPONENT_H_INCLUDED

#include "JuceHeader.h"//keep

class ControllableContainer;
class Inspector;
class InspectorEditor;


class InspectableComponent : public Component
{
public:
	InspectableComponent(ControllableContainer * relatedContainer = nullptr, const String &inspectableType = "none");

	virtual ~InspectableComponent();

	Inspector * inspector;
	const String inspectableType;

	virtual InspectorEditor * createEditor();

	ControllableContainer * relatedControllableContainer;
	int recursiveInspectionLevel;
	bool canInspectChildContainersBeyondRecursion;

	bool paintBordersWhenSelected;
	bool bringToFrontOnSelect;

	bool isSelected;
	virtual void selectThis();
	virtual void setSelected(bool value);

	virtual void setSelectedInternal(bool value); //to be overriden


  //
   void paintOverChildren(juce::Graphics &g) override;
	//Listener
	class  InspectableListener
	{
	public:
		/** Destructor. */
		virtual ~InspectableListener() {}
		virtual void inspectableSelectionChanged(InspectableComponent *) {};
		virtual void inspectableRemoved(InspectableComponent *) {};
	};

	ListenerList<InspectableListener> listeners;
	void addInspectableListener(InspectableListener* newListener) { listeners.add(newListener); }
	void removeInspectableListener(InspectableListener* listener) { listeners.remove(listener); }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InspectableComponent)
};



#endif  // INSPECTABLECOMPONENT_H_INCLUDED
