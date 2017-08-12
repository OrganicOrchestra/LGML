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


#ifndef MULTINEEDLE_H_INCLUDED
#define MULTINEEDLE_H_INCLUDED

#include "AudioConfig.h"
#include "JuceHeader.h"
#include "BufferBlockList.h"



class FadeNeedle{
public:
  FadeNeedle();
  void set(const int & c,const int fIn,const int fOut,const int sus,bool _reverse = false);


  void reset();
  int startNeedle;
  int startFadeOutCount;
  bool isFadingOut;
  float lastFade,currentFade;
  int consumedSamples;
  int fadeInNumSamples ,sustainNumSamples;
  int fadeOutNumSamples;
  bool hasBeenSet;
  int num;
  bool reverse;

  int fadeOutValue();
  int getCurrentPosition();

  bool isFree(bool allowNotStarted =false);

  void consume(const int block);

  void setMaxLength(int max);
  void startFadeOut(bool force = true);

  float getFadeValueStart();
  float getFadeValueEnd();
  float maxFadeOutValue;
private:
  inline float computeCurrentFade();
  int getStartFadeOut() const;
  int getEndFadeOut()const;


};



class MultiNeedle{
public:
  int currentPos;
  Array<FadeNeedle> needles;
  int maxNeedles;
  bool isJumping;
  int fadeInNumSamples ;
  int fadeOutNumSamples;
  int needleIdx = 0;
  int loopSize;
  int numActiveNeedle;
  bool isStitching;
  CriticalSection readMutex;

  MultiNeedle(int fIn=512,int fOut=512,int max = 10);

  void setLoopSize(int _loopSize);
  void jumpTo(const int to);
  FadeNeedle *  consumeNextNeedle(int numSamples);
  void fadeAllOut();
  void resetAll();
  void addToBuffer( BufferBlockList & originBuffer,AudioBuffer<float> & destBuffer,int numSamples,bool isLooping);



private:

  FadeNeedle * getMostConsumedNeedle(const int time);
  FadeNeedle * getLessConsumedNeedle(const int time);
  
  
  
};

#endif  // MULTINEEDLE_H_INCLUDED
