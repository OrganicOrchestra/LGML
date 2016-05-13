/*
  ==============================================================================

    ControlVariableReference.h
    Created: 12 May 2016 4:04:14pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLVARIABLEREFERENCE_H_INCLUDED
#define CONTROLVARIABLEREFERENCE_H_INCLUDED

#include "ControlVariable.h"
#include "ControllableContainer.h"

class ControlVariableReference : public ControllableContainer
{
public:
	ControlVariableReference();
	virtual ~ControlVariableReference();

	StringParameter * alias;

	ControlVariable * currentVariable;
	Parameter * referenceParam;
	
	void setCurrentVariable(ControlVariable * v);

	void remove();


	class  ControlVariableReferenceListener
	{
	public:
		virtual ~ControlVariableReferenceListener() {}
		virtual void askForRemoveReference(ControlVariableReference *) {};
		virtual void currentReferenceChanged(ControlVariableReference *) {};
	};


	ListenerList<ControlVariableReferenceListener> referenceListeners;
	void addReferenceListener(ControlVariableReferenceListener* newListener) { referenceListeners.add(newListener); }
	void removeReferenceListener(ControlVariableReferenceListener* listener) { referenceListeners.remove(listener); }
};



#endif  // CONTROLVARIABLEREFERENCE_H_INCLUDED
