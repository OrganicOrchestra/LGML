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

class ControlVariable
{
public :
	ControlVariable(Parameter * p);
	virtual ~ControlVariable();

	ScopedPointer<Parameter> parameter;

	void remove();

	class  VariableListener
	{
	public:
		/** Destructor. */
		virtual ~VariableListener() {}
		virtual void askForRemoveVariable(ControlVariable *) {};
	};

	ListenerList<VariableListener> variableListeners;
	void addOSCControllerListener(VariableListener* newListener) { variableListeners.add(newListener); }
	void removeOSCControllerListener(VariableListener* listener) { variableListeners.remove(listener); }
};



#endif  // CONTROLVARIABLE_H_INCLUDED
