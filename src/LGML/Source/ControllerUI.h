/*
  ==============================================================================

    ControllerUI.h
    Created: 8 Mar 2016 10:46:01pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLERUI_H_INCLUDED
#define CONTROLLERUI_H_INCLUDED

#include "JuceHeader.h"

#include "Controller.h"
#include "InspectableComponent.h"
#include "BoolToggleUI.h"
#include "StringParameterUI.h"


class ControllerUI : public InspectableComponent, public ButtonListener
{
public:
    ControllerUI(Controller * controller);
    virtual ~ControllerUI();

	ScopedPointer<StringParameterUI> nameTF;
	ScopedPointer<BoolToggleUI> enabledBT;
	ImageButton removeBT;

    Controller * controller;

    virtual void paint(Graphics &g) override;
    virtual void resized() override;

	virtual void mouseDown(const MouseEvent &e) override;
	virtual void buttonClicked(Button *) override;

	virtual InspectorEditor * getEditor() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ControllerUI)
};


#endif  // CONTROLLERUI_H_INCLUDED
