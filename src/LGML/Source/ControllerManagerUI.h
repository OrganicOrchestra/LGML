/*
  ==============================================================================

    ControllerManagerUI.h
    Created: 8 Mar 2016 10:13:00pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLERMANAGERUI_H_INCLUDED
#define CONTROLLERMANAGERUI_H_INCLUDED

#include "JuceHeader.h"
#include "ControlManager.h"
#include "ControllerUI.h"

//==============================================================================
/*
*/



class ControllerManagerUI : public ContourComponent, public ControllerManager::Listener
{
public:
    ControllerManagerUI(ControllerManager * manager);
    ~ControllerManagerUI();

    ControllerManager * manager;

    OwnedArray<ControllerUI> controllersUI;

    ControllerUI * addControllerUI(Controller * controller);
    void removeControllerUI(Controller * controller);

    ControllerUI * getUIForController(Controller * controller);

    void placeElements(); //update size when controllerUIs added or removed

    void paint (Graphics&)override;
    void resized()override;

    void mouseDown(const MouseEvent &e) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ControllerManagerUI)

    // Inherited via Listener
    virtual void controllerAdded(Controller *) override;
    virtual void controllerRemoved(Controller *) override;
};



class ControllerManagerViewport : public Viewport
{
public:
    ControllerManagerViewport(ControllerManager * controllerManager);

    ControllerManagerUI * cmui;

    void paint(Graphics &g) override;
    void resized() override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllerManagerViewport)
};

#endif  // CONTROLLERMANAGERUI_H_INCLUDED
