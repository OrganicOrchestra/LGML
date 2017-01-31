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
#include "AudioDebugPipe.h"
#include "AudioDebugCrack.h"

#define NO_QUANTIZE (uint64)-1 //std::numeric_limits<uint64>::max()

LooperTrack::LooperTrack(LooperNode * looperNode, int _trackIdx) :
ControllableContainer(String(_trackIdx)),
parentLooper(looperNode),
quantizedRecordStart(NO_QUANTIZE),
quantizedRecordEnd(NO_QUANTIZE),
quantizedPlayStart(NO_QUANTIZE),
quantizedPlayEnd(NO_QUANTIZE),
loopSample(2, 44100 * MAX_LOOP_LENGTH_S,looperNode->getSampleRate(),looperNode->getBlockSize()),
trackState(CLEARED),
desiredState(CLEARED),
trackIdx(_trackIdx),
someOneIsSolo(false),
isSelected (false),
originBPM(0),
lastVolume(0),
startPlayBeat(0),
startRecBeat(0)
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
  togglePlayStopTrig = addTrigger("Toggle Play Stop", "Toggle Play / Stop");

  mute->invertVisuals = true;

  stateParameterString = addStringParameter("state", "track state", "cleared");
  stateParameterStringSynchronizer = new AsyncTrackStateStringSynchronizer(stateParameterString);
  addTrackListener(stateParameterStringSynchronizer);
  stateParameterString->isControllableFeedbackOnly = true;
  stateParameterString->isSavable = false;



  // post init
  volume->setValue(defaultVolumeValue);
  logVolume = float01ToGain(volume->value);


  

}

LooperTrack::~LooperTrack() {

}

