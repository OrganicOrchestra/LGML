/*
  ==============================================================================

    SerialControllerEditor.h
    Created: 22 May 2016 4:51:02pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef SERIALCONTROLLEREDITOR_H_INCLUDED
#define SERIALCONTROLLEREDITOR_H_INCLUDED


#include "CustomEditor.h"
#include "SerialControllerUI.h"
#include "SerialUIHelper.h"
#include "JsEnvironmentUI.h"

class SerialControllerEditor :
	public CustomEditor, ComboBoxListener,
	public SerialController::SerialControllerListener,
	public ButtonListener,
	public Controller::ControllerListener
{
public:
	SerialControllerEditor(SerialControllerUI * controllerUI);
	virtual ~SerialControllerEditor();

	SerialDeviceChooser deviceChooser;

	SerialController * serialController;
	TextButton connectPortBT;

	OwnedArray<ParameterUI> variablesUI;

	void addVariableUI(ControlVariable *);
	void removeVariableUI(ControlVariable *);
	ParameterUI * getUIForVariable(ControlVariable *);

	void paint(Graphics &g) override;
	void resized() override;

	void updateConnectBTAndIndic();

	void comboBoxChanged(ComboBox *) override;
	void buttonClicked(Button *) override;

	void portOpened() override;
	void portClosed() override;
	void currentPortChanged() override;

	void variableAdded(Controller *, ControlVariable * ) override;
	void variableRemoved(Controller *, ControlVariable *) override;

};


#endif  // SERIALCONTROLLEREDITOR_H_INCLUDED
