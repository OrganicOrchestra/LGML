/*
 ==============================================================================

 FastMapUI.cpp
 Created: 17 May 2016 6:05:44pm
 Author:  bkupe

 ==============================================================================
 */

#include "FastMapUI.h"
#include "BoolToggleUI.h"

FastMapUI::
FastMapUI(FastMap * f) :
InspectableComponent(f,"fastMap"),
fastMap(f)
{
  fastMap->addFastMapListener(this);

  refUI.addControllableReferenceUIListener(this);
  targetUI.addControllableReferenceUIListener(this);



  //	refUI->setAliasVisible(false);
  //	refUI->setRemoveBTVisible(false);

  enabledUI = fastMap->enabledParam->createToggle();
  addAndMakeVisible(enabledUI);

  minInputUI = fastMap->minInputVal->createSlider();
  maxInputUI = fastMap->maxInputVal->createSlider();
  minOutputUI = fastMap->minOutputVal->createSlider();
  maxOutputUI = fastMap->maxOutputVal->createSlider();
  invertUI = fastMap->invertParam->createToggle();

  addAndMakeVisible(refUI);
  addAndMakeVisible(targetUI);
  addAndMakeVisible(minInputUI);
  addAndMakeVisible(maxInputUI);
  addAndMakeVisible(minOutputUI);
  addAndMakeVisible(maxOutputUI);
  addAndMakeVisible(invertUI);

  Image removeImage = ImageCache::getFromMemory(BinaryData::removeBT_png, BinaryData::removeBT_pngSize);

  removeBT.setImages(false, true, true, removeImage,
                     0.7f, Colours::transparentBlack,
                     removeImage, 1.0f, Colours::transparentBlack,
                     removeImage, 1.0f, Colours::white.withAlpha(.7f),
                     0.5f);
  removeBT.addListener(this);

  addAndMakeVisible(&removeBT);

  setSize(100, 40);
}

FastMapUI::~FastMapUI()
{
  fastMap->removeFastMapListener(this);
}

void FastMapUI::paint(Graphics & g)
{
  g.setColour(PANEL_COLOR);
  g.fillRoundedRectangle(getLocalBounds().toFloat(), 2);
}

void FastMapUI::resized()
{
  Rectangle<int> r = getLocalBounds().reduced(3);
  enabledUI->setBounds(r.removeFromLeft(15));
  removeBT.setBounds(r.removeFromRight(r.getHeight()).reduced(4));

  int w = r.getWidth();
  int h = r.getHeight();
  refUI.setBounds(r.removeFromLeft((int)(w*.3f)));
  targetUI.setBounds(r.removeFromRight((int)(w*.2f)).reduced(2,3));

  Rectangle<int> inR = r.removeFromLeft((int)(w*.2f));
  minInputUI->setBounds(inR.removeFromTop((int)(h*.4f)));
  maxInputUI->setBounds(inR.removeFromBottom((int)(h*.4f)));


  Rectangle<int> outR = r.removeFromRight((int)(w*.3f));
  invertUI->setBounds(outR.removeFromRight(30));
  minOutputUI->setBounds(outR.removeFromTop((int)(h*.4f)));
  maxOutputUI->setBounds(outR.removeFromBottom((int)(h*.4f)));



}

void FastMapUI::choosedControllableChanged(ControllableReferenceUI* ui,Controllable * c)
{
  if(ui==&refUI){
    fastMap->setReference(c);
  }
  else if(ui==&targetUI){
    fastMap->setTarget(c);
  }
  else{

    jassertfalse;
  }
}

void FastMapUI::fastMapTargetChanged(FastMap *f)
{
  if (f->referenceOut.get() != nullptr)
  {
    targetUI.setButtonText(f->referenceOut.get()->niceName);
    targetUI.setTooltip("Current Controllable :" + f->referenceOut.get()->niceName + String("\n") + f->referenceOut.get()->controlAddress);

    minOutputUI->setVisible(f->referenceOut.get()->type != Controllable::TRIGGER && f->referenceOut.get()->type != Controllable::BOOL);
    maxOutputUI->setVisible(f->referenceOut.get()->type != Controllable::TRIGGER && f->referenceOut.get()->type != Controllable::BOOL);
  }
  else
  {
    targetUI.setButtonText("[Target]");
    targetUI.setTooltip("Choose a target");
    
  }
}

void FastMapUI::buttonClicked(Button * b)
{
  if(b == &removeBT) fastMap->remove();
}
