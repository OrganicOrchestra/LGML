/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#include "SerialControllerEditor.h"
#include "../../../Controllable/Parameter/UI/SliderUI.h"
#include "../../../Controllable/Parameter/UI/ParameterUIFactory.h"
#include "../../../UI/Style.h"

//#include "SerialControllerUI.h"//keep

SerialControllerEditor::SerialControllerEditor(SerialController * cont) :
ControllerEditor(cont,false),
serialController(cont)
{
  jsUI = new JsEnvironmentUI(serialController->jsParameters);


  addAndMakeVisible(deviceChooser);
  deviceChooser.addListener(this);

  addAndMakeVisible(&connectPortBT);
  connectPortBT.addListener(this);

  addAndMakeVisible(jsUI);

  incomingToogle = ParameterUIFactory::createDefaultUI(serialController->logIncoming);
  addAndMakeVisible(incomingToogle);

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
  ControllerEditor::resized();

  Rectangle<int> r = getLocalBounds();
//  r.removeFromTop(ControllerEditor::getContentHeight()+10);

  incomingToogle->setBounds(r.removeFromTop(20));
  jsUI->setBounds(r.removeFromTop(40));
  deviceChooser.setBounds(r.removeFromTop(20));
  r.removeFromTop(2);

  Rectangle<int> connectR = r.removeFromTop(20);
  connectR.removeFromRight(40);
  connectPortBT.setBounds(connectR);

  /*
   r.removeFromTop(20);

   for (auto &vui : variablesUI)
   {
   vui->setBounds(r.removeFromTop(10));
   r.removeFromTop(2);
   }
   */
}


int SerialControllerEditor::getContentHeight()
{
  return ControllerEditor::getContentHeight() + 10 + 150;
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
  else{
    connectPortBT.setButtonText( "No Serial");
  }
}

void SerialControllerEditor::comboBoxChanged(ComboBox *cb)
{
  if (cb == &deviceChooser)
  {
    DBG("Device chooser changed, selectedId = " + String(deviceChooser.getSelectedId()));
    if (deviceChooser.getSelectedId() >= 2)
    {
      if(SerialManager * smi = SerialManager::getInstance()){
        SerialPort * p = smi->getPort(SerialManager::getInstance()->portInfos[deviceChooser.getSelectedId() - 2]);
        serialController->setCurrentPort(p);
      }
    }
  }

  updateConnectBTAndIndic();
}

void SerialControllerEditor::buttonClicked(Button * b)
{
  ControllerEditor::buttonClicked(b);

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
