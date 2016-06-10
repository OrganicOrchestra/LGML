/*
 ==============================================================================

 LooperTrack.cpp
 Created: 26 Apr 2016 4:11:41pm
 Author:  bkupe

 ==============================================================================
 */


#include "LooperTrack.h"
#include "TimeManager.h"

#include "LooperNode.h"

#include "DebugHelpers.h"

LooperTrack::LooperTrack(LooperNode * looperNode, int _trackIdx) :
ControllableContainer(String(_trackIdx)),
parentLooper(looperNode),
quantizedRecordStart(-1),
quantizedRecordEnd(-1),
quantizedPlayStart(-1),
quantizedPlayEnd(-1),
streamAudioBuffer(16384),// 16000 ~ 300ms and 256*64
loopSample(1, 44100 * MAX_LOOP_LENGTH_S),
trackState(CLEARED),
trackIdx(_trackIdx),
someOneIsSolo(false),
isSelected (false),
isFadingIn(false),
isCrossFading(false),
isJumping(false),
originBPM(0)
{


    selectTrig = addTrigger("Select", "Select this track");
    recPlayTrig = addTrigger("Rec Or Play", "Tells the track to wait for the next bar and then start record or play");
    playTrig = addTrigger("Play", "Tells the track to wait for the next bar and then stop recording and start playing");
    stopTrig = addTrigger("Stop", "Tells the track to stop ");
    clearTrig = addTrigger("Clear", "Tells the track to clear it's content if got any");
    volume = addFloatParameter("Volume", "Set the volume of the track", defaultVolumeValue, 0, 1);
    mute = addBoolParameter("Mute", "Sets the track muted (or not.)", false);
    solo = addBoolParameter("Solo", "Sets the track solo (or not.)", false);
    beatLength = addIntParameter("Length", "length in bar", 0, 0, 200);


    stateParameterString = addStringParameter("state", "track state", "");
    stateParameterStringSynchronizer = new AsyncTrackStateStringSynchroizer(stateParameterString);
    addTrackListener(stateParameterStringSynchronizer);
    stateParameterString->isControllableFeedbackOnly = true;



    // post init
    volume->setValue(defaultVolumeValue);
    logVolume = float01ToGain(volume->value);
}

void LooperTrack::processBlock(AudioBuffer<float>& buffer, MidiBuffer &) {


    if(updatePendingLooperTrackState(TimeManager::getInstance()->timeInSample.get(), buffer.getNumSamples())){
        padBufferIfNeeded();
    }

    // RECORDING
    if (loopSample.isRecording())
    {
        if(!loopSample.writeAudioBlock(buffer)){
            setTrackState(STOPPED);
        }


    }

    else {
        streamAudioBuffer.writeBlock(buffer);
    }

    // PLAYING
    // allow circular reading , although not sure that overflow need to be handled as its written with same block sizes than read
    // we may need it if we dynamicly change blockSize
    if (loopSample.isOrWasPlaying() )
    {
        loopSample.readNextBlock(buffer);

        if(isFadingIn){ lastVolume = 0;isFadingIn = false;}

        float newVolume = ((someOneIsSolo && !solo->boolValue()) || mute->boolValue()) ? 0 : logVolume;
        // fadeOut
        if(isCrossFading){
            if(!isFadingIn){ newVolume = 0; isFadingIn = true;}
            else isCrossFading = false;
        }
        // fade out on buffer_stop (clear or stop)
        if(loopSample.isStopping() ){
            newVolume = 0;
        }
        for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
            buffer.applyGainRamp(i, 0, buffer.getNumSamples(), lastVolume, newVolume);
        }

        lastVolume = newVolume;

    }
    else {
        // silence output buffer
        buffer.clear();

    }


    loopSample.endProcessBlock();



}
bool LooperTrack::updatePendingLooperTrackState(const uint64 curTime, int _blockSize) {

    bool stateChanged = false;
	jassert (curTime>=0);
    // TODO subBlock precision ?
    // not sure -> triggers are updated at block size granularity

    // for now reduce block approximation noise when quantized
    const uint64 triggeringTime = curTime + _blockSize / 2;
    if (quantizedRecordStart>=0) {
        if (triggeringTime >= quantizedRecordStart) {
            trackState = RECORDING;
            loopSample.setState( PlayableBuffer::BUFFER_RECORDING);
            quantizedRecordStart = -1;
            stateChanged = true;
        }
        else{
            //int waiting = 0;
        }

    }
    else if (quantizedRecordEnd>=0) {
        if (triggeringTime >= quantizedRecordEnd) {
            trackState = PLAYING;
            loopSample.setState( PlayableBuffer::BUFFER_PLAYING);
            quantizedRecordEnd = -1;
            stateChanged = true;

        }
    }



    if (quantizedPlayStart>=0) {
        if (triggeringTime >= quantizedPlayStart) {
            trackState =  PLAYING;
            loopSample.setState( PlayableBuffer::BUFFER_PLAYING);
            quantizedPlayStart = -1;
            stateChanged = true;
        }
    }
    else if (quantizedPlayEnd>=0) {
        if (triggeringTime >= quantizedPlayEnd) {
            trackState = STOPPED;
            loopSample.setState( PlayableBuffer::BUFFER_STOPPED);
            quantizedPlayEnd = -1;
            stateChanged = true;
        }
    }


    if(trackState == SHOULD_STOP){
        trackState = STOPPED;
        loopSample.setState( PlayableBuffer::BUFFER_STOPPED);
        cleanAllQuantizeNeedles();
        stateChanged = true;
    }
    else if(trackState==SHOULD_CLEAR){
        trackState = CLEARED;
        loopSample.setState( PlayableBuffer::BUFFER_STOPPED);
        cleanAllQuantizeNeedles();
        stateChanged = true;
    }

    stateChanged|=loopSample.stateChanged;

    if (!loopSample.checkTimeAlignment(curTime)) {
        LOG("dropping play needle was :" + String(loopSample.getPlayPos())+" but time is"+String(curTime%loopSample.getRecordedLength()));
    }
    //    DBG(playNeedle);
    if(stateChanged){
        trackStateListeners.call(&LooperTrack::Listener::internalTrackStateChanged, trackState);
        //        DBG("a:"+trackStateToString(trackState));

    }


    return stateChanged;





}