void LooperTrack::processBlock(AudioBuffer<float>& buffer, MidiBuffer &) {



  handleStartOfRecording();


  TimeManager * tm = TimeManager::getInstance();
  if(tm->getTimeInSample()== 0 && tm->getTimeInSample()<startPlayBeat*tm->beatTimeInSample){
    startPlayBeat = 0;
  }
  jassert(tm->getTimeInSample()>=startPlayBeat*tm->beatTimeInSample);
  if(!loopSample.processNextBlock(buffer,tm->getTimeInSample()-startPlayBeat*tm->beatTimeInSample) && trackState!=STOPPED){
    LOG("Stopping, too many audio (more than 1mn)");
    setTrackState(STOPPED);
  }
  if(loopSample.isPlaying()){
    trackStateListeners.call(&LooperTrack::Listener::internalTrackTimeChanged,loopSample.getPlayPos()*1.0/(1+loopSample.getRecordedLength()));
  }

  if( DEBUGPIPE_ENABLED){

  const float div = jmax((uint64)1,loopSample.getRecordedLength());

  DBGAUDIO("trackPos"+String(trackIdx),  loopSample.getPlayPos()/div);
  }
//  handleEndOfRecording();


  float newVolume = ((someOneIsSolo && !solo->boolValue()) || mute->boolValue()) ? 0 : logVolume;

  for (int i = buffer.getNumChannels()- 1; i >= 0; --i) {
    buffer.applyGainRamp(i, 0, buffer.getNumSamples(), lastVolume, newVolume);
  }

  lastVolume = newVolume;


  loopSample.endProcessBlock();

  DBGAUDIOCRACK("track"+String(trackIdx),buffer);


}
bool LooperTrack::updatePendingLooperTrackState(  int blockSize) {
TimeManager * tm = TimeManager::getInstance();
  // the sample act as free running clock when no quantization
  uint64 curTime = tm->getTimeInSample();
  if(getQuantization()==0) curTime = loopSample.getGlobalPlayPos();

  jassert (curTime>=0);



  bool stateChanged = (trackState != desiredState);

  if(shouldWaitFirstOnset() && desiredState == WILL_RECORD){
    if(!hasOnset()){
      quantizedRecordStart=curTime+blockSize;
    }
    else{
      quantizedRecordStart=curTime;
      //      int dbg;dbg=0;
    }
  }




  if(stateChanged){
    //        LOG(trackStateToString(trackState) << ":" << trackStateToString(desiredState));
    if(desiredState == WILL_STOP  ){

      loopSample.setState( PlayableBuffer::BUFFER_STOPPED);
      desiredState = STOPPED;
      cleanAllQuantizeNeedles();
      stateChanged = true;
      if(isMasterTempoTrack())releaseMasterTrack();
    }

    else if(desiredState==CLEARED  ){
      loopSample.setState( PlayableBuffer::BUFFER_STOPPED);
      cleanAllQuantizeNeedles();
      stateChanged = loopSample.stateChanged;
      if(isMasterTempoTrack())releaseMasterTrack();
    }
  }



  ////
  // apply quantization on play / rec
  uint64 triggeringTime = curTime+blockSize;



  if (quantizedRecordStart!=NO_QUANTIZE) {
    if (triggeringTime > quantizedRecordStart ) {
      int firstPart = jmax(0, (int)(quantizedRecordStart-curTime));
      int secondPart = (int)(triggeringTime-firstPart);


      if(isMasterTempoTrack() ){
        if(!tm->playState->boolValue()){
          tm->playState->setValue(true,false,false,false);

        }
        // we will handle the block in this call so we notify time to be in sync with what we play
        tm->goToTime(firstPart,true);
        curTime = TimeManager::getInstance()->getTimeInSample();
        triggeringTime = curTime+blockSize;

      }

      desiredState = RECORDING;
      loopSample.setState( PlayableBuffer::BUFFER_RECORDING,firstPart);
      startRecBeat = TimeManager::getInstance()->getBeatInNextSamples(firstPart);
      quantizedRecordStart = NO_QUANTIZE;
      stateChanged = true;
    }
    else{
      //int waiting = 0;
    }

  }
  else if (quantizedRecordEnd!=NO_QUANTIZE) {
    if (triggeringTime > quantizedRecordEnd) {
      int firstPart = jmax(0, (int)(quantizedRecordEnd-curTime));
      //      int secondPart = triggeringTime-firstPart;

      if(parentLooper->isOneShot->boolValue()){
        loopSample.setState( PlayableBuffer::BUFFER_STOPPED,firstPart);
        desiredState = STOPPED;
      }
      else{


        loopSample.setState( PlayableBuffer::BUFFER_PLAYING,firstPart);
        handleEndOfRecording();


        curTime = TimeManager::getInstance()->getTimeInSample();
        triggeringTime = curTime+blockSize;
        
        desiredState = PLAYING;
        quantizedPlayStart = curTime+firstPart;
      }
      quantizedRecordEnd = NO_QUANTIZE;
      stateChanged = true;

    }
  }



  if (quantizedPlayStart!=NO_QUANTIZE) {
    if (triggeringTime > quantizedPlayStart) {
      int firstPart = jmax(0, (int)(quantizedPlayStart-curTime));
      //      int secondPart = triggeringTime-firstPart;

      desiredState =  PLAYING;
      loopSample.setState( PlayableBuffer::BUFFER_PLAYING,firstPart);
      startPlayBeat = TimeManager::getInstance()->getBeatInNextSamples(firstPart);

      // stop oneShot if needed
      if(parentLooper->isOneShot->boolValue() ){
        quantizedPlayEnd = quantizedPlayStart + loopSample.getRecordedLength() - loopSample.getNumSampleFadeOut();
      }

      quantizedPlayStart = NO_QUANTIZE;
      stateChanged = true;


    }
  }
  else if (quantizedPlayEnd!=NO_QUANTIZE) {
    if (triggeringTime > quantizedPlayEnd) {
      int firstPart = jmax(0, (int)(quantizedPlayEnd-curTime));
      //      int secondPart = triggeringTime-firstPart;
      desiredState = STOPPED;
      loopSample.setState( PlayableBuffer::BUFFER_STOPPED,firstPart);
      quantizedPlayEnd = NO_QUANTIZE;
      stateChanged = true;
    }
  }




  stateChanged|=loopSample.stateChanged;

  trackState = desiredState ;




  //    DBG(playNeedle);
  if(stateChanged){

    trackStateListeners.call(&LooperTrack::Listener::internalTrackStateChanged, trackState);
    // DBG("a:"+trackStateToString(trackState));

  }
  //  if(getQuantization()>0 && !isMasterTempoTrack() && trackState == PLAYING) {
  //    TimeManager * tm = TimeManager::getInstance();
  //    loopSample.checkTimeAlignment(curTime,tm->beatTimeInSample/getQuantization());
  //  }

  return stateChanged;





}

int LooperTrack::getQuantization(){
  return parentLooper->getQuantization();
}

