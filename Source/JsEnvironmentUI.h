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
#include "ParameterUI.h"
class StringParameterUI;

class JsEnvironmentUI : public juce::Component,public JsEnvironment::Listener{
public:
	JsEnvironmentUI(JSEnvContainer * _env);
  ~JsEnvironmentUI();

    ScopedPointer<ParameterUI> loadFileB;
    ScopedPointer<ParameterUI>  reloadB;
    ScopedPointer<ParameterUI> openB;
    ScopedPointer<ParameterUI> logEnvB;
    ScopedPointer<ParameterUI> watchT;
  ScopedPointer<StringParameterUI> path;
    DrawablePath validJsLed;
	void resized()override;

	void buildLed(int size);

	void newJsFileLoaded(bool s) override;

  JSEnvContainer * cont;
    JsEnvironment * env;
};



#endif  // JSENVIRONMENTUI_H_INCLUDED
