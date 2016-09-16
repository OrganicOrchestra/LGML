/*
 ==============================================================================

 ConnectableNodeHeaderUI.cpp
 Created: 8 Mar 2016 5:53:52pm
 Author:  bkupe

 ==============================================================================
 */
#include "ConnectableNodeHeaderUI.h"
#include "ConnectableNodeUI.h"

#include "PresetManager.h"
#include "FloatSliderUI.h"

ConnectableNodeHeaderUI::ConnectableNodeHeaderUI() :
vuMeterIn(VuMeter::Type::IN),
vuMeterOut(VuMeter::Type::OUT),
removeBT("X"),
miniModeBT("-"),
miniMode(false)
{
  node = nullptr;
  nodeUI = nullptr;

  Image removeImage = ImageCache::getFromMemory(BinaryData::removeBT_png, BinaryData::removeBT_pngSize);

  removeBT.setImages(false, true, true, removeImage,
                     0.7f, Colours::transparentBlack,
                     removeImage, 1.0f, Colours::transparentBlack,
                     removeImage, 1.0f, Colours::white.withAlpha(.7f),
                     0.5f);
  removeBT.addListener(this);

  miniModeBT.addListener(this);


  setSize(20, 40);
}

ConnectableNodeHeaderUI::~ConnectableNodeHeaderUI()
{
  if (node != nullptr)
  {
    node->removeRMSListener(&vuMeterOut);
    node->removeRMSListener(&vuMeterIn);

    node->removeControllableContainerListener(this);
    node->removeConnectableNodeListener(this);
    if(NodeBase *kk = dynamic_cast<NodeBase*>(node)){kk->removeNodeBaseListener(this);}
  }

}

void ConnectableNodeHeaderUI::setNodeAndNodeUI(ConnectableNode * _node, ConnectableNodeUI * _nodeUI)
{
  this->node = _node;
  this->nodeUI = _nodeUI;

  if (node != nullptr)
  {
    if(NodeBase *kk = dynamic_cast<NodeBase*>(node)){kk->addNodeBaseListener(this);}
    updateVuMeters();

  }

  titleUI = node->nameParam->createStringParameterUI();
  addAndMakeVisible(titleUI);

  descriptionUI = node->descriptionParam->createStringParameterUI();
  addAndMakeVisible(descriptionUI);
  descriptionUI->valueLabel.setColour(Label::ColourIds::textColourId, TEXT_COLOR.darker(.3f));

  enabledUI = node->enabledParam->createToggle();
  addAndMakeVisible(enabledUI);

  addAndMakeVisible(grabber);

  if(node->canBeRemovedByUser) addAndMakeVisible(removeBT);

  addAndMakeVisible(miniModeBT);

  presetCB = new ComboBox("preset");
  if (node->canHavePresets)
  {
    updatePresetComboBox();
    addAndMakeVisible(presetCB);
    presetCB->addListener(this);
    presetCB->setTextWhenNothingSelected("Preset");
    presetCB->setTooltip("Set the current preset at :\n" + node->currentPresetName->getControlAddress() + " <presetName>");
  }

  node->addControllableContainerListener(this);
  node->addConnectableNodeListener(this);

  init();
  resized();

}

void ConnectableNodeHeaderUI::updateVuMeters(){
  if (!vuMeterOut.isVisible() && node->hasAudioOutputs()) {
    node->addRMSListener(&vuMeterOut);
    addAndMakeVisible(vuMeterOut);

  }
  else if(vuMeterOut.isVisible() && !node->hasAudioOutputs()){
    node->removeRMSListener(&vuMeterOut);
    vuMeterOut.setVisible(false);
  }

  if (!vuMeterIn.isVisible() &&node->hasAudioInputs())
  {
    node->addRMSListener(&vuMeterIn);
    addAndMakeVisible(vuMeterIn);
  }
  else if(vuMeterIn.isVisible() && !node->hasAudioInputs()){
    node->removeRMSListener(&vuMeterIn);
    vuMeterIn.setVisible(false);
  }
}

void ConnectableNodeHeaderUI::updatePresetComboBox()
{

  bool emptyFilter = node->getPresetFilter().isEmpty();
  presetCB->setEnabled(!emptyFilter);

  if (!emptyFilter)
  {
    PresetManager::getInstance()->fillWithPresets(presetCB, node->getPresetFilter());
    if (node->currentPreset != nullptr) presetCB->setSelectedId(node->currentPreset->presetId, NotificationType::dontSendNotification);
  }
}

void ConnectableNodeHeaderUI::init()
{
  //to override
}

void ConnectableNodeHeaderUI::resized()
{
  if (!node) return;

  int vuMeterWidth = 8;
  int miniModeBTWidth = 15;
  int removeBTWidth = 15;
  int grabberHeight = 12;
  int presetCBWidth = 80;


  Rectangle<int> r = getLocalBounds();

  grabber.setBounds(r.removeFromTop(grabberHeight));
  r.reduce(4, 0);
  r.removeFromTop(2);

  if (node->hasAudioOutputs())
  {
    vuMeterOut.setBounds(r.removeFromRight(vuMeterWidth));
  }

  if (node->hasAudioInputs())
  {
    vuMeterIn.setBounds(r.removeFromLeft(vuMeterWidth));

  }

  r.reduce(5, 2);

  enabledUI->setBounds(r.removeFromLeft(10).reduced(0,2));

  r.removeFromLeft(3);
  if (node->canBeRemovedByUser)
  {
    removeBT.setBounds(r.removeFromRight(removeBTWidth));
    r.removeFromRight(2);
  }

  miniModeBT.setBounds(r.removeFromRight(miniModeBTWidth).reduced(0,2));
  r.removeFromRight(2);

  if (node->canHavePresets && !miniMode)
  {
    presetCB->setBounds(r.removeFromRight(presetCBWidth));
    r.removeFromRight(5);
  }


  titleUI->setBounds(r.removeFromTop(12));
  descriptionUI->setBounds(r);
}

