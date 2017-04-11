/*
 ==============================================================================

 MultiNeedle.h
 Created: 16 Feb 2017 6:28:48pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef MULTINEEDLE_H_INCLUDED
#define MULTINEEDLE_H_INCLUDED

#include "AudioConfig.h"
#include "JuceHeader.h"


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
  bool isStitching;
  CriticalSection readMutex;

  MultiNeedle(int fIn=512,int fOut=512,int max = 10);

  void setLoopSize(int _loopSize);
  void jumpTo(const int to);
  FadeNeedle *  consumeNextNeedle(int numSamples);
  void fadeAllOut();
  void resetAll();
  void addToBuffer(const AudioBuffer<float> & originBuffer,AudioBuffer<float> & destBuffer,int numSamples,bool isLooping);



private:

  FadeNeedle * getMostConsumedNeedle(const int time);
  FadeNeedle * getLessConsumedNeedle(const int time);
  
  
  
};

#endif  // MULTINEEDLE_H_INCLUDED
