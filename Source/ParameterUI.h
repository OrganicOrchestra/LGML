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


#ifndef PARAMETERUI_H_INCLUDED
#define PARAMETERUI_H_INCLUDED

#include "Parameter.h"
#include "ControllableUI.h"

class ParameterUI : protected Parameter::AsyncListener, private Parameter::Listener,public ControllableUI
{
public:
  ParameterUI(Parameter * parameter);
  virtual ~ParameterUI();

  WeakReference<Parameter> parameter;

  bool showLabel;
  bool showValue;

  void setCustomText(const String text);
protected:

  String customTextDisplayed;
  // helper to spot wrong deletion order
  bool shouldBailOut();

  // here we are bound to only one parameter so no need to pass parameter*
  // for general behaviour see AsyncListener
  virtual void valueChanged(const var & ){};
  virtual void rangeChanged(Parameter * ){};
private:
  // see Parameter::AsyncListener
  virtual void newMessage(const Parameter::ParamWithValue & p) override{
    if(p.isRange()){
      rangeChanged(p.parameter);
    }
    else{
      valueChanged(p.value);
    }
  };

  // never change this as value can be changed from other threads
  void parameterValueChanged(Parameter * ) override{};
  void parameterRangeChanged(Parameter * )override{};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterUI)
};


#endif  // PARAMETERUI_H_INCLUDED
