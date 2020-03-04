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

#include "MultiNeedle.h"


#define DEBUG_NEEDLE 0
#if DEBUG_NEEDLE

    #define DBGNEEDLE(txt) DBG(txt)
#else
    #define DBGNEEDLE(txt)
#endif

#define NEEDLE_STITCHING 0

FadeNeedle::FadeNeedle()
{
    reset();

};
void FadeNeedle::set (const int& c, const int fIn, const int fOut, const int sus, bool _reverse )
{
    reset();

    startNeedle = c;
    fadeInNumSamples = fIn;
    fadeOutNumSamples = fOut;
    sustainNumSamples = sus;
    hasBeenSet = true;
    reverse = _reverse;
    jassert (sustainNumSamples >= 0 || sustainNumSamples == -1 );
    //  jassert(startNeedle>=0);

};


void FadeNeedle::reset()
{
    currentFade = 0;
    lastFade = 0;
    consumedSamples = 0;
    isFadingOut = false;
    startFadeOutCount = 0;
    maxFadeOutValue = 1;
    hasBeenSet = false;
    reverse = false;



}

float FadeNeedle::fadeOutValue() {return  (consumedSamples > getStartFadeOut()) ? currentFade : 1;}
int FadeNeedle::getCurrentPosition() {jassert (consumedSamples >= 0); return startNeedle + (reverse ? -1 : 1) * consumedSamples;}

bool FadeNeedle::isFree (bool allowNotStarted ) {return !hasBeenSet || (allowNotStarted && consumedSamples == 0) || consumedSamples > getEndFadeOut() || (isFadingOut && currentFade <= 0);}

void FadeNeedle::consume (const int block)
{

    if (!isFadingOut && consumedSamples + block > getStartFadeOut())
    {
        startFadeOut (false);
    }

    lastFade = currentFade;
    consumedSamples += block;

    currentFade = computeCurrentFade();

    if ((isFadingOut && lastFade == 0 ) || consumedSamples > getEndFadeOut())
    {
        hasBeenSet = false;
    }


    jassert ((currentFade >= 0) && (currentFade <= 1));
}

void FadeNeedle::setMaxLength (int max)
{
    if (!hasBeenSet)
    {
        reset();
        return;
    }

    if (sustainNumSamples < 0)
    {
        // TODO : handle infinite sustain
#if DEBUG_NEEDLE
        jassertfalse;
#endif
    }
    else
    {

        int curPos = getCurrentPosition();

        if (curPos > max)
        {
            // dropping needle
#if DEBUG_NEEDLE
            jassertfalse;
#endif
            reset();
        }
        // already being fading
        else if ( isFadingOut && (curPos > max - fadeOutNumSamples))
        {
#if DEBUG_NEEDLE
            jassertfalse;
#endif
            fadeOutNumSamples = max - curPos;
        }

        // still in sustain
        else if ( getEndFadeOut() > max)
        {
            int overShoot = getEndFadeOut() - max;

            if (sustainNumSamples >= overShoot)
            {
                sustainNumSamples -= overShoot;
            }
            else
            {
                sustainNumSamples = 0 ;
                fadeOutNumSamples = max - curPos;
#if DEBUG_NEEDLE
                jassertfalse;
#endif


            }
        }
    }

}
void FadeNeedle::startFadeOut (bool force)
{
    if (!isFree() && !isFadingOut)
    {
        //      if( consumedSamples>0){
        if (force || sustainNumSamples < 0)
        {
            sustainNumSamples = consumedSamples - fadeInNumSamples;
        }

        // fade out befor end of fadeIn
        if (sustainNumSamples < 0)
        {
            fadeInNumSamples = consumedSamples;
            sustainNumSamples = 0;
        }

        maxFadeOutValue = currentFade;

        if (force)
        {
            startFadeOutCount = consumedSamples;
        }
        else
        {
            startFadeOutCount = getStartFadeOut();
        }

        isFadingOut = true;
        //      }
        //
        //      else{
        //        reset();
        //      }
    }

}

float FadeNeedle::getFadeValueStart() {return lastFade;}
float FadeNeedle::getFadeValueEnd() {return currentFade;}

forcedinline float sinFadeFunc (float x)
{
    return (sin ((x - .5f) * float_Pi) + 1.0f) / 2.0f;
}

float FadeNeedle::computeCurrentFade()
{
    if (consumedSamples < fadeInNumSamples)
    {
        float lin = jmin (1.0f, jmax (0.0f, (consumedSamples * 1.0f  / fadeInNumSamples ) ));
        float sinV = sinFadeFunc (lin);
        return maxFadeOutValue * sqrtf (sinV);

    }

    if (isFadingOut)
    {
        float lin =  jmin (1.0f, jmax (0.0f, (fadeOutNumSamples - (consumedSamples - startFadeOutCount)) * 1.0f / fadeOutNumSamples ));
        float sinV = sinFadeFunc (lin);
        return maxFadeOutValue * sqrtf (sinV);
    }

    return 1;

}

