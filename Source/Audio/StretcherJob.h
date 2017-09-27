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

#ifndef STRETCHERJOB_H_INCLUDED
#define STRETCHERJOB_H_INCLUDED

#if !BUFFER_CAN_STRETCH
    #pragma error shouldnt be included if buffer can not stretch
#endif

#include "../JuceHeaderAudio.h"

////////////////
// stretcher
namespace RubberBand {class RubberBandStretcher;};
class PlayableBuffer;

class StretcherJob : public ThreadPoolJob
{
public:

    StretcherJob (PlayableBuffer* pb, double _ratio):
        ThreadPoolJob ("stretch"),
        owner (pb),
        ratio (_ratio),
        tmpStretchBuf (1, 44100, 4096)

    {
    };


    JobStatus runJob()override;

    int studyStretch (double ratio, int start, int blockSize);
    void processStretch (int start, int block, int* read, int* produced);
    // stretching function
    void initStretcher (int sR, int c);
    PlayableBuffer* owner;
    int originNumSamples;
    double ratio;

    BufferBlockList tmpStretchBuf;


    ScopedPointer<RubberBand::RubberBandStretcher> stretcher;

    CriticalSection jobLock;

};



#endif  // STRETCHERJOB_H_INCLUDED
