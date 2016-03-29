/*
 ==============================================================================

 LooperNode.cpp
 Created: 3 Mar 2016 10:32:16pm
 Author:  bkupe

 ==============================================================================
 */

#include "LooperNode.h"
#include "TimeManager.h"

#include "LooperNodeUI.h"

LooperNode::LooperNode(NodeManager * nodeManager,uint32 nodeId) :NodeBase(nodeManager,nodeId,"Looper",new Looper(this)) {
    looper = dynamic_cast<Looper*>(audioProcessor);
    addChildControllableContainer(looper);
}


LooperNode::Looper::Looper(LooperNode * looperNode):
ControllableContainer("InnerLooper"),
selectedTrack(nullptr),
looperNode(looperNode)
{

    recPlaySelectedTrig =   addTrigger("Rec Or Play",
                                       "Tells the selected track to wait for the next bar \
                                       and then start record or play");

    playSelectedTrig =      addTrigger("Play",
                                       "Tells the selected track to wait for the next bar and \
                                       then stop recording and start playing");
									   
    stopSelectedTrig =      addTrigger("Stop",
                                       "Tells the selected track to stop ");

    clearSelectedTrig =     addTrigger("Clear",
                                       "Tells the selected track to clear it's content if got any");

    volumeSelected =        addFloatParameter("Volume",
                                              "Set the volume of the selected track",
                                              1, 0, 1);


    clearAllTrig = addTrigger("ClearAll",
                              "Tells all tracks to clear it's content if got any");

    stopAllTrig = addTrigger("StopAll",
                             "Tells all tracks to stop it's content if got any");

    isMonitoring = addBoolParameter("monitor", "do we monitor audio input ? ", true);

    numberOfTracks = addIntParameter("numberOfTracks", "number of tracks in this looper", 0, 0, MAX_NUM_TRACKS);
                                       
                                       
    skipControllableNameInAddress = true;

    
    recPlaySelectedTrig->addTriggerListener(this);
    playSelectedTrig->addTriggerListener(this);
    clearSelectedTrig->addTriggerListener(this);
    stopSelectedTrig->addTriggerListener(this);
    clearAllTrig->addTriggerListener(this);
    stopAllTrig->addTriggerListener(this);
									   
	numberOfTracks->setValue(8);
									   
									   
}

void LooperNode::Looper::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer &midiMessages){

    // TODO check if we can optimize copies
    // handle multiples channels outs

    bufferIn.setSize(buffer.getNumChannels(),buffer.getNumSamples());
    bufferOut.setSize(buffer.getNumChannels(),buffer.getNumSamples());

    if(isMonitoring->value){
        for(int i = buffer.getNumChannels()-1;i>=0 ;--i){
            bufferOut.copyFrom(i, 0, buffer, i, 0, buffer.getNumSamples());
        }
    }
    else{
        bufferOut.clear();
    }
    for(int i = buffer.getNumChannels()-1;i>=0 ;--i){
        bufferIn.copyFrom(i, 0, buffer, i, 0, buffer.getNumSamples());
    }
    for( auto & t:tracks){
        t->processBlock(buffer,midiMessages);
        for(int i = buffer.getNumChannels()-1;i>=0 ;--i){
            bufferOut.addFrom(i,0,buffer,i,0,buffer.getNumSamples());
            buffer.copyFrom(i,0,bufferIn,i,0,buffer.getNumSamples());
        }
    }
    for(int i = buffer.getNumChannels()-1;i>=0 ;--i){
        buffer.copyFrom(i,0, bufferOut,i,0,buffer.getNumSamples());

    }




}



void LooperNode::Looper::addTrack(){
    Track * t = new Track(this, tracks.size());
    tracks.add(t);
    addChildControllableContainer(t);
}

void LooperNode::Looper::removeTrack( int i){
    removeChildControllableContainer(tracks[i]);
    tracks.remove(i);
}


void LooperNode::Looper::setNumTracks(int numTracks){
    int oldSize = tracks.size();
    if(numTracks>oldSize)   { for(int i = oldSize ; i< numTracks ; i++)     {addTrack();}}
    else                    {for (int i = oldSize - 1; i > numTracks; --i)  {removeTrack(i);}}
    looperListeners.call(&Looper::Listener::trackNumChanged,numTracks);
}


