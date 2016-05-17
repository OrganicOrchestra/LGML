/*
  ==============================================================================

    JsEnvironmentUI.h
    Created: 10 May 2016 9:33:22am
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef JSENVIRONMENTUI_H_INCLUDED
#define JSENVIRONMENTUI_H_INCLUDED


#include "DebugHelpers.h"
#include "JsEnvironment.h"

class JsEnvironmentUI : public Component,public Button::Listener,public JsEnvironment::Listener{
public:
	JsEnvironmentUI(JsEnvironment * _env);
    ~JsEnvironmentUI(){
        env->removeListener(this);
    }

    TextButton loadFileB;
    TextButton reloadB;
    TextButton openB;
    TextButton logEnvB;
    TextButton watchT;
    DrawablePath validJsLed;
	void resized()override;

	void buildLed(int size);

	void newJsFileLoaded(bool s) override;
	void buttonClicked(Button* b) override;;

    JsEnvironment * env;
};



#endif  // JSENVIRONMENTUI_H_INCLUDED