void LooperTrack::handleStartOfRecording(){
  TimeManager * tm = TimeManager::getInstance();
  if (loopSample.stateChanged) {
    //    process changed internalState

    if (loopSample.isFirstRecordedFrame()) {
      if (isMasterTempoTrack()) {
        int samplesToGet = (int)(parentLooper->preDelayMs->intValue()*0.001f*parentLooper->getSampleRate());
        //        we need to advance because pat of the block may have be processed
        tm->goToTime(samplesToGet);
        tm->play(true);
        if(samplesToGet>0){ loopSample.writeAudioBlock(parentLooper->streamAudioBuffer.getLastBlock(samplesToGet));}
        startRecBeat = 0;
      }
      else{
        startRecBeat = TimeManager::getInstance()->getBeatInNextSamples(loopSample.getSampleOffsetBeforeNewState());

      }

    }
  }
}

void LooperTrack::handleEndOfRecording(){


    jassert (loopSample.wasLastRecordingFrame() );
      //            DBG("a:firstPlay");
      // get howMuch we have allready played in loopSample
      int offsetForPlay = (int)loopSample.getPlayPos();

      TimeManager * tm = TimeManager::getInstance();
      if (isMasterTempoTrack()) {

        //                DBG("release predelay : "+String (trackIdx));
        const int sampleToRemove = (int)(parentLooper->preDelayMs->intValue()*0.001f*parentLooper->getSampleRate());
        if(sampleToRemove>0){loopSample.cropEndOfRecording(sampleToRemove);}
        SampleTimeInfo info = tm->findSampleTimeInfoForLength(loopSample.getRecordedLength());
        // need to tell it right away to avoid bpm changes call back while originBPM not updated
        if(getQuantization()>0)originBPM = info.bpm ;

        tm->setBPMFromTimeInfo(info,false);
        
        uint64 desiredSize = (uint64)(info.barLength*tm->beatPerBar->intValue()*info.beatInSample);

        //        DBG("resizing loop : " << (int)(desiredSize-loopSample.getRecordedLength()));

        loopSample.setSizePaddingIfNeeded(desiredSize);
        beatLength->setValue(loopSample.getRecordedLength()*1.0/info.beatInSample,false,false,true);
        tm->goToTime(offsetForPlay,true);//desiredSize+offsetForPlay,true);

        jassert(tm->playState->boolValue());
        releaseMasterTrack();


      }
      else{
        beatLength->setValue(loopSample.getRecordedLength()*1.0/tm->beatTimeInSample);
        if(getQuantization()>0)originBPM = tm->BPM->doubleValue();
      }

      

      

//      DBGAUDIO("track"+String(trackIdx), loopSample.originLoopSample);
//      DBGAUDIOSETBPM("track"+String(trackIdx), originBPM);
//    }
//  }
//
//
//  if( loopSample.isFirstPlayingFrame()){
////    startPlayBeat = TimeManager::getInstance()->getBeatInNextSamples(loopSample.getSampleOffsetBeforeNewState());
//  }


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
      recPlay();
  }
  else if (t == playTrig) {
      play();
  }
  else if (t == clearTrig) {
      clear();
  }
  else if (t == stopTrig) {
      stop();
  } else if (t == togglePlayStopTrig)
  {
	  setTrackState(trackState != PLAYING ? WILL_PLAY : WILL_STOP);
  }
}
void LooperTrack::clear(){
    setTrackState(CLEARED);
    volume->setValue(DB0_FOR_01);
    mute->setValue(false);
  TimeManager::getInstance()->notifyListenerCleared();
}

void LooperTrack::stop(){
    setTrackState(WILL_STOP);
}

void LooperTrack::play(){
        setTrackState(WILL_PLAY);
}

