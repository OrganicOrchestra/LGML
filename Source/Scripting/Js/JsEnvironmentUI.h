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


#ifndef JSENVIRONMENTUI_H_INCLUDED
#define JSENVIRONMENTUI_H_INCLUDED


#include "JsEnvironment.h"
#include "../../Controllable/Parameter/UI/ParameterUI.h"

class JsEnvironmentUI : public juce::Component
{
public:
    JsEnvironmentUI (JSEnvContainer* _env);
    ~JsEnvironmentUI();

    ScopedPointer<ParameterUI> filePathUI;
    ScopedPointer<ParameterUI> logEnvB;

    
    void resized()override;

    void buildLed (int size);

    

    WeakReference<JSEnvContainer> cont;
    JsEnvironment* env;
};



#endif  // JSENVIRONMENTUI_H_INCLUDED
