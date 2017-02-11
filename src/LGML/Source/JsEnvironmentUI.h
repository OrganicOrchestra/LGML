/*
  ==============================================================================

    JsEnvironmentUI.h
    Created: 10 May 2016 9:33:22am
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef JSENVIRONMENTUI_H_INCLUDED
#define JSENVIRONMENTUI_H_INCLUDED


#include "JsEnvironment.h"
#include "TriggerBlinkUI.h"
#include "BoolToggleUI.h"
#include "StringParameterUI.h"

class JsEnvironmentUI : public Component,public JsEnvironment::Listener{
public:
	JsEnvironmentUI(JSEnvContainer * _env);
    ~JsEnvironmentUI(){
        env->removeListener(this);
    }

    ScopedPointer<TriggerBlinkUI> loadFileB;
    ScopedPointer<TriggerBlinkUI>  reloadB;
    ScopedPointer<TriggerBlinkUI> openB;
    ScopedPointer<TriggerBlinkUI> logEnvB;
    ScopedPointer<BoolToggleUI> watchT;
  ScopedPointer<StringParameterUI> path;
    DrawablePath validJsLed;
	void resized()override;

	void buildLed(int size);

	void newJsFileLoaded(bool s) override;

  JSEnvContainer * cont;
    JsEnvironment * env;
};



#endif  // JSENVIRONMENTUI_H_INCLUDED
