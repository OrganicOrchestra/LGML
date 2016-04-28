/*
 ==============================================================================

 Looper.cpp
 Created: 26 Apr 2016 4:46:37pm
 Author:  bkupe

 ==============================================================================
 */

#include "Looper.h"

#include "TimeManager.h"
#include "LooperTrack.h"
#include "LooperNode.h"

Looper::Looper(LooperNode * looperNode) :
ControllableContainer("InnerLooper"),
selectedTrack(nullptr),
looperNode(looperNode),
wasMonitoring(false)
{

    numberOfTracks = addIntParameter("numberOfTracks", "number of tracks in this looper", 8, 1, MAX_NUM_TRACKS);

    selectAllTrig = addTrigger("Select All", "Select All tracks, for all clear or main volume for instance");
    recPlaySelectedTrig = addTrigger("Rec Or Play",
                                     "Tells the selected track to wait for the next bar and then start record or play");
    playSelectedTrig = addTrigger("Play",
                                  "Tells the selected track to wait for the next bar and then stop recording and start playing");
    stopSelectedTrig = addTrigger("Stop",
                                  "Tells the selected track to stop ");
    clearSelectedTrig = addTrigger("Clear",
                                   "Tells the selected track to clear it's content if got any");
    volumeSelected = addFloatParameter("Volume",
                                       "Set the volume of the selected track",
                                       1, 0, 1);
    clearAllTrig = addTrigger("ClearAll",
                              "Tells all tracks to clear it's content if got any");
    stopAllTrig = addTrigger("StopAll",
                             "Tells all tracks to stop it's content if got any");
    isMonitoring = addBoolParameter("monitor", "do we monitor audio input ? ", false);

    skipControllableNameInAddress = true;

    setNumTracks(numberOfTracks->intValue());

    TimeManager::getInstance()->playState->addParameterListener(this);

}
Looper::~Looper(){
    if(TimeManager::getInstanceWithoutCreating()){
        TimeManager::getInstance()->playState->removeParameterListener(this);
    }

}

void Looper::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer &midiMessages) {

    // TODO check if we can optimize copies
    // handle multiples channels outs

    bufferIn.setSize(buffer.getNumChannels(), buffer.getNumSamples());
    bufferOut.setSize(buffer.getNumChannels(), buffer.getNumSamples());

    if (isMonitoring->boolValue()) {
        if(!wasMonitoring){
            for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
                bufferOut.copyFromWithRamp(i, 0, buffer.getReadPointer(i), buffer.getNumSamples(),0.0f,1.0f);
            }
            wasMonitoring = true;
        }
        else{
            for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
                bufferOut.copyFrom(i, 0, buffer, i, 0, buffer.getNumSamples());
            }
        }
    }
    else {
        if(wasMonitoring){
            for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
                bufferOut.copyFromWithRamp(i, 0, buffer.getReadPointer(i), buffer.getNumSamples(),1.0f,0.0f);
            }
            wasMonitoring = false;
        }
        else{
            bufferOut.clear();
        }
    }
    for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
        bufferIn.copyFrom(i, 0, buffer, i, 0, buffer.getNumSamples());
    }
    for (auto & t : tracks) {
        t->processBlock(buffer, midiMessages);
        for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
            bufferOut.addFrom(i, 0, buffer, i, 0, buffer.getNumSamples());
            buffer.copyFrom(i, 0, bufferIn, i, 0, buffer.getNumSamples());
        }
    }
    for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
        buffer.copyFrom(i, 0, bufferOut, i, 0, buffer.getNumSamples());

    }
}


void Looper::addTrack() {
    LooperTrack * t = new LooperTrack(this, tracks.size());
    tracks.add(t);
    addChildControllableContainer(t);
}

void Looper::removeTrack(int i) {
    removeChildControllableContainer(tracks[i]);
    tracks.remove(i);
}


void Looper::setNumTracks(int numTracks) {
    int oldSize = tracks.size();
    if (numTracks>oldSize) {
        for (int i = oldSize; i< numTracks; i++) { addTrack(); }
        looperListeners.call(&Looper::Listener::trackNumChanged, numTracks);
    }
    else {
        looperListeners.call(&Looper::Listener::trackNumChanged, numTracks);
        for (int i = oldSize - 1; i > numTracks; --i) { removeTrack(i); }
    }

}


void Looper::checkIfNeedGlobalLooperStateUpdate() {
    bool needToReleaseMasterTempo = true;
    for (auto & t : tracks) {
        needToReleaseMasterTempo &= (t->trackState == LooperTrack::TrackState::CLEARED);
    }

    if (needToReleaseMasterTempo) {
        TimeManager::getInstance()->releaseMasterNode(looperNode);
    }
}


bool Looper::askForBeingMasterTrack(LooperTrack * t) {
    bool res = areAllTrackClearedButThis(t);
    if (res)lastMasterTempoTrack = t;
    return res;
}

bool Looper::askForBeingAbleToPlayNow(LooperTrack * _t) {
    bool result = true;
    for (auto & t : tracks) {
        if (t != _t)result &= (t->trackState == LooperTrack::TrackState::STOPPED) || (t->trackState == LooperTrack::TrackState::CLEARED);
    }
    return result;
}

bool Looper::areAllTrackClearedButThis(LooperTrack * _t) {
    bool result = true;
    for (auto & t : tracks) {
        if (t != _t)result &= t->trackState == LooperTrack::TrackState::CLEARED;
    }
    return result;
}

void Looper::onContainerTriggerTriggered(Trigger * t) {
    if (t == recPlaySelectedTrig) {

        if (selectedTrack != nullptr) selectedTrack->recPlayTrig->trigger();

    }
    else if (t == playSelectedTrig) {

        if (selectedTrack != nullptr) selectedTrack->playTrig->trigger();

    }
    else if (t == clearSelectedTrig) {

        if (selectedTrack != nullptr) selectedTrack->clearTrig->trigger();
        else clearAllTrig->trigger();

    }
    else if (t == stopSelectedTrig) {

        if (selectedTrack != nullptr) selectedTrack->stopTrig->trigger();
        else stopAllTrig->trigger();
    }

    if (t == clearAllTrig) {
        for (int i = tracks.size() - 1; i >= 0; --i) {
            tracks[i]->clearTrig->trigger();
        }
        tracks[0]->askForSelection(true);
    }
    if (t == stopAllTrig) {
        for (int i = tracks.size() - 1; i >= 0; --i) {
            tracks[i]->stopTrig->trigger();
        }
    }
    if (t == selectAllTrig)
    {
        selectMe(nullptr);
    }
}

void Looper::selectMe(LooperTrack * t) {
    if (selectedTrack != nullptr) {
        selectedTrack->setSelected(false);
    }

    selectedTrack = t;

    if (selectedTrack != nullptr) {
        selectedTrack->setSelected(true);
        volumeSelected->setValue(selectedTrack->volume->floatValue());
    }
}

void Looper::onContainerParameterChanged(Parameter * p) {
    if (p == numberOfTracks) {
        setNumTracks(numberOfTracks->value);
    }
    else if (p == volumeSelected)
    {
        if (selectedTrack != nullptr)
        {
            selectedTrack->volume->setValue(volumeSelected->floatValue());
        }
        else
        {
            //define master volume, or all volume ?
        }
    }

    else if(p == TimeManager::getInstance()->playState){
        if(!p->value){
            for(auto &t:tracks){
                t->stopTrig->trigger();
            }
        }
        else{
            for(auto &t:tracks){
                t->playTrig->trigger();
            }
        }
    }
}
