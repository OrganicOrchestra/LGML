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


#ifndef OSCDIRECTCONTROLLERCONTENTUI_H_INCLUDED
#define OSCDIRECTCONTROLLERCONTENTUI_H_INCLUDED


#include "OSCDirectController.h"
#include "StringParameterUI.h"
#include "TriggerBlinkUI.h"

class OSCDirectControllerContentUI :public  OSCDirectController::OSCDirectListener
{
public:
    OSCDirectControllerContentUI();
    virtual ~OSCDirectControllerContentUI();
    OSCDirectController * oscd;

    ScopedPointer<StringParameterUI> localPortUI;
    ScopedPointer<StringParameterUI> remoteHostUI;
    ScopedPointer<StringParameterUI> remotePortUI;


    Trigger activityTrigger;
    ScopedPointer<TriggerBlinkUI> activityTriggerUI;

    StringArray activityLines;
    Label activityLog;



    void resized() override;
    void mouseDown(const MouseEvent &e) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSCDirectControllerContentUI)

    // Inherited via OSCDirectListener
    virtual void messageProcessed(const OSCMessage & msg, Result success) override;
};


#endif  // OSCDIRECTCONTROLLERCONTENTUI_H_INCLUDED
