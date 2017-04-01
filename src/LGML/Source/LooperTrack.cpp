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
playableBuffer(2, 44100 * MAX_LOOP_LENGTH_S,looperNode->getSampleRate(),looperNode->getBlockSize()),
trackState(CLEARED),
desiredState(CLEARED),
trackIdx(_trackIdx),
someOneIsSolo(false),
isSelected (false),

lastVolume(0),
startPlayBeat(0),
startRecBeat(0),
logVolume(float01ToGain(DB0_FOR_01),0.5)
{


  selectTrig = addTrigger("Select", "Select this track");
  recPlayTrig = addTrigger("Rec Or Play", "Tells the track to wait for the next bar and then start record or play");
  playTrig = addTrigger("Play", "Tells the track to wait for the next bar and then stop recording and start playing");
  stopTrig = addTrigger("Stop", "Tells the track to stop ");
  clearTrig = addTrigger("Clear", "Tells the track to clear it's content if got any");
  volume = addFloatParameter("Volume", "Set the volume of the track", DB0_FOR_01, 0, 1);
  mute = addBoolParameter("Mute", "Sets the track muted (or not.)", false);
  solo = addBoolParameter("Solo", "Sets the track solo (or not.)", false);
  beatLength = addFloatParameter("Length", "length in bar", 0, 0, 200);
  togglePlayStopTrig = addTrigger("Toggle Play Stop", "Toggle Play / Stop");
  originBPM = addFloatParameter("originBPM","bpm of origin audio loop",0,0,999);

  sampleChoice = addEnumParameter("sample", "loaded sample");
  sampleChoice->addEnumParameterListener(this);
  
  mute->invertVisuals = true;

  stateParameterString = addStringParameter("state", "track state", "cleared");
  stateParameterStringSynchronizer = new AsyncTrackStateStringSynchronizer(stateParameterString);
  addTrackListener(stateParameterStringSynchronizer);
  stateParameterString->isControllableFeedbackOnly = true;
  stateParameterString->isSavable = false;



  // post init
  volume->setValue(DB0_FOR_01);





}

LooperTrack::~LooperTrack() {
  
  removeTrackListener(stateParameterStringSynchronizer);
}

