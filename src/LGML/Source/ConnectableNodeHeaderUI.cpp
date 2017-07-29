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

#include "VuMeter.h"
#include "ParameterUIFactory.h"

ConnectableNodeHeaderUI::ConnectableNodeHeaderUI() :
removeBT("X"),
miniModeBT("-"),
bMiniMode(false)
{
  node = nullptr;
  nodeUI = nullptr;
  vuMeterIn = new VuMeter(VuMeter::Type::IN);
  vuMeterOut = new VuMeter(VuMeter::Type::OUT);
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
    node->removeRMSListener(vuMeterOut);
    node->removeRMSListener(vuMeterIn);

    node->removeControllableContainerListener(this);
    node->removeConnectableNodeListener(this);
  }
  else{
    jassertfalse;
  }

}

void ConnectableNodeHeaderUI::setNodeAndNodeUI(ConnectableNode * _node, ConnectableNodeUI * _nodeUI)
{
  this->node = _node;
  this->nodeUI = _nodeUI;


  node->addConnectableNodeListener(this);
  updateVuMeters();

  titleUI = new StringParameterUI(node->nameParam);
  titleUI->setBackGroundIsTransparent(true);
  addAndMakeVisible(titleUI);

  descriptionUI = new StringParameterUI(node->descriptionParam);
  descriptionUI->setBackGroundIsTransparent(true);
  addAndMakeVisible(descriptionUI);
  descriptionUI->valueLabel.setColour(Label::ColourIds::textColourId, TEXT_COLOR.darker(.3f));

  enabledUI = ParameterUIFactory::createDefaultUI(node->enabledParam);
  addAndMakeVisible(enabledUI);

  addAndMakeVisible(grabber);

  if(node->canBeRemovedByUser) addAndMakeVisible(removeBT);

  addAndMakeVisible(miniModeBT);

 
  if (node->canHavePresets)
  {
	presetChooser = new PresetChooser(node);
    addAndMakeVisible(presetChooser);
  }

  node->addControllableContainerListener(this);


  init();
  resized();

}

void ConnectableNodeHeaderUI::updateVuMeters(){
  if (!vuMeterOut->isVisible() && node->hasAudioOutputs()) {
    node->addRMSListener(vuMeterOut);
    addAndMakeVisible(vuMeterOut);

  }
  else if(vuMeterOut->isVisible() && !node->hasAudioOutputs()){
    node->removeRMSListener(vuMeterOut);
    vuMeterOut->setVisible(false);
  }

  if (!vuMeterIn->isVisible() &&node->hasAudioInputs())
  {
    node->addRMSListener(vuMeterIn);
    addAndMakeVisible(vuMeterIn);
  }
  else if(vuMeterIn->isVisible() && !node->hasAudioInputs()){
    node->removeRMSListener(vuMeterIn);
    vuMeterIn->setVisible(false);
  }
}



void ConnectableNodeHeaderUI::init()
{
  //to overri
}

void ConnectableNodeHeaderUI::resized()
{
  if (!node) return;

  int vuMeterWidth = 8;
  int miniModeBTWidth = 15;
  int removeBTWidth = 15;
  int grabberHeight = 12;



  Rectangle<int> r = getLocalBounds();

  grabber.setBounds(r.removeFromTop(grabberHeight));
  r.reduce(4, 0);
  r.removeFromTop(2);

  if (node->hasAudioOutputs())
  {
    vuMeterOut->setBounds(r.removeFromRight(vuMeterWidth));
  }

  if (node->hasAudioInputs())
  {
    vuMeterIn->setBounds(r.removeFromLeft(vuMeterWidth));

  }

  r.reduce(5, 2);

  enabledUI->setBounds(r.removeFromLeft(10).reduced(0,2));

  r.removeFromLeft(3);
  if (node->canBeRemovedByUser)
  {
    if(r.getWidth()<80){
      removeBT.setVisible(false);
    }
    else{
      removeBT.setVisible(true);
      removeBT.setBounds(r.removeFromRight(removeBTWidth));
      r.removeFromRight(2);
    }

  }

  if(r.getWidth()<100){
    miniModeBT.setVisible(false);
  }
  else{
    miniModeBT.setVisible(true);
    miniModeBT.setBounds(r.removeFromRight(miniModeBTWidth).reduced(0,2));
    r.removeFromRight(2);
  }


  if (node->canHavePresets && !bMiniMode )
  {
    int presetCBWidth = jmin(r.getWidth()/3,80);
    if(presetCBWidth<20){
      presetChooser->setVisible(false);
    }
    else{
      presetChooser->setVisible(true);
      presetChooser->setBounds(r.removeFromRight(presetCBWidth));
      r.removeFromRight(5);
    }

  }


  titleUI->setBounds(r.removeFromTop(12));
  descriptionUI->setBounds(r);
}

void ConnectableNodeHeaderUI::setMiniMode(bool value)
{
  if (bMiniMode == value) return;
  bMiniMode = value;

  if (bMiniMode)
  {
    if(node->canHavePresets) removeChildComponent(presetChooser);
    miniModeBT.setButtonText("+");
  } else
  {
    if (node->canHavePresets) addChildComponent(presetChooser);
    miniModeBT.setButtonText("-");
  }
}

void ConnectableNodeHeaderUI::nodeParameterChanged(ConnectableNode *, Parameter *p)
{
  if (p == node->enabledParam)
  {
    if (!node->enabledParam->boolValue())
    {
      vuMeterOut->setVoldB(0);

    }


    Colour c = !node->enabledParam->boolValue() ? Colour(0xff3cacd5) : Colours::lightgreen;
    Colour c2 = !node->enabledParam->boolValue() ? Colours::blue.brighter(.6f) : Colours::red;
    vuMeterIn->colorLow = c;
    vuMeterOut->colorLow = c;
    vuMeterIn->colorHigh = c2;
    vuMeterOut->colorHigh = c2;

    postCommandMessage(repaintId);
  }

}


void ConnectableNodeHeaderUI::buttonClicked(Button * b)
{
  if (b == &removeBT)
  {
      int result = AlertWindow::showOkCancelBox(AlertWindow::AlertIconType::QuestionIcon, "Remove node", "Do you want to remove the node ?");
      if (result == 0) return;

    node->remove();
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
      if (node->canHavePresets && node->currentPreset != nullptr) presetChooser->setSelectedId(node->currentPreset->presetId, NotificationType::dontSendNotification);
      break;

    case repaintId:
      repaint();
      break;
    case audioInputChangedId:
    case audioOutputChangedId:
      updateVuMeters();
      resized();
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

void ConnectableNodeHeaderUI::numAudioInputChanged(ConnectableNode *, int /*newNumInput*/) {
  postCommandMessage(audioInputChangedId);

}
void ConnectableNodeHeaderUI::numAudioOutputChanged(ConnectableNode *, int /*newNumOutput*/) {
  postCommandMessage(audioOutputChangedId);

}
