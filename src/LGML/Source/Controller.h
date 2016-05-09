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
class ControllerUI;

class Controller : public ControllableContainer
{
public:
    Controller(const String &name = "[Controller]");
    virtual ~Controller();

    int controllerTypeEnum;

    StringParameter * nameParam;
    BoolParameter * enabledParam;

    void remove(); //will dispatch askForRemoveController
    virtual void onContainerParameterChanged(Parameter * p) override;

    virtual ControllerUI * createUI();

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

    class  ControllerListener
    {
    public:
        /** Destructor. */
        virtual ~ControllerListener() {}

        virtual void askForRemoveController(Controller *) = 0;
    };

    ListenerList<ControllerListener> controllerListeners;
    void addControllerListener(ControllerListener* newListener) { controllerListeners.add(newListener); }
    void removeControllerListener(ControllerListener* listener) { controllerListeners.remove(listener); }


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Controller)
};



#endif  // CONTROLLER_H_INCLUDED
