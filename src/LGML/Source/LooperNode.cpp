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
trackGroup(this),
streamAudioBuffer(2,16384)// 16000 ~ 300ms and 256*64
{

  numberOfTracks =		addIntParameter("numberOfTracks",		"number of tracks in this looper", 8, 1, MAX_NUM_TRACKS);
  numberOfAudioChannelsIn = addIntParameter("numberOfChannelsPerTrack", "number of channels on each audioTrack", 2,1,2);
  exportAudio =			addTrigger("exportAudio",				"export audio of all recorded Tracks");
  selectAllTrig =		addTrigger("Select All",				"Select All tracks, for all clear or main volume for instance");
  selectTrack =			addIntParameter("Select track",			"set track selected", 0, -1, 0);
  recPlaySelectedTrig = addTrigger("Rec Or Play",				"Tells the selected track to wait for the next bar and then start record or play");
  playSelectedTrig =	addTrigger("Play",						"Tells the selected track to wait for the next bar and then stop recording and start playing");
  stopSelectedTrig =	addTrigger("Stop",						"Tells the selected track to stop ");
  clearSelectedTrig =	addTrigger("Clear",						"Tells the selected track to clear it's content if got any");
  volumeSelected =		addFloatParameter("Volume",				"Set the volume of the selected track",1, 0, 1);
  clearAllTrig =		addTrigger("ClearAll",					"Tells all tracks to clear it's content if got any");
  stopAllTrig =			addTrigger("StopAll",					"Tells all tracks to stop it's content if got any");
  isMonitoring =		addBoolParameter("monitor",				"do we monitor audio input ? ", false);
  preDelayMs =			addIntParameter("Pre Delay MS",			"Pre process delay (in milliseconds)", 0, 0, 250);
  quantization =		addIntParameter("quantization",			"quantization for this looper - 1 is global", -1, -1, 32);
  isOneShot =			addBoolParameter("isOneShot",			"do we play once or loop track", false);
  firstTrackSetTempo =	addBoolParameter("firstTrackSetTempo",	"do the first track sets the global tempo or use quantization", true);
  waitForOnset =		addBoolParameter("wait for onset",		"wait for onset before actually recording", false);
  onsetThreshold =		addFloatParameter("onsetThreshold",		"threshold before onset", 0.01f,0.0001f,0.1f);
  outputAllTracksSeparately = addBoolParameter("tracksOutputSeparated", "split all tracks in separate audio channel out", false);
  addChildControllableContainer(&trackGroup);

  trackGroup.setNumTracks(numberOfTracks->intValue());

  selectTrack->setValue(0,false,true);
  setPlayConfigDetails(2, 2, 44100, 256);
  TimeManager::getInstance()->playState->addParameterListener(this);
  setPreferedNumAudioInput(2);
  setPreferedNumAudioOutput(2);
}

LooperNode::~LooperNode()
{
  if (TimeManager::getInstanceWithoutCreating()) {
    TimeManager::getInstance()->playState->removeParameterListener(this);
  }
}