void LooperTrack::recPlay(){
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
bool LooperTrack::askForBeingMasterTempoTrack() {
  if(getQuantization()>0){

    if(parentLooper->askForBeingMasterTrack(this) && TimeManager::getInstance()->askForBeingMasterCandidate(parentLooper))
    {
        return true;
    }

  }

  return false;
}

bool LooperTrack::isMasterTempoTrack(){
  return TimeManager::getInstance()->isMasterCandidate(parentLooper)
  && parentLooper->lastMasterTempoTrack  == this;
}




void LooperTrack::setSelected(bool _isSelected) {
  isSelected=_isSelected;
  trackStateListeners.call(&LooperTrack::Listener::internalTrackSetSelected, isSelected);
}

bool LooperTrack::isEmpty()
{
	return trackState == TrackState::CLEARED || desiredState==TrackState::CLEARED;
}


void LooperTrack::askForSelection(bool) {
  selectTrig->trigger();
}


void LooperTrack::setTrackState(TrackState newState) {

  int quantizeTime = getQuantization();
  TimeManager * timeManager = TimeManager::getInstance();

  if(newState==desiredState)return;

  if (newState == WILL_RECORD && (!shouldWaitFirstOnset() || hasOnset())) {
    // are we able to set the tempo

    if (askForBeingMasterTempoTrack()) {
      // start As soon as possible
      quantizedRecordStart = 0;
    }

    else if (!timeManager->isSettingTempo->boolValue()) {
      if(parentLooper->askForBeingAbleToRecNow(this) ){//&& !timeManager->playState->boolValue()) {
        if(getQuantization()>0)
          timeManager->playTrigger->trigger();
        quantizedRecordStart = 0;

      }
      else{
        quantizedRecordStart = timeManager->getNextQuantifiedTime(quantizeTime);

      }
    }
    //            Record per default if triggering other rec while we are current master and we are recording

    else if (timeManager->isMasterCandidate(parentLooper)) {
      releaseMasterTrack();
      jassert(parentLooper->lastMasterTempoTrack);
      parentLooper->lastMasterTempoTrack->setTrackState(WILL_PLAY);
      quantizedRecordStart = 0;
    }
    // ignore in other cases (ask recording while another is setting tempo)
    else{
      newState = desiredState;
    }
  }


  // on should play
  else if (newState == WILL_PLAY) {


    // end of first track
    if ( trackState == RECORDING ){
      if(isMasterTempoTrack() ) {
        quantizedRecordEnd = 0;
        
        //            timeManager->lockTime(true);

        //        int minRecordTime = (int)(parentLooper->getSampleRate()*0.1f);
        //        minRecordTime-= minRecordTime%parentLooper->getBlockSize();
        //        if(loopSample.getRecordedLength()< minRecordTime){
        //          // avoid feedBack when trigger play;
        //          newState = WILL_RECORD;
        //          desiredState = WILL_RECORD;
        //          quantizedRecordEnd =minRecordTime;
        //        }
        //        else{
        newState=WILL_PLAY;
        quantizedPlayStart = 0;
        //        }
      }
      else{
        if(getQuantization()>0)
          quantizedRecordEnd = timeManager->getNextQuantifiedTime(quantizeTime);
        else
          quantizedRecordEnd = 0;

      }

    }
    // if every one else is stopped
    else if(parentLooper->askForBeingAbleToPlayNow(this) && !loopSample.isOrWasPlaying()) {
      quantizedRecordEnd = NO_QUANTIZE;

      if(timeManager->isMasterCandidate(parentLooper)){
        newState=WILL_PLAY;
        bool wasLocked = timeManager->isLocked();
        if(!wasLocked)timeManager->lockTime(true);
        timeManager->playTrigger->trigger();
        quantizedPlayStart = 0;
        if(!wasLocked)timeManager->lockTime(false);
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
    else{// if(timeManager->playState->boolValue()){
      cleanAllQuantizeNeedles();
      if(getQuantization()==0) quantizedPlayStart=0;
      else quantizedPlayStart = timeManager->getNextQuantifiedTime(quantizeTime);
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
};

void LooperTrack::cleanAllQuantizeNeedles() {
  quantizedPlayEnd = NO_QUANTIZE;
  quantizedPlayStart = NO_QUANTIZE;
  quantizedRecordEnd = NO_QUANTIZE;
  quantizedRecordStart = NO_QUANTIZE;
}


bool LooperTrack::shouldWaitFirstOnset(){
  return (trackState == WILL_RECORD || desiredState==WILL_RECORD) && parentLooper->waitForOnset->boolValue();
}

bool LooperTrack::hasOnset(){
  return parentLooper->hasOnset();
}



void LooperTrack::setNumChannels(int numChannels){
  loopSample.setNumChannels(numChannels);
  loopSample.setSampleRate( parentLooper->getSampleRate());
}


void LooperTrack::releaseMasterTrack(){
  TimeManager::getInstance()->releaseMasterCandidate(parentLooper);
  parentLooper->lastMasterTempoTrack = nullptr;
}
