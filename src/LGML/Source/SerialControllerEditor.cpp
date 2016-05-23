/*
  ==============================================================================

    SerialControllerEditor.cpp
    Created: 22 May 2016 4:51:02pm
    Author:  bkupe

  ==============================================================================
*/

#include "SerialControllerEditor.h"

SerialControllerEditor::SerialControllerEditor(SerialControllerUI * controllerUI) :
	CustomEditor(controllerUI),
	serialController(controllerUI->serialController)
{
	addAndMakeVisible(deviceChooser);
	deviceChooser.addListener(this);



	addAndMakeVisible(&connectPortBT);
	connectPortBT.addListener(this);

	serialController->addSerialControllerListener(this);

	updateConnectBTAndIndic();
}

SerialControllerEditor::~SerialControllerEditor()
{
	serialController->removeSerialControllerListener(this);
}

void SerialControllerEditor::paint(Graphics & g)
{
	Colour c = NORMAL_COLOR;
	if (serialController->port != nullptr && serialController->port->isOpen())
	{
		c = Colours::lightgreen;
	}
	g.setColour(c);
	g.fillEllipse(connectPortBT.getBounds().withWidth(20).translated(connectPortBT.getRight()+5, 0).reduced(4).toFloat());
}

void SerialControllerEditor::resized()
{
	Rectangle<int> r = getLocalBounds();
	deviceChooser.setBounds(r.removeFromTop(20));
	r.removeFromTop(2);
	Rectangle<int> connectR = r.removeFromTop(20);
	connectR.removeFromRight(40);
	connectPortBT.setBounds(connectR);
}

void SerialControllerEditor::updateConnectBTAndIndic()
{
	repaint();

	connectPortBT.setEnabled(serialController->port != nullptr);

	if (serialController->port != nullptr)
	{
		connectPortBT.setButtonText(serialController->port->isOpen() ? "Disconnect" : "Connect");
		deviceChooser.setSelectedItemIndex(SerialManager::getInstance()->portInfos.indexOf(serialController->port->info) + 1,NotificationType::dontSendNotification);
	}
}

void SerialControllerEditor::comboBoxChanged(ComboBox *cb)
{
	if (cb == &deviceChooser)
	{
		if (deviceChooser.getSelectedId() >  1)
		{
			SerialPort * p = SerialManager::getInstance()->getPort(SerialManager::getInstance()->portInfos[deviceChooser.getSelectedId() - 2]);
			serialController->setCurrentPort(p);
		}
	}

	updateConnectBTAndIndic();
}

void SerialControllerEditor::buttonClicked(Button * b)
{
	if (b == &connectPortBT)
	{
		if (serialController->port != nullptr)
		{
			if (!serialController->port->isOpen()) serialController->port->open();
			else serialController->port->close();
		}

		updateConnectBTAndIndic();
	}
}

void SerialControllerEditor::portOpened()
{
	updateConnectBTAndIndic();
}

void SerialControllerEditor::portClosed()
{
	updateConnectBTAndIndic();
}

void SerialControllerEditor::currentPortChanged()
{
	updateConnectBTAndIndic();
}
