/*
  ==============================================================================

    JavascriptControllerUI.cpp
    Created: 7 May 2016 7:28:40am
    Author:  Martin Hermant

  ==============================================================================
*/

#include "JavascriptControllerUI.h"
#include "JavascriptControllerEditor.h"

JavascriptControllerUI::JavascriptControllerUI(JavascriptController * _controller) :
	OSCControllerUI(_controller),
	jsController(_controller)
{
}

JavascriptControllerUI::~JavascriptControllerUI()
{
}

InspectorEditor * JavascriptControllerUI::getEditor()
{
	return new JavascriptControllerEditor(this);
}
