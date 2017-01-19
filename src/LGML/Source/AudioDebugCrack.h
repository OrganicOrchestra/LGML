/*
  ==============================================================================

    AudioDebugCrack.h
    Created: 19 Jan 2017 8:44:07am
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef AudioDebugCrack_H_INCLUDED
#define AudioDebugCrack_H_INCLUDED

#include "JuceHeader.h"
#define FORCE_DISABLE_CRACK 0

class AudioDebugCrack{
public:
  AudioDebugCrack(const String & name);

  static void deleteInstanciated();
  static Array<AudioDebugCrack*> allAudioDebugCracks;
  static AudioDebugCrack * getOrCreateDetector(const String & name);

  void processBuffer(const AudioBuffer<float> & b);
    void notifyCrack(const AudioBuffer<float> & b,int i);
  String name;
  float lastValue = 0;
    float threshold  =0.05;

    float lastDerivative  =0;
    float derivativeThreshold = 0.01;
  bool hasCrack;
  uint64 lastCrack,sampleCount;
  int debounceSample = 20;
};

#if !FORCE_DISABLE_CRACK
#define DBGAUDIOCRACK(name,b) if(juce_isRunningUnderDebugger()){AudioDebugCrack::getOrCreateDetector(name)->processBuffer(b);}
#else
#define DBGAUDIOCRACK(name,b)
#endif
#endif  // AudioDebugCrack_H_INCLUDED
