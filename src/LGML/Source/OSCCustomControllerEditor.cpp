/*
  ==============================================================================

    OSCCustomControllerEditor.cpp
    Created: 10 May 2016 2:29:40pm
    Author:  bkupe

  ==============================================================================
*/

#include "OSCCustomControllerEditor.h"
#include "DebugHelpers.h"

OSCCustomControllerEditor::OSCCustomControllerEditor(OSCCustomControllerUI * controllerUI) :
	OSCControllerEditor(controllerUI),
	customController(controllerUI->customController)
{
	
}

OSCCustomControllerEditor::~OSCCustomControllerEditor()
{
	
}

