/*
  ==============================================================================

    OSCCustomControllerEditor.h
    Created: 10 May 2016 2:29:40pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef OSCCUSTOMCONTROLLEREDITOR_H_INCLUDED
#define OSCCUSTOMCONTROLLEREDITOR_H_INCLUDED

#include "OSCControllerEditor.h"
#include "OSCCustomControllerUI.h"

class OSCCustomControllerEditor : public OSCControllerEditor
{
public :
	OSCCustomControllerEditor(OSCCustomControllerUI * controllerUI);
	virtual ~OSCCustomControllerEditor();

	OSCCustomController * customController;


};


#endif  // OSCCUSTOMCONTROLLEREDITOR_H_INCLUDED