int FadeNeedle::getStartFadeOut() const
{
    jassert (sustainNumSamples >= 0 || sustainNumSamples == -1 );
    return sustainNumSamples == -1 ?
           std::numeric_limits<int>::max() :
           fadeInNumSamples + sustainNumSamples;
}

int FadeNeedle::getEndFadeOut()const
{
    jassert (sustainNumSamples >= 0 || sustainNumSamples == -1 );
    return sustainNumSamples == -1 ?
           std::numeric_limits<int>::max() :
           fadeInNumSamples + sustainNumSamples + fadeOutNumSamples;
}






MultiNeedle::MultiNeedle (int fIn, int fOut, int _maxNeedles):
    fadeInNumSamples (fIn),
    fadeOutNumSamples (fOut),
    maxNeedles (_maxNeedles)
{
    needles.resize (maxNeedles);
    int idx = 0;

    for (auto& n : needles)
    {
        n.num = idx;
        idx++;
    }
}

void MultiNeedle::setLoopSize (int _loopSize)
{
    if (_loopSize == loopSize)return;

    loopSize = _loopSize;
    ScopedLock lk (readMutex);

    if (_loopSize > 0)
    {
        //    jassert(loopSize>fadeInNumSamples+fadeOutNumSamples);
        //    loopSize = jmax(loopSize,fadeInNumSamples+fadeOutNumSamples);
        for (auto& fN : needles)
        {
            fN.setMaxLength (loopSize);
        }
    }
    else
    {
        //      for(auto & fN : needles){
        //        fN.reset();
        //      }
    }
}

void MultiNeedle::jumpTo (const int to)
{
    jassert (loopSize > 0);
    jassert (to >= 0);

//    if (to > 256)
//    {
//        int dbg;
//        dbg++;
//    }

    fadeAllOut();

    if (FadeNeedle* fN = getMostConsumedNeedle (to))
    {
        jassert (!fN->reverse);
        int targetSustain =   loopSize - to - (fadeInNumSamples + fadeOutNumSamples);

        if (targetSustain >= 0)
        {
            fN->set (to, fadeInNumSamples, fadeOutNumSamples, targetSustain);
        }
        else
        {
            //TODO implement setting needle to its fade part
            int crop = ( loopSize - to) / 2 - 1;
            jassert (crop < fadeInNumSamples && crop < fadeOutNumSamples);
            fN->set ( loopSize - to, fadeInNumSamples - crop, fadeOutNumSamples - crop, 0);
            DBG ("multineedle : cropping sustain");

        }

        currentPos = to;
    }
    else
    {
        jassertfalse;
    }


}

FadeNeedle*   MultiNeedle::consumeNextNeedle (int numSamples)
{

    FadeNeedle*   res = nullptr;

    if (needleIdx < needles.size())
    {
        res = &needles.getReference (needleIdx);

        while (res->isFree() )
        {
            needleIdx++;

            // hit last while looking for free
            if (needleIdx >= needles.size())
            {
                needleIdx = 0;
                return nullptr;
            }

            res = &needles.getReference (needleIdx);
        }

        res->consume (numSamples);

        needleIdx++;


        return res;
    }
    else
    {
        needleIdx = 0;
        return nullptr;
    }

}
void MultiNeedle::fadeAllOut()
{
    for (auto& n : needles)
    {
        n.startFadeOut (true);
    }
}

void MultiNeedle::resetAll()
{
    for (auto& n : needles)
    {
        n.reset();
    }
}

