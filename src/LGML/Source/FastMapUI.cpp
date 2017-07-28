/*
 ==============================================================================

 FastMapUI.cpp
 Created: 17 May 2016 6:05:44pm
 Author:  bkupe

 ==============================================================================
 */

#include "FastMapUI.h"
#include "ParameterUIFactory.h"
#include "FastMapper.h"

FastMapUI::
FastMapUI(FastMap * f) :
InspectableComponent(f,"fastMap"),
fastMap(f),
refUI(f->referenceIn),
targetUI(f->referenceOut)

{
  addMouseListener(this,true);

  enabledUI = ParameterUIFactory::createDefaultUI(fastMap->enabledParam);
  addAndMakeVisible(enabledUI);

  minInputUI = ParameterUIFactory::createDefaultUI(fastMap->minInputVal);
  maxInputUI = ParameterUIFactory::createDefaultUI(fastMap->maxInputVal);
  minOutputUI = ParameterUIFactory::createDefaultUI(fastMap->minOutputVal);
  maxOutputUI = ParameterUIFactory::createDefaultUI(fastMap->maxOutputVal);
  invertUI = ParameterUIFactory::createDefaultUI(fastMap->invertParam);

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
  refUI.setBounds(r.removeFromLeft((int)(w*.25f)).reduced(1));
  targetUI.setBounds(r.removeFromRight((int)(w*.25f)).reduced(1));

  Rectangle<int> inR = r.removeFromLeft((int)(w*.2f));
  minInputUI->setBounds(inR.removeFromTop((int)(h*.45f)));
  maxInputUI->setBounds(inR.removeFromBottom((int)(h*.45f)));


  Rectangle<int> outR = r.removeFromRight((int)(w*.3f));
  invertUI->setBounds(outR.removeFromRight(30));
  minOutputUI->setBounds(outR.removeFromTop((int)(h*.45f)));
  maxOutputUI->setBounds(outR.removeFromBottom((int)(h*.45f)));



}

void FastMapUI::buttonClicked(Button * b)
{
  if(b == &removeBT) FastMapper::getInstance()->removeFastmap(fastMap);
}

void FastMapUI::mouseDown(const MouseEvent &e) {
  selectThis();
};
