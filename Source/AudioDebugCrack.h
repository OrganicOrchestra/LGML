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


#ifndef AudioDebugCrack_H_INCLUDED
#define AudioDebugCrack_H_INCLUDED

#include "JuceHeader.h"
#define FORCE_DISABLE_CRACK 1



#if !FORCE_DISABLE_CRACK
#include "AudioHelpers.h"

class AudioDebugCrack {
public:
  AudioDebugCrack(const String & name);

  static void deleteInstanciated();
  static Array<AudioDebugCrack*> allAudioDebugCracks;
  static AudioDebugCrack * getOrCreateDetector(const String & name);

  void processBuffer(const AudioBuffer<float> & b);
  void notifyCrack(const AudioBuffer<float> & b, int i);
  String name;
  float lastValue = 0;
  static constexpr float threshold = 0.15f;

  float lastDerivative = 0;
  static constexpr float derivativeThreshold = 0.01f;
  bool hasCrack;
  sample_clk_t lastCrack, sampleCount;
  int debounceSample = 20;
};
#define DBGAUDIOCRACK(name,b) if(juce_isRunningUnderDebugger()){AudioDebugCrack::getOrCreateDetector(name)->processBuffer(b);}
#else
#define DBGAUDIOCRACK(name,b)
#endif
#endif  // AudioDebugCrack_H_INCLUDED
