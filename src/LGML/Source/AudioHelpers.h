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
// create a gain value for a float  between 0 and 1
// DB0_FOR_01   -> 0dB
// 1            -> +6dB
inline float float01ToGain(float f){
    if(f==0)return 0;
    float minus6Pos = (1-2*(1-DB0_FOR_01));
    if(f>minus6Pos) return Decibels::decibelsToGain(jmap<float>(f,DB0_FOR_01,1.0f,0.0f,6.0f));
    return Decibels::decibelsToGain(jmap<float>(f,0,minus6Pos,-70.0f,-6.0f));
};

inline float rmsToGain01(float rms){
        return jmap<float>(20.0f*log10(rms/0.74f),0.0f,6.0f,DB0_FOR_01,1.0f);
}


// helper for handling sample level fading in and out, autoCrossFade relaunches a fadeIn whenFadeOutEnds
class FadeInOut{
public:
  FadeInOut(int _fadeInSamples,int _fadeOutSamples,bool autoCrossFade = false,double _skew = 1): fadeInNumSamples(_fadeInSamples), fadeOutNumSamples(_fadeOutSamples),crossFade(autoCrossFade),skew(_skew){
    fadeOutCount = 0;
    fadeInCount = -1;
  }

  double getCurrentFade(){
    if(fadeInCount>=0){
      if(skew==1)return (1.0 - fadeInCount*1.0/fadeInNumSamples);
      return pow(1.0-fadeInCount*1.0/fadeInNumSamples,skew);
    }
    if(fadeOutCount>=0){
      if(skew==1)return fadeOutCount*1.0/fadeOutNumSamples;
      return pow(fadeOutCount*1.0/fadeOutNumSamples,skew);
    }

    jassertfalse;
    return 0.0;
  }

  void startFadeOut(){
    if(fadeOutCount>=0)return;
    fadeOutCount = fadeInCount>0?fadeInCount*fadeOutNumSamples*1.0/fadeInNumSamples :fadeOutNumSamples;
    fadeInCount = -1;
  }
  void setFadedOut(){
    fadeOutCount = 0;
    fadeInCount = -1;
  }

  void setFadedIn(){
    fadeInCount = 0;
    fadeOutCount = -1;
  }


  void startFadeIn(){
    if(fadeInCount>=0)return;
    fadeInCount = fadeOutCount>0?fadeOutCount*fadeInNumSamples*1.0/fadeOutNumSamples :fadeInNumSamples;
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
    else if(fadeInCount>0){fadeInCount-=i;fadeInCount = jmax(0,fadeInCount);}
  }

  int fadeInNumSamples;
  int fadeOutNumSamples;
  int fadeInCount,fadeOutCount;
  double skew;
  bool crossFade;

};




#endif  // AUDIOHELPERS_H_INCLUDED
