/*
 ==============================================================================
 
 TimeManager.cpp
 Created: 2 Mar 2016 8:33:44pm
 Author:  bkupe
 
 ==============================================================================
 */

#include "TimeManager.h"


juce_ImplementSingleton(TimeManager);

#include "NodeBase.h"


TimeManager::TimeManager():
timeInSample(0),
playState(false),
beatTimeInSample(44100),
beatPerBar(4),sampleRate(44100),
timeMasterNode(nullptr),
beatPerQuantizedTime(8){
    
};


void TimeManager::incrementClock(int time){
    int lastBeat = getBeat();
    
    if(playState){
        timeInSample+=time;
    }
    int newBeat = getBeat();
    if(lastBeat!=newBeat){
        listeners.call(&Listener::internal_newBeat,newBeat);
        if(newBeat%beatPerBar == 0){
            listeners.call(&Listener::internal_newBar,getBar());
            
        }
    }
}


void TimeManager::audioDeviceIOCallback (const float** inputChannelData,
                                         int numInputChannels,
                                         float** outputChannelData,
                                         int numOutputChannels,
                                         int numSamples) {
    incrementClock(numSamples);
    
    for (int i = 0; i < numOutputChannels; ++i)
        zeromem (outputChannelData[i], sizeof (float) * (size_t) numSamples);
}

bool TimeManager::askForBeingMasterNode(NodeBase * n){
    if(hasMasterNode() && timeMasterNode!=n)return false;
        else{
            timeMasterNode = n;
            return true;
        }
}

void TimeManager::setPlayState(bool s){
    playState = s;
    if(!s){ listeners.call(&Listener::internal_stop);DBG("stop");}
    else{ listeners.call(&Listener::internal_play);DBG("play");}
}
void TimeManager::setSampleRate(int sr){
    sampleRate = sr;
    // actualize beatTime in sample
    setBPM(getBPM());
    // shouldWe notify something here?
}
void TimeManager::setBPM(double BPM){
    beatTimeInSample = sampleRate*60.0/BPM;
    listeners.call(&Listener::newBPM,BPM);
}


int TimeManager::setBPMForLoopLength(int time){
    double time_seconds = time* 1.0/ sampleRate;
    double beatTime = time_seconds* 1.0/beatPerBar;
    int barLength = 1;
    // over 150 bpm
    if(beatTime < .40){beatTime*=2;barLength/=2;}
    // under 60 bpm
    else if(beatTime > 1){beatTime/=2;barLength*=2;}
    
    setBPM( 60.0/beatTime);
    
    return barLength;
}

void TimeManager::setNumBeatForQuantification(int n){beatPerQuantizedTime = n;}
int TimeManager::getNextQuantifiedTime(){return ceil(getBeat()*1.0/beatPerQuantizedTime) *beatPerQuantizedTime* beatTimeInSample;}
double TimeManager::getBPM(){return sampleRate*60.0/beatTimeInSample;}

int TimeManager::getBeat(){return timeInSample/beatTimeInSample;}
double TimeManager::getBeatPercent(){return timeInSample*1.0/beatTimeInSample-getBeat();}

int TimeManager::getBar(){return getBeat()/beatPerBar;}
void TimeManager::stop(){timeInSample = 0;setPlayState(false);}