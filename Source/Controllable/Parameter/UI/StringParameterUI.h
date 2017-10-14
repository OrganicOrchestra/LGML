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


#ifndef STRINGPARAMETERUI_H_INCLUDED
#define STRINGPARAMETERUI_H_INCLUDED


#include "ParameterUI.h"

class StringParameterUI : public ParameterUI, public Label::Listener
{
public:
    StringParameterUI (Parameter* p);
    virtual ~StringParameterUI() {};

    Label nameLabel;
    Label valueLabel;

    String prefix;
    String suffix;
    int arraySize;
    float maxFontHeight;

    bool autoSize;
    void setAutoSize (bool value);

    void setPrefix (const String& _prefix);
    void setSuffix (const String& _suffix);

    bool nameLabelIsVisible;
    void setNameLabelVisible (bool visible);
    void setBackGroundIsTransparent (bool t);


    void resized() override;


protected:
    void valueChanged (const var& v) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StringParameterUI)

    // Inherited via Listener
    virtual void labelTextChanged (Label* labelThatHasChanged) override;
};


#endif  // STRINGPARAMETERUI_H_INCLUDED
