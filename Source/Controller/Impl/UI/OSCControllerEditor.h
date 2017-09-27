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


#ifndef OSCCONTROLLEREDITOR_H_INCLUDED
#define OSCCONTROLLEREDITOR_H_INCLUDED

class OSCControllerUI;
#include "../../UI/ControllerEditor.h"

#include "../../../Controllable/Parameter/UI/ParameterUI.h"
#include "../OSCController.h"

class OSCControllerEditor : public ControllerEditor, public OSCController::OSCControllerListener
{
public:
    OSCControllerEditor (OSCController* controller);
    virtual ~OSCControllerEditor();

    OSCController* oscController;

    ScopedPointer<ParameterUI> localPortUI;
    ScopedPointer<ParameterUI> remoteHostUI;
    ScopedPointer<ParameterUI> remotePortUI;
    ScopedPointer<ParameterUI> logIncomingOSC, logOutOSC;
    ScopedPointer<ParameterUI> speedLimit;
    ScopedPointer<ParameterUI> blockFeedback;
    ScopedPointer<ParameterUI> sendAllTrigger;


    Component innerContainer;

    virtual void resized() override;

    virtual int getContentHeight() override;

    // Inherited via OSCControllerListener
    virtual void messageProcessed (const OSCMessage& msg, bool success) override;
};


#endif  // OSCCONTROLLEREDITOR_H_INCLUDED
