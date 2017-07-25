/*
  ==============================================================================

    ParameterUIFactory.h
    Created: 24 Jul 2017 8:23:35pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once

#include "ParameterUI.h"
class Parameter;


class ParameterUIFactory{
public:
  // used for generating editor
  static ParameterUI * createDefaultUI(Parameter * targetControllable = nullptr);



};