bool MultiNeedle::addToBuffer ( BufferBlockList& originBufferList, AudioBuffer<float>& destBuffer,int destStartSample, int numSamples, bool isLooping)
{
    //    jassert(destBuffer.getNumChannels()>=originBuffer.getNumChannels());
    const int minComonChannels = jmin (destBuffer.getNumChannels(), originBufferList.getAllocatedNumChannels());
    // ensure buffer is larger than last possible read sample
    //  jassert(originBufferList.getAllocatedNumSample()>loopSize+fadeOutNumSamples);
    jassert (loopSize > 0);
    //
    jassert (loopSize == originBufferList.getNumSamples());

    const int nextPos = currentPos + numSamples;

#if NEEDLE_STITCHING
    const bool isAfterStitchingPoint = nextPos > loopSize - fadeOutNumSamples  ;

    if (isLooping )
    {

        if (!isStitching && isAfterStitchingPoint)
        {
            isStitching = true;
            const int startStitch = loopSize - (fadeOutNumSamples + fadeInNumSamples)  - 2 * numSamples;
            const int tSutain = (currentPos - fadeInNumSamples) % numSamples;
            auto fN = getMostConsumedNeedle (startStitch   );
            fN->set (startStitch, fadeOutNumSamples, fadeInNumSamples, tSutain);
        }
    }
    else
    {
        int dbg;
        dbg++;
    }

    if ( isStitching && !isAfterStitchingPoint)
    {
        isStitching = false;
    }

#endif

    FadeNeedle* fN ;
    numActiveNeedle = 0;
    float accumNeedle = 0;
    float accumNeedleSq = 0;
    ScopedLock lk (readMutex);

    while ((fN = consumeNextNeedle (numSamples)))
    {
        numActiveNeedle++;
        accumNeedle += fN->getFadeValueStart();
        accumNeedleSq += fN->getFadeValueStart() * fN->getFadeValueStart();

        int curEndPos = fN->getCurrentPosition();
        int curStartPos = curEndPos - numSamples;
        jassert (curStartPos >= 0);
        jassert (curStartPos <= loopSize );
        DBGNEEDLE (currentPos << "\t" << fN->num <<   " " << nextPos - curEndPos <<  " : " << fN->lastFade << "\t" << fN->currentFade << "\t" << fN->getCurrentPosition() - loopSize);
        //          << "\t" << (fN->reverse?1:0));
        //
        //    if(fN->currentFade!=1){
        //      int dbg;dbg++;
        //    }
        //    int firstPart = jmin(numSamples,loopSize-curStartPos);
        //    // overshoot
        //    if(firstPart<0){
        //      jassertfalse;
        //      curStartPos = (curStartPos%loopSize);
        //      curEndPos = curStartPos+numSamples;
        //      firstPart = jmin(numSamples,loopSize-curStartPos);
        //      fN->startNeedle= jmax(0,fN->startNeedle-loopSize);
        //    }
        //    int secondPart = numSamples - firstPart;
        //    jassert(secondPart>=0);
        //    jassert(firstPart+secondPart==numSamples);
        //    const double firstRatio = firstPart/(numSamples);
        //
        //
        //    int fadeOutSize = numSamples;

        //    if(!fN->reverse && firstPart>0){
        //      for(int  i = minComonChannels-1; i >=0  ; i--){
        //        destBuffer.addFromWithRamp(i, 0, tmpContiguous.getReadPointer(i), firstPart, fN->getFadeValueStart(), firstRatio*fN->getFadeValueEnd());
        //      }
        //    }
        //    if(!fN->reverse && secondPart>0){
        //      for(int  i = minComonChannels-1; i >=0  ; i--){
        //        destBuffer.addFromWithRamp(i, 0, tmpContiguous.firstPart) % loopSize), secondPart, firstRatio*fN->getFadeValueEnd(), fN->getFadeValueEnd());
        //      }
        //    }

        AudioSampleBuffer tmpContiguous = originBufferList.getContiguousBuffer (curStartPos, numSamples);

        for (int  i = minComonChannels - 1; i >= 0  ; i--)
        {
            destBuffer.addFromWithRamp (i, destStartSample, tmpContiguous.getReadPointer (i), numSamples,  fN->getFadeValueStart(), fN->getFadeValueEnd());
        }

        //    if(fN->reverse){
        //      // not supported atm
        //      jassertfalse;
        //      const double fadeStep =  (fN->getFadeValueEnd() - fN->getFadeValueStart())*1.0/fadeOutSize;
        //      for(int  i = minComonChannels-1; i >=0  ; i--){
        //        double fade = fN->getFadeValueStart();
        //        for(int s = 0 ; s<fadeOutSize;s++ ){
        //          destBuffer.addSample(i, s, originBuffer.getSample(i, curStartPos - s)*fade);
        //          fade+=fadeStep;
        //        }
        //        jassert(fade == fN->getFadeValueEnd());
        //      }
        //    }
    }

    if (isLooping)
    {
        DBGNEEDLE (numActiveNeedle << " // " << accumNeedle << " // " << accumNeedleSq );
    }

    //  if(numActiveNeedle>1){
    //    int dbg;dbg++;

    //      jassertfalse.
    //  }


    if (nextPos >= loopSize)
    {
        if (isLooping)
        {
            //      int firstPart = loopSize - currentPos;
            const int secondPart = nextPos - loopSize;
            jumpTo (secondPart);

        }
        else
        {
        }


    }

    else
    {
        currentPos = nextPos;

    }

    return numActiveNeedle>0;
}





FadeNeedle* MultiNeedle::getMostConsumedNeedle (const int time)
{
    FadeNeedle* res = &needles.getReference (0);

    if (res->isFree (false) || (res->getCurrentPosition() == time && res->consumedSamples == 0 && !res->reverse))return res;

    float minFade = res->fadeOutValue();

    for (int i = 1 ; i < maxNeedles ; i++)
    {
        FadeNeedle* cN = &needles.getReference (i);

        if (cN->isFree (false) || (res->getCurrentPosition() == time && res->consumedSamples == 0 && !res->reverse))return cN;

        float cR = cN->fadeOutValue();

        if (cR < minFade)
        {
            res = cN;
            minFade = cR;
        }
    }

    return res;


}

FadeNeedle* MultiNeedle::getLessConsumedNeedle (const int time)
{
    FadeNeedle* res = &needles.getReference (0);

    if (!res->isFree (false) && (res->getCurrentPosition() == time ))return res;

    float maxFade = res->fadeOutValue();

    for (int i = 1 ; i < maxNeedles ; i++)
    {
        FadeNeedle* cN = &needles.getReference (i);

        if (!cN->isFree (false) && (res->getCurrentPosition() == time))return cN;

        float cR = cN->fadeOutValue();

        if (cR > maxFade)
        {
            res = cN;
            maxFade = cR;
        }
    }

    return res;
}




