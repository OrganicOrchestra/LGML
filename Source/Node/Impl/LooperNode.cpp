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


#include "LooperNode.h"


#include "../NodeContainer/NodeContainer.h"
#include "../../Utils/AudioDebugPipe.h"
#include "../../Engine.h" // for audio Exporting

REGISTER_NODE_TYPE (LooperNode)

LooperNode::LooperNode (StringRef name) :
    NodeBase (name),
    //selectedTrack(nullptr),
    wasMonitoring (false),
    trackGroup (this),
    streamAudioBuffer (2, 16384) // 16000 ~ 300ms and 256*64
{

    numberOfTracks = addNewParameter<IntParameter> ("Number of tracks", "number of tracks in this looper", 8, 1, MAX_NUM_TRACKS);
    numberOfAudioChannelsIn = addNewParameter<IntParameter> ("Number of channels per track", "number of channels on each audioTrack in local folder LGML_audio/LooperName/LGML_Loop_trackNum_index.wav", 1, 1, 16);
    exportAudio =  addNewParameter<Trigger> ("Export audio", "export audio of all recorded Tracks");
    selectAllTrig =  addNewParameter<Trigger> ("Select all", "Select All tracks, for all clear or main volume for instance");
    selectTrack = addNewParameter<IntParameter> ("Select track", "set track selected", 0, -1, 0);
    selectTrack->setSavable(false);
    recPlaySelectedTrig =  addNewParameter<Trigger> ("Rec or play", "Tells the selected track to wait for the next bar and then start record or play");
    playSelectedTrig =  addNewParameter<Trigger> ("Play", "Tells the selected track to wait for the next bar and then stop recording and start playing");
    stopSelectedTrig =  addNewParameter<Trigger> ("Stop", "Tells the selected track to stop ");
    clearSelectedTrig =  addNewParameter<Trigger> ("Clear", "Tells the selected track to clear it's content if got any");
    volumeSelected = addNewParameter<FloatParameter> ("Volume", "Set the volume of the selected track", 1.f, 0.f, 1.f);
    volumeSelected->setSavable(false);
    clearAllTrig =  addNewParameter<Trigger> ("Clear all", "Tells all tracks to clear it's content if got any");
    stopAllTrig =  addNewParameter<Trigger> ("Stop all", "Tells all tracks to stop it's content if got any");
    playAllTrig =  addNewParameter<Trigger> ("Play all", "Tells all tracks to play it's content if got any");
    togglePlayStopAllTrig =  addNewParameter<Trigger> ("Toggle play stop", "Toggle Play/Stop all, will stop if at least one track is playing");
    isMonitoring = addNewParameter<BoolParameter> ("Monitor", "do we monitor audio input ? ", false);
    preDelayMs = addNewParameter<IntParameter> ("Pre Delay ms", "Pre process delay (in milliseconds)", 0, 0, 250);
    quantization = addNewParameter<IntParameter> ("Quantization", "quantization for this looper - 1 is global", -1, -1, 32);
    isOneShot = addNewParameter<BoolParameter> ("Is one shot", "do we play once or loop track", false);
    crossFadeMs = addNewParameter<IntParameter> ("Crossfade", "time of the crossfade when recording (in milliseconds)", 0, 0, 200);
    firstTrackSetTempo = addNewParameter<BoolParameter> ("First track set tempo", "do the first track sets the global tempo or use quantization", true);
    waitForOnset = addNewParameter<BoolParameter> ("Wait for onset", "wait for onset before actually recording", false);
    onsetThreshold = addNewParameter<FloatParameter> ("Onset threshold", "threshold before onset", 0.01f, 0.0001f, 0.1f);
    outputAllTracksSeparately = addNewParameter<BoolParameter> ("Tracks output separated", "split all tracks in separate audio channel out", false);

    autoNextTrackAfterRecord = addNewParameter<BoolParameter> ("Auto next", "If enabled, it will select automatically the next track after a track record.", false);
    autoClearPreviousIfEmpty = addNewParameter<BoolParameter> ("Auto clear previous", "/!\\ Will only work if 'Auto Next' is enabled !\nIf enabled, it will automatically clear the previous track if 'clear' is triggered and the actual selected track is empty.", false);

    selectNextTrig =  addNewParameter<Trigger> ("Select next", "Select Next Track");
    autoSelectTrack =  addNewParameter<BoolParameter> ("Auto select", "If enabled, it will select automatically the track if rec or play is triggered", true);

    addChildControllableContainer (&trackGroup);
    setRateAndBufferSizeDetails (44100, 256);

    trackGroup.setNumTracks (numberOfTracks->intValue());

    selectTrack->setValueFrom (0, false, false);
    if(trackGroup.tracks.size()>=0 ){trackGroup.tracks[0]->setSelected(true);}
    
    setPlayConfigDetails (1, 1, 44100, 256);
    TimeManager::getInstance()->playState->addParameterListener (this);
    TimeManager::getInstance()->BPM->addParameterListener (this);
    setPreferedNumAudioInput (1);
    setPreferedNumAudioOutput (1);
    TimeManager::getInstance()->addTimeManagerListener (this);
#if !BUFFER_CAN_STRETCH
    TimeManager::getInstance()->BPM->isEditable = false;
#endif
}