void LooperNode::Looper::checkIfNeedGlobalLooperStateUpdate(){
    bool needToStop = true;
    bool needToReleaseMasterTempo = true;
    for(auto & t : tracks){
        needToStop &= (t->trackState == Track::STOPPED  ||t->trackState == Track::CLEARED  ) ;
        needToReleaseMasterTempo &= (t->trackState == Track::CLEARED );
    }


    if (TimeManager::getInstance()->isMasterNode(looperNode) && needToStop) {
        TimeManager::getInstance()->stop();
    }

    if (needToReleaseMasterTempo) {
        TimeManager::getInstance()->removeIfMaster(looperNode);
    }
}


void LooperNode::Looper::triggerTriggered(Trigger * t){
    if(selectedTrack!=nullptr){
        if(t == recPlaySelectedTrig){selectedTrack->recPlayTrig->trigger();
        }else if(t == playSelectedTrig){selectedTrack->playTrig->trigger();
        }else if(t == clearSelectedTrig){selectedTrack->clearTrig->trigger();
        }else if(t == stopSelectedTrig){selectedTrack->stopTrig->trigger();}
    }
    if(t == clearAllTrig){
        for(int i = tracks.size()-1 ; i>=0 ; --i){tracks[i]->clearTrig->trigger();}
    }
    if(t == stopAllTrig){
        for(int i = tracks.size()-1 ; i>=0 ; --i){tracks[i]->stopTrig->trigger();}
    }
}

void LooperNode::Looper::selectMe(Track * t){
    if(selectedTrack!=nullptr ){
        selectedTrack->setSelected(false);
    }
    selectedTrack = t;
    if(selectedTrack!=nullptr ){
        selectedTrack->setSelected(true);
    }
}

void LooperNode::Looper::parameterValueChanged(Parameter * p) {
	if(p==numberOfTracks){
		setNumTracks(numberOfTracks->value);
	}
}

/////////
// TRACK
////////




LooperNode::Looper::Track::Track(Looper * looper, int _trackIdx) :
ControllableContainer("Track " + String(_trackIdx)),
parentLooper(looper),
quantizedRecordStart(0),
quantizedRecordEnd(0),
quantizedPlayStart(0),
quantizedPlayEnd(0),
recordNeedle(0),
playNeedle(0),
streamAudioBuffer(16384),// 16000 ~ 300ms and 256*64
monoLoopSample(1,44100*MAX_LOOP_LENGTH_S),
trackState(CLEARED),
internalTrackState(BUFFER_STOPPED),
lastInternalTrackState(internalTrackState),
trackIdx(_trackIdx)
{

    //setCustomShortName("track/" + String(_trackIdx)); //can't use "/" in shortName, will use ControllableIndexedContainer for that when ready.

    recPlayTrig =   addTrigger("Rec Or Play",
                               "Tells the track to wait for the next bar \
                               and then start record or play");

    playTrig =      addTrigger("Play",
                               "Tells the track to wait for the next bar and \
                               then stop recording and start playing");
    stopTrig =     addTrigger("Stop",
                              "Tells the track to stop ");

    clearTrig =     addTrigger("Clear",
                               "Tells the track to clear it's content if got any");

    volume =        addFloatParameter("Volume",
                                      "Set the volume of the track",
                                      1, 0, 1);

    preDelayMs =    addIntParameter("Pre Delay MS",
                                    "Pre process delay (in milliseconds)",
                                    40, 0, 200);


    stateParameterString = addStringParameter("state", "track state", "");
    stateParameterStringSynchronizer = new AsyncTrackStateStringSynchroizer(stateParameterString);
    addTrackListener(stateParameterStringSynchronizer);
    stateParameterString->isControllableFeedbackOnly = true;
    preDelayMs->isControllableExposed = false;

    recPlayTrig->addTriggerListener(this);
    playTrig->addTriggerListener(this);
    clearTrig->addTriggerListener(this);
    stopTrig->addTriggerListener(this);


    // post init
    volume->setValue(defaultVolumeValue);
}

