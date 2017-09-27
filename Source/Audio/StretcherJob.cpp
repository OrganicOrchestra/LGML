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

#include "AudioConfig.h"
#if BUFFER_CAN_STRETCH

#include "rubberband/RubberBandStretcher.h"
using namespace RubberBand;

#include "PlayableBuffer.h"

#include "StretcherJob.h"



void StretcherJob::initStretcher (int sampleRate, int numChannels)
{
    if (sampleRate == 0)
    {
        sampleRate = 44100;
    }

    stretcher = new    RubberBandStretcher (sampleRate, //size_t sampleRate,
                                            numChannels,//size_t channels,
                                            RubberBandStretcher::OptionProcessOffline
                                            //                                       | RubberBandStretcher::OptionTransientsMixed
                                            | RubberBandStretcher::OptionTransientsSmooth
                                            //| RubberBandStretcher::OptionPhaseAdaptive
                                            | RubberBandStretcher::OptionThreadingNever
                                            | RubberBandStretcher::OptionWindowStandard

                                            //                                       | RubberBandStretcher::OptionStretchElastic
                                            | RubberBandStretcher::OptionStretchPrecise

                                            //Options options = DefaultOptions,
                                            //double initialTimeRatio = 1.0,
                                            //double initialPitchScale = 1.0
                                           );

    stretcher->setDebugLevel (0);
    stretcher->setPitchScale (1.0);



}





ThreadPoolJob::JobStatus StretcherJob::runJob()
{
    owner->isStretchReady = false;
    int processed = 0;
    int block = tmpStretchBuf.bufferBlockSize;

    originNumSamples = owner->originAudioBuffer.getNumSamples();

    while (!shouldExit() && processed < originNumSamples)
    {
        processed += studyStretch (ratio, processed, block);
    }

    processed = 0;
    int read = 0;
    int produced = 0;
    tmpStretchBuf.setNumChannels (owner->getNumChannels());
    owner->multiNeedle.fadeAllOut();

    while (!shouldExit() && processed < originNumSamples)
    {
        processStretch (processed, block, &read, &produced);
        processed += read;

        if (read == 0)
        {
            break;
        }

    }

    if (!shouldExit() )
    {
        ScopedTryLock lk (jobLock);

        if (lk.isLocked())
        {

            int targetNumSamples = originNumSamples * ratio;
            jassert (targetNumSamples != 0);

            int diffSample = abs (produced - targetNumSamples);

            if (diffSample > 128)
            {
                jassertfalse;
            }


            double actualRatio = produced * 1.0 / originNumSamples;
            jassert (fabs (ratio - actualRatio) < 0.01 );
            tmpStretchBuf.setNumSample (targetNumSamples);
            jassert (owner->isStretchReady == false);
            std::vector<int> tp = stretcher->getExactTimePoints();
            owner->onsetSamples.clear() ;
            int inc = stretcher->getInputIncrement();

            for (size_t i = 0 ; i < tp.size(); i++)
            {
                owner->onsetSamples.add (tp[i]*inc);
            }

            //      std::swap(owner->tmpBufferBlockList, tmpStretchBuf);
            owner->tmpBufferStretch.setSize (tmpStretchBuf.getAllocatedNumChannels(), tmpStretchBuf.getNumSamples());
            tmpStretchBuf.copyTo (owner->tmpBufferStretch, 0);
            owner->isStretchReady = true;


            //    int dbg =stretcher->getSamplesRequired();
            //    jassert(dbg<=0);

            //    owner->fadeInOut(owner->fadeSamples, 0);

            int dbg = stretcher->available();
            jassert (dbg <= 0);
        }
    }

    return jobHasFinished;
}




int StretcherJob::studyStretch (double _ratio, int start, int block)
{

    if (start == 0)
    {
        if (block == -1)block = owner->originAudioBuffer.getNumSamples();

        initStretcher (owner->sampleRate, owner->getNumChannels());
        jassert (std::isfinite (_ratio));
        stretcher->setTimeRatio (_ratio);
        stretcher->setExpectedInputDuration (originNumSamples);
        stretcher->setMaxProcessSize (block);
        //  jassert(stretcher->getInputIncrement() == originAudioBuffer.getNumSamples());
    }


    bool isFinal =  (start + block) >= originNumSamples;

    if (isFinal)
    {
        block -= jmax (0, (start + block) - originNumSamples);

    }

    const int numCh ( owner->originAudioBuffer.getNumChannels());
    const float** tmp = new const float*[numCh];

    for (int i = 0 ; i  < numCh ; i++)
    {
        tmp[i] = owner->originAudioBuffer.getReadPointer (i) + start;
    }

    stretcher->study (const_cast<const float* const*> (tmp), block, isFinal);
    delete[] tmp;
    return block;

}
void StretcherJob::processStretch (int start, int block, int* read, int* produced)
{


    if (block == -1)block = originNumSamples;

    //  int latency = stretcher->getLatency();
    //  jassert (latency == 0);


    bool isFinal = start + block >= originNumSamples;

    if (isFinal)
    {
        block -= jmax (0, (start + block) - originNumSamples);

    }

    // VS screw with the constness here
    const int numCh (owner->originAudioBuffer.getNumChannels());
    const float** tmpIn = new const float*[numCh];

    for (int i = 0 ; i  < numCh ; i++)
    {
        tmpIn[i] = owner->originAudioBuffer.getReadPointer (i) + start;
    }

    stretcher->process (const_cast<const float* const*> (tmpIn), block, isFinal);
    delete [] tmpIn;
    int available = stretcher->available();
    //  jassert( *produced + available< owner->getAllocatedNumSample());


    AudioSampleBuffer tmpOutBuf (owner->getNumChannels(), available);
    float* const* tmpOut = tmpOutBuf.getArrayOfWritePointers();
    int retrievedSamples = (int)stretcher->retrieve (tmpOut, available);
    tmpStretchBuf.setNumSample (*produced + retrievedSamples);
    tmpStretchBuf.copyFrom (tmpOutBuf, *produced, 0, retrievedSamples);
    jassert (retrievedSamples == available);

    *read = block;
    *produced += retrievedSamples;

    if (isFinal)
    {
        int dbg = stretcher->available();
        jassert (dbg <= 0);
    }

}




#endif /* BUFFER_CAN_STRETCH*/
