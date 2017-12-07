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


#ifndef CONTROLLERMANAGERUI_H_INCLUDED
#define CONTROLLERMANAGERUI_H_INCLUDED


#include "../ControllerManager.h"
#include "ControllerUI.h"

#include "../../UI/ShapeShifter/ShapeShifterContent.h"
#include "../../UI/Style.h"
//==============================================================================
/*
*/

class ControllerManagerUI : public juce::Component, ControllerManager::Listener
{
public:
    ControllerManagerUI (ControllerManager* manager);
    ~ControllerManagerUI();

    ControllerManager* manager;

    OwnedArray<ControllerUI> controllersUI;

    ControllerUI* addControllerUI (Controller* controller);
    void removeControllerUI (Controller* controller,bool notify=true);

    ControllerUI* getUIForController (Controller* controller);

    void paint (Graphics&)override;
    void resized()override;

    void mouseDown (const MouseEvent& e) override;

    int getContentHeight() const;

    void clear(bool notify=true);
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControllerManagerUI)

    // Inherited via Listener
    void controllerAdded (Controller*) override;
    void controllerRemoved (Controller*) override;
    void notifyParentViewPort() ;
};


class ControllerManagerUIViewport :
    public ShapeShifterContentComponent,
    private ButtonListener
{
public:
    ControllerManagerUIViewport (const String& contentName, ControllerManagerUI* _UI) ;
    virtual ~ControllerManagerUIViewport();

    void resized() override;

    void buttonClicked (Button*) override;

    Viewport vp;
    ControllerManagerUI* controllerManagerUI;
    AddElementButton addControllerBt;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControllerManagerUIViewport)
};
#endif  // CONTROLLERMANAGERUI_H_INCLUDED