void LooperNode::Looper::Track::processBlock(AudioBuffer<float>& buffer, MidiBuffer &){


    updatePendingLooperTrackState(TimeManager::getInstance()->timeInSample,buffer.getNumSamples());


    // RECORDING
    if (internalTrackState == BUFFER_RECORDING )
    {
        if(recordNeedle + buffer.getNumSamples()> parentLooper->getSampleRate() * MAX_LOOP_LENGTH_S){
            setTrackState(STOPPED);
        }
        else{
            for(int i = monoLoopSample.getNumChannels()-1;i>=0 ;--i){
                monoLoopSample.copyFrom(i, recordNeedle, buffer, i, 0, buffer.getNumSamples());
            }
            recordNeedle += buffer.getNumSamples();
        }

    }

    else{
        streamAudioBuffer.writeBlock(buffer);
    }

    // PLAYING
    // allow circular reading , although not sure that overflow need to be handled as its written with same block sizes than read
    // we may need it if we start to use a different clock  than looperState in OOServer that has a granularity of blockSize
    // or if we dynamicly change blockSize
    if (internalTrackState==BUFFER_PLAYING && recordNeedle>0 && monoLoopSample.getNumSamples())
    {
        if ( (playNeedle + buffer.getNumSamples()) > recordNeedle)
        {

            //assert false for now see above
            //            jassert(false);
            int firstSegmentLength = recordNeedle - playNeedle;
            int secondSegmentLength = buffer.getNumSamples() - firstSegmentLength;
            for(int i = buffer.getNumChannels()-1;i>=0 ;--i){
                int maxChannelFromRecorded =jmin(monoLoopSample.getNumChannels()-1,i);
                buffer.copyFrom(i, 0, monoLoopSample, maxChannelFromRecorded, playNeedle, firstSegmentLength);
                buffer.copyFrom(i, 0, monoLoopSample, maxChannelFromRecorded, 0, secondSegmentLength);
            }
            playNeedle = secondSegmentLength;

        }else{
            for(int i = buffer.getNumChannels()-1;i>=0 ;--i){
                int maxChannelFromRecorded =jmin(monoLoopSample.getNumChannels()-1,i);
                buffer.copyFrom(i, 0, monoLoopSample, maxChannelFromRecorded, playNeedle, buffer.getNumSamples());
            }
            playNeedle += buffer.getNumSamples();
            playNeedle %= recordNeedle;
        }
        buffer.applyGainRamp(0, 0, buffer.getNumSamples(), lastVolume,volume->value);
        lastVolume = volume->value;


    }
    else{
        // silence output buffer
        for(int i = buffer.getNumChannels()-1;i>=0 ;--i){
            buffer.applyGain(i, 0, buffer.getNumSamples(), 0);
        }
    }


}
void LooperNode::Looper::Track::updatePendingLooperTrackState(const uint64 curTime, int _blockSize){


    //    process changed internalState
    if(internalTrackState!=lastInternalTrackState){

        if(internalTrackState == BUFFER_RECORDING){
            if(askForBeingMasterTempoTrack() ){
                int samplesToGet = (int)(preDelayMs->value*0.001f*parentLooper->getSampleRate());
                for(int i = monoLoopSample.getNumChannels()-1;i>=0 ;--i){
                    monoLoopSample.copyFrom(i,0,streamAudioBuffer.getLastBlock(samplesToGet,i),samplesToGet);
                }
                recordNeedle = samplesToGet;
            }

            else{
                recordNeedle = 0;
            }
        }

        if(lastInternalTrackState ==BUFFER_RECORDING){
            if( askForBeingMasterTempoTrack()){
                recordNeedle-= (int)(preDelayMs->value*0.001f*parentLooper->getSampleRate());

                const int fadeNumSamples = (int)(parentLooper->getSampleRate()*0.022f);
                if(fadeNumSamples>0 && recordNeedle>2*fadeNumSamples){
                    monoLoopSample.applyGainRamp(0, 0, fadeNumSamples, 0, 1);
                    monoLoopSample.applyGainRamp(0,recordNeedle - fadeNumSamples, fadeNumSamples, 1, 0);
                }
                TimeManager::getInstance()->setBPMForLoopLength(recordNeedle);


            }
            playNeedle=0;
        }
    }

    // TODO subBlock precision ?
    // not sure -> triggers are updated at block size granularity

    // for now reduce block approximation noise when quantized
    const uint64 triggeringTime = curTime  + _blockSize/2;
    if(quantizedRecordStart>0){
        if(triggeringTime>=quantizedRecordStart){
            setTrackState(RECORDING);
        }

    }
    else if( quantizedRecordEnd>0){
        if(triggeringTime>=quantizedRecordEnd){
            setTrackState(PLAYING);

        }
    }



    if(quantizedPlayStart>0){
        if(triggeringTime>=quantizedPlayStart){
            setTrackState(PLAYING);
        }
    }
    else if( quantizedPlayEnd>0){
        if(triggeringTime>=quantizedPlayEnd){
            setTrackState(STOPPED);
        }
    }

    lastInternalTrackState = internalTrackState;
}


String LooperNode::Looper::Track::trackStateToString(const TrackState & ts){

    switch (ts) {
        case SHOULD_CLEAR:
        case CLEARED:
            return "empty";
        case PLAYING:
            return "playing";
        case RECORDING:
            return "recording";
        case SHOULD_PLAY:
            return "willPlay";
        case SHOULD_RECORD:
            return "willRecord";
        case STOPPED:
            return "stopped";

        default:
            jassertfalse;
            break;
    }

	return "[noState]";
}

