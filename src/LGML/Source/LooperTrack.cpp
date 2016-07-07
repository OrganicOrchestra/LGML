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
loopSample(2, 44100 * MAX_LOOP_LENGTH_S),
trackState(CLEARED),
desiredState(CLEARED),
trackIdx(_trackIdx),
someOneIsSolo(false),
isSelected (false),
isFadingIn(false),
isCrossFading(false),
originBPM(0),
lastVolume(0)
{


  selectTrig = addTrigger("Select", "Select this track");
  recPlayTrig = addTrigger("Rec Or Play", "Tells the track to wait for the next bar and then start record or play");
  playTrig = addTrigger("Play", "Tells the track to wait for the next bar and then stop recording and start playing");
  stopTrig = addTrigger("Stop", "Tells the track to stop ");
  clearTrig = addTrigger("Clear", "Tells the track to clear it's content if got any");
  volume = addFloatParameter("Volume", "Set the volume of the track", defaultVolumeValue, 0, 1);
  mute = addBoolParameter("Mute", "Sets the track muted (or not.)", false);
  solo = addBoolParameter("Solo", "Sets the track solo (or not.)", false);
  beatLength = addFloatParameter("Length", "length in bar", 0, 0, 200);


  stateParameterString = addStringParameter("state", "track state", "");
  stateParameterStringSynchronizer = new AsyncTrackStateStringSynchroizer(stateParameterString);
  addTrackListener(stateParameterStringSynchronizer);
  stateParameterString->isControllableFeedbackOnly = true;
  stateParameterString->isSavable = false;



  // post init
  volume->setValue(defaultVolumeValue);
  logVolume = float01ToGain(volume->value);
}