void ConnectableNodeHeaderUI::setMiniMode(bool value)
{
  if (miniMode == value) return;
  miniMode = value;

  if (miniMode)
  {
    if(node->canHavePresets) removeChildComponent(presetCB);
    miniModeBT.setButtonText("+");
  } else
  {
    if (node->canHavePresets) addChildComponent(presetCB);
    miniModeBT.setButtonText("-");
  }
}

void ConnectableNodeHeaderUI::nodeParameterChanged(ConnectableNode *, Parameter *p)
{
  if (p == node->enabledParam)
  {
    if (!node->enabledParam->boolValue())
    {
      vuMeterOut.setVoldB(0);

    }


    Colour c = !node->enabledParam->boolValue() ? Colour(0xff3cacd5) : Colours::lightgreen;
    Colour c2 = !node->enabledParam->boolValue() ? Colours::blue.brighter(.6f) : Colours::red;
    vuMeterIn.colorLow = c;
    vuMeterOut.colorLow = c;
    vuMeterIn.colorHigh = c2;
    vuMeterOut.colorHigh = c2;

      postCommandMessage(repaintId);
  }

}

void ConnectableNodeHeaderUI::comboBoxChanged(ComboBox * cb)
{
  int presetID = cb->getSelectedId();

  if (presetID == PresetChoice::SaveCurrent)
  {
    bool result = node->saveCurrentPreset();
    if(result) cb->setSelectedId(node->currentPreset->presetId, NotificationType::dontSendNotification);
    else cb->setSelectedItemIndex(-1, NotificationType::dontSendNotification);

  }else if (presetID == PresetChoice::SaveToNew)
  {
    AlertWindow nameWindow("Save a new Preset","Choose a name for the new preset",AlertWindow::AlertIconType::QuestionIcon,this);
    nameWindow.addTextEditor("newPresetName", "New Preset");
    nameWindow.addButton("OK", 1, KeyPress(KeyPress::returnKey));
    nameWindow.addButton("Cancel", 0, KeyPress(KeyPress::escapeKey));

    int nameResult = nameWindow.runModalLoop();

    if (nameResult)
    {
      String presetName = nameWindow.getTextEditorContents("newPresetName");
      PresetManager::Preset * p = node->saveNewPreset(presetName);
      cb->clear(NotificationType::dontSendNotification);
      updatePresetComboBox();
      cb->setSelectedId(p->presetId, NotificationType::dontSendNotification);
    }
    else
    {
      cb->setSelectedItemIndex(-1, NotificationType::dontSendNotification);
    }


  }else if (presetID == PresetChoice::ResetToDefault) //Reset to default
  {
    node->resetFromPreset();
    cb->setSelectedItemIndex(-1, NotificationType::dontSendNotification);
  }
  else if(presetID >=0 && presetID < PresetChoice::deleteStartId)
  {
    PresetManager::Preset * pre = PresetManager::getInstance()->getPreset(node->getPresetFilter(), cb->getItemText(cb->getSelectedItemIndex()));
    node->loadPreset(pre);
  }
  else if (presetID >= PresetChoice::deleteStartId){
    PresetManager * pm =PresetManager::getInstance();
    int originId = cb->getSelectedId()-PresetChoice::deleteStartId - 1;
    String originText = cb->getItemText(pm->getNumOption() + originId);
    PresetManager::Preset * pre = pm->getPreset(node->getPresetFilter(), originText);
    pm->presets.removeObject(pre);
    updatePresetComboBox();


  }
  else{
      jassertfalse;
  }

}

void ConnectableNodeHeaderUI::buttonClicked(Button * b)
{
  if (b == &removeBT)
  {
    node->remove(true);
  } else if(b == &miniModeBT)
  {
    node->miniMode->setValue(!node->miniMode->boolValue());
  }
}

void ConnectableNodeHeaderUI::controllableContainerPresetLoaded(ControllableContainer *)
{

  if (!node->canHavePresets) return;

  //  int numOptions = PresetManager::getNumOption();
    if (node->currentPreset != nullptr) postCommandMessage(updatePresetCBID);
}

void ConnectableNodeHeaderUI::handleCommandMessage(int id){
    switch(id){
        case updatePresetCBID:
    
            if (!node->canHavePresets) return;
    
            //  int numOptions = PresetManager::getNumOption();
            if (node->currentPreset != nullptr)presetCB->setSelectedId(node->currentPreset->presetId, NotificationType::dontSendNotification);
            break;
            
        case repaintId:
            repaint();
            break;
        default:
            break;
    }
}

void ConnectableNodeHeaderUI::Grabber::paint(Graphics & g)
{
  g.setColour(PANEL_COLOR.brighter(.2f));

  Rectangle<int> r = getLocalBounds().reduced(4);
  float left = (float)r.getTopLeft().x;
  float right = (float)r.getRight();
  g.drawHorizontalLine(r.getTopLeft().y, left, right);
  g.drawHorizontalLine(r.getCentreY(), left, right);
  g.drawHorizontalLine(r.getBottom(), left, right);
}

void ConnectableNodeHeaderUI::numAudioInputChanged(NodeBase *, int /*newNumInput*/) {updateVuMeters();resized();}
void ConnectableNodeHeaderUI::numAudioOutputChanged(NodeBase *, int /*newNumOutput*/) {updateVuMeters();resized();}
