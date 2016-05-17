/*
  ==============================================================================

    ControlVariable.h
    Created: 10 May 2016 3:17:16pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLVARIABLE_H_INCLUDED
#define CONTROLVARIABLE_H_INCLUDED

#include "Parameter.h"

class Controller;
class ControlVariable;

class  ControlVariableListener
{
public:
	/** Destructor. */
	virtual ~ControlVariableListener() {}
	
	virtual void variableNameChanged(ControlVariable *) {};

	virtual void askForRemoveVariable(ControlVariable *) {};
	virtual void variableRemoved(ControlVariable *) {};

};

class ControlVariable :
	public Controllable::Listener
{
public :
	ControlVariable(Controller * c, Parameter * p);
	virtual ~ControlVariable();

	Controller * controller;
	ScopedPointer<Parameter> parameter;

	void remove();

	virtual void controllableNameChanged(Controllable *) override;

	ListenerList<ControlVariableListener> variableListeners;
	void addControlVariableListener(ControlVariableListener* newListener) { variableListeners.add(newListener); }
	void removeControlVariableListener(ControlVariableListener* listener) { variableListeners.remove(listener); }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControlVariable)
};



#endif  // CONTROLVARIABLE_H_INCLUDED
