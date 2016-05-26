/*
 ==============================================================================

 LooperNode.cpp
 Created: 3 Mar 2016 10:32:16pm
 Author:  bkupe

 ==============================================================================
 */

#include "LooperNode.h"
#include "LooperNodeUI.h"
#include "TimeManager.h"

LooperNode::LooperNode() :
NodeBase("Looper",NodeType::LooperType),
selectedTrack(nullptr),
wasMonitoring(false),
trackGroup(this)
{

    numberOfTracks = addIntParameter("numberOfTracks", "number of tracks in this looper", 8, 1, MAX_NUM_TRACKS);

    selectAllTrig = addTrigger("Select All", "Select All tracks, for all clear or main volume for instance");
    selectTrack = addIntParameter("Select track", "set track selected", 0, -1, 0);
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

    addChildControllableContainer(&trackGroup);

    trackGroup.setNumTracks(numberOfTracks->intValue());

    selectTrack->setValue(0,false,true);
    TimeManager::getInstance()->playState->addParameterListener(this);
}

LooperNode::~LooperNode()
{
    if (TimeManager::getInstanceWithoutCreating()) {
        TimeManager::getInstance()->playState->removeParameterListener(this);
    }
}

ConnectableNodeUI * LooperNode::createUI(){
    NodeBaseUI * ui = new NodeBaseUI(this, new LooperNodeContentUI);
    ui->recursiveInspectionLevel = 2;
    ui->canInspectChildContainersBeyondRecursion = false;
    return ui;
}



void LooperNode::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer &midiMessages) {

    // TODO check if we can optimize copies
    // handle multiples channels outs

    bufferIn.setSize(buffer.getNumChannels(), buffer.getNumSamples());
    bufferOut.setSize(buffer.getNumChannels(), buffer.getNumSamples());

    if (isMonitoring->boolValue()) {
        if (!wasMonitoring) {
            for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
                bufferOut.copyFromWithRamp(i, 0, buffer.getReadPointer(i), buffer.getNumSamples(), 0.0f, 1.0f);
            }
            wasMonitoring = true;
        }
        else {
            for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
                bufferOut.copyFrom(i, 0, buffer, i, 0, buffer.getNumSamples());
            }
        }
    }
    else {
        if (wasMonitoring) {
            for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
                bufferOut.copyFromWithRamp(i, 0, buffer.getReadPointer(i), buffer.getNumSamples(), 1.0f, 0.0f);
            }
            wasMonitoring = false;
        }
        else {
            bufferOut.clear();
        }
    }
    for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
        bufferIn.copyFrom(i, 0, buffer, i, 0, buffer.getNumSamples());
    }
    for (auto & t : trackGroup.tracks) {
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


void LooperNode::TrackGroup::addTrack() {
    LooperTrack * t = new LooperTrack(owner, tracks.size());
    tracks.add(t);
    owner->selectTrack->setRange(-1,tracks.size()-1);
    addChildIndexedControllableContainer(t);
}

void LooperNode::TrackGroup::removeTrack(int i) {
    removeChildControllableContainer(tracks[i]);
    tracks.remove(i);
    owner->selectTrack->setRange(-1,tracks.size()-1);}


void LooperNode::TrackGroup::setNumTracks(int numTracks) {
    int oldSize = tracks.size();
    if (numTracks>oldSize) {
        for (int i = oldSize; i< numTracks; i++) { addTrack(); }
        owner->looperListeners.call(&LooperListener::trackNumChanged, numTracks);
    }
    else {
        owner->looperListeners.call(&LooperListener::trackNumChanged, numTracks);
        for (int i = oldSize - 1; i >= numTracks; --i) { removeTrack(i); }
    }

}


void LooperNode::checkIfNeedGlobalLooperStateUpdate() {
    if(TimeManager::getInstance()->hasMasterNode()){
        bool needToReleaseMasterTempo = true;
        bool needToStop = TimeManager::getInstance()->playState->boolValue() && TimeManager::getInstance()->isMasterNode(this);
        for (auto & t : trackGroup.tracks) {
            //            DBG("s"+LooperTrack::trackStateToString(t->trackState));
            needToReleaseMasterTempo &= (t->trackState == LooperTrack::TrackState::CLEARED ||
                                         t->trackState == LooperTrack::TrackState::SHOULD_CLEAR);
            needToStop &=   (t->trackState == LooperTrack::TrackState::CLEARED ||
                             t->trackState == LooperTrack::TrackState::SHOULD_CLEAR)||
            (t->trackState == LooperTrack::TrackState::STOPPED ||
             t->trackState == LooperTrack::TrackState::SHOULD_STOP);
        }

        if (needToReleaseMasterTempo) {
            TimeManager::getInstance()->releaseMasterNode(this);
        }
        if (needToStop){
            TimeManager::getInstance()->stopTrigger->trigger();
        }
    }
}


bool LooperNode::askForBeingMasterTrack(LooperTrack * t) {
    bool res = areAllTrackClearedButThis(t);
    if (res)lastMasterTempoTrack = t;
    return res;
}

bool LooperNode::askForBeingAbleToPlayNow(LooperTrack * _t) {
    bool result = true;
    for (auto & t : trackGroup.tracks) {
        if (t != _t)result &=
            (t->trackState == LooperTrack::TrackState::STOPPED) ||
            (t->trackState == LooperTrack::TrackState::CLEARED) ;
    }
    return result;
}

bool LooperNode::areAllTrackClearedButThis(LooperTrack * _t) {
    bool result = true;
    for (auto & t : trackGroup.tracks) {
        if (t != _t)result &= t->trackState == LooperTrack::TrackState::CLEARED;
    }
    return result;
}

void LooperNode::onContainerTriggerTriggered(Trigger * t) {
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
        for (int i = trackGroup.tracks.size() - 1; i >= 0; --i) {
            trackGroup.tracks[i]->clearTrig->trigger();
        }
        trackGroup.tracks[0]->askForSelection(true);
    }
    if (t == stopAllTrig) {
        for (int i = trackGroup.tracks.size() - 1; i >= 0; --i) {
            trackGroup.tracks[i]->stopTrig->trigger();
        }
    }
    if (t == selectAllTrig)
    {
        selectMe(nullptr);
    }
}

