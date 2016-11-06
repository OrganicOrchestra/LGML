/*
  ==============================================================================

    OSCCustomControllerUI.cpp
    Created: 10 May 2016 2:29:34pm
    Author:  bkupe

  ==============================================================================
*/

#include "OSCCustomControllerUI.h"
#include "OSCCustomControllerEditor.h"

OSCCustomControllerUI::OSCCustomControllerUI(OSCCustomController * controller) :
	OSCControllerUI(controller),
	customController(controller)
{
}

OSCCustomControllerUI::~OSCCustomControllerUI()
  {
  }

InspectorEditor * OSCCustomControllerUI::getEditor()
{
	return new OSCCustomControllerEditor(this);
}