void LooperTrack::processBlock(AudioBuffer<float>& buffer, MidiBuffer &) {



  handleStartOfRecording();


  TimeManager * tm = TimeManager::getInstance();
  uint64 curTime = tm->getTimeInSample();
  int offset = startPlayBeat*tm->beatTimeInSample;
  if(getQuantization()==0) {
    curTime = playableBuffer.getGlobalPlayPos();
    offset = 0;
  }
  else if( !playableBuffer.isOrWasRecording()  ){
    if(curTime<offset){
      float negativeStartPlayBeat = startPlayBeat/beatLength->doubleValue();
      negativeStartPlayBeat = startPlayBeat - beatLength->doubleValue()*ceil(negativeStartPlayBeat);
      offset =  negativeStartPlayBeat*tm->beatTimeInSample;

    }
  }
  else{
    offset = 0;
  }
  jassert((int)curTime -(int)offset>=0 );
  if(!playableBuffer.processNextBlock(buffer,curTime-offset ) && trackState!=STOPPED){
    LOG("Stopping, too many audio (more than 1mn)");
    setTrackState(STOPPED);
  }
  if(playableBuffer.isPlaying()){
    if(playableBuffer.getRecordedLength()>0){
      trackStateListeners.call(&LooperTrack::Listener::internalTrackTimeChanged,playableBuffer.getPlayPos()*1.0/playableBuffer.getRecordedLength());
    }
  }
  if(playableBuffer.wasLastRecordingFrame()){
    handleEndOfRecording();
  }

  //  if( DEBUGPIPE_ENABLED){
  //
  //  const float div = jmax((uint64)1,playableBuffer.getRecordedLength());
  //
  //  DBGAUDIO("trackPos"+String(trackIdx),  playableBuffer.getPlayPos()/div);
  //  }

  logVolume.update();

  float newVolume = ((someOneIsSolo && !solo->boolValue()) || mute->boolValue()) ? 0 : logVolume.get();

  for (int i = buffer.getNumChannels()- 1; i >= 0; --i) {
    buffer.applyGainRamp(i, 0, buffer.getNumSamples(), lastVolume, newVolume);
  }

  lastVolume = newVolume;


  playableBuffer.endProcessBlock();

  DBGAUDIOCRACK("track"+String(trackIdx),buffer);


}
bool LooperTrack::updatePendingLooperTrackState(  int blockSize) {
  TimeManager * tm = TimeManager::getInstance();
  // the sample act as free running clock when no quantization
  uint64 curTime = tm->getTimeInSample();
  if(getQuantization()==0) curTime = playableBuffer.getGlobalPlayPos();

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

      playableBuffer.setState( PlayableBuffer::BUFFER_STOPPED);
      desiredState = STOPPED;
      cleanAllQuantizeNeedles();
      stateChanged = true;
      if(isMasterTempoTrack())releaseMasterTrack();
    }

    else if(desiredState==CLEARED  ){
      playableBuffer.setState( PlayableBuffer::BUFFER_STOPPED);
      cleanAllQuantizeNeedles();
      stateChanged = playableBuffer.stateChanged;
      if(isMasterTempoTrack())releaseMasterTrack();
    }
  }



  ////
  // apply quantization on play / rec
  uint64 triggeringTime = curTime+blockSize;



  if (quantizedRecordStart!=NO_QUANTIZE) {
    if (triggeringTime > quantizedRecordStart ) {
      int firstPart = jmax(0, (int)(quantizedRecordStart-curTime));
      //      int secondPart = (int)(triggeringTime-firstPart);


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
      playableBuffer.setState( PlayableBuffer::BUFFER_RECORDING,firstPart);
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
        playableBuffer.setState( PlayableBuffer::BUFFER_STOPPED,firstPart);
        desiredState = STOPPED;
      }
      else{


        playableBuffer.setState( PlayableBuffer::BUFFER_PLAYING,firstPart);






        desiredState = PLAYING;
        quantizedPlayStart = curTime+firstPart;
      }
      quantizedRecordEnd = NO_QUANTIZE;
      stateChanged = true;

    }
  }



  if (quantizedPlayStart!=NO_QUANTIZE) {
    if (triggeringTime > quantizedPlayStart) {
      // cancel play if nothing recorded
      if(playableBuffer.getRecordedLength()==0){
        cleanAllQuantizeNeedles();
        desiredState = CLEARED;
        stateChanged = true;
      }

      else{
        int firstPart = jmax(0, (int)(quantizedPlayStart-(int)curTime));
        jassert(firstPart>=0);
        //      int secondPart = triggeringTime-firstPart;

        desiredState =  PLAYING;
        playableBuffer.setState( PlayableBuffer::BUFFER_PLAYING,firstPart);
        startPlayBeat = TimeManager::getInstance()->getBeatInNextSamples(firstPart);

        // stop oneShot if needed
        if(parentLooper->isOneShot->boolValue() ){
          quantizedPlayEnd = quantizedPlayStart + playableBuffer.getRecordedLength() - playableBuffer.getNumSampleFadeOut();
        }

        quantizedPlayStart = NO_QUANTIZE;
        stateChanged = true;
      }

    }
  }
  else if (quantizedPlayEnd!=NO_QUANTIZE) {
    if (triggeringTime > quantizedPlayEnd) {
      int firstPart = jmax(0, (int)(quantizedPlayEnd-curTime));
      //      int secondPart = triggeringTime-firstPart;
      desiredState = STOPPED;
      playableBuffer.setState( PlayableBuffer::BUFFER_STOPPED,firstPart);
      quantizedPlayEnd = NO_QUANTIZE;
      stateChanged = true;
    }
  }




  stateChanged|=playableBuffer.stateChanged;

  trackState = desiredState ;




  //    DBG(playNeedle);
  if(stateChanged){

    trackStateListeners.call(&LooperTrack::Listener::internalTrackStateChanged, trackState);
    // DBG("a:"+trackStateToString(trackState));

  }
  //  if(getQuantization()>0 && !isMasterTempoTrack() && trackState == PLAYING) {
  //    TimeManager * tm = TimeManager::getInstance();
  //    playableBuffer.checkTimeAlignment(curTime,tm->beatTimeInSample/getQuantization());
  //  }

  return stateChanged;





}

