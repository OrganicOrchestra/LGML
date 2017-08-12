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


#ifndef LOOPERNODE_H_INCLUDED
#define LOOPERNODE_H_INCLUDED


#include "NodeBase.h"
#include "LooperTrack.h"

#define MAX_NUM_TRACKS 32
#include "TimeMasterCandidate.h"

#include "RingBuffer.h"
#include "TimeManager.h"

class LooperNode :
public NodeBase,
public TimeMasterCandidate,TimeManager::TimeManagerListener

{

public:
  LooperNode();
  virtual ~LooperNode();

  ConnectableNodeUI * createUI() override;

  class TrackGroup : public ControllableContainer{
  public:
    TrackGroup(LooperNode* l):ControllableContainer("tracks"),
    owner(l),
    selectedTrack(nullptr),
    lastMasterTempoTrack(nullptr){};

    void setNumTracks(int numTracks);
    void addTrack();
    void removeTrack(int i);

    OwnedArray<LooperTrack> tracks;
    LooperNode * owner;
    LooperTrack *  selectedTrack;

    LooperTrack * lastMasterTempoTrack;

  };

  TrackGroup trackGroup;
  


  //Parameters
  Trigger * recPlaySelectedTrig;
  Trigger * playSelectedTrig;
  Trigger * clearSelectedTrig;
  Trigger * stopSelectedTrig;

  Trigger * selectAllTrig;
  Trigger * clearAllTrig;
  Trigger * stopAllTrig;
  Trigger * playAllTrig;
  Trigger * togglePlayStopAllTrig;

  Trigger * selectNextTrig;
  Trigger * selectPrevTrig;

  FloatParameter * volumeSelected;
  BoolParameter * isMonitoring;
  IntParameter * numberOfTracks;
  IntParameter * numberOfAudioChannelsIn;
  IntParameter * selectTrack;
  IntParameter * quantization;
  IntParameter * preDelayMs;
  BoolParameter * isOneShot;
  BoolParameter*  firstTrackSetTempo;
  BoolParameter * waitForOnset;
  FloatParameter * onsetThreshold;
  BoolParameter * outputAllTracksSeparately;
  BoolParameter * autoNextTrackAfterRecord;
  BoolParameter * autoClearPreviousIfEmpty;

  Trigger * exportAudio;

  AudioBuffer<float> bufferIn;
  AudioBuffer<float>bufferOut;

  int getQuantization();


  void selectMe(LooperTrack * t);


  bool askForBeingMasterTrack(LooperTrack * t);
  bool askForBeingAbleToPlayNow(LooperTrack *_t);
  bool askForBeingAbleToRecNow(LooperTrack * _t);
  bool areAllTrackClearedButThis(LooperTrack * _t);
  bool hasAtLeastOneTrackPlaying();


  void onContainerTriggerTriggered(Trigger * t) override;
  void onContainerParameterChanged(Parameter * p) override;
//  void parameterValueChanged(Parameter *p)override;
  // internal
  void processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)override;
  

  bool wasMonitoring;

  //Listener
  class  LooperListener
  {
  public:

    /** Destructor. */
    virtual ~LooperListener() {}
    virtual void trackNumChanged(int num) = 0;
  };

  ListenerList<LooperListener> looperListeners;
  void addLooperListener(LooperListener* newListener) { looperListeners.add(newListener); }
  void removeLooperListener(LooperListener* listener) { looperListeners.remove(listener); }


  void clearInternal()override;
  bool hasOnset();


  // TimeListener functions
  void playStop(bool isPlaying) override;
  void timeJumped(sample_clk_t time)override;
  void BPMChanged(double BPM) override;
  bool isBoundToTime()override;

private:
  // keeps track of few bits of audio
  // to readjust the loop when controllers are delayed
  void numChannelsChanged(bool isInput)override;
  PhantomBuffer streamAudioBuffer;
  friend class LooperTrack;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LooperNode)
};

#endif  // LOOPERNODE_H_INCLUDED