void LooperTrack::padBufferIfNeeded(){

    if (loopSample.stateChanged) {
        //    process changed internalState

        if (loopSample.firstRecordedFrame()) {
            loopSample.startRecord();
            if (isMasterTempoTrack()) {
                //                DBG("init predelay : "+String (trackIdx));
                int samplesToGet = (int)(parentLooper->preDelayMs->intValue()*0.001f*parentLooper->getSampleRate());
				TimeManager::getInstance()->jump(samplesToGet);
                loopSample.writeAudioBlock(streamAudioBuffer.getLastBlock(samplesToGet));
            }

            startBeat = TimeManager::getInstance()->getBeat();
        }

        if (loopSample.isFirstPlayingFrameAfterRecord()) {
            if (isMasterTempoTrack()) {
                //                DBG("release predelay : "+String (trackIdx));
                loopSample.cropEndOfRecording((int)(parentLooper->preDelayMs->intValue()*0.001f*parentLooper->getSampleRate()));
//                loopSample.fadeInOut ((int)(parentLooper->getSampleRate()*0.04f),0.05);
//                loopSample.fadeInOut ((int)(parentLooper->getSampleRate()*0.002f),0.000);
				loopSample.fadeInOut (500,0.200);
                loopSample.fadeInOut (100,0.000);
                beatLength->setValue(TimeManager::getInstance()->setBPMForLoopLength(loopSample.getRecordedLength()));
            }
            else{
                beatLength->setValue(TimeManager::getInstance()->getBeat() - startBeat);
            }
            originBPM = TimeManager::getInstance()->BPM->doubleValue();
            loopSample.startPlay();
        }


    }


}


String LooperTrack::trackStateToString(const TrackState & ts) {

    switch (ts) {
        case SHOULD_CLEAR:
        case CLEARED:
            return "cleared";
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
        case SHOULD_STOP:
            return "willStop";


        default:
            jassertfalse;
            break;
    }

    return "[noState]";
}

void LooperTrack::onContainerParameterChanged(Parameter * p)
{
    if (p == volume)
    {
        if (parentLooper->selectedTrack == this) parentLooper->volumeSelected->setValue(volume->floatValue());
        logVolume = float01ToGain(volume->value);

    }
    if(p==solo){
        someOneIsSolo = false;
        for(auto &t:parentLooper->trackGroup.tracks){
            someOneIsSolo |= t->solo->boolValue();
        }
        for(auto &t:parentLooper->trackGroup.tracks){
            t->someOneIsSolo = someOneIsSolo;
        }
    }
}

void LooperTrack::onContainerTriggerTriggered(Trigger * t) {
    if (t == selectTrig)
    {
        parentLooper->selectMe(this);

    }
    else if (t == recPlayTrig) {

        if (trackState == CLEARED) {
            setTrackState(SHOULD_RECORD);
        }
        else  if(trackState!=SHOULD_RECORD){
            setTrackState(SHOULD_PLAY);
        }

    }
    else if (t == playTrig) {
        setTrackState(SHOULD_PLAY);
    }
    else if (t == clearTrig) {
        setTrackState(SHOULD_CLEAR);
    }
    else if (t == stopTrig) {
        setTrackState(SHOULD_STOP);
    }
}

