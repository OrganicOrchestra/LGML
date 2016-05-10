/*
 ==============================================================================

 JavascriptControllerUI.h
 Created: 7 May 2016 7:28:40am
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef JAVASCRIPTCONTROLLERUI_H_INCLUDED
#define JAVASCRIPTCONTROLLERUI_H_INCLUDED
#include "JavascriptController.h"
#include "OSCDirectControllerContentUI.h"
#include "JsEnvironmentUI.h"


class JavascriptControllerUI : public ControllerContentUI{
public:

    JavascriptControllerUI(JsEnvironment * env){
        oscUI = new OSCDirectControllerContentUI();

        scriptUI = new JsEnvironmentUI(env);
        addAndMakeVisible(oscUI);
        addAndMakeVisible(scriptUI);
    }

    void init() override{
        oscUI->controller = controller;
        oscUI->cui = cui;
        oscUI->init();
    }



    void resized()override{
        Rectangle<int> area = getLocalBounds();
        scriptUI->setBounds(area.removeFromTop(30));
        oscUI->setBounds(area);
    }
    ScopedPointer<OSCDirectControllerContentUI> oscUI;
    ScopedPointer<JsEnvironmentUI> scriptUI;
    
    
};




#endif  // JAVASCRIPTCONTROLLERUI_H_INCLUDED
