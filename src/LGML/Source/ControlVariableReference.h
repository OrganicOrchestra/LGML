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

class ControlVariableReference;

class  ControlVariableReferenceListener
{
public:
	virtual ~ControlVariableReferenceListener() {}
	virtual void askForRemoveReference(ControlVariableReference *) {};

	virtual void referenceVariableChanged(ControlVariableReference *) {};
	virtual void referenceVariableNameChanged(ControlVariableReference *) {};
	virtual void referenceValueChanged(ControlVariableReference *) {};
	virtual void referenceAliasChanged(ControlVariableReference *) {};
	
};


class ControlVariableReference : 
	public ControllableContainer,
	public ControlVariableListener
{
public:
	ControlVariableReference();
	virtual ~ControlVariableReference();

	StringParameter * alias;
	ControlVariable * currentVariable;
	
	var getValue();

	void setCurrentVariable(ControlVariable * v);

	void onContainerParameterChanged(Parameter * p) override;
	void parameterValueChanged(Parameter * p) override;
	void remove();

	virtual var getJSONData() override;
	virtual void loadJSONDataInternal(var data) override;
	
	virtual void variableRemoved(ControlVariable *) override;
	virtual void variableNameChanged(ControlVariable *) override;

	ListenerList<ControlVariableReferenceListener> referenceListeners;
	void addReferenceListener(ControlVariableReferenceListener* newListener) { referenceListeners.add(newListener); }
	void removeReferenceListener(ControlVariableReferenceListener* listener) { referenceListeners.remove(listener); }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlVariableReference)

};



#endif  // CONTROLVARIABLEREFERENCE_H_INCLUDED
