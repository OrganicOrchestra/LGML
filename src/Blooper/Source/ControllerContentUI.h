/*
  ==============================================================================

    ControllerContentUI.h
    Created: 8 Mar 2016 10:48:41pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLERCONTENTUI_H_INCLUDED
#define CONTROLLERCONTENTUI_H_INCLUDED

#include "ControllerUI.h"
#include "UIHelpers.h"

class ControllerContentUI : public ContourComponent
{
public:
    ControllerContentUI();

    Controller * controller;
    ControllerUI * cui;

    void setControllerAndUI(Controller * _controller, ControllerUI * _cui);
    virtual void init(); //override this function for proper init with controller and UI

    virtual void mouseDown(const MouseEvent &e) override
    {
        if (e.mods.isCtrlDown()) controller->remove();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllerContentUI)
};



#endif  // CONTROLLERCONTENTUI_H_INCLUDED
