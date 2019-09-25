/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#ifndef CONTROLLER_H_INCLUDED
#define CONTROLLER_H_INCLUDED


#include "../Controllable/Parameter/ParameterContainer.h"


class ControllerUI;
class ControllerEditor;

constexpr int activityTriggerDebounceTime =50;
class Controller :
    public ParameterContainer
{
public:


    Controller (StringRef );
    virtual ~Controller();
    int controllerTypeEnum;

    BoolParameter* enabledParam;
    BoolParameter *isConnected;

    Trigger* inActivityTrigger,*outActivityTrigger;
    BoolParameter* blockFeedback; 
    ParameterContainer  userContainer;



    virtual void internalVariableAdded ( ParameterBase* ) {};
    virtual void internalVariableRemoved ( ParameterBase* ) {};
    virtual void onContainerParameterChanged ( ParameterBase* p) override;
    virtual void onContainerTriggerTriggered (Trigger* ) override;



    

    void remove();

    void setMappingMode(bool state) ;
    bool autoAddParams;

private:
    WeakReference<Controller>::Master masterReference;
    friend class WeakReference<Controller>;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Controller)
};



#endif  // CONTROLLER_H_INCLUDED