LooperNode::~LooperNode()
{
    if (TimeManager* tm = TimeManager::getInstanceWithoutCreating())
    {
        tm->playState->removeParameterListener (this);
        tm->BPM->removeParameterListener (this);
        tm->removeTimeManagerListener (this);
    }
}





void LooperNode::processBlockInternal (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{


    streamAudioBuffer.writeBlock (buffer);

    // TODO check if we can optimize copies
    // handle multiples channels outs
    int totalNumInputChannels = jmin(getTotalNumInputChannels(),buffer.getNumChannels());
    int totalNumOutputChannels = jmin(getTotalNumInputChannels(),buffer.getNumChannels());

    jassert (buffer.getNumChannels() >= jmax (totalNumInputChannels, totalNumOutputChannels));
    bufferIn.setSize (totalNumInputChannels, buffer.getNumSamples());
    bufferOut.setSize (totalNumOutputChannels, buffer.getNumSamples());

    if (isMonitoring->boolValue())
    {
        if (!wasMonitoring)
        {
            for (int i = bufferOut.getNumChannels() - 1; i >= 0; --i)
            {
                bufferOut.copyFromWithRamp (i, 0, buffer.getReadPointer (i), buffer.getNumSamples(), 0.0f, 1.0f);
            }

            wasMonitoring = true;
        }
        else
        {
            for (int i = bufferOut.getNumChannels() - 1; i >= 0; --i)
            {
                bufferOut.copyFrom (i, 0, buffer, i, 0, buffer.getNumSamples());
            }
        }
    }
    else
    {
        if (wasMonitoring)
        {
            for (int i = bufferOut.getNumChannels() - 1; i >= 0; --i)
            {
                bufferOut.copyFromWithRamp (i, 0, buffer.getReadPointer (i), buffer.getNumSamples(), 1.0f, 0.0f);
            }

            wasMonitoring = false;
        }
        else
        {
            // todo use reference to buffer if not monitoring (will save last bufferout copy
            bufferOut.clear();
        }
    }


    int numSample = buffer.getNumSamples();

    bool needAudioIn = false;

    for (auto& t : trackGroup.tracks)
    {
        t->updatePendingLooperTrackState ( numSample);
        // avoid each track clearing the buffer if not needed
        needAudioIn |= t->needAudioIn();
    }

    //
    if (!needAudioIn)
    {
        buffer.clear();
        bufferIn.clear();
    }
    else
    {
        for (int i = bufferIn.getNumChannels() - 1; i >= 0; --i)
        {
            bufferIn.copyFrom (i, 0, buffer, i, 0, buffer.getNumSamples());
        }
    }

    //


    if (outputAllTracksSeparately->boolValue())
    {
        for (int i = buffer.getNumChannels() - 1; i >= totalNumInputChannels; --i)
        {
            buffer.copyFrom (i, 0, buffer, i % totalNumInputChannels, 0, numSample);
        }

        int i = 0;
        AudioBuffer<float> tmp;

        for (auto& track : trackGroup.tracks)
        {
            tmp.setDataToReferTo (buffer.getArrayOfWritePointers() + i, totalNumInputChannels, numSample);
            track->processBlock (tmp, midiMessages);
            i += totalNumInputChannels;

        }

    }
    else
    {
        for (auto& t : trackGroup.tracks)
        {
            if (t->needAudioIn()){
                for (int i = totalNumInputChannels - 1; i >= 0; --i)
                {
                    buffer.copyFrom (i, 0, bufferIn, i, 0, buffer.getNumSamples());
                }
            }
            else{
//                buffer.clear();
            }
            
            t->processBlock (buffer, midiMessages);

            for (int i = totalNumInputChannels - 1; i >= 0; --i)
            {
                bufferOut.addFrom (i, 0, buffer, i, 0, buffer.getNumSamples());
            }

            
        }

        for (int i = bufferOut.getNumChannels() - 1; i >= 0; --i)
        {
            buffer.copyFrom (i, 0, bufferOut, i, 0, buffer.getNumSamples());
        }
    }


}


void LooperNode::TrackGroup::addTrack()
{

    LooperTrack* t = new LooperTrack (owner, tracks.size());
    t->setNumChannels(owner->numberOfAudioChannelsIn->intValue());
    tracks.add (t);
    owner->selectTrack->getAs<MinMaxParameter>()->setMinMax (-1, tracks.size() - 1);
    addChildIndexedControllableContainer (t);
}

void LooperNode::TrackGroup::removeTrack (int i)
{
    jassert (i < tracks.size());

    if (selectedTrack == tracks[i])
    {
        selectedTrack = nullptr;
        owner->selectTrack->setValue (i - 1);
    }

    removeChildControllableContainer (tracks[i]);
    tracks.remove (i);
    owner->selectTrack->getAs<MinMaxParameter>()->setMinMax (-1, tracks.size() - 1);
}


void LooperNode::TrackGroup::setNumTracks (int numTracks)
{
    const ScopedLock lk (owner->getCallbackLock());
    int oldSize = tracks.size();

    if (numTracks > oldSize)
    {
        for (int i = oldSize; i < numTracks; i++) { addTrack(); }

        owner->looperListeners.call (&LooperListener::trackNumChanged, numTracks);
    }
    else
    {
        owner->looperListeners.call (&LooperListener::trackNumChanged, numTracks);

        for (int i = oldSize - 1; i >= numTracks; --i) { removeTrack (i); }
    }

    int numToSelect = owner->selectTrack->intValue();
    if(numToSelect>=0){
        if( numToSelect<tracks.size()){
            selectedTrack = tracks.getUnchecked(numToSelect);
        }
        else{
            selectedTrack = nullptr;
        }
    }

}




bool LooperNode::askForBeingMasterTrack (LooperTrack* t)
{
    bool res = firstTrackSetTempo->boolValue() && areAllTrackClearedButThis (t);

    if (res)trackGroup.lastMasterTempoTrack = t;

    return res;
}

bool LooperNode::askForBeingAbleToPlayNow (LooperTrack* _t)
{

    if ( _t->getQuantization() == 0)return true;

    if (TimeManager::getInstance()->playTrigger->isSettingValue()) return true;

    if (!_t->isMasterTempoTrack()) return false;

    bool result = true;

    for (auto& t : trackGroup.tracks)
    {
        if (t != _t)result &=
                (t->trackState == LooperTrack::TrackState::STOPPED) ||
                (t->trackState == LooperTrack::TrackState::CLEARED);
    }

    return result;
}

bool LooperNode::askForBeingAbleToRecNow (LooperTrack* _t)
{
    if ((!firstTrackSetTempo->boolValue() && !TimeManager::getInstance()->isPlaying()) || _t->getQuantization() == 0)return true;

    if (!_t->isMasterTempoTrack()) return false;

    bool result = true;

    for (auto& t : trackGroup.tracks)
    {
        if (t != _t)result &=
                (t->trackState == LooperTrack::TrackState::STOPPED) ||
                (t->trackState == LooperTrack::TrackState::CLEARED);
    }

    return result;
}

bool LooperNode::hasAtLeastOneTrackPlaying()
{
    for (auto& t : trackGroup.tracks)
    {
        if (t->trackState == LooperTrack::TrackState::RECORDING ||
            t->trackState == LooperTrack::TrackState::WILL_PLAY ||
            t->trackState == LooperTrack::TrackState::PLAYING)
        {
            return true;
        }
    }

    return false;
}

bool LooperNode::areAllTrackClearedButThis (LooperTrack* _t)
{
    bool result = true;

    for (auto& t : trackGroup.tracks)
    {
        if (t != _t)result &= t->trackState == LooperTrack::TrackState::CLEARED;
    }

    return result;
}
int LooperNode::getQuantization()
{
    return quantization->intValue() >= 0 ? quantization->intValue() : TimeManager::getInstance()->quantizedBarFraction->intValue();
}
void LooperNode::onContainerTriggerTriggered (Trigger* t)
{
    if (t == recPlaySelectedTrig  )
    {

        if (trackGroup.selectedTrack)
        {

            trackGroup.selectedTrack->recPlay();
            if (autoNextTrackAfterRecord->boolValue() && trackGroup.selectedTrack->trackState == LooperTrack::TrackState::RECORDING) selectTrack->setValue (selectTrack->intValue() + 1);
        }

    }
    else if( t == recSelectedTrig){
        if (trackGroup.selectedTrack)
        {

            trackGroup.selectedTrack->recOrStop();
            if (autoNextTrackAfterRecord->boolValue() && trackGroup.selectedTrack->trackState == LooperTrack::TrackState::RECORDING) selectTrack->setValue (selectTrack->intValue() + 1);
        }
    }
    else if (t == playSelectedTrig)
    {
        if (trackGroup.selectedTrack) trackGroup.selectedTrack->play();

    }
    else if (t == clearSelectedTrig)
    {

        if (trackGroup.selectedTrack)
        {
            if (autoNextTrackAfterRecord->boolValue() && autoClearPreviousIfEmpty->boolValue())
            {
                if (trackGroup.selectedTrack->isEmpty()) selectTrack->setValue (selectTrack->intValue() - 1);
            }

            if (trackGroup.selectedTrack)trackGroup.selectedTrack->clear();
        }
        else
        {
            clearAllTrig->trigger();
        }

    }
    else if (t == stopSelectedTrig)
    {
        if (trackGroup.selectedTrack) trackGroup.selectedTrack->stop();
        else stopAllTrig->trigger();
    }

    if (t == clearAllTrig)
    {
        for (int i = trackGroup.tracks.size() - 1; i >= 0; --i)
        {
            trackGroup.tracks[i]->clear();
        }

        selectTrack->setValue (0);
        outputVolume->setValue (DB0_FOR_01);
    }

    if (t == stopAllTrig)
    {
        for (int i = trackGroup.tracks.size() - 1; i >= 0; --i)
        {
            trackGroup.tracks[i]->stop();
        }
    }
    else if (t == playAllTrig)
    {
        for (int i = trackGroup.tracks.size() - 1; i >= 0; --i)
        {
            trackGroup.tracks[i]->play();
        }
    }
    else if (t == togglePlayStopAllTrig)
    {

        if (hasAtLeastOneTrackPlaying())
        {
            stopAllTrig->trigger();
        }
        else
        {
            playAllTrig->trigger();
        }
    }
    else if (t == selectAllTrig)
    {
        selectTrack->setValue (-1);

    }
    else if (t == selectNextTrig)
    {
        selectTrack->setValue (selectTrack->intValue() + 1);
    }
    else if (t == selectPrevTrig)
    {
        selectTrack->setValue (selectTrack->intValue() - 1);
    }





    if (t == exportAudio)
    {
        ScopedLock lk(getCallbackLock());
        File exportFolder = getEngine()->getCurrentProjectFolder();
        if(!getEngine()->getCurrentProjectFolder().exists()){
            exportFolder = File::getSpecialLocation(File::SpecialLocationType::tempDirectory);
            
            if(exportFolder.exists()){
                OLOGW(juce::translate("session not loaded, exporting to temp folder : ")+ exportFolder.getFullPathName());
            }
            else{
                OLOGE(juce::translate("can't create temporary folder for export path"));
                return;
            }
            
        }
        File folder = exportFolder.getChildFile ("LGML_audio").getChildFile (shortName);
        folder.createDirectory();
        jassert (folder.exists());

        WavAudioFormat format;

        AudioBuffer<float> bufferCached;

        for (auto& tr : trackGroup.tracks)
        {
            if (tr->playableBuffer.getRecordedLength())
            {

                File destFile;
                bool isOutsideFile = false;
                String loadedLGMLPath =tr->sampleChoice->getFirstSelectedValue().toString();
                // overwrite if selected and generated by LGML
                // fileName should start with LGML to allow such special behavior
                if (tr->sampleChoice->selectionIsNotEmpty() )
                {
                    
                    File loadedFile = getEngine()->getFileAtNormalizedPath (loadedLGMLPath);

                    if (loadedFile.getFileNameWithoutExtension().startsWith ("LGML"))
                    {
                        destFile = loadedFile;
                    }
                    else if(loadedLGMLPath.startsWith(File::getSeparatorString())){
                        isOutsideFile = true;
                        destFile = folder.getChildFile(loadedFile.getFileName());
                        OLOGW(juce::translate("copying sample : ")+loadedLGMLPath);
                        loadedFile.copyFileTo(destFile);
                        

                    }
                    else{
                        OLOGW(juce::translate("existing sample already in LGML Folder"));
                        continue;
                        
                    }
                }
                // find first valid name
                else
                {
                    int i = 0;

                    do
                    {
                        destFile = (folder.getChildFile ( "LGML_Loop_" + String (tr->trackIdx) + "_" + String (i) + ".wav"));
                        i++;
                    }
                    while (destFile.exists());
                }

                std::unique_ptr<FileOutputStream> fp ( destFile.createOutputStream());

                if (!isOutsideFile && (fp ))
                {
                    std::unique_ptr<AudioFormatWriter> afw ( format.createWriterFor (fp.get(),
                                                                                   getSampleRate(),
                                                                                   tr->playableBuffer.getNumChannels(),
                                                                                   24,
                                                                                   StringPairArray(), 0));

                    if (afw)
                    {
                        fp.release();
                        afw->writeFromAudioSampleBuffer (tr->playableBuffer.bufferBlockList.fillAll (bufferCached), 0, (int)tr->playableBuffer.getRecordedLength());
                        afw->flush();

                    }
                    else
                    {
                        OLOGE("export audio : can't create output format writer");
                        jassertfalse;
                        continue;
                    }
                }


                tr->sampleChoice->addOrSetOption (destFile.getFileNameWithoutExtension(), getEngine()->getNormalizedFilePath (destFile),false);
                tr->sampleChoice->selectId (destFile.getFileNameWithoutExtension(), true, false);
            }
        }



    }


}

void LooperNode::selectMe (LooperTrack* t)
{
    ControllablesScopedLockType lk (controllableContainers.getLock());

    if (t != nullptr)
    {
        for (auto& tt : trackGroup.tracks)
        {
            if (tt->isSelected)
                tt->setSelected (false);
        };
    }

    trackGroup.selectedTrack = t;

    if (trackGroup.selectedTrack != nullptr)
    {
        trackGroup.selectedTrack->setSelected (true);
        volumeSelected->setValue (trackGroup.selectedTrack->volume->floatValue());
        selectTrack->setValue (trackGroup.selectedTrack->trackIdx);
    }
}
void LooperNode::numChannelsChanged (bool isInput)
{
    if (isInput)
    {
        for (auto& t : trackGroup.tracks)
        {
            t->setNumChannels (getTotalNumInputChannels());
        }

        streamAudioBuffer.setNumChannels (getTotalNumInputChannels());
    }
}
void LooperNode::onContainerParameterChanged ( ParameterBase* p)
{
    NodeBase::onContainerParameterChanged (p);

    if (p == numberOfTracks)
    {
        int oldIdx = trackGroup.selectedTrack ? trackGroup.selectedTrack->trackIdx : 0;
        std::unique_ptr<ScopedLock> lkp;

        if (parentNodeContainer)
        {
            lkp = std::make_unique< ScopedLock> (parentNodeContainer->getAudioGraph()->getCallbackLock());
        }

        trackGroup.setNumTracks (numberOfTracks->intValue());

        if (outputAllTracksSeparately->boolValue())
        {
            setPreferedNumAudioOutput (getTotalNumInputChannels()*numberOfTracks->intValue());
        }

        if (oldIdx >= numberOfTracks->intValue())
        {
            if (trackGroup.tracks.size())
            {
                trackGroup.selectedTrack = trackGroup.tracks[trackGroup.tracks.size() - 1];
            }
            else
                trackGroup.selectedTrack = nullptr;
        }
    }
    else if(p==crossFadeMs){
        for(auto t : trackGroup.tracks){
            t->playableBuffer.setFadeBufferTime( crossFadeMs->intValue());
        }
    }
    else if (p == volumeSelected)
    {
        if (trackGroup.selectedTrack != nullptr)
        {
            trackGroup.selectedTrack->volume->setValueFrom (this,volumeSelected->floatValue());
        }
        else
        {
            //define master volume, or all volume ?
        }
    }
    else if (p == outputAllTracksSeparately)
    {
        if (outputAllTracksSeparately->boolValue())
        {
            setPreferedNumAudioOutput (getTotalNumInputChannels()*numberOfTracks->intValue());
        }
        else
        {
            setPreferedNumAudioOutput (getTotalNumInputChannels());
        }
    }
    else if (p == numberOfAudioChannelsIn)
    {
        std::unique_ptr<ScopedLock> lkp;

        if (parentNodeContainer)
        {
            lkp = std::make_unique< ScopedLock> (parentNodeContainer->getAudioGraph()->getCallbackLock());
        }

        setPreferedNumAudioInput (numberOfAudioChannelsIn->intValue());
        setPreferedNumAudioOutput (numberOfAudioChannelsIn->intValue() * (outputAllTracksSeparately->boolValue() ? trackGroup.tracks.size() : 1));
    }
    else if (p == selectTrack)
    {
        bool changed = true;

        if (trackGroup.selectedTrack)changed = trackGroup.selectedTrack->trackIdx != p->intValue();

        if (changed)
        {
            if (selectTrack->intValue() >= 0)
            {
                if (selectTrack->intValue() < trackGroup.tracks.size())
                {
                    trackGroup.tracks.getUnchecked (selectTrack->intValue())->selectTrig->trigger();

                }

            }
            else
            {
                selectMe (nullptr);

                for (auto& t : trackGroup.tracks)
                {
                    t->setSelected (true);
                }

            }
        }
    }
    else if(p == quantization){
        // TODO should react 
        bool wasQuantized = (int)quantization->lastValue !=0;
        bool isQuantized = getQuantization()>0;
        if(wasQuantized!=isQuantized)
            setAllTimeRatios();

    }

    // TimeManager
    else if (p == TimeManager::getInstance()->playState)
    {
        if(getQuantization()>0){
        if (!TimeManager::getInstance()->playState->boolValue())
        {
            for (auto& t : trackGroup.tracks)
            {
                t->stop();
            }
        }
        else //if (!isOneShot->boolValue())
        {
            // prevent time manager to update track internal state before all tracks are updated
            TimeManager::getInstance()->lockTime (true);

            for (auto& t : trackGroup.tracks)
            {
                if (t->trackState != LooperTrack::CLEARED && t->trackState != LooperTrack::WILL_RECORD && t->trackState != LooperTrack::PLAYING) t->setTrackState (LooperTrack::TrackState::WILL_PLAY);
            }

            TimeManager::getInstance()->lockTime (false);
        }
    }
    }


    else  if (p == TimeManager::getInstance()->BPM)
    {
        BPMChanged (p->doubleValue());
    }


}

void LooperNode::clearInternal()
{
    // get called after deletion of TimeManager on app exit
    TimeManager* tm = TimeManager::getInstanceWithoutCreating();

    if (tm != nullptr)
    {
        tm->releaseIfMasterCandidate (this);
    }

}

// worst onset detection function ever ...
bool LooperNode::hasOnset()
{
    bool hasOnset = globalRMSValueIn > onsetThreshold->floatValue();
    return hasOnset;
}

void LooperNode::BPMChanged (double /*BPM*/) { setAllTimeRatios();}
void LooperNode::setAllTimeRatios(){
#if BUFFER_CAN_STRETCH

    if (!TimeManager::getInstance()->isMasterCandidate (this) )
    {
        bool needStretch =getQuantization() > 0;
        double currentBPM = TimeManager::getInstance()->BPM->doubleValue();
        for (auto& t : trackGroup.tracks)
        {
            if (!t->isEmpty())
            {
                double ratio =needStretch? t->originBPM->doubleValue()/currentBPM:1;
                if (std::isnormal (ratio))
                {
//                    bool wasPlaying = t->trackState==LooperTrack::PLAYING;
                    t->playableBuffer.setTimeRatio (ratio,false);
//                    if(wasPlaying)
//                        t->play();

                    //          if( DEBUGPIPE_ENABLED){
                    //            if(ratio!=1){
                    //
                    //              AudioBuffer<float> b;
                    //              b.setDataToReferTo(t->playableBuffer.audioBuffer.getArrayOfWritePointers(), 1, t->playableBuffer.getRecordedLength());
                    //              //          DBGAUDIO("trackStretch"+String(t->trackIdx),b);
                    //              //          DBGAUDIOSETBPM("trackStretch"+String(t->trackIdx),TimeManager::getInstance()->BPM->doubleValue());
                    //            }
                    //          }
                }

                else
                {
                    DBG ("wrong bpms for stretch : " << TimeManager::getInstance()->BPM->doubleValue() << "," << t->originBPM->doubleValue());
                    jassertfalse;
                }

            }
        }
    }

#endif

};

void LooperNode::timeJumped (sample_clk_t /*time*/)
{

};

bool LooperNode::isBoundToTime()
{
    if (getQuantization() > 0)
    {
        for (auto& t : trackGroup.tracks)
        {
            if (!t->isEmpty()) {return true;}
        }

        return false;
    }
    else
    {
        return false;
    }

};
void LooperNode::playStop (bool isPlaying)
{
    if (isPlaying)
    {
        for (auto& t : trackGroup.tracks)
        {
            if (t->trackState == LooperTrack::TrackState::WILL_RECORD)
            {
                //      t->quantizedRecordStart=0;
            }
        }
    }

}
