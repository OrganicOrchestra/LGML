/*
  ==============================================================================

    JavascriptControllerEditor.cpp
    Created: 10 May 2016 8:01:18pm
    Author:  bkupe

  ==============================================================================
*/

#include "JavascriptControllerEditor.h"
#include "JsEnvironmentUI.h"


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




