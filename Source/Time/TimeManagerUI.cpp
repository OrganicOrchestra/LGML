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
#include "TimeManagerUI.h"
#include "../Controllable/Parameter/UI/ParameterUIFactory.h"
#include "../UI/Style.h"


TimeManagerUI::TimeManagerUI (const String& contentName, TimeManager* _timeManager) :
InspectableComponent(_timeManager),
ShapeShifterContent (this,contentName,"Be the master of Time")
,timeBar (_timeManager)
,timeManager (_timeManager)
{

    contentIsFlexible = false;

    timeManager->isSettingTempo->addAsyncParameterListener (this);

    timeManager->playState->addAsyncParameterListener (this);
    timeManager->beatPerBar->addAsyncParameterListener (this);
    timeManager->currentBeat->addAsyncParameterListener (this);
    timeManager->currentBar->addAsyncParameterListener (this);
    timeManager->linkNumPeers->addAsyncParameterListener (this);
    timeManager->linkEnabled->addAsyncParameterListener (this);

    addAndMakeVisible (timeBar);
    bpmStepper = std::make_unique< FloatSliderUI> (timeManager->BPM);
    //  bpmStepper->assignOnMousePosDirect = true;
    //  bpmStepper->changeParamOnMouseUpOnly = true;

    addAndMakeVisible (bpmStepper.get());


    quantizStepper = ParameterUIFactory::createDefaultUI (timeManager->quantizedBarFraction);

    addAndMakeVisible (quantizStepper.get());

    playTrig = ParameterUIFactory::createDefaultUI (timeManager->playTrigger);
    addAndMakeVisible (playTrig.get());
    stopTrig = ParameterUIFactory::createDefaultUI (timeManager->stopTrigger);
    addAndMakeVisible (stopTrig.get());
    tapTempo = ParameterUIFactory::createDefaultUI (timeManager->tapTempo);
    addAndMakeVisible (tapTempo.get());
    click = ParameterUIFactory::createDefaultUI (timeManager->click);
    addAndMakeVisible (click.get());
    clickVolumeUI = ParameterUIFactory::createDefaultUI (timeManager->clickVolume);
    addAndMakeVisible (clickVolumeUI.get());

    linkEnabled = ParameterUIFactory::createDefaultUI (timeManager->linkEnabled);
    addAndMakeVisible (linkEnabled.get());


    //    linkNumPeers = ParameterUIFactory::createDefaultUI(timeManager->linkNumPeers);
    //    addAndMakeVisible(linkNumPeers);

    
}


TimeManagerUI::~TimeManagerUI()
{
    timeManager->isSettingTempo->removeAsyncParameterListener (this);

    timeManager->playState->removeAsyncParameterListener (this);
    timeManager->beatPerBar->removeAsyncParameterListener (this);
    timeManager->currentBeat->removeAsyncParameterListener (this);
    timeManager->currentBar->removeAsyncParameterListener (this);
}


void TimeManagerUI::newMessage (const ParameterBase::ParamWithValue& pv)
{

    if (pv.parameter == timeManager->playState)
    {
        if ((bool)pv.value) { timeBar.async_play(); }
        else { timeBar.async_stop(); }
    }
    else if (pv.parameter == timeManager->isSettingTempo)
    {
        timeBar.isSettingTempo = pv.value;
        timeBar.showBeatComponents (! (bool)pv.value);
        timeBar.repaint();
    }

    else if (pv.parameter == timeManager->beatPerBar)
    {
        timeBar.initComponentsForNumBeats (pv.value);
    }
    else if (pv.parameter == timeManager->currentBeat)
    {
        timeBar.async_newBeat (pv.value);
    }
    else if (pv.parameter == timeManager->linkNumPeers)
    {
        linkEnabled->setCustomText (String (timeManager->linkNumPeers->intValue()) + " link peers");
    }
    else if (pv.parameter == timeManager->linkEnabled)
    {
        if (!timeManager->linkEnabled->boolValue())linkEnabled->setCustomText ("");
        else {linkEnabled->setCustomText (String (timeManager->linkNumPeers->intValue()) + " link peers");}
    }

}



#pragma warning(push)
#pragma warning(disable:4244)
void TimeManagerUI::resized()
{
    

    int width = 800;
    int gap = 2;

    Rectangle<int> r = getLocalBounds().withWidth (width).withCentre (getLocalBounds().getCentre()).reduced (4);
    r = r.withSizeKeepingCentre(r.getWidth(), jmin(100,r.getHeight()));
    timeBar.setBounds (r.removeFromRight ((int)(width * 0.3)).reduced (0, 2));
    r.removeFromRight (5);
    playTrig->setBounds (r.removeFromRight ((int)(width * 0.05)).reduced (gap, 0));
    r.removeFromRight (2);
    stopTrig->setBounds (r.removeFromRight ((int)(width * 0.05)).reduced (gap, 0));
    r.removeFromRight (10);
    bpmStepper->setBounds (r.removeFromRight ((int)(width * 0.1)).reduced (gap, 0));
    r.removeFromRight (10);
    quantizStepper->setBounds (r.removeFromRight ((int)(width * 0.1)).reduced (gap, 0));
    r.removeFromRight (10);
    tapTempo->setBounds (r.removeFromRight ((int)(width * 0.1)).reduced (gap, 0));
    r.removeFromRight (10);
    Rectangle<int> clickR = r.removeFromRight ((int)(width * 0.1)).reduced (gap, 0);
    click->setBounds (clickR.removeFromTop (10));
    clickR.removeFromTop (4);
    clickVolumeUI->setBounds (clickR);

    linkEnabled->setBounds (r.removeFromRight ((int)(width * 0.1)).reduced (gap, 0));
    //    linkNumPeers->setBounds(r.removeFromRight(width*0.1).reduced(gap, 0));
}
#pragma warning(pop)

