/*
 ==============================================================================

 AudioDebugCrack.cpp
 Created: 19 Jan 2017 9:07:23am
 Author:  Martin Hermant

 ==============================================================================
 */

#include "AudioDebugCrack.h"
Array<AudioDebugCrack*> AudioDebugCrack::allAudioDebugCracks;



AudioDebugCrack::AudioDebugCrack(const String & n):name(n),hasCrack(false),lastCrack(0),sampleCount(0){
  allAudioDebugCracks.add(this);
}

AudioDebugCrack * AudioDebugCrack::getOrCreateDetector(const juce::String &name){
  for(auto & c:allAudioDebugCracks){
    if(c->name==name)return c;
  }
  return new AudioDebugCrack(name);
}


void AudioDebugCrack::processBuffer(const AudioBuffer<float> & b){

  for(int i = 0 ; i < b.getNumSamples() ; i++){
    float curValue = 0;
    for(int c = 0 ; c < b.getNumChannels() ; c++)
    {
      curValue += b.getSample(c,i);
    }

    if(std::abs(curValue - lastValue)>threshold){
      notifyCrack(b,i);

    }
    float derivative = std::abs(curValue- lastValue);

    if(std::abs(derivative - lastDerivative)>derivativeThreshold){
      notifyCrack(b,i);
    }


    lastValue = curValue;
    const float alpha = 0.3;
    lastDerivative += alpha * (derivative-lastDerivative);
    sampleCount++;
  }
}
void AudioDebugCrack::notifyCrack(const AudioBuffer<float> & b,int i){
  //    jassertfalse;

  if(sampleCount-lastCrack>debounceSample){
    if(!hasCrack)lastCrack=sampleCount;
    hasCrack = true;

    DBG("crack for " <<name<<" at " << i);
    int dbg = 0;
    dbg++;
  }
}


void AudioDebugCrack::deleteInstanciated(){
  for(auto & c:allAudioDebugCracks){
    delete c;
  }
  allAudioDebugCracks.clear();
}
