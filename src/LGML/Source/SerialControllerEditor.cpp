/*
 ==============================================================================

 SerialControllerEditor.cpp
 Created: 22 May 2016 4:51:02pm
 Author:  bkupe

 ==============================================================================
 */

#include "SerialControllerEditor.h"
#include "FloatSliderUI.h"
//#include "SerialControllerUI.h"//keep

SerialControllerEditor::SerialControllerEditor(Controller * cont) :
ControllerEditor(cont,false)
{

  serialController = dynamic_cast<SerialController*>(cont);
  jassert(serialController);
  jsUI = new JsEnvironmentUI(serialController->jsParameters);
  serialController->addControllerListener(this);

  addAndMakeVisible(deviceChooser);
  deviceChooser.addListener(this);

  addAndMakeVisible(&connectPortBT);
  connectPortBT.addListener(this);

  addAndMakeVisible(jsUI);

  incomingToogle = serialController->logIncoming->createToggle();
  addAndMakeVisible(incomingToogle);

  serialController->addSerialControllerListener(this);

  updateConnectBTAndIndic();

  /*
   for (auto &v : serialController->variables)
   {
   addVariableUI(v);
   }
   */
}

SerialControllerEditor::~SerialControllerEditor()
{
  serialController->removeControllerListener(this);
  serialController->removeSerialControllerListener(this);
}

/*
 void SerialControllerEditor::addVariableUI(ControlVariable * v)
 {
	ParameterUI * vui = (ParameterUI *)v->parameter->createDefaultUI();
	variablesUI.add(vui);
	addAndMakeVisible(vui);
	resized();
 }

 void SerialControllerEditor::removeVariableUI(ControlVariable * v)
 {
	ParameterUI * vui = getUIForVariable(v);
	if (vui != nullptr)
	{
 removeChildComponent(vui);
 variablesUI.removeObject(vui);
	}
	resized();
 }

 ParameterUI * SerialControllerEditor::getUIForVariable(ControlVariable * v)
 {
	for (auto & vui : variablesUI)
	{
 if (vui->parameter == v->parameter) return vui;
	}
	return nullptr;
 }
 */

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

  Rectangle<int> r = area;
  r.removeFromTop(ControllerEditor::getContentHeight()+10);

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

/*
 void SerialControllerEditor::variableAdded(Controller *, ControlVariable *v)
 {
 
	addVariableUI(v);
 }
 
 void SerialControllerEditor::variableRemoved(Controller *, ControlVariable *v)
 {
	removeVariableUI(v);
 }
 */