void LooperTrack::processBlock(AudioBuffer<float>& buffer, MidiBuffer &) {


  if(updatePendingLooperTrackState(TimeManager::getInstance()->getTimeInSample(), buffer.getNumSamples())){
    padBufferIfNeeded();
#ifdef BLOCKSIZEGRANULARITY
    if(trackState==PLAYING)jassert((TimeManager::getInstance()->beatTimeInSample%parentLooper->getBlockSize())==0);
#endif
  }


  // RECORDING
  if (loopSample.isRecording())
  {
    if(!loopSample.writeAudioBlock(buffer)){
      LOG("Stopping, too many audio (more than 1mn)");
      setTrackState(STOPPED);
    }

  }



  // PLAYING
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
    for (int i = parentLooper->getTotalNumOutputChannels() - 1; i >= 0; --i) {
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
bool LooperTrack::updatePendingLooperTrackState(const uint64 curTime, int /*_blockSize*/) {


  jassert (curTime>=0);



  bool stateChanged = (trackState != desiredState);


  if(stateChanged){
    //        LOG(trackStateToString(trackState) << ":" << trackStateToString(desiredState));
    if(desiredState == WILL_STOP ||desiredState == STOPPED ){
      desiredState = STOPPED;
      loopSample.setState( PlayableBuffer::BUFFER_STOPPED);
      cleanAllQuantizeNeedles();
      stateChanged = true;
    }

    else if(desiredState==CLEARED  ){
      desiredState = CLEARED;
      loopSample.setState( PlayableBuffer::BUFFER_STOPPED);
      cleanAllQuantizeNeedles();
      stateChanged = true;
    }
  }
  // stop oneShot if needed
  if(parentLooper->isOneShot->boolValue()){
    if((trackState == PLAYING || trackState==WILL_PLAY) && loopSample.numTimePlayed>=1){
      desiredState = WILL_STOP;
      quantizedPlayEnd = 0;
      stateChanged=true;
    }
  }


  ////
  // apply quantization on play / rec
  const uint64 triggeringTime = curTime;
  if (quantizedRecordStart.get()>=0) {
    if (triggeringTime >= quantizedRecordStart.get()) {
      if(isMasterTempoTrack() ){
        if(!TimeManager::getInstance()->playState->boolValue())TimeManager::getInstance()->playState->setValue(true);
      }
      desiredState = RECORDING;
      loopSample.setState( PlayableBuffer::BUFFER_RECORDING);
      startRecBeat = TimeManager::getInstance()->getBeat();
      quantizedRecordStart = -1;
      stateChanged = true;
    }
    else{
      //int waiting = 0;
    }

  }
  else if (quantizedRecordEnd.get()>=0) {
    if (triggeringTime >= quantizedRecordEnd.get()) {

      if(parentLooper->isOneShot->boolValue()){
        desiredState = STOPPED;
      }
      else{
				if(isMasterTempoTrack()){
					TimeManager::getInstance()->playState->setValue(true,false,true);
				}
        desiredState = PLAYING;
        loopSample.setState( PlayableBuffer::BUFFER_PLAYING);
        startPlayBeat = TimeManager::getInstance()->getBeat();
        quantizedPlayStart = 0;
      }
      quantizedRecordEnd = -1;
      stateChanged = true;

    }
  }



  if (quantizedPlayStart.get()>=0) {
    if (triggeringTime >= quantizedPlayStart.get()) {

      desiredState =  PLAYING;
      loopSample.setState( PlayableBuffer::BUFFER_PLAYING);
      startPlayBeat = TimeManager::getInstance()->getBeat();
      quantizedPlayStart = -1;
      stateChanged = true;
    }
  }
  else if (quantizedPlayEnd.get()>=0) {
    if (triggeringTime >= quantizedPlayEnd.get()) {
      desiredState = STOPPED;
      loopSample.setState( PlayableBuffer::BUFFER_STOPPED);
      quantizedPlayEnd = -1;
      stateChanged = true;
    }
  }




  stateChanged|=loopSample.stateChanged;

  trackState = desiredState ;




  //    DBG(playNeedle);
  if(stateChanged){
    if(getQuantization()>0) {
      TimeManager * tm = TimeManager::getInstance();
      loopSample.checkTimeAlignment(curTime,tm->beatTimeInSample * tm->beatPerBar->intValue()/getQuantization());
    }
    trackStateListeners.call(&LooperTrack::Listener::internalTrackStateChanged, trackState);
    DBG("a:"+trackStateToString(trackState));

  }


  return stateChanged;





}

int LooperTrack::getQuantization(){
  return parentLooper->getQuantization();
}

void LooperTrack::padBufferIfNeeded(){

  if (loopSample.stateChanged) {
    //    process changed internalState

    if (loopSample.firstRecordedFrame()) {
      loopSample.startRecord();
      if (isMasterTempoTrack()) {
        int samplesToGet = (int)(parentLooper->preDelayMs->intValue()*0.001f*parentLooper->getSampleRate());
        TimeManager::getInstance()->goToTime(samplesToGet);
        if(samplesToGet>0){ loopSample.writeAudioBlock(parentLooper->streamAudioBuffer.getLastBlock(samplesToGet));}
        startRecBeat = 0;
      }
      else{
        startRecBeat = TimeManager::getInstance()->getBeat();

      }

    }

    if (loopSample.isFirstPlayingFrameAfterRecord() ){
      //            DBG("a:firstPlay");
      int offsetForPlay = 0;
      if (isMasterTempoTrack()) {
        //                DBG("release predelay : "+String (trackIdx));
        const int sampleToRemove = (int)(parentLooper->preDelayMs->intValue()*0.001f*parentLooper->getSampleRate());
        if(sampleToRemove>0){loopSample.cropEndOfRecording(sampleToRemove);}
        TimeManager::getInstance()->setBPMForLoopLength(loopSample.getRecordedLength());
        beatLength->setValue(loopSample.getRecordedLength()*1.0/TimeManager::getInstance()->beatTimeInSample);
        TimeManager::getInstance()->goToTime(offsetForPlay);


      }
      else{
        beatLength->setValue(loopSample.getRecordedLength()*1.0/TimeManager::getInstance()->beatTimeInSample);
      }
      loopSample.fadeInOut (parentLooper->getSampleRate() * 0.01,0);
      originBPM = TimeManager::getInstance()->BPM->doubleValue();
      loopSample.setPlayNeedle(offsetForPlay);
    }


  }
  if( loopSample.isFirstPlayingFrame()){startPlayBeat = TimeManager::getInstance()->getBeat();}


}


String LooperTrack::trackStateToString(const TrackState & ts) {

  switch (ts) {

    case CLEARED:
      return "cleared";
    case PLAYING:
      return "playing";
    case RECORDING:
      return "recording";
    case WILL_PLAY:
      return "willPlay";
    case WILL_RECORD:
      return "willRecord";
    case STOPPED:
      return "stopped";
    case WILL_STOP:
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

    if (desiredState == CLEARED) {
      setTrackState(WILL_RECORD);
    }
    else  if(desiredState!=WILL_RECORD){
      if(parentLooper->isOneShot->boolValue() && desiredState==RECORDING){
        setTrackState(WILL_STOP);
      }
      else{setTrackState(WILL_PLAY);}
    }

  }
  else if (t == playTrig) {
    setTrackState(WILL_PLAY);
  }
  else if (t == clearTrig) {
    setTrackState(CLEARED);
    volume->setValue(DB0_FOR_01);
    mute->setValue(false);
  }
  else if (t == stopTrig) {
    setTrackState(WILL_STOP);
  }
}

bool LooperTrack::askForBeingMasterTempoTrack() {
  if(getQuantization()>0){return TimeManager::getInstance()->askForBeingMasterCandidate(parentLooper)
    && parentLooper->askForBeingMasterTrack(this);}
  else return false;
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


void LooperTrack::setTrackState(TrackState newState) {
  int quantizeTime = getQuantization();
  TimeManager * timeManager = TimeManager::getInstance();
  if(newState==desiredState)return;

  if (newState == WILL_RECORD) {
    // are we able to set the tempo
    if (askForBeingMasterTempoTrack()) {
      timeManager->isSettingTempo->setValue(true);
      // start As soon as possible
      quantizedRecordStart = 0;
    }

    else if (!timeManager->isSettingTempo->boolValue()) {
			if(parentLooper->askForBeingAbleToPlayNow(this) && !timeManager->playState->boolValue()) {
				timeManager->playTrigger->trigger();
				quantizedRecordStart = 0;
				
			}
			else{
				quantizedRecordStart = timeManager->getNextQuantifiedTime(quantizeTime);

			}
    }
    //            Record per default if triggering other rec while we are current master and we are recording

    else if (timeManager->isMasterCandidate(parentLooper)) {
      timeManager->isSettingTempo->setValue(false);
      parentLooper->lastMasterTempoTrack->setTrackState(WILL_PLAY);
      quantizedRecordStart = 0;
    }
  }


  // on should play
  else if (newState == WILL_PLAY) {


    // end of first track
    if ( trackState == RECORDING ){
      if(askForBeingMasterTempoTrack() ) {
        quantizedRecordEnd = -1;
        timeManager->isSettingTempo->setValue(false);
        //            timeManager->lockTime(true);

        int minRecordTime = parentLooper->getSampleRate()*0.5f;
        if(loopSample.getRecordedLength()< minRecordTime){
          // avoid feedBack when trigger play;
          newState = WILL_RECORD;
          desiredState = WILL_RECORD;
          quantizedRecordEnd =minRecordTime;
        }
        else{
            newState=WILL_PLAY;
            quantizedPlayStart = 0;
        }
      }
      else{
          if(!askForBeingMasterTempoTrack() )  {
						
          quantizedRecordEnd = timeManager->getNextQuantifiedTime(quantizeTime);
        }
      }

    }
    // if every one else is stopped
    else if(parentLooper->askForBeingAbleToPlayNow(this) && !loopSample.isOrWasPlaying()) {
      quantizedRecordEnd = -1;

      if(timeManager->isMasterCandidate(parentLooper)){
        newState=WILL_PLAY;
        timeManager->lockTime(true);
        timeManager->playTrigger->trigger();
        quantizedPlayStart = 0;


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



    // on ask for play
    else if(timeManager->playState->boolValue()){
      cleanAllQuantizeNeedles();
      quantizedPlayStart = timeManager->getNextQuantifiedTime(quantizeTime);
      //            quantizedPlayStart = timeManager->getNextQuantifiedTime(1.0/beatLength->intValue());
      //            quantizedPlayStart = timeManager->getTimeForNextBeats(beatLength->value);

    }
  }


  // on should clear
  if (newState == CLEARED) {


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


  if (newState == WILL_STOP) {
    // force a track to stay in cleared state if stop triggered
    if (trackState == CLEARED || desiredState==CLEARED) { newState = CLEARED; }
  }
  //DBG(newState <<","<<trackState );
  //    DBG(trackStateToString(trackState));

  if(desiredState!=newState){
    desiredState = newState;
    trackStateListeners.call(&LooperTrack::Listener::internalTrackStateChanged, desiredState);
  }
  
  if((desiredState==CLEARED  || desiredState==STOPPED || desiredState==WILL_STOP) ){
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
