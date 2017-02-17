/*
 ==============================================================================

 MultiNeedle.h
 Created: 16 Feb 2017 6:28:48pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef MULTINEEDLE_H_INCLUDED
#define MULTINEEDLE_H_INCLUDED


#include "JuceHeader.h"


class FadeNeedle{
public:
  FadeNeedle():startNeedle(0),fadeInNumSamples(0),isFadingOut(false),
  fadeOutNumSamples(0),sustainNumSamples(0),consumedSamples(0){
    reset();
  };
  void set(const int & c,const int fIn,const int fOut,const int sus){
    reset();
    startNeedle = c;


    fadeInNumSamples =fIn;
    fadeOutNumSamples=fOut;
    sustainNumSamples = sus;
    hasBeenSet = true;


  };

  void reset(){
    currentFade = 0;
    lastFade = 0;
    consumedSamples=0;
    isFadingOut = false;
    startFadeOutCount = 0;
    maxFadeOutValue = 1;
    hasBeenSet = false;


  }
  int startNeedle;
  int startFadeOutCount;
  bool isFadingOut;
  float lastFade,currentFade;
  int consumedSamples;
  int fadeInNumSamples ,sustainNumSamples;
  int fadeOutNumSamples;
  bool hasBeenSet;

  int fadeOutValue(){return  (consumedSamples>getStartFadeOut())?currentFade:1;}
  int getCurrentPosition(){jassert(consumedSamples>=0);return startNeedle+consumedSamples;}

  bool isFree(){return !hasBeenSet || consumedSamples>getEndFadeOut();}

  void consume(const int block){

    if(!isFadingOut && getCurrentPosition()>=getStartFadeOut()){
      startFadeOutCount = consumedSamples;
      isFadingOut = true;
    }
    lastFade = currentFade;
    consumedSamples+=block;

    currentFade = computeCurrentFade();
    jassert(currentFade <=1);
  }

  void startFadeOut(){
    if (!isFree()){
      sustainNumSamples = jmax(0,sustainNumSamples);
      startFadeOutCount =consumedSamples;
      maxFadeOutValue = currentFade;
      isFadingOut=true;
    }

  }

  float getFadeValueStart(){return lastFade;}
  float getFadeValueEnd(){return currentFade;}
private:
  inline float computeCurrentFade(){
    if(consumedSamples<fadeInNumSamples){
      return jmax(0.0f,consumedSamples*1.0f  /fadeInNumSamples);

    }

    if(isFadingOut){
      return jmax(0.0f,maxFadeOutValue * (fadeOutNumSamples - (consumedSamples - startFadeOutCount))*1.0f /fadeOutNumSamples);
    }
    return 1;

  }
  int getStartFadeOut() const{
    return sustainNumSamples<0?
    std::numeric_limits<int>::max():
    fadeInNumSamples+sustainNumSamples;
  }
  int getEndFadeOut()const{
    return sustainNumSamples<0?
    std::numeric_limits<int>::max():
    fadeInNumSamples+sustainNumSamples+fadeOutNumSamples;
  }
  float maxFadeOutValue;

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


  MultiNeedle(int fIn=512,int fOut=512,int max = 10):
  fadeInNumSamples(fIn),
  fadeOutNumSamples(fOut),
  isJumping(false),
  maxNeedles(max),
  currentPos(0)
  {
    needles.resize(maxNeedles);

  }

  void jumpTo(const int to,const int sustain=-1){
    fadeAllOut();
    if(FadeNeedle * fN = getMostConsumedNeedle()){

        fN->set(to, fadeInNumSamples, fadeOutNumSamples, sustain);
        currentPos=to;
    }
    else{
      jassertfalse;
    }


  }

  FadeNeedle *  consumeNextNeedle(int numSamples){

    FadeNeedle *  res = nullptr;
    if(needleIdx<needles.size()){
      res = &needles.getReference(needleIdx);
      while(res->isFree() ){
        needleIdx++;
        // hit last while looking for free
        if(needleIdx>=needles.size()){
          needleIdx = 0;
          return nullptr;
        }
        res = &needles.getReference(needleIdx);
      }
      res->consume(numSamples);



      needleIdx++;


      return res;
    }
    else{
      needleIdx = 0;
      return nullptr;
    }

  }
  void fadeAllOut(){
    for(auto & n:needles){
      n.startFadeOut();
    }
  }

  void resetAll(){
    for(auto & n:needles){
      n.reset();
    }
  }

  void addToBuffer(const AudioBuffer<float> & originBuffer,AudioBuffer<float> & destBuffer,int numSamples,int loopSize){
    jassert(destBuffer.getNumChannels()>=originBuffer.getNumChannels());
    // ensure buffer is larger than last possible read sample
    jassert(originBuffer.getNumSamples()>loopSize+fadeOutNumSamples);
    FadeNeedle * fN ;
    while((fN = consumeNextNeedle(numSamples))){
      for(int  i = originBuffer.getNumChannels()-1; i >=0  ; i--){
        destBuffer.addFromWithRamp(i, 0, originBuffer.getReadPointer(i, fN->getCurrentPosition()), numSamples, fN->getFadeValueStart(), fN->getFadeValueEnd());
      }
    }
    int newPos = currentPos+numSamples;
    if(newPos>=loopSize){
//      int firstPart = loopSize - currentPos;
      int secondPart = newPos-loopSize;

      jumpTo(secondPart,loopSize - fadeInNumSamples-fadeOutNumSamples);

    }
    else{
    currentPos = newPos;
    }


  }



private:

  FadeNeedle * getMostConsumedNeedle(){
    FadeNeedle * res = &needles.getReference(0);
    if(res->isFree())return res;
    float minFade = res->fadeOutValue();
    for(int i = 1 ; i < maxNeedles ; i++){
      FadeNeedle * cN =&needles.getReference(i);
      if(cN->isFree())return cN;
      float cR = cN->fadeOutValue();
      if(cR< minFade){
        res = cN;
        minFade = cR;
      }
    }
    jassertfalse;
    return res;
    
    
  }
  
  
};

#endif  // MULTINEEDLE_H_INCLUDED