void LooperNode::selectMe(LooperTrack * t) {
    if (selectedTrack != nullptr) {
        selectedTrack->setSelected(false);
    }

    selectedTrack = t;

    if (selectedTrack != nullptr) {
        selectedTrack->setSelected(true);
        volumeSelected->setValue(selectedTrack->volume->floatValue());
        selectTrack->setValue(selectedTrack->trackIdx);
    }
}

void LooperNode::onContainerParameterChanged(Parameter * p) {
    NodeBase::onContainerParameterChanged(p);
    if (p == numberOfTracks) {
        trackGroup.setNumTracks(numberOfTracks->value);
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

    else if (p == TimeManager::getInstance()->playState) {
        if (!TimeManager::getInstance()->playState->boolValue()) {
            for (auto &t : trackGroup.tracks) {
                t->stopTrig->trigger();
            }
        }
        else {
            // prevent time manager to update track internal state before all tracks are updated
            TimeManager::getInstance()->lockTime(true);
            for (auto &t : trackGroup.tracks) {
                t->setTrackState(LooperTrack::TrackState::SHOULD_PLAY,0);
            }
            TimeManager::getInstance()->lockTime(false);
        }
    }
    else if(p==selectTrack){
        bool changed = true;
        if( selectedTrack!=nullptr)changed  = selectedTrack->trackIdx!=p->intValue();

        if(changed){
            if(selectTrack->intValue()>=0){
                if(selectTrack->intValue() < trackGroup.tracks.size()){
                    trackGroup.tracks.getUnchecked(selectTrack->intValue())->selectTrig->trigger();
                    
                }
                
            }
            else{
                selectMe(nullptr);
            }
        }
    }
}
