/*
  ==============================================================================

    JavascriptControllerEditor.cpp
    Created: 10 May 2016 8:01:18pm
    Author:  bkupe

  ==============================================================================
*/

#include "JavascriptControllerEditor.h"
#include "JsEnvironmentUI.h"
#include "JsEnvironment.h"

JavascriptControllerEditor::JavascriptControllerEditor(JavascriptControllerUI * jsControllerUI) :
	OSCControllerEditor(jsControllerUI),
	jsController(jsControllerUI->jsController)
{
	scriptUI = new JsEnvironmentUI(jsController);
	addAndMakeVisible(scriptUI);
}

JavascriptControllerEditor::~JavascriptControllerEditor()
  {
  }

void JavascriptControllerEditor::resizedInternalOSC(Rectangle<int> r)
{
	scriptUI->setBounds(r);

}


JavascriptControllerUI(JsEnvironment * env) {
	oscUI = new OSCDirectControllerContentUI();

	addAndMakeVisible(oscUI);
	
}

void init() override {
	oscUI->controller = controller;
	oscUI->cui = cui;
	oscUI->init();
}



void resized()override {
	Rectangle<int> area = getLocalBounds();
	oscUI->setBounds(area);
}
ScopedPointer<OSCDirectControllerContentUI> oscUI;
ScopedPointer<JsEnvironmentUI> scriptUI;