void LooperNode::Looper::Track::triggerTriggered(Trigger * t){
    if(t == recPlayTrig){
        if(trackState == CLEARED || trackState == STOPPED){
            setTrackState(SHOULD_RECORD);
        }
        else{
            setTrackState(SHOULD_PLAY);
        }
    }
    else if(t == playTrig){
        setTrackState(SHOULD_PLAY);
    }
    else if(t== clearTrig){
        setTrackState(SHOULD_CLEAR);
    }
    else if (t == stopTrig){
        setTrackState(STOPPED);
    }
}

bool LooperNode::Looper::Track::askForBeingMasterTempoTrack(){
    return TimeManager::getInstance()->askForBeingMasterNode(parentLooper->looperNode)
    && parentLooper->askForBeingMasterTrack(this);
}


void LooperNode::Looper::Track::setSelected(bool isSelected){
    trackStateListeners.call(&LooperNode::Looper::Track::Listener::trackSelected,isSelected);
}
void LooperNode::Looper::Track::askForSelection(bool isSelected){
    parentLooper->selectMe(isSelected?this:nullptr);
}


void LooperNode::Looper::Track::setTrackState(TrackState newState){



    // quantify
    if(newState == SHOULD_RECORD){
        // are we able to set the tempo
        if( askForBeingMasterTempoTrack()){
            TimeManager::getInstance()->stop();
            TimeManager::getInstance()->setPlayState(true,true);
            quantizedRecordStart = -1;
            setTrackState(RECORDING);
            return;
        }
        else if(!TimeManager::getInstance()->getIsSettingTempo()){
            quantizedRecordStart =TimeManager::getInstance()->getNextQuantifiedTime();
        }
        else {
//            RecordPer default if triggering other rec whil master is recording
            newState = RECORDING;
            TimeManager::getInstance()->setPlayState(true,false);
            //        another master track  is recording
//            if(parentLooper->lastMasterTempoTrack->trackState==LooperNode::Looper::Track::TrackState::RECORDING){
                parentLooper->lastMasterTempoTrack->setTrackState(LooperNode::Looper::Track::TrackState::PLAYING);
//            }
        }
    }

    // on true start recording
    if(newState == RECORDING){
        internalTrackState = BUFFER_RECORDING;
        quantizedRecordStart = -1;

    }
    // on true end recording
    else if(trackState == RECORDING && newState==SHOULD_PLAY){
        {

            if(askForBeingMasterTempoTrack()){
                quantizedRecordEnd = -1;
                newState = PLAYING;
            }
            else{
                quantizedRecordEnd = TimeManager::getInstance()->getNextQuantifiedTime();
            }
        }
    }

    // on ask for play
    if(newState ==SHOULD_PLAY){
        cleanAllQuantizeNeedles();
        quantizedPlayStart = TimeManager::getInstance()->getNextQuantifiedTime();
    }
    // on true start of play
    else if(newState ==PLAYING){
        internalTrackState = BUFFER_PLAYING;
        cleanAllQuantizeNeedles();
        playNeedle = 0;

    }
    // on true end of play
    else if (trackState== PLAYING && newState!=PLAYING){
        quantizedPlayEnd = -1;
    }
    // on should clear
    if(newState == SHOULD_CLEAR){
        recordNeedle = 0;
        playNeedle = 0;
        cleanAllQuantizeNeedles();
        volume->setValue(defaultVolumeValue);
        newState = CLEARED;
        internalTrackState = BUFFER_STOPPED;


    }


    if(newState == STOPPED){
        internalTrackState = BUFFER_STOPPED;
        cleanAllQuantizeNeedles();
        // force a track to stay in cleared state if stop triggered
        if(trackState == CLEARED){newState = CLEARED;}
    }
    //DBG(newState <<","<<trackState );

    trackState = newState;



    parentLooper->checkIfNeedGlobalLooperStateUpdate();
    trackStateListeners.call(&LooperNode::Looper::Track::Listener::internalTrackStateChanged,trackState);
};

void LooperNode::Looper::Track::cleanAllQuantizeNeedles(){
    quantizedPlayEnd = -1;
    quantizedPlayStart = -1;
    quantizedRecordEnd = -1;
    quantizedRecordStart = -1;
}




NodeBaseUI * LooperNode::createUI(){return new NodeBaseUI(this, new LooperNodeUI);}
