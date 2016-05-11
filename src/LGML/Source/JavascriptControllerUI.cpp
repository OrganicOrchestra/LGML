/*
  ==============================================================================

    JavascriptControllerUI.cpp
    Created: 7 May 2016 7:28:40am
    Author:  Martin Hermant

  ==============================================================================
*/

#include "JavascriptControllerUI.h"
#include "JavascriptControllerEditor.h"


// @ben I really like this i.e deporting create instanciation in UI class (it avoid poluting engine with UI include) what about you?
ControllerUI * JavascriptController::createUI()
{
    return new JavascriptControllerUI(this);
}


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
