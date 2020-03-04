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

#ifndef MULTINEEDLE_H_INCLUDED
#define MULTINEEDLE_H_INCLUDED

#include "AudioConfig.h"
#include "../JuceHeaderAudio.h"
#include "BufferBlockList.h"



class FadeNeedle
{
public:
    FadeNeedle();
    void set (const int& c, const int fIn, const int fOut, const int sus, bool _reverse = false);


    void reset();
    
    int startNeedle=0;
    int startFadeOutCount=0;
    bool isFadingOut=false;
    float lastFade=0, currentFade=0;
    int consumedSamples = 0;
    int fadeInNumSamples=0, sustainNumSamples=0;
    int fadeOutNumSamples=0;
    bool hasBeenSet;
    int num=-1;
    float maxFadeOutValue =1;
    bool reverse =false;

    float fadeOutValue();
    int getCurrentPosition();

    bool isFree (bool allowNotStarted = false);

    void consume (const int block);

    void setMaxLength (int max);
    void startFadeOut (bool force = true);

    float getFadeValueStart();
    float getFadeValueEnd();

private:
    inline float computeCurrentFade();
    int getStartFadeOut() const;
    int getEndFadeOut()const;


};



class MultiNeedle
{
public:

    Array<FadeNeedle> needles;

    int fadeInNumSamples ;
    int fadeOutNumSamples;
    int maxNeedles;

    int currentPos=0;
    bool isJumping = false;
    int needleIdx = 0;
    int loopSize=0;
    int numActiveNeedle = 0;
    bool isStitching = false;
    
    CriticalSection readMutex;

    MultiNeedle (int fIn = 512, int fOut = 512, int maxNeedles = 10);

    void setLoopSize (int _loopSize);
    void jumpTo (const int to);
    FadeNeedle*   consumeNextNeedle (int numSamples);
    void fadeAllOut();
    void resetAll();
    bool addToBuffer ( BufferBlockList& originBuffer, AudioBuffer<float>& destBuffer,int destStartSample, int numSamples, bool isLooping);



private:

    FadeNeedle* getMostConsumedNeedle (const int time);
    FadeNeedle* getLessConsumedNeedle (const int time);



};

#endif  // MULTINEEDLE_H_INCLUDED
