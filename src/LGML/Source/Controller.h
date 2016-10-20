/*
  ==============================================================================

    Controller.h
    Created: 2 Mar 2016 8:49:50pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLER_H_INCLUDED
#define CONTROLLER_H_INCLUDED


#include "ControllableContainer.h"
#include "ControlVariable.h"

class ControllerUI;

class Controller :
	public ControllableContainer,
	public ControlVariableListener
{
public:
    Controller(const String &name = "[Controller]");
    virtual ~Controller();

    int controllerTypeEnum;

    StringParameter * nameParam;
    BoolParameter * enabledParam;

	Trigger * activityTrigger;

	OwnedArray<ControlVariable> variables; // These are values that can be set only by the external controller (osc, midi, serial...).
										   // they are stored so they can be used by other mechanisms in the software, such as rules.

	ControlVariable * addVariable(Parameter * p);
	void removeVariable(ControlVariable * variable);

	ControlVariable * getVariableForAddress(const String &address);
	ControlVariable * getVariableForName(const String &name);
    virtual void internalVariableAdded(ControlVariable * ){};
    virtual void internalVariableRemoved(ControlVariable * ){};
    void remove(); //will dispatch askForRemoveController
    virtual void onContainerParameterChanged(Parameter * p) override;
  virtual void onContainerTriggerTriggered(Trigger * ) override;

	void askForRemoveVariable(ControlVariable * variable)override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;


	//helper
	String getUniqueVariableNameFor(const String &baseName, int index = 1);


	virtual ControllerUI * createUI();

    class  ControllerListener
    {
    public:
        /** Destructor. */
        virtual ~ControllerListener() {}

		virtual void askForRemoveController(Controller *) {}

		virtual void variableAdded(Controller *, ControlVariable * ) {}
		virtual void variableRemoved(Controller *, ControlVariable * ) {}
    };

    ListenerList<ControllerListener> controllerListeners;
    void addControllerListener(ControllerListener* newListener) { controllerListeners.add(newListener); }
    void removeControllerListener(ControllerListener* listener) { controllerListeners.remove(listener); }


    // identifiers
    static const Identifier controllerTypeIdentifier;
  static const Identifier variableNameIdentifier;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Controller)
};



#endif  // CONTROLLER_H_INCLUDED
