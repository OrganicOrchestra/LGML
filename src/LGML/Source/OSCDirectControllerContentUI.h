/*
  ==============================================================================

    OSCDirectControllerContentUI.h
    Created: 8 Mar 2016 10:47:26pm
    Author:  bkupe

  ==============================================================================
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