void LooperNode::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer &midiMessages) {


  streamAudioBuffer.writeBlock(buffer);

  // TODO check if we can optimize copies
  // handle multiples channels outs


  jassert(buffer.getNumChannels()>= jmax(totalNumInputChannels,totalNumOutputChannels));
  bufferIn.setSize(totalNumInputChannels, buffer.getNumSamples());
  bufferOut.setSize(totalNumOutputChannels, buffer.getNumSamples());

  if (isMonitoring->boolValue()) {
    if (!wasMonitoring) {
      for (int i = bufferOut.getNumChannels() - 1; i >= 0; --i) {
        bufferOut.copyFromWithRamp(i, 0, buffer.getReadPointer(i), buffer.getNumSamples(), 0.0f, 1.0f);
      }
      wasMonitoring = true;
    }
    else {
      for (int i = bufferOut.getNumChannels() - 1; i >= 0; --i) {
        bufferOut.copyFrom(i, 0, buffer, i, 0, buffer.getNumSamples());
      }
    }
  }
  else {
    if (wasMonitoring) {
      for (int i = bufferOut.getNumChannels() - 1; i >= 0; --i) {
        bufferOut.copyFromWithRamp(i, 0, buffer.getReadPointer(i), buffer.getNumSamples(), 1.0f, 0.0f);
      }
      wasMonitoring = false;
    }
    else {
      // todo use reference to buffer if not monitoring (will save last bufferout copy
      bufferOut.clear();
    }
  }

  int64 curTime = TimeManager::getInstance()->getTimeInSample();
  int numSample = buffer.getNumSamples();
  bool needAudioIn = false;
  for (auto & t : trackGroup.tracks) {
    t->updatePendingLooperTrackState(curTime, numSample);
    // avoid each track clearing the buffer if not needed
    needAudioIn |= t->loopSample.isOrWasRecording();
  }
  //
  if(!needAudioIn){
    buffer.clear();
    bufferIn.clear();
  }
  else{
    for (int i = bufferIn.getNumChannels() - 1; i >= 0; --i) {
      bufferIn.copyFrom(i, 0, buffer, i, 0, buffer.getNumSamples());
    }
  }
  //


  if(outputAllTracksSeparately->boolValue()){
    for (int i = buffer.getNumChannels() - 1; i >= totalNumInputChannels; --i) {
      buffer.copyFrom(i, 0, buffer, i%totalNumInputChannels, 0, numSample);
    }
    int i = 0;
    AudioBuffer<float> tmp;
    for (auto & track:trackGroup.tracks) {
      tmp.setDataToReferTo(buffer.getArrayOfWritePointers()+i, totalNumInputChannels, numSample);
      track->processBlock(tmp, midiMessages);
      i+=totalNumInputChannels;

    }

  }
  else{
    for (auto & t : trackGroup.tracks) {
      t->processBlock(buffer, midiMessages);
      for (int i = totalNumInputChannels - 1; i >= 0; --i) {
        bufferOut.addFrom(i, 0, buffer, i, 0, buffer.getNumSamples());
        if(needAudioIn)
          buffer.copyFrom(i, 0, bufferIn, i, 0, buffer.getNumSamples());
      }
      if(!needAudioIn){buffer.clear();}
    }

    for (int i = bufferOut.getNumChannels() - 1; i >= 0; --i) {
      buffer.copyFrom(i, 0, bufferOut, i, 0, buffer.getNumSamples());
    }
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
  const ScopedLock lk(owner->getCallbackLock());
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
  if(TimeManager::getInstance()->hasMasterCandidate()){
    bool needToReleaseMasterTempo = true;
    for (auto & t : trackGroup.tracks) {
      needToReleaseMasterTempo &= (t->desiredState == LooperTrack::TrackState::CLEARED);

    }

    if (needToReleaseMasterTempo) {
      TimeManager::getInstance()->releaseMasterCandidate(this);
    }
    //    if (!isOneShot->boolValue() && needToStop){
    //      TimeManager::getInstance()->stopTrigger->trigger();
    //    }
  }
}


bool LooperNode::askForBeingMasterTrack(LooperTrack * t) {
  bool res = firstTrackSetTempo->boolValue() && areAllTrackClearedButThis(t);
  if (res)lastMasterTempoTrack = t;
  return res;
}

bool LooperNode::askForBeingAbleToPlayNow(LooperTrack * _t) {
  if(isOneShot->boolValue())return true;
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
int LooperNode::getQuantization(){
  return quantization->intValue()>=0?quantization->intValue():TimeManager::getInstance()->quantizedBarFraction->intValue();
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
    selectTrack->setValue(0);
    outputVolume->setValue(DB0_FOR_01);
  }
  if (t == stopAllTrig) {
    for (int i = trackGroup.tracks.size() - 1; i >= 0; --i) {
      trackGroup.tracks[i]->stopTrig->trigger();
    }
  }
  if (t == selectAllTrig)
  {
    selectTrack->setValue(-1);

  }
#if !LGML_UNIT_TESTS
  if(t==exportAudio){
    FileChooser myChooser("Please select the directory for exporting audio ...");

    if (myChooser.browseForDirectory())
    {
      File folder(myChooser.getResult());
      WavAudioFormat format;



      for(auto & tr:trackGroup.tracks){
        if(tr->loopSample.getRecordedLength()){
          File f(myChooser.getResult().getChildFile(nameParam->stringValue()+"_"+String(tr->trackIdx)+".wav"));
          ScopedPointer<FileOutputStream> fp;
          if((fp = f.createOutputStream())){
            ScopedPointer<AudioFormatWriter> afw= format.createWriterFor(fp,
                                                                         getSampleRate(),
                                                                         tr->loopSample.loopSample.getNumChannels(),
                                                                         24,
                                                                         StringPairArray(),0);
            if(afw){
              fp.release();
              afw->writeFromAudioSampleBuffer(tr->loopSample.loopSample,0,(int)tr->loopSample.getRecordedLength());
              afw->flush();

            }
          }
        }
      }

    }
  }
#endif

}

void LooperNode::selectMe(LooperTrack * t) {
  if (t != nullptr) {
    for(auto &tt:trackGroup.tracks){
      if(tt->isSelected)
        tt->setSelected(false);
    };
  }

  selectedTrack = t;

  if (selectedTrack != nullptr) {
    selectedTrack->setSelected(true);
    volumeSelected->setValue(selectedTrack->volume->floatValue());
    selectTrack->setValue(selectedTrack->trackIdx);
  }
}
void LooperNode::numChannelsChanged(bool isInput){
  if(isInput){
  for (auto & t:trackGroup.tracks){
    t->setNumChannels(totalNumInputChannels);
  }
    streamAudioBuffer.setNumChannels(totalNumInputChannels);
  }
}
void LooperNode::onContainerParameterChanged(Parameter * p) {
  NodeBase::onContainerParameterChanged(p);
  if (p == numberOfTracks) {
    int oldIdx = selectedTrack->trackIdx;
    trackGroup.setNumTracks(numberOfTracks->value);
    if(outputAllTracksSeparately->boolValue()){
      setPreferedNumAudioOutput(totalNumInputChannels*numberOfTracks->intValue());
    }
    if(oldIdx>numberOfTracks->intValue()){
      if(trackGroup.tracks.size()){
        selectedTrack = trackGroup.tracks[trackGroup.tracks.size()-1];
      }
      else
        selectedTrack = nullptr;
    }
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
  else if(p==outputAllTracksSeparately){
    if(outputAllTracksSeparately->boolValue()){
      setPreferedNumAudioOutput(totalNumInputChannels*numberOfTracks->intValue());
    }
    else{
      setPreferedNumAudioOutput(totalNumInputChannels);
    }
  }
  else if(p == numberOfAudioChannelsIn){
    setPreferedNumAudioInput(numberOfAudioChannelsIn->intValue());
    setPreferedNumAudioOutput(numberOfAudioChannelsIn->intValue()*(outputAllTracksSeparately->boolValue()?trackGroup.tracks.size():1));
  }
  else if (p == TimeManager::getInstance()->playState) {
    if (!TimeManager::getInstance()->playState->boolValue()) {
      for (auto &t : trackGroup.tracks) {
        t->stopTrig->trigger();
      }
    }
    else if(!isOneShot->boolValue()){
      // prevent time manager to update track internal state before all tracks are updated
      TimeManager::getInstance()->lockTime(true);
      for (auto &t : trackGroup.tracks) {
        if(t->trackState!=LooperTrack::CLEARED && t->trackState!=LooperTrack::WILL_RECORD) t->setTrackState(LooperTrack::TrackState::WILL_PLAY);
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
        for(auto & t:trackGroup.tracks){
          t->setSelected(true);
        }

      }
    }
  }
}

void LooperNode::clearInternal(){
  // get called after deletion of TimeManager on app exit
  TimeManager * tm = TimeManager::getInstanceWithoutCreating();
  if (tm != nullptr)
  {
    tm->releaseMasterCandidate(this);
  }
  
}

// worst onset detection function ever ...
bool LooperNode::hasOnset(){
  bool hasOnset=  globalRMSValueIn>onsetThreshold->floatValue();
  return hasOnset;
}
