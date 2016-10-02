/*
 ==============================================================================

 AudioHelpers.h
 Created: 8 Mar 2016 12:33:13pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef AUDIOHELPERS_H_INCLUDED
#define AUDIOHELPERS_H_INCLUDED


#define DB0_FOR_01 0.8f
#define MIN_DB -70.0f
// create a gain value for a float  between 0 and 1
// DB0_FOR_01   -> 0dB
// 1            -> +6dB
inline float float01ToGain(float f){
  if(f==0)return 0;
  float minus6Pos = (1-2*(1-DB0_FOR_01));
  if(f>minus6Pos) return Decibels::decibelsToGain(jmap<float>(f,DB0_FOR_01,1.0f,0.0f,6.0f));
  return Decibels::decibelsToGain(jmap<float>(f,0,minus6Pos,MIN_DB,-6.0f));
};

inline float rmsToDB_6dBHR(float rms){
  // alows a +6db headroom for rms=1
  return Decibels::gainToDecibels(rms/0.5f);
}


// helper for handling sample level fading in and out, autoCrossFade relaunches a fadeIn whenFadeOutEnds
class FadeInOut{
public:
  FadeInOut(int _fadeInSamples,int _fadeOutSamples,bool autoCrossFade = false,double _skew = 1): fadeInNumSamples(_fadeInSamples), fadeOutNumSamples(_fadeOutSamples),crossFade(autoCrossFade),skew(_skew){
    fadeOutCount = 0;
    fadeInCount = -1;
  }
  FadeInOut(int numSample): fadeInNumSamples(numSample), fadeOutNumSamples(numSample),crossFade(false),skew(1.0){

  }

  double getCurrentFade(){
    if(fadeInCount>=0){
      lastFade =  getFade(fadeOutNumSamples-fadeInCount,fadeInNumSamples);
      return lastFade;
    }
    if(fadeOutCount>=0){
      lastFade =  getFade(fadeOutCount,fadeOutNumSamples);
      return lastFade;
    }
    jassertfalse;
    return 0.0;

  }
  double getLastFade(){
    return lastFade;
  }

  inline double getFade(int cur,int max){
    if(skew==1)return cur*1.0/max;
    return pow(cur*1.0/max,skew);
  }


  void startFadeOut(){
    if(fadeOutCount>=0)return;
    fadeOutCount = (int)(fadeInCount>0?fadeInCount*fadeOutNumSamples*1.0/fadeInNumSamples :fadeOutNumSamples);
    fadeInCount = -1;
  }
  void setFadedOut(){
    fadeOutCount = 0;
    fadeInCount = -1;
    lastFade = 0.0;
  }

  void setFadedIn(){
    fadeInCount = 0;
    fadeOutCount = -1;
    lastFade = 1.0;
  }


  void startFadeIn(){
    if(fadeInCount>=0)return;
    fadeInCount = (int)(fadeOutCount > 0 ? fadeOutCount*fadeInNumSamples*1.0 / fadeOutNumSamples : fadeInNumSamples);
    fadeOutCount = -1;
  }
  // should be called at each sample to compute resulting fade
  void incrementFade(int i = 1){
    if(fadeOutCount>0){
      fadeOutCount-=i;
      fadeOutCount = jmax(0,fadeOutCount);
      if(crossFade && fadeOutCount<=0){
        fadeInCount = fadeInNumSamples;
      }
    }
    else if(fadeInCount>0){
      fadeInCount-=i;
      fadeInCount = jmax(0,fadeInCount);
    }
  }

  int fadeInNumSamples;
  int fadeOutNumSamples;
  int fadeInCount,fadeOutCount;
  double lastFade;
  double skew;
  bool crossFade;

};




#endif  // AUDIOHELPERS_H_INCLUDED
