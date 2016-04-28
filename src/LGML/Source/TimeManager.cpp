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
beatTimeInSample(22050),
sampleRate(44100),
timeMasterNode(nullptr),
beatPerQuantizedTime(4),
isSettingTempo(false),
ControllableContainer("time")
//asyncNotifier(this)
{

    BPM = addFloatParameter("bpm","current BPM",120,10,600);
    playState = addBoolParameter("Play_Stop", "play or stop global transport", false);
    isSettingTempo = addBoolParameter("isSettingTempo", "is someone setting tempo (recording first loop)", false);
    currentBar  = addIntParameter("currentBar", "currentBar in transport", 0, 0, 4);
    currentBeat  = addIntParameter("currentBeat", "currentBeat in transport", 0, 0, 4);
    beatPerBar = addIntParameter("beatPerBar", "beat Per Bar", 4, 1, 8);
    //    addTimeManagerListener(&asyncNotifier);
}
TimeManager::~TimeManager()
{
}
;


void TimeManager::incrementClock(int time){
    int lastBeat = getBeat();

    if(playState->boolValue()){
        timeInSample+=time;
    }
    int newBeat = getBeat();
    if(lastBeat!=newBeat){
        currentBeat->setValue(newBeat);
        if(newBeat%((int)beatPerBar->value) == 0){
            currentBar->setValue(getBar());

        }
    }
}


void TimeManager::audioDeviceIOCallback (const float** /*inputChannelData*/,
                                         int /*numInputChannels*/,
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


void TimeManager::onContainerParameterChanged(Parameter * p){
    if(p==playState){
        if(!playState->boolValue()){
            timeInSample = 0;

        }
        else{
        }
    }
    else if(p==BPM){
        setBPMInternal(BPM->floatValue());
    }
    else if(p==beatPerBar){
        currentBeat->maximumValue = beatPerBar->intValue();
    }

};

void TimeManager::setSampleRate(int sr){
    sampleRate = sr;
    // actualize beatTime in sample
    beatTimeInSample = (int)(sampleRate*60.0f / (float)BPM->value);
}

void TimeManager::setBPMInternal(double _BPM){
    isSettingTempo->setValue(false);
    timeInSample = 0;
}



int TimeManager::setBPMForLoopLength(int time){
    double time_seconds = time* 1.0/ sampleRate;
    double beatTime = time_seconds* 1.0/beatPerBar->intValue();
    int barLength = 1;

    // over 150 bpm
    if(beatTime < .40){beatTime*=2;barLength/=2;}
    // under 60 bpm
    else if(beatTime > 1){beatTime/=2;barLength*=2;}

    BPM->setValue( 60.0/beatTime);
    return barLength;
}

void TimeManager::setNumBeatForQuantification(int n){beatPerQuantizedTime = n;}
int TimeManager::getNextQuantifiedTime(){return (int)(ceil((getBeat() + 1)*1.0/beatPerQuantizedTime) *beatPerQuantizedTime* beatTimeInSample);}


int TimeManager::getBeat(){return (int)(floor(timeInSample*1.0/beatTimeInSample));}
double TimeManager::getBeatPercent(){return timeInSample*1.0/beatTimeInSample-getBeat();}

int TimeManager::getBar(){return getBeat()/beatPerBar->intValue();}
