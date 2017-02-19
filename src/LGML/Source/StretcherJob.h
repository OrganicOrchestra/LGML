/*
  ==============================================================================

    StretcherJob.h
    Created: 16 Feb 2017 6:29:06pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef STRETCHERJOB_H_INCLUDED
#define STRETCHERJOB_H_INCLUDED

#if !BUFFER_CAN_STRETCH
#pragma error shouldnt be included if buffer can not stretch
#endif

#include "JuceHeader.h"

////////////////
// stretcher
namespace RubberBand{class RubberBandStretcher;};
class PlayableBuffer;

class StretcherJob : public ThreadPoolJob{
public:

  StretcherJob(PlayableBuffer * pb,double _ratio):
  ThreadPoolJob("stretch"),
  owner(pb),
  ratio(_ratio)

  {
  };


  JobStatus runJob()override;

  int studyStretch(double ratio,int start,int blockSize);
  void processStretch(int start,int block,int *read,int * produced);
  // stretching function
  void initStretcher(int sR,int c);
  PlayableBuffer * owner;
  int originNumSamples;
  double ratio;

  ScopedPointer<RubberBand::RubberBandStretcher> stretcher;

};



#endif  // STRETCHERJOB_H_INCLUDED
