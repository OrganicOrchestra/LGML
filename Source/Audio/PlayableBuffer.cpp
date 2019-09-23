/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

/*
 ==============================================================================

 PlayableBuffer.cpp
 Created: 10 Jan 2017 4:40:14pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "PlayableBuffer.h"



#include "../Utils/AudioDebugPipe.h"


extern ThreadPool* getEngineThreadPool();


#if BUFFER_CAN_STRETCH
#include "StretcherJob.h"
#include "rubberband/RubberBandStretcher.h"
using namespace RubberBand;
#endif



//////////////////////
// Playable buffer


PlayableBuffer::PlayableBuffer (int numChannels, int numSamples, float _sampleRate, int _blockSize):

recordNeedle (0),

playNeedle (0), globalPlayNeedle (0),
state (BUFFER_STOPPED),
lastState (BUFFER_STOPPED),
stateChanged (false),
numTimePlayed (0),
sampleOffsetBeforeNewState (0),
multiNeedle (512, 512)

#if BUFFER_CAN_STRETCH
, stretchJob (nullptr)
#if RT_STRETCH
, pendingTimeStretchRatio (1)
, isStretchPending (false)
, isStretchReady (false)
, fadePendingStretch (256, 256)
, desiredRatio(1)
, appliedRatio(1)
, RTStretcherSamplerate(-1)
#endif
#endif

, bufferBlockList (numChannels, numSamples)
, blockSize (_blockSize)
, sampleRate (_sampleRate)

{

    jassert (numSamples < std::numeric_limits<int>::max());

#if RT_STRETCH
    fadePendingStretch.setFadedOut();
    initRTStretch();
#endif
}

PlayableBuffer::~PlayableBuffer()
{

#if PROCESS_FINAL_STRETCH

    cancelStretchJob(true);
#endif

}


void PlayableBuffer::setNumChannels (int numChannels)
{
    cancelStretchJob(true);
    bufferBlockList.setNumChannels (numChannels);
#if RT_STRETCH
    initRTStretch();
#endif


}
int PlayableBuffer::getNumChannels() const
{
    return bufferBlockList.getAllocatedNumChannels();
}
int PlayableBuffer::getAllocatedNumSample() const
{
    return bufferBlockList.getAllocatedNumSample();
}
void PlayableBuffer::setBlockSize (int bs)
{
    blockSize = bs;
#if RT_STRETCH
    initRTStretch();
#endif
}

bool PlayableBuffer::processNextBlock (AudioBuffer<float>& buffer, sample_clk_t time)
{

#if PROCESS_FINAL_STRETCH
    if(desiredRatio!=appliedRatio){
        setTimeRatio(desiredRatio,true);
    }
#endif
    bool succeeded = true;

    if (buffer.getNumSamples() != blockSize)
    {
        setBlockSize (buffer.getNumSamples());
    }


    if (sampleOffsetBeforeNewState)
    {
        jassert (isFirstRecordingFrame() || wasLastRecordingFrame() || globalPlayNeedle == 0);
    }

    if (isFirstRecordingFrame())
    {
        succeeded &= writeAudioBlock (buffer, sampleOffsetBeforeNewState);

    }
    else if (isRecording() )
    {
        succeeded &= writeAudioBlock (buffer);
    }
    else if ( wasLastRecordingFrame())
    {
        // check if was not cleared before
        if(getRecordedLength()>0){
            jassert ( getRecordedLength() >= getMinRecordSampleLength());
            succeeded &= writeAudioBlock (buffer, 0, sampleOffsetBeforeNewState);
        }
        //    findFadeLoopPoints();



    }



    // save to buffer when end recording
    if ( wasLastRecordingFrame() && recordNeedle > 0 && originAudioBuffer.getNumSamples() == 0)
    {
        originAudioBuffer.setSize (getNumChannels(), getRecordedLength());
        bufferBlockList.setNumSample (getRecordedLength());
        fadeInOut();
        bufferBlockList.copyTo (originAudioBuffer, 0, 0, getRecordedLength());

    }

    buffer.clear();


#if RT_STRETCH


    processPendingRTStretch (buffer, time);
    readNextBlock (buffer, time, sampleOffsetBeforeNewState);

    //update play state
    if(isStretchPending){
        if(isFirstPlayingFrame() ){
            stretchNeedle = playNeedle * (originAudioBuffer.getNumSamples() * 1.0 / getRecordedLength());
            multiNeedle.fadeAllOut();
            fadePendingStretch.startFadeIn();
        }
        if(isStopping() ){
            fadePendingStretch.startFadeOut();
        }
    }




    //  int stretchedSample = tmpBufferStretch.getNumSamples();
    if (originAudioBuffer.getNumSamples() == 0)
    {
        isStretchPending = false;
        isStretchReady = false;
    }

    if (isStretchReady)
    {
        if (fadePendingStretch.getLastFade() == 0)
        {

            applyStretch();

            if (isPlaying())setPlayNeedle ( time % tmpBufferStretch.getNumSamples());
        }
        else if (!fadePendingStretch.isFadingOut())
        {
            if (onsetSamples.size() > 0)
            {
                int nextOnset = -1;
                for (auto o : onsetSamples)
                {
                    int delta = (int) ((sample_clk_t)o - (sample_clk_t) (stretchNeedle % originAudioBuffer.getNumSamples()) );
                    int absDelta = abs(delta);
                    if (absDelta <= fadePendingStretch.fadeOutNumSamples )
                    {
                        fadePendingStretch.startFadeOut();
                        break;
                    }
                    if(delta>0){
                        if(nextOnset==-1){
                            nextOnset = delta;
                        }
                        else{
                            // assure ordered
                            jassert(delta>nextOnset);
                        }
                    }
                }
                // if we need to wait to long, prefere
                if(nextOnset>44100*0.5){
                    fadePendingStretch.startFadeOut();
                }
            }
            else
            {
                fadePendingStretch.startFadeOut();
            }
        }


    }

#else
    readNextBlock (buffer, time, sampleOffsetBeforeNewState);
#endif

    //  }
    if (isStopped() && playNeedle > 0)
    {
        multiNeedle.fadeAllOut();
        //     setPlayNeedle(0);
        playNeedle = 0;
        //    globalPlayNeedle = 0;
    }

    return succeeded;
}



bool PlayableBuffer::writeAudioBlock (const AudioBuffer<float>& buffer, int fromSample, int samplesToWrite)
{



    samplesToWrite = samplesToWrite == -1 ? buffer.getNumSamples() - fromSample : samplesToWrite;

    if (samplesToWrite == 0) {return true;}

    // allocate more if needed
    if (recordNeedle + buffer.getNumSamples() >= getAllocatedNumSample())
    {
        bufferBlockList.allocateSamples (bufferBlockList.getAllocatedNumChannels(), recordNeedle + 10 * buffer.getNumSamples());
    }

    bufferBlockList.copyFrom (buffer, recordNeedle);
    recordNeedle += samplesToWrite;


    return true;
}


inline bool PlayableBuffer::readNextBlock (AudioBuffer<float>& buffer, sample_clk_t time, int fromSample   )
{
    bool hasAdded=  false;
    if (isRecording())
    {
        //    jassertfalse;
        return false;
    }

//    if (fromSample > 0)
//    {
//        jassert (isFirstPlayingFrame() ||
//                 (playNeedle + fromSample + getRecordedLength()) % getRecordedLength() == 0);
//
//    }

    int numSamples = buffer.getNumSamples() - fromSample;


    if (wasLastRecordingFrame())
    {
        jassert ((getRecordedLength() == 0) || (getRecordedLength() >= getMinRecordSampleLength()));
        multiNeedle.setLoopSize (getRecordedLength());
    }

    if (state == BUFFER_PLAYING && !isStretchPending && !wasLastRecordingFrame())
    {
        jassert (multiNeedle.loopSize == getRecordedLength() );
        sample_clk_t targetTime = (time  + getRecordedLength()) % getRecordedLength();

        if (targetTime != playNeedle)
        {
//                  jassertfalse;
            setPlayNeedle (targetTime);

        }
    }



    // assert false for now to check alignement
    //  if(isFirstPlayingFrame()){
    //
    //  }


    //  buffer.clear();
    if (recordNeedle > 0 && !isRecording() )
    {
        if (isPlaying() && multiNeedle.currentPos != playNeedle && !isStretchPending)
        {
            jassertfalse;
            multiNeedle.jumpTo (playNeedle);

        }


        hasAdded = multiNeedle.addToBuffer (bufferBlockList, buffer,fromSample,numSamples, isPlaying());
        //    }

    }

    if (isPlaying())
    {
        if (getRecordedLength())
        {
            playNeedle += numSamples;
            globalPlayNeedle += numSamples;

            if (playNeedle >= getRecordedLength())
            {
                numTimePlayed ++;
            }

            playNeedle %= getRecordedLength();
        }
        else
        {
            jassertfalse;
        }
    }
    return hasAdded;

}


void PlayableBuffer::setPlayNeedle (int n)
{
    if (n < 0)
    {
        n = getRecordedLength() + n;
    }

    multiNeedle.jumpTo (n);
    playNeedle = n;

}

void PlayableBuffer::cropEndOfRecording (int* sampletoRemove)
{
    if (*sampletoRemove > recordNeedle)
    {
        *sampletoRemove = 0;
    }

    recordNeedle -= *sampletoRemove;
    multiNeedle.setLoopSize (recordNeedle);
}
//void PlayableBuffer::padEndOfRecording(int sampleToAdd){
//  audioBufferList.clear((int)recordNeedle, sampleToAdd);
//  recordNeedle+=sampleToAdd;
//  multiNeedle.setLoopSize(recordNeedle);
//}
void PlayableBuffer::setRecordedLength (sample_clk_t targetSamples)
{
    //  jassert(targetSamples<=getAllocatedNumSample());
    if(targetSamples==0){
        onsetSamples.clearQuick();
    }
    recordNeedle = targetSamples;
    multiNeedle.setLoopSize (targetSamples);
    bufferBlockList.setNumSample (targetSamples);

    //  findFadeLoopPoints();

}
//
//inline int findFirstZeroCrossing(const AudioBuffer<float> & b, int start,int end,int c){
//  float fS = b.getSample(c, start);
//  if(fS ==0) return start;
//  bool sgn = fS>0;
//  if( start>end){
//    for(int i = start-1 ; i > end ; i--){
//      if((b.getSample(c,i)>0)!=sgn){ return i;}
//    }
//  }
//  else{
//    for(int i = start+1 ; i < end ; i++){
//      if((b.getSample( c,i)>0)!=sgn){return i;}
//    }
//
//  }
//  return -1;
//}
//
//void PlayableBuffer::findFadeLoopPoints(){
////  const int zeroSearch = 64;
////  int c = 0;
//
////  int lZ = findFirstZeroCrossing(audioBuffer, recordNeedle, recordNeedle-zeroSearch, c);
////  if(lZ>0){
////    fadeLoopOutPoint = lZ;
////    reverseFadeOut = true;
////  }
////  else{
////    reverseFadeOut = false;
////    fadeLoopOutPoint = recordNeedle - getNumSampleFadeOut();
////  }
//  fadeLoopOutPoint = recordNeedle;
//  reverseFadeOut = true;
//}


bool PlayableBuffer::isFirstPlayingFrameAfterRecord()const {return lastState == BUFFER_RECORDING && state == BUFFER_PLAYING;}
bool PlayableBuffer::isFirstStopAfterRec()const {return lastState == BUFFER_RECORDING && state == BUFFER_STOPPED;}
bool PlayableBuffer::isFirstPlayingFrame()const {return lastState != BUFFER_PLAYING && state == BUFFER_PLAYING;}
bool PlayableBuffer::isFirstRecordingFrame()const {return lastState != BUFFER_RECORDING && state == BUFFER_RECORDING;}
bool PlayableBuffer::wasLastRecordingFrame()const {return lastState == BUFFER_RECORDING && state != BUFFER_RECORDING;}
bool PlayableBuffer::isStopping() const {return (lastState != BUFFER_STOPPED  ) && (state == BUFFER_STOPPED);}
bool PlayableBuffer::isStopped() const {return (state == BUFFER_STOPPED);}
bool PlayableBuffer::isRecording() const {return state == BUFFER_RECORDING;}
bool PlayableBuffer::isPlaying() const {return state == BUFFER_PLAYING;}
bool PlayableBuffer::isFirstRecordedFrame() const {return state == BUFFER_RECORDING && (lastState != BUFFER_RECORDING);}
bool PlayableBuffer::isOrWasPlaying() const {return (state == BUFFER_PLAYING || lastState == BUFFER_PLAYING) &&  recordNeedle > 0 && getAllocatedNumSample() > 0;}
bool PlayableBuffer::isOrWasRecording() const {return (state == BUFFER_RECORDING || lastState == BUFFER_RECORDING) && getAllocatedNumSample() > 0;}


void PlayableBuffer::startRecord() {
    if(isStretchJobPending()){
        jassertfalse;
        cancelStretchJob(true);
    }
    recordNeedle = 0; multiNeedle.setLoopSize (0); playNeedle = 0; globalPlayNeedle = 0;
    originAudioBuffer.setSize (0, 0, false, false, true);


}
inline void PlayableBuffer::startPlay() {multiNeedle.setLoopSize (recordNeedle); setPlayNeedle (0);}
void PlayableBuffer::clear(){
    appliedRatio = 1.0;
    desiredRatio = 1.0;
    if(!fadePendingStretch.isFadingOut()){
        fadePendingStretch.startFadeOut();
    }

    cancelStretchJob(false);
}



void PlayableBuffer::endProcessBlock()
{
    lastState = state;
    stateChanged = false;
    sampleOffsetBeforeNewState = 0;
}


void PlayableBuffer::setState (BufferState newState, int _sampleOffsetBeforeNewState)
{
    //        lastState = state;
    stateChanged |= newState != state;

    switch (newState)
    {
        case BUFFER_RECORDING:
            startRecord();
            break;

        case BUFFER_PLAYING:
        {
            // if was recording adjust multineedle loopsize
            if(state==BUFFER_RECORDING){
                jassert (getRecordedLength() >= getMinRecordSampleLength());
                auto targetRecordedLength =getRecordedLength()+_sampleOffsetBeforeNewState;
                multiNeedle.setLoopSize (targetRecordedLength);
                jassert(_sampleOffsetBeforeNewState>=0);
            }

            setPlayNeedle (0);
            globalPlayNeedle = 0;
            break;
        }
        case BUFFER_STOPPED:
            jassert (getRecordedLength() == 0 || getRecordedLength() >= getMinRecordSampleLength());
            numTimePlayed = 0;
            multiNeedle.fadeAllOut();
            //        setPlayNeedle(0);
            break;
    }

    state = newState;

    // don't use sample offset when stopping as it's already triggering stop fade out
    // messes up the running time
    if (newState != BUFFER_STOPPED)sampleOffsetBeforeNewState = _sampleOffsetBeforeNewState;
}

PlayableBuffer::BufferState PlayableBuffer::getState() const {return state;}
PlayableBuffer::BufferState PlayableBuffer::getLastState() const {return lastState;}


sample_clk_t PlayableBuffer::getRecordedLength() const {return recordNeedle;}
//sample_clk_t PlayableBuffer::getStretchedLength() const{return recordNeedle*pendingTimeStretchRatio;}
int PlayableBuffer::getMinRecordSampleLength() const
{
    return multiNeedle.fadeInNumSamples + multiNeedle.fadeOutNumSamples + 1024;
}
sample_clk_t PlayableBuffer::getPlayPos() const {return playNeedle;}
sample_clk_t PlayableBuffer::getGlobalPlayPos() const {return globalPlayNeedle;}



int PlayableBuffer::getSampleOffsetBeforeNewState() {return sampleOffsetBeforeNewState;};
int PlayableBuffer::getNumSampleFadeOut() const {return multiNeedle.fadeOutNumSamples;};

void PlayableBuffer::setSampleRate (float sR) {
    sampleRate = sR;
};

void PlayableBuffer::fadeInOut()
{
    int fadeIn = multiNeedle.fadeInNumSamples;
    int fadeOut = multiNeedle.fadeOutNumSamples;
    auto startBlock = bufferBlockList.getUnchecked (0);
    startBlock->applyGainRamp ( 0, fadeIn, 0.0f, 1.f);

    int lIdx = floor (bufferBlockList.getNumSamples() / bufferBlockList.bufferBlockSize);
    auto endBlock = bufferBlockList.getUnchecked (lIdx);
    int endPoint = bufferBlockList.getNumSamples() - (lIdx * bufferBlockList.bufferBlockSize);
    jassert (endPoint > 0);

    if (endPoint < fadeOut)
    {
        float ratio = endPoint * 1.0 / fadeOut;
        int firstPart = (fadeOut - endPoint);

        if (lIdx > 0)
        {
            auto eendBlock = bufferBlockList.getUnchecked (lIdx - 1);
            eendBlock->applyGainRamp (bufferBlockList.bufferBlockSize - firstPart, firstPart, 1.0f, ratio);
            endBlock->applyGainRamp (0, endPoint, ratio, 0.0f);
        }
        else
        {
            jassertfalse;
            endBlock->applyGainRamp (0, endPoint + 1, 1.0f, 0.0f);
            DBG ("truncate fadeOut for small buffer");

        }

    }
    else
    {
        endBlock->applyGainRamp (endPoint - fadeOut, fadeOut + 1, 1.0f, 0.0f);
    }


}
#if BUFFER_CAN_STRETCH
void PlayableBuffer::cancelStretchJob(bool waitForIt){
    if (stretchJob)
    {
        ScopedLock lk (stretchJob->jobLock);
        stretchJob->signalJobShouldExit();
        if(waitForIt){
            int64 mil = Time::currentTimeMillis();
            if(auto tp = getEngineThreadPool()){
                tp->waitForJobToFinish (stretchJob, 5000);
            }

            DBG("Waited : " <<(Time::currentTimeMillis() - mil));
        }
    }

}
bool PlayableBuffer::isStretchJobPending(){
    auto tp = getEngineThreadPool();
    return stretchJob.get() &&  tp->contains(stretchJob);


}

void PlayableBuffer::setTimeRatio (const double ratio,bool now)
{
    desiredRatio = ratio;

    //  jassert(isOrWasPlaying());
    if (originAudioBuffer.getNumSamples() == 0) {return;}

#if RT_STRETCH


    if (isPlaying() && ratio!=pendingTimeStretchRatio)
    {
        if (!isStretchPending && ratio != 1.0 )
        {
            stretchNeedle = playNeedle * (originAudioBuffer.getNumSamples() * 1.0 / getRecordedLength());
            multiNeedle.fadeAllOut();
            fadePendingStretch.startFadeIn();

        }


        pendingTimeStretchRatio = ratio;
        isStretchPending =   ( ratio != 1.0);
    }

#endif

    if(!now){

        return;
    }

#if PROCESS_FINAL_STRETCH
    ThreadPool* tp = getEngineThreadPool();

    if (tp->contains (stretchJob) )
    {
        ScopedLock lk (stretchJob->jobLock);
        if( stretchJob && stretchJob->ratio!=ratio && !stretchJob->shouldExit()){
            stretchJob->signalJobShouldExit();
            return;
        }

    }
    if (ratio != 1.0)
    {
        if(!tp->contains (stretchJob)   &&
           ratio != appliedRatio        &&
           (!stretchJob || stretchJob->ratio!=ratio))
        {

            jassert(!tp->contains(stretchJob));
            jassert(!stretchJob || !stretchJob->isRunning());
            jassert(!stretchJob || stretchJob->shouldExit());
            auto tmpStretchJob = new StretcherJob (this, ratio);
            tp->addJob (tmpStretchJob, true);
            stretchJob = tmpStretchJob;
        }
#else
        if (ratio != 1.0)
        {
            bufferBlockList.setNumSample(originAudioBuffer.getNumSamples());
            bufferBlockList.copyFrom (originAudioBuffer, 0);
            setRecordedLength (originAudioBuffer.getNumSamples());
#endif
        }
        else
        {

            tmpBufferStretch.makeCopyOf(originAudioBuffer);
            isStretchReady = true;
            appliedRatio=1.0;

            //        bufferBlockList.copyFrom (originAudioBuffer, 0);
            //        setRecordedLength (originAudioBuffer.getNumSamples());


        }



    }
#endif

#if RT_STRETCH
    constexpr bool use_pb_blocksize = true;
    constexpr int forcedBlockSize = 0;//1024;
    void PlayableBuffer::initRTStretch()
    {
        pendingTimeStretchRatio = 1.0;

        jassert(sampleRate>0);
        jassert (blockSize > 0);

        auto targetNumChannel = bufferBlockList.getAllocatedNumChannels();

        if(!RTStretcher.get() ||
           RTStretcher->getChannelCount()!=targetNumChannel ||
           RTStretcherSamplerate!=sampleRate){
            RTStretcherSamplerate = sampleRate;
            RTStretcher = std::make_unique<RubberBandStretcher>  (sampleRate,//size_t sampleRate,
                                                    bufferBlockList.getAllocatedNumChannels(),//size_t channels,
                                                    RubberBandStretcher::OptionProcessRealTime
                                                    //
                                                    | RubberBandStretcher::OptionTransientsCrisp
//                                                    | RubberBandStretcher::OptionTransientsMixed

                                                    //| RubberBandStretcher::OptionPhaseAdaptive
                                                    | RubberBandStretcher::OptionThreadingNever
                                                    | RubberBandStretcher::OptionWindowStandard

                                                    | RubberBandStretcher::OptionChannelsTogether

                                                    | RubberBandStretcher::OptionStretchPrecise // rubberband forced to this mode in real time (StretchElastic is useless)

                                                    //Options options = DefaultOptions,
                                                    //double initialTimeRatio = 1.0,
                                                    //double initialPitchScale = 1.0
                                                    );

        }
        else{
            RTStretcher->reset();
        }

        if(forcedBlockSize>0)
            RTStretcher->setMaxProcessSize(forcedBlockSize);
        else{
            if(use_pb_blocksize)
                RTStretcher->setMaxProcessSize(blockSize);
        }
        RTStretcher->setPitchScale (1.0);
        stretchNeedle = 0;

    }

    bool PlayableBuffer::processPendingRTStretch (AudioBuffer<float>& b, sample_clk_t /*time*/)
    {
        //    if(auto f = fadePendingStretch.getCurrentFade())DBGRT(f);

        if (isStretchPending )
        {

            int outNumSample = b.getNumSamples();

            fadePendingStretch.incrementFade (outNumSample);

            double lastFade = fadePendingStretch.getLastFade();
            double curFade = fadePendingStretch.getCurrentFade();


            //    jassert(curFade>0 || lastFade>0);
            if(curFade!=0 || lastFade!=0){

                int originNumSamples = originAudioBuffer.getNumSamples();


                jassert (pendingTimeStretchRatio != 0);

                const int numChannels (getNumChannels());


                int available = RTStretcher->available();
                const float** inBuf = new const float*[numChannels];

                //    if(stretchNeedle!=originNumSamples)   {
                while (available < outNumSample  )
                {

                    //      int targetLen = (originNumSamples*pendingTimeStretchRatio);
                    //      double curBeat = (time%(targetLen))*1.0/targetLen + 0.0000001;
                    //      double localBeat = stretchNeedle*1.0/originNumSamples;
                    if (!fadePendingStretch.isFadingOut())
                    {
                        //        double diff =(localBeat*1.0/curBeat);
                        //        diff=pow(diff,3);
                        //        double adaptStretch=jmin(jmax(0.5,diff),2.0);
                        const double adaptStretch = 1;
                        //                DBG(curBeat << " : " << localBeat << " : " <<diff << " : " << adaptStretch);
                        RTStretcher->setTimeRatio (pendingTimeStretchRatio * adaptStretch);
                    }

                    int toProcess =  forcedBlockSize!=0? forcedBlockSize:
                    use_pb_blocksize? blockSize:
                    (int)RTStretcher->getSamplesRequired();
                    //      jassert(toProcess>0);



                    if (stretchNeedle + toProcess >= originNumSamples)
                    {
                        toProcess = originNumSamples - stretchNeedle;
                    }

                    if (toProcess > 0)
                    {
                        for (int i = 0 ; i  < numChannels ; i++)
                        {
                            inBuf [i] = originAudioBuffer.getReadPointer (i) + stretchNeedle;
                        }

                        RTStretcher->process (const_cast<const float* const*> (inBuf), toProcess, false);
                        stretchNeedle += toProcess;

                        if (stretchNeedle >= originNumSamples)
                        {
                            stretchNeedle -= originNumSamples;
                            numTimePlayed++;
                        }
                    }
                    else
                    {
                        available = RTStretcher->available();
                        jassert (available > outNumSample);
                        break;
                    }

                    available = RTStretcher->available();
                    jassert (available >= 0);


                }

                delete [] inBuf;


                jassert(available>=outNumSample);
                float* const*   outBuf = b.getArrayOfWritePointers();
                RTStretcher->retrieve (outBuf, jmin (outNumSample, available));
                
                
                b.applyGainRamp (0, outNumSample,
                                 lastFade,
                                 curFade);
                
                
                //                double ratio = jmin (stretchNeedle * 1.0 / originNumSamples, 1.0);
                //        if (!fadePendingStretch.isFadingOut())
                //            playNeedle = ratio * multiNeedle.loopSize;
                
                
                return true;
            }
        }
        
        return false;
        
    }
    void PlayableBuffer::applyStretch()
    {
        
        
        //     DBGRT ("apply stretch");
        int targetNumSamples = tmpBufferStretch.getNumSamples();
        //  playNeedle =  0;
        setRecordedLength (targetNumSamples);
        bufferBlockList.copyFrom (tmpBufferStretch, 0);
        isStretchReady = false;
        isStretchPending = false;
        pendingTimeStretchRatio = 1.0;
        initRTStretch();
        //  RTStretcher->reset();
        
        
    };
#endif
