/*
 ==============================================================================

 TimeManagerUI.cpp
 Created: 8 Mar 2016 11:06:41pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "TimeManagerUI.h"



TimeManagerUI::TimeManagerUI(TimeManager * _timeManager):
timeManager(_timeManager),
timeBar(_timeManager){

    timeManager->isSettingTempo->addParameterListener(this);
    timeManager->playState->addParameterListener(this);
    timeManager->beatPerBar->addParameterListener(this);
    timeManager->currentBeat->addParameterListener(this);
    timeManager->currentBar->addParameterListener(this);

    addAndMakeVisible(timeBar);
    bpmSlider = timeManager->BPM->createSlider();
    bpmSlider->assignOnMousePosDirect = false;
    //bpmSlider->orientation = FloatSliderUI::VERTICAL;
    bpmSlider->displayText = true;
    bpmSlider->displayBar = false;
    bpmSlider->scaleFactor = 0.01;
    addAndMakeVisible(bpmSlider);


    playTrig = timeManager->playTrigger->createBlinkUI();
    addAndMakeVisible(playTrig);
    stopTrig = timeManager->stopTrigger->createBlinkUI();
    addAndMakeVisible(stopTrig);

}


TimeManagerUI::~TimeManagerUI(){
    timeManager->isSettingTempo->removeParameterListener(this);
    timeManager->playState->removeParameterListener(this);
    timeManager->beatPerBar->removeParameterListener(this);
    timeManager->currentBeat->removeParameterListener(this);
    timeManager->currentBar->removeParameterListener(this);
}


void TimeManagerUI::asyncParameterValueChanged(Parameter* p ,var & v) {
    if(p == timeManager->playState){
        if((bool)v){timeBar.async_play();}
        else{ timeBar.async_stop();}
    }
    else if(p==timeManager->isSettingTempo){
        timeBar.isSettingTempo = v;
        timeBar.showBeatComponents(!(bool)v);
        timeBar.repaint();
    }
    else if(p==timeManager->beatPerBar){
        timeBar.initComponentsForNumBeats(v);
    }
    else if(p==timeManager->currentBeat){
        timeBar.async_newBeat(v);
    }
}


void TimeManagerUI::resized(){
    Rectangle<int> area = getLocalBounds();
    timeBar.setBounds(area.removeFromRight(area.getWidth()/2));
    playTrig->setBounds(area.removeFromRight(70).reduced(2));
    stopTrig->setBounds(area.removeFromRight(70).reduced(2));
    bpmSlider->setBounds(area.removeFromRight(area.getWidth()/2));
}



TimeManagerUI::TimeBar::TimeBar(TimeManager * t):timeManager(t){
    initComponentsForNumBeats(timeManager->beatPerBar->intValue());
}


void TimeManagerUI::TimeBar::initComponentsForNumBeats(int beatPerBar){
    beatComponents.clear();
    for(int i = 0 ;i <beatPerBar ; i++){
        BeatComponent * bc=new BeatComponent();
        addAndMakeVisible(bc);
        beatComponents.add(bc);
    }
    resized();

}

void TimeManagerUI::TimeBar::resized() {
    int beatPerBar =beatComponents.size();
    if(beatPerBar==0)return;

    Rectangle<int> area = getLocalBounds();
    int beatWidth = area.getWidth()/beatPerBar;
    for(int i = 0 ; i < beatPerBar ; i++){beatComponents.getUnchecked(i)->setBounds(area.removeFromLeft(beatWidth));}
}
void  TimeManagerUI::TimeBar::async_play(){
    blinkCount = 0;
    zeroOutBeatComponents();
    startTimerHz(refreshHz);
}
void TimeManagerUI::TimeBar::async_stop(){
    zeroOutBeatComponents();
    repaint();
    stopTimer();
}
void TimeManagerUI::TimeBar::async_newBeat( int b){
    if(b%beatComponents.size()==0){
        zeroOutBeatComponents();
    }
}
void TimeManagerUI::TimeBar::async_beatPerBarChanged(int bpb){
    initComponentsForNumBeats(bpb);
}


void TimeManagerUI::TimeBar::zeroOutBeatComponents(){
    for(int i = 0 ; i< beatComponents.size() ; i++){
        BeatComponent * bc = beatComponents.getUnchecked(i);
        bc->percentDone = 0;
        bc->repaint();

    }

}

void TimeManagerUI::TimeBar::showBeatComponents(bool show){
    for(int i = 0 ; i< beatComponents.size() ; i++){beatComponents.getUnchecked(i)->setVisible(show);}
}
void TimeManagerUI::TimeBar::timerCallback(){
    if(isSettingTempo){repaint();}
    else{
        int lastBeat =timeManager->getBeat()%beatComponents.size();
        for(int i = 0 ; i< beatComponents.size() ; i++){
            BeatComponent * bc = beatComponents.getUnchecked(i);
            if(i==lastBeat){
                bc->percentDone = (float)timeManager->getBeatPercent();
                bc->repaint();
            }
            // ensure old beats are filled
            else if (i<lastBeat){
                if(bc->percentDone!=1){
                    bc->percentDone = 1;
                    bc->repaint();
                }
            }
        }
    }
}


void TimeManagerUI::TimeBar::paint(Graphics & g) {

    if(isSettingTempo){
        // called only if setting tempo
        Rectangle<int> area = getLocalBounds();
        blinkCount+=blinkHz*1.0/refreshHz;
        if(blinkCount>1){
            blinkCount-=1;
        }

        g.setColour(Colours::red.brighter(1-sin((float)(2.0f*double_Pi*blinkCount))));
        g.fillRect(area);
    }
}


void TimeManagerUI::TimeBar::BeatComponent::paint(Graphics & g){
    Rectangle<int> area = getLocalBounds();
    static int beatBarWidth  =2;
    g.setColour(Colours::grey);
    g.fillRect(area.removeFromLeft(beatBarWidth));
//    g.fillRect(area.removeFromRight(beatBarWidth));


    if(percentDone >= 1){
        g.setColour(Colours::green);
        g.fillRect(area);
    }
    else{
        g.setColour(Colours::orange);
        g.fillRect(area.removeFromLeft((int)(percentDone*area.getWidth())));
        g.setColour(Colours::black);
        g.fillRect(area);

    }

}
