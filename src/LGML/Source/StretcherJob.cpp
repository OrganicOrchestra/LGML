/*
  ==============================================================================

    StretcherJob.cpp
    Created: 16 Feb 2017 6:29:06pm
    Author:  Martin Hermant

  ==============================================================================
*/
#include "AudioConfig.h"
#if BUFFER_CAN_STRETCH

#include "RubberBandStretcher.h"
using namespace RubberBand;

#include "PlayableBuffer.h"

#include "StretcherJob.h"



void StretcherJob::initStretcher(int sampleRate,int numChannels){
  if(sampleRate==0){
    sampleRate=44100;
  }
  stretcher=new    RubberBandStretcher(sampleRate,//size_t sampleRate,
                                       numChannels,//size_t channels,
                                       RubberBandStretcher::OptionProcessOffline
                                       | RubberBandStretcher::OptionTransientsMixed
                                       //~ | RubberBandStretcher::OptionTransientsSmooth
                                       //| RubberBandStretcher::OptionPhaseAdaptive
                                       | RubberBandStretcher::OptionThreadingNever
                                       | RubberBandStretcher::OptionWindowStandard

                                       //                                       | RubberBandStretcher::OptionStretchElastic
                                       | RubberBandStretcher::OptionStretchPrecise

                                       //Options options = DefaultOptions,
                                       //double initialTimeRatio = 1.0,
                                       //double initialPitchScale = 1.0
                                       );

  stretcher->setDebugLevel(0);
  stretcher->setPitchScale(1.0);


}





ThreadPoolJob::JobStatus StretcherJob::runJob(){
  int processed = 0;
  int block = 4096;

  originNumSamples = owner->originAudioBuffer.getNumSamples();
  while(!shouldExit() && processed<originNumSamples){
    processed+=studyStretch(ratio,processed,block);
  }

  processed = 0;
  int read = 0;
  int produced = 0;
  while(!shouldExit()&& processed<originNumSamples){
    processStretch(processed,block,&read,&produced);
    processed+=read;
    if(read==0){
      break;
    }

  }
  if(!shouldExit()){

    int targetNumSamples = originNumSamples*ratio;

    int diffSample =abs(produced-targetNumSamples);
    if(diffSample>128){
      jassertfalse;
    }

    owner->recordNeedle = produced;
    owner->playNeedle = owner->playNeedle*1.0/owner->recordNeedle*targetNumSamples;
    owner->setRecordedLength(targetNumSamples);
    //    int dbg =stretcher->getSamplesRequired();
    //    jassert(dbg<=0);

//    owner->fadeInOut(owner->fadeSamples, 0);

    int dbg=stretcher->available();
    jassert(dbg<=0);
  }
  return jobHasFinished;
}




int StretcherJob::studyStretch(double ratio,int start,int block){

  if(start==0){
    if(block==-1)block=owner->originAudioBuffer.getNumSamples();

    initStretcher(owner->sampleRate , owner->audioBuffer.getNumChannels());
    jassert(isfinite(ratio));
    stretcher->setTimeRatio(ratio);
    stretcher->setExpectedInputDuration(originNumSamples);
    stretcher->setMaxProcessSize(block);
    //  jassert(stretcher->getInputIncrement() == originAudioBuffer.getNumSamples());
  }


  bool isFinal =  (start+block)>=originNumSamples;
  if(isFinal){
    block -= jmax(0,(start+block)-originNumSamples);

  }
  const float* tmp[owner->originAudioBuffer.getNumChannels()];
  for(int i = 0 ; i  < owner->originAudioBuffer.getNumChannels() ; i++){
    tmp[i] = owner->originAudioBuffer.getReadPointer(i) + start;
  }
  stretcher->study(tmp, block, isFinal);
  return block;

}
void StretcherJob::processStretch(int start,int block,int * read, int * produced){


  if(block==-1)block=originNumSamples;

  //  int latency = stretcher->getLatency();
  //  jassert (latency == 0);


  bool isFinal = start+block>=originNumSamples;
  if(isFinal){
    block -= jmax(0,(start+block)-originNumSamples);

  }

  const float * tmpIn[owner->originAudioBuffer.getNumChannels()];
  for(int i = 0 ; i  < owner->originAudioBuffer.getNumChannels() ; i++){
    tmpIn[i] = owner->originAudioBuffer.getReadPointer(i) + start;
  }




  stretcher->process(tmpIn, block, isFinal);
  int available = stretcher->available();
  jassert( *produced + available< owner->audioBuffer.getNumSamples());

  float * tmpOut[owner->audioBuffer.getNumChannels()];
  for(int i = 0 ; i  < owner->audioBuffer.getNumChannels() ; i++){
    tmpOut[i] = owner->audioBuffer.getWritePointer(i) + *produced;
  }

  int retrievedSamples = stretcher->retrieve(tmpOut, available);
  jassert(retrievedSamples==available);
  
  *read = block;
  *produced+=retrievedSamples;
  
  if(isFinal){
    int dbg=stretcher->available();
    jassert(dbg<=0);
  }
  
}




#endif /* BUFFER_CAN_STRETCH*/
