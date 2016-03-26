/*
  ==============================================================================

    StringParameterUI.h
    Created: 9 Mar 2016 12:29:57am
    Author:  bkupe

  ==============================================================================
*/

#ifndef STRINGPARAMETERUI_H_INCLUDED
#define STRINGPARAMETERUI_H_INCLUDED


#include "ParameterUI.h"
#include "StringParameter.h"

class StringParameterUI : public ParameterUI, public Label::Listener
{
public:
    StringParameterUI(Parameter * p);

    StringParameter * stringParam;
    Label nameLabel;
    Label valueLabel;

    bool nameLabelIsVisible;
    void setNameLabelVisible(bool visible);

    void resized() override;

protected:
    void parameterValueChanged(Parameter * p) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StringParameterUI)

        // Inherited via Listener
    virtual void labelTextChanged(Label * labelThatHasChanged) override;
};


#endif  // STRINGPARAMETERUI_H_INCLUDED
