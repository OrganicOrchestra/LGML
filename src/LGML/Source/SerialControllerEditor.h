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
	public ButtonListener
{
public:
	SerialControllerEditor(SerialControllerUI * controllerUI);
	virtual ~SerialControllerEditor();

	SerialDeviceChooser deviceChooser;

	SerialController * serialController;
	TextButton connectPortBT;

	void paint(Graphics &g) override;
	void resized() override;

	void updateConnectBTAndIndic();

	void comboBoxChanged(ComboBox *) override;
	void buttonClicked(Button *) override;

	void portOpened() override;
	void portClosed() override;
	void currentPortChanged() override;


};


#endif  // SERIALCONTROLLEREDITOR_H_INCLUDED
