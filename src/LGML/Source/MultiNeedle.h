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
  int num;

  int fadeOutValue(){return  (consumedSamples>getStartFadeOut())?(int)currentFade:1;}
  int getCurrentPosition(){jassert(consumedSamples>=0);return startNeedle+consumedSamples;}

  bool isFree(bool allowNotStarted =false){return !hasBeenSet || (allowNotStarted&&consumedSamples==0) ||consumedSamples>getEndFadeOut() || (isFadingOut && currentFade<=0);}

  void consume(const int block){

    if(!isFadingOut && consumedSamples+block>=getStartFadeOut()){
      startFadeOut();
    }
    lastFade = currentFade;
    consumedSamples+=block;

    currentFade = computeCurrentFade();
    if(isFadingOut && lastFade==0){
      hasBeenSet = false;
    }


    jassert((currentFade>=0) && (currentFade <=1));
  }

  void startFadeOut(){
    if (!isFree() &&!isFadingOut){
      if( consumedSamples>0){
        sustainNumSamples = consumedSamples - fadeInNumSamples;
        // fade out befor end of fadeIn
        if(sustainNumSamples<0){
          fadeInNumSamples = consumedSamples;
          sustainNumSamples = 0;
          maxFadeOutValue = currentFade;
        }
        else{
          jassert(currentFade==1);
        }
        startFadeOutCount =consumedSamples;

        isFadingOut=true;
      }

      else{
        reset();
      }
    }

  }

  float getFadeValueStart(){return lastFade;}
  float getFadeValueEnd(){return currentFade;}
private:
  inline float computeCurrentFade(){
    if(consumedSamples<fadeInNumSamples){
      float lin = jmax(0.0f,consumedSamples*1.0f  /fadeInNumSamples    );
      return sin(lin*float_Pi/2.0f);

    }

    if(isFadingOut){
      float lin =  jmax(0.0f,maxFadeOutValue * (fadeOutNumSamples - (consumedSamples - startFadeOutCount))*1.0f /fadeOutNumSamples );
      return sin(lin*float_Pi/2.0f);
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
  int loopSize;


  MultiNeedle(int fIn=512,int fOut=512,int max = 10):
  fadeInNumSamples(fIn),
  fadeOutNumSamples(fOut),
  isJumping(false),
  maxNeedles(max),
  currentPos(0),
  loopSize(0)
  {
    needles.resize(maxNeedles);
    int idx = 0;
    for(auto & n:needles){
      n.num = idx;
      idx++;
    }
  }

  void setLoopSize(int _loopSize){
    loopSize = _loopSize;
  }

  void jumpTo(const int to){
    jassert(loopSize>0);

    if(to!=0){
      int dbg;dbg++;
    }
    fadeAllOut();
    if(FadeNeedle * fN = getMostConsumedNeedle(to)){

      fN->set(to, fadeInNumSamples, fadeOutNumSamples, loopSize - fadeInNumSamples -to   );
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

  void addToBuffer(const AudioBuffer<float> & originBuffer,AudioBuffer<float> & destBuffer,int numSamples,bool isLooping){
//    jassert(destBuffer.getNumChannels()>=originBuffer.getNumChannels());
    int minComonChannels = jmin(destBuffer.getNumChannels(),originBuffer.getNumChannels());
    // ensure buffer is larger than last possible read sample
    jassert(originBuffer.getNumSamples()>loopSize+fadeOutNumSamples);
    jassert(loopSize>0);
    FadeNeedle * fN ;
    bool hasNeedle =false;

    while((fN = consumeNextNeedle(numSamples))){
      hasNeedle = true;

//      DBG(fN->num << " : "<<fN->lastFade << "\t" << fN->currentFade << "\t" << fN->getCurrentPosition()-loopSize);
      int curPos =fN->getCurrentPosition();

      for(int  i = minComonChannels-1; i >=0  ; i--){
        destBuffer.addFromWithRamp(i, 0, originBuffer.getReadPointer(i, curPos), numSamples, fN->getFadeValueStart(), fN->getFadeValueEnd());
      }
    }
    if(!hasNeedle){
      int dbg;dbg++;
    }
    int newPos = currentPos+numSamples;


    if(newPos>=loopSize){
      if(isLooping){
        //      int firstPart = loopSize - currentPos;
        int secondPart = newPos-loopSize;

        jumpTo(secondPart);

      }
      else{
      }


    }

    else{
      currentPos = newPos;

    }


  }



private:

  FadeNeedle * getMostConsumedNeedle(const int time){
    FadeNeedle * res = &needles.getReference(0);
    if(res->isFree(true) || (res->getCurrentPosition()==time && res->consumedSamples==0))return res;
    float minFade = (float)res->fadeOutValue();
    for(int i = 1 ; i < maxNeedles ; i++){
      FadeNeedle * cN =&needles.getReference(i);
      if(cN->isFree(true) || (res->getCurrentPosition()==time && res->consumedSamples==0))return cN;
      float cR = (float)cN->fadeOutValue();
      if(cR< minFade){
        res = cN;
        minFade = cR;
      }
    }
#ifndef LINK_SUPPORT
#error should be defined here
#endif
#if !LINK_SUPPORT
    jassertfalse;
#endif
    return res;
    
    
  }
  
  
};

#endif  // MULTINEEDLE_H_INCLUDED
