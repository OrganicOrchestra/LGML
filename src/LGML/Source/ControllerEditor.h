/*
  ==============================================================================

    ControllerEditor.h
    Created: 10 May 2016 9:31:40am
    Author:  bkupe

  ==============================================================================
*/

#ifndef CONTROLLEREDITOR_H_INCLUDED
#define CONTROLLEREDITOR_H_INCLUDED

#include "CustomEditor.h"
#include "ControllerUI.h"

class ControllerEditor : public CustomEditor
{
public:
	ControllerEditor(ControllerUI * controllerUI);
	virtual ~ControllerEditor();

	Controller * controller;
};


#endif  // CONTROLLEREDITOR_H_INCLUDED
