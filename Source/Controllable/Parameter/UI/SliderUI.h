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


#ifndef SliderUI_H_INCLUDED
#define SliderUI_H_INCLUDED

#include "ParameterUI.h"

template<class T>
class SliderUI    : public ParameterUI ,private Label::Listener
{

public:
    SliderUI ( ParameterBase* parameter = nullptr);
    virtual ~SliderUI();

    enum Direction { HORIZONTAL, VERTICAL };

    //settings
    Direction orientation;
    Colour defaultColor;

    bool changeParamOnMouseUpOnly;
    bool assignOnMousePosDirect;
    float scaleFactor;

    int fixedDecimals;

    //interaction
    float initValue;

    void paint (Graphics& g) override;
    void resized() override;
    void mouseDown (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;
    void mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel) override;

    T getValueFromMouse();
    T getValueFromPosition (const Point<int>& pos);

    virtual void setParamNormalizedValue (float value);
    virtual float getParamNormalizedValue();
    void rangeChanged ( ParameterBase* )override
    {
        repaint();
    };


protected:

    void processUICommand(int ) override;
    const UICommandType &  getUICommands( ) const override;

    void valueChanged (const var&) override ;
    bool scrollWheelAllowed;
    Time lastMouseWheelTime;
    ScopedPointer<Label> valueBox;
    void labelTextChanged (Label* labelThatHasChanged) override;
    void editorHidden (Label*, TextEditor&) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SliderUI)
};

typedef SliderUI<floatParamType> FloatSliderUI;
typedef SliderUI<int> IntSliderUI;
#endif  // SliderUI_H_INCLUDED
