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


#include "FastMapUI.h"
#include "ParameterUIFactory.h"
#include "FastMapper.h"
#include "RangeParameterUI.h"

FastMapUI::
FastMapUI(FastMap * f) :
InspectableComponent(f,"fastMap"),
fastMap(f),
refUI(f->referenceIn),
inRangeUI(f->inputRange),
targetUI(f->referenceOut),
outRangeUI(f->outputRange)


{
  addMouseListener(this,true);

  enabledUI = ParameterUIFactory::createDefaultUI(fastMap->enabledParam);
  addAndMakeVisible(enabledUI);


  invertUI = ParameterUIFactory::createDefaultUI(fastMap->invertParam);

  addAndMakeVisible(refUI);
  f->referenceIn->addParameterProxyListener(this);
  addChildComponent(inRangeUI);
  addAndMakeVisible(targetUI);
  f->referenceOut->addParameterProxyListener(this);

  addChildComponent(outRangeUI);
  addAndMakeVisible(invertUI);

  Image removeImage = ImageCache::getFromMemory(BinaryData::removeBT_png, BinaryData::removeBT_pngSize);

  removeBT.setImages(false, true, true, removeImage,
                     0.7f, Colours::transparentBlack,
                     removeImage, 1.0f, Colours::transparentBlack,
                     removeImage, 1.0f, Colours::white.withAlpha(.7f),
                     0.5f);
  removeBT.addListener(this);

  addAndMakeVisible(&removeBT);
  linkedParamChanged(f->referenceIn);
  linkedParamChanged(f->referenceOut);

  setSize(100, 40);
}

FastMapUI::~FastMapUI()
{
  fastMap->referenceOut->removeParameterProxyListener(this);
  fastMap->referenceIn->removeParameterProxyListener(this);
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
  auto refRect = r.removeFromLeft((int)(w*.45f));
  if(inRangeUI.isVisible()){
    inRangeUI.setBounds(refRect.removeFromBottom(h/2));
  }
  refUI.setBounds(refRect.reduced(6,0));

  auto targetRect = r.removeFromRight((int)(w*.45f));
  if(outRangeUI.isVisible()){
    outRangeUI.setBounds(targetRect.removeFromBottom(h/2));
  }
  targetUI.setBounds(targetRect.reduced(6,0));


  invertUI->setBounds(r.reduced(1));




}

void FastMapUI::buttonClicked(Button * b)
{
  if(b == &removeBT) FastMapper::getInstance()->removeFastmap(fastMap);
}

void FastMapUI::mouseDown(const MouseEvent &) {
  selectThis();
};

void FastMapUI::linkedParamChanged(ParameterProxy * p ) {
  if(!MessageManager::getInstance()->isThisTheMessageThread()){
    MessageManager::getInstance()->callAsync([this,p](){linkedParamChanged(p);});
  }
  else{
  if(p==refUI.controllable){
    inRangeUI.setVisible(p->linkedParam && p->linkedParam->isNumeric());
  }
  else if(p==targetUI.controllable){
    outRangeUI.setVisible(p->linkedParam && p->linkedParam->isNumeric());
  }
  resized();
  }
};
