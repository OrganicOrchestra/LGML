/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

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

    ParameterContainer  userContainer;



    virtual void internalVariableAdded (Parameter* ) {};
    virtual void internalVariableRemoved (Parameter* ) {};
    virtual void onContainerParameterChanged (Parameter* p) override;
    virtual void onContainerTriggerTriggered (Trigger* ) override;



    

    void remove();

    void setMappingMode(bool state) ;
    bool autoAddParams;




    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Controller)
};



#endif  // CONTROLLER_H_INCLUDED