TimeManagerUI::TimeBar::TimeBar (TimeManager* t) :
timeManager (t),
refreshHz(20),
blinkHz(1)
{
    initComponentsForNumBeats (timeManager->beatPerBar->intValue());
    setOpaque (true);
    setPaintingIsUnclipped(true);
}


void TimeManagerUI::TimeBar::initComponentsForNumBeats (int beatPerBar)
{
    for (auto& bc : beatComponents)
    {
        removeChildComponent (bc);
    }

    beatComponents.clear();

    for (int i = 0; i < beatPerBar; i++)
    {
        BeatComponent* bc = new BeatComponent();
        addAndMakeVisible (bc);
        beatComponents.add (bc);
    }

    resized();

}

void TimeManagerUI::TimeBar::resized()
{
    int beatPerBar = beatComponents.size();

    if (beatPerBar == 0)return;

    Rectangle<int> area = getLocalBounds();
    int beatWidth = area.getWidth() / beatPerBar;

    for (int i = 0; i < beatPerBar; i++) { beatComponents.getUnchecked (i)->setBounds (area.removeFromLeft (beatWidth)); }
}
void  TimeManagerUI::TimeBar::async_play()
{
    blinkCount = 0;
    zeroOutBeatComponents();
    startTimerHz (refreshHz);
}
void TimeManagerUI::TimeBar::async_stop()
{
    zeroOutBeatComponents();
    repaint();
    stopTimer();
}
void TimeManagerUI::TimeBar::async_newBeat (int b)
{
    if (b % beatComponents.size() == 0)
    {
        zeroOutBeatComponents();
    }
}
void TimeManagerUI::TimeBar::async_beatPerBarChanged (int bpb)
{
    initComponentsForNumBeats (bpb);
}


void TimeManagerUI::TimeBar::zeroOutBeatComponents()
{
    for (int i = 0; i < beatComponents.size(); i++)
    {
        BeatComponent* bc = beatComponents.getUnchecked (i);
        bc->percentDone = 0;
        bc->repaint();

    }


}

void TimeManagerUI::TimeBar::showBeatComponents (bool show)
{
    for (int i = 0; i < beatComponents.size(); i++) { beatComponents.getUnchecked (i)->setVisible (show); }
}

void TimeManagerUI::TimeBar::timerCallback()
{
    if (isSettingTempo) { repaint(); }
    else
    {
        int lastBeat = timeManager->getBeatInt() % beatComponents.size();

        //    DBG(timeManager->getBeatPercent());
        for (int i = 0; i < beatComponents.size(); i++)
        {
            BeatComponent* bc = beatComponents.getUnchecked (i);

            if (i == lastBeat)
            {
                bc->percentDone = (float)timeManager->getBeatPercent();
                bc->repaint();
            }
            // ensure old beats are filled
            else if (i < lastBeat)
            {
                if (bc->percentDone != 1)
                {
                    bc->percentDone = 1;
                    bc->repaint();
                }
            }
        }
    }
}


void TimeManagerUI::TimeBar::paint (Graphics& g)
{
    LGMLUIUtils::fillBackground(this,g);
    if (isSettingTempo)
    {
        // called only if setting tempo
        Rectangle<int> r = getLocalBounds();


        blinkCount += blinkHz * 1.0 / refreshHz;

        if (blinkCount > 1)
        {
            blinkCount -= 1;
        }

        g.setColour (Colours::red.brighter (1 - sin ((float) (2.0f * double_Pi * blinkCount))));
        g.fillRect (r);
    }
}

TimeManagerUI::TimeBar::BeatComponent::BeatComponent(){
    setPaintingIsUnclipped(true);
    setOpaque(true);
}
void TimeManagerUI::TimeBar::BeatComponent::paint (Graphics& g)
{
    int beatBarWidth = 1;

    Rectangle<int> r = getLocalBounds();
    LGMLUIUtils::fillBackground(this,g);
    Rectangle<int> lineR = r.removeFromTop (1);
    lineR.removeFromLeft (beatBarWidth);
    lineR.removeFromRight (beatBarWidth);

    r.removeFromTop (1);
    r.removeFromLeft (beatBarWidth);
    r.removeFromRight (beatBarWidth);
    //    g.fillRect(area.removeFromRight(beatBarWidth));


    if (percentDone == 0) g.setColour (findColour (ResizableWindow::backgroundColourId).brighter());
    else g.setColour (Colours::orange);

    g.fillRect (r);

    g.setColour (findColour (TextButton::buttonOnColourId));
    g.fillRect (lineR.removeFromLeft ((int) (percentDone * lineR.getWidth())));

}




#endif
