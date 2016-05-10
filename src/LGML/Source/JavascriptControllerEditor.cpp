/*
  ==============================================================================

    JavascriptControllerEditor.cpp
    Created: 10 May 2016 8:01:18pm
    Author:  bkupe

  ==============================================================================
*/

#include "JavascriptControllerEditor.h"

JavascriptControllerEditor::JavascriptControllerEditor(JavascriptControllerUI * jsControllerUI) :
	OSCControllerEditor(jsControllerUI),
	jsController(jsControllerUI->jsController)
{
}

JavascriptControllerEditor::~JavascriptControllerEditor()
  {
  }
