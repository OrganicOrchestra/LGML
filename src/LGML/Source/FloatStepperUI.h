/*
  ==============================================================================

    FloatStepperUI.h
    Created: 28 Apr 2016 6:00:28pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef FLOATSTEPPERUI_H_INCLUDED
#define FLOATSTEPPERUI_H_INCLUDED

#include "ParameterUI.h"
#include "BetterStepper.h"

class FloatStepperUI : public ParameterUI, private Slider::Listener,private ComponentListener
{

public:
    FloatStepperUI(Parameter * _parameter);
    virtual ~FloatStepperUI();

    ScopedPointer<BetterStepper> slider;

  void setScrollAllowed(bool );

    void resized() override;
protected:
    void valueChanged(const var &) override;
    void rangeChanged(Parameter * p) override;

    // Inherited via Listener
    virtual void sliderValueChanged(Slider * slider) override;

  void componentParentHierarchyChanged (Component&)override;

};


#endif  // FLOATSTEPPERUI_H_INCLUDED
