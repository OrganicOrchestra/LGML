/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#if !ENGINE_HEADLESS
#include "FastMapUI.h"
#include "../Controllable/Parameter/UI/ParameterUIFactory.h"
#include "FastMapper.h"
#include "../Controllable/Parameter/UI/RangeParameterUI.h"
#include "../Controllable/Parameter/NumericParameter.h"
#include "../UI/Style.h"

FastMapUI::
FastMapUI (FastMap* f) :
    InspectableComponent (f, "fastMap"),
    fastMap (f),
    refUI (f->referenceIn),
    inRangeUI (f->inputRange),
    targetUI (f->referenceOut),
    outRangeUI (f->outputRange),
    displayRef(true),
    displayTarget(true)


{
    addMouseListener (this, true);

    enabledUI = ParameterUIFactory::createDefaultUI (fastMap->enabledParam);
    addAndMakeVisible (enabledUI.get());


    invertUI = ParameterUIFactory::createDefaultUI (fastMap->invertParam);
    toggleUI =ParameterUIFactory::createDefaultUI (fastMap->toggleParam);

    fullSyncUI = ParameterUIFactory::createDefaultUI (fastMap->fullSync);

    addAndMakeVisible (refUI);
    f->referenceIn->addParameterProxyListener (this);
    addChildComponent (inRangeUI);
    addAndMakeVisible (targetUI);
    addAndMakeVisible (toggleUI.get());
    f->referenceOut->addParameterProxyListener (this);

    addChildComponent (outRangeUI);
    addAndMakeVisible (invertUI.get());
    addAndMakeVisible(fullSyncUI.get());
    Image removeImage = ImageCache::getFromMemory (BinaryData::removeBT_png, BinaryData::removeBT_pngSize);

    removeBT.setImages (false, true, true, removeImage,
                        0.7f, Colours::transparentBlack,
                        removeImage, 1.0f, Colours::transparentBlack,
                        removeImage, 1.0f, Colours::white.withAlpha (.7f),
                        0.5f);
    removeBT.addListener (this);
    
    addAndMakeVisible (&removeBT);
    linkedParamChanged (f->referenceIn);
    linkedParamChanged (f->referenceOut);
    setPaintingIsUnclipped(false);
//    setSize (100, 40);
}

FastMapUI::~FastMapUI()
{
    fastMap->referenceOut->removeParameterProxyListener (this);
    fastMap->referenceIn->removeParameterProxyListener (this);
}

void FastMapUI::paint (Graphics& g)
{
    LGMLUIUtils::fillBackground(this, g);
    g.setColour (findColour (LGMLColors::elementBackground));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 2);
}

void FastMapUI::resized()
{
    Rectangle<int> r = getLocalBounds().reduced (3);
    enabledUI->setBounds (r.removeFromLeft (15));
    removeBT.setBounds (r.removeFromRight (r.getHeight()).reduced (4));

    int w = r.getWidth();
    int h = r.getHeight();
    auto refRect = r.removeFromLeft ((int) (w * .45f));

    if (inRangeUI.isVisible())
    {
        inRangeUI.setBounds (refRect.removeFromBottom (h / 2));
    }

    refUI.setBounds (refRect.reduced (6, 0));

    auto targetRect = r.removeFromRight ((int) (w * .45f));

    if (outRangeUI.isVisible())
    {
        outRangeUI.setBounds (targetRect.removeFromBottom (h / 2));
    }

    targetUI.setBounds (targetRect.reduced (6, 0));
    int numB = (invertUI->isVisible()?1:0) +
    (toggleUI->isVisible()?1:0) +
    (fullSyncUI->isVisible()?1:0);
    if(invertUI->isVisible()){
        invertUI->setBounds (r.removeFromTop(r.getHeight()/numB).reduced (1));
        numB--;
    }
    if(toggleUI->isVisible()){
        toggleUI->setBounds (r.removeFromTop(r.getHeight()/numB).reduced (1));
        numB--;
    }
    if(fullSyncUI->isVisible()){
    fullSyncUI->setBounds(r.reduced(1));
    }




}

void FastMapUI::buttonClicked (Button* b)
{
    if (b == &removeBT) FastMapper::getInstance()->removeFastmap (fastMap);
}



void FastMapUI::linkedParamChanged (ParameterProxy* p )
{
    if (!MessageManager::getInstance()->isThisTheMessageThread())
    {
        MessageManager::getInstance()->callAsync ([this, p]() {linkedParamChanged (p);});
    }
    else
    {
        updateComponentsVisibility();

        resized();
    }
};

void FastMapUI::updateRefAndTargetVisibility(bool ref,bool target){

         displayRef=ref;
    displayTarget = target;

    updateComponentsVisibility();

}

void FastMapUI::updateComponentsVisibility(){

    auto refProxy = refUI.paramProxy;
    auto targetProxy = targetUI.paramProxy;
    refUI.setVisible(displayRef);
    inRangeUI.setVisible (displayRef &&
                          refProxy->linkedParam &&
                          refProxy->linkedParam->getAs<MinMaxParameter>());

    targetUI.setVisible(displayTarget);

    outRangeUI.setVisible (displayTarget &&
                           targetProxy->linkedParam &&
                           targetProxy->linkedParam->getAs<MinMaxParameter>());
    toggleUI->setVisible( refUI.parameter &&
                         !refUI.parameter->getAs<Trigger>() &&
                         targetProxy->linkedParam &&
                         (targetProxy->linkedParam->getAs<BoolParameter>() ||
                          targetProxy->linkedParam->getAs<Trigger>()));

}
#endif
