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
#include "ControlVariableUI.h"

class OSCCustomControllerEditor : public OSCControllerEditor, public ButtonListener, public Controller::ControllerListener
{
public :
	OSCCustomControllerEditor(OSCCustomControllerUI * controllerUI);
	virtual ~OSCCustomControllerEditor();

	OSCCustomController * customController;

	TextButton addVariableBT;
	OwnedArray<ControlVariableUI> variablesUI;

	void addVariableUI(ControlVariable *, bool doResize = true);
	void removeVariableUI(ControlVariable *, bool doResize = true);

	ControlVariableUI * getUIForVariable(ControlVariable *);

	void resized() override;

	int getContentHeight() override;

	void buttonClicked(Button * b) override;
	void variableAdded(Controller * , ControlVariable *) override;
	void variableRemoved(Controller *, ControlVariable *) override;

};


#endif  // OSCCUSTOMCONTROLLEREDITOR_H_INCLUDED