int LooperTrack::getQuantization(){
  return parentLooper->getQuantization();
}

void LooperTrack::handleStartOfRecording(){
  TimeManager * tm = TimeManager::getInstance();
  if (playableBuffer.stateChanged) {
    //    process changed internalState

    if (playableBuffer.isFirstRecordedFrame()) {
      if (isMasterTempoTrack()) {
        int samplesToGet = (int)(parentLooper->preDelayMs->intValue()*0.001f*parentLooper->getSampleRate());
        //        we need to advance because pat of the block may have be processed

        tm->play(true);
        tm->goToTime(samplesToGet,true);
        if(samplesToGet>0){
          playableBuffer.writeAudioBlock(parentLooper->streamAudioBuffer.getLastBlock(samplesToGet));
        }
        startRecBeat = 0;
      }
      else{
        startRecBeat = TimeManager::getInstance()->getBeatInNextSamples(playableBuffer.getSampleOffsetBeforeNewState());

      }

    }
  }
}

void LooperTrack::handleEndOfRecording(){


  jassert (playableBuffer.wasLastRecordingFrame() );
  //            DBG("a:firstPlay");
  // get howMuch we have allready played in playableBuffer
  int offsetForPlay = (int)playableBuffer.getPlayPos();

  TimeManager * tm = TimeManager::getInstance();
  if (isMasterTempoTrack()) {

    //                DBG("release predelay : "+String (trackIdx));
    int sampleToRemove = (int)(parentLooper->preDelayMs->intValue()*0.001f*parentLooper->getSampleRate());
    if(sampleToRemove>0){playableBuffer.cropEndOfRecording(&sampleToRemove);}
    TransportTimeInfo info = tm->findTransportTimeInfoForLength(playableBuffer.getRecordedLength());
    // need to tell it right away to avoid bpm changes call back while originBPM not updated
    if(getQuantization()>0)originBPM->setValue( info.bpm) ;

    tm->setBPMFromTransportTimeInfo(info,false);

    uint64 desiredSize = (uint64)(info.barLength*tm->beatPerBar->intValue()*info.beatInSample +0.5);

    //        DBG("resizing loop : " << (int)(desiredSize-playableBuffer.getRecordedLength()));

    playableBuffer.setRecordedLength(desiredSize);
    beatLength->setValue(playableBuffer.getRecordedLength()*1.0/info.beatInSample,false,false,true);
    tm->goToTime(offsetForPlay,false);//desiredSize+offsetForPlay,true);
    startPlayBeat = 0;
    jassert(tm->playState->boolValue());
    releaseMasterTrack();


  }
  else{
    beatLength->setValue(playableBuffer.getRecordedLength()*1.0/tm->beatTimeInSample);
    if(getQuantization()>0)originBPM->setValue( tm->BPM->doubleValue());
    else{
      // non quantified
      // we assign one but obviously not related to master (avoid null bpms)
      originBPM ->setValue(tm->findTransportTimeInfoForLength(playableBuffer.getRecordedLength()).bpm);
    }
  }





  //      DBGAUDIO("track"+String(trackIdx), playableBuffer.originplayableBuffer);
  //      DBGAUDIOSETBPM("track"+String(trackIdx), originBPM);
  //    }
  //  }
  //
  //
  //  if( playableBuffer.isFirstPlayingFrame()){
  ////    startPlayBeat = TimeManager::getInstance()->getBeatInNextSamples(playableBuffer.getSampleOffsetBeforeNewState());
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
    if (parentLooper->trackGroup.selectedTrack == this) parentLooper->volumeSelected->setValue(volume->floatValue());
    logVolume.set( float01ToGain(volume->value));

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
  && parentLooper->trackGroup.lastMasterTempoTrack  == this;
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

    else if (!timeManager->isSettingTempo->boolValue()  ) {
      if(parentLooper->askForBeingAbleToRecNow(this)){//&& !timeManager->playState->boolValue()) {
        if(getQuantization()>0&&  !timeManager->isPlaying())
          timeManager->playTrigger->trigger();

        quantizedRecordStart = 0;

      }
      else{
        quantizedRecordStart = timeManager->getNextQuantifiedTime(quantizeTime);
        if(getQuantization()>0 &&  !timeManager->isPlaying()){
          timeManager->playTrigger->trigger();
        }
      }
    }
    //            Record per default if triggering other rec while we are current master and we are recording

    else if (timeManager->isMasterCandidate(parentLooper)) {
      LooperTrack * lastMaster = parentLooper->trackGroup.lastMasterTempoTrack;
      releaseMasterTrack();
      if(lastMaster){
        lastMaster->setTrackState(WILL_PLAY);
        quantizedRecordStart = 0;
      }
      else{
        jassertfalse;

      }
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
        //        if(playableBuffer.getRecordedLength()< minRecordTime){
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
    else if(trackState!=CLEARED && parentLooper->askForBeingAbleToPlayNow(this) && !playableBuffer.isOrWasPlaying()) {
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
      startPlayBeat=0;
      startRecBeat=0;
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
  playableBuffer.setNumChannels(numChannels);
  playableBuffer.setSampleRate( parentLooper->getSampleRate());
}


void LooperTrack::releaseMasterTrack(){
  TimeManager::getInstance()->releaseMasterCandidate(parentLooper);
  parentLooper->trackGroup.lastMasterTempoTrack = nullptr;
}


void LooperTrack::enumOptionAdded(EnumParameter *, const Identifier &) {};
void LooperTrack::enumOptionRemoved(EnumParameter *, const Identifier &) {};
void LooperTrack::enumOptionSelectionChanged(EnumParameter *ep,bool isSelected, bool isValid, const Identifier & k){
  if(ep==sampleChoice && isSelected){
    String path = ep->getValueForId(k);
    if(!path.isEmpty()){
      loadAudioSample(path);
    }
  }
};
void LooperTrack::loadAudioSample(const String & path){
  jassert(MessageManager::getInstance()->isThisTheMessageThread());
  File audioFile(path);
  if(audioFile.exists()){

      AudioFormatManager formatManager ;
      formatManager.registerBasicFormats();
      auto codec = formatManager.createReaderFor (audioFile);

    if(codec){
      int64 maxSample= 44100 * MAX_LOOP_LENGTH_S;
      if(codec->lengthInSamples>maxSample){
        LOG("sample loading : truncating input bigger than 1mn");

      }
      int destSize = jmin(maxSample,codec->lengthInSamples);
      int destNumChannels = codec->numChannels;
      AudioSampleBuffer tempBuf ;
      tempBuf.setSize(destNumChannels,destSize);
      codec->read(&tempBuf,0,destSize,0,true,playableBuffer.audioBuffer.getNumChannels()>1?true:false);
      if(codec->sampleRate != parentLooper->getSampleRate()){
        LOG("sample loading : resampling is still experimental : " \
            <<audioFile.getFileName() << " : " << codec->sampleRate);

        LagrangeInterpolator interpolator;
        AudioSampleBuffer full ;
        full.makeCopyOf(tempBuf);
        double ratio = codec->sampleRate/ parentLooper->getSampleRate();
        int newSize  = ratio*destSize;
        interpolator.process(ratio, full.getReadPointer(0), tempBuf.getWritePointer(0), newSize);
        destSize = newSize;

      }
      playableBuffer.originAudioBuffer.makeCopyOf(tempBuf);

      auto tm = TimeManager::getInstance();
      auto ti = tm->findTransportTimeInfoForLength(playableBuffer.originAudioBuffer.getNumSamples());
      double timeRatio = ti.bpm / tm->BPM->doubleValue();
      playableBuffer.setRecordedLength(destSize);
      originBPM->setValue( ti.bpm);
      beatLength->setValue(playableBuffer.getRecordedLength()*1.0/ti.beatInSample,false,false,true);
      playableBuffer.setNumChannels(destNumChannels);
      playableBuffer.setTimeRatio(timeRatio);
      setTrackState(STOPPED);

      

    }
    else{
      LOG("sample loading : format not supported : " << audioFile.getFileExtension());
    }
  }
  else{
    LOG("sample loading : file not found : " << audioFile.getFullPathName());
  }
}
