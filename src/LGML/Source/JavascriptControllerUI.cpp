/*
  ==============================================================================

    JavascriptControllerUI.cpp
    Created: 7 May 2016 7:28:40am
    Author:  Martin Hermant

  ==============================================================================
*/

#include "JavascriptControllerUI.h"

// @ ben this structure in general avoid cyclic dependencyes and unnecessary includes in non UI class
// should we change every thing (compile time should decrease a bit..)

ControllerUI * JavascriptController::createUI(){
    return new ControllerUI(this, new JavascriptControllerUI(this));
}