bool LooperTrack::askForBeingMasterTempoTrack() {
    return TimeManager::getInstance()->askForBeingMasterCandidate(parentLooper)
    && parentLooper->askForBeingMasterTrack(this);
}

bool LooperTrack::isMasterTempoTrack(){
    return TimeManager::getInstance()->isMasterCandidate(parentLooper)
    && parentLooper->lastMasterTempoTrack  == this;
}




void LooperTrack::setSelected(bool _isSelected) {
    isSelected=_isSelected;
    trackStateListeners.call(&LooperTrack::Listener::trackSelected, isSelected);
}


void LooperTrack::askForSelection(bool) {
    selectTrig->trigger();
}


void LooperTrack::setTrackState(TrackState newState,int quantizeTime) {

    TimeManager * timeManager = TimeManager::getInstance();
    if(newState==trackState)return;

    if (newState == SHOULD_RECORD) {
        // are we able to set the tempo
        if (askForBeingMasterTempoTrack()) {
            timeManager->isSettingTempo->setValue(true);
            // start As soon as possible
            quantizedRecordStart = 0;
            return;
        }

        else if (!timeManager->isSettingTempo->boolValue()) {
            quantizedRecordStart = timeManager->getNextQuantifiedTime(quantizeTime);
        }
        //            Record per default if triggering other rec while we are current master and we are recording

        else if (timeManager->isMasterCandidate(parentLooper)) {
            timeManager->isSettingTempo->setValue(false);
            timeManager->playState->setValue(true);
            parentLooper->lastMasterTempoTrack->setTrackState(SHOULD_PLAY);

        }
    }


    // on should play
    else if (newState == SHOULD_PLAY) {


        // end of first track
        if ( trackState == RECORDING && askForBeingMasterTempoTrack()
            //            &&  !timeManager->playState->boolValue()
            ) {
            quantizedRecordEnd = -1;

            timeManager->isSettingTempo->setValue(false);
            timeManager->lockTime(true);
            timeManager->playTrigger->trigger();
            trackState=SHOULD_PLAY;
            quantizedPlayStart = 0;
            timeManager->lockTime(false);
        }


        // if every one else is stopped
        else if(parentLooper->askForBeingAbleToPlayNow(this) && trackState == STOPPED) {
            quantizedRecordEnd = -1;

            if(timeManager->isMasterCandidate(parentLooper)){
                trackState=SHOULD_PLAY;
                timeManager->lockTime(true);
                quantizedPlayStart = 0;
                timeManager->playTrigger->trigger();

                timeManager->lockTime(false);
            }
            else{
                quantizedPlayStart = 0;
            }
        }
        // a cleared track can't be played
        else  if(trackState==CLEARED){
            newState=CLEARED;
        }

        else if( trackState == RECORDING && !askForBeingMasterTempoTrack() )  {
            quantizedRecordEnd = timeManager->getNextQuantifiedTime(quantizeTime);
        }

        // on ask for play
        else if(timeManager->playState->boolValue()){
            cleanAllQuantizeNeedles();
            quantizedPlayStart = timeManager->getNextQuantifiedTime(quantizeTime);
            //            quantizedPlayStart = timeManager->getNextQuantifiedTime(1.0/beatLength->intValue());
            //            quantizedPlayStart = timeManager->getTimeForNextBeats(beatLength->value);

        }
    }


    // on should clear
    if (newState == SHOULD_CLEAR) {


        if (parentLooper->currentPreset != nullptr)
        {
            volume->setValue(parentLooper->getPresetValueFor(volume));
            mute->setValue(parentLooper->getPresetValueFor(mute));
            solo->setValue(parentLooper->getPresetValueFor(solo));
        }else
        {
            volume->resetValue();
            mute->resetValue();
            solo->resetValue();
        }
    }


    if (newState == SHOULD_STOP) {
        // force a track to stay in cleared state if stop triggered
        if (trackState == CLEARED) { newState = CLEARED; }
        else if (trackState == SHOULD_CLEAR) { newState = SHOULD_CLEAR; }
    }
    //DBG(newState <<","<<trackState );
    //    DBG(trackStateToString(trackState));

    if(trackState!=newState){
        trackState = newState;
        trackStateListeners.call(&LooperTrack::Listener::internalTrackStateChanged, trackState);
    }

    if((trackState==CLEARED || trackState==SHOULD_CLEAR || trackState==STOPPED || trackState==SHOULD_STOP) ){
        parentLooper->checkIfNeedGlobalLooperStateUpdate();
    }
};

void LooperTrack::cleanAllQuantizeNeedles() {
    quantizedPlayEnd = -1;
    quantizedPlayStart = -1;
    quantizedRecordEnd = -1;
    quantizedRecordStart = -1;
}


//Component * LooperTrack::createDefaultUI(Component * ) {
//    return nullptr;
//}
