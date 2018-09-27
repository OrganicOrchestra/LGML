/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#ifndef AUDIOHELPERS_H_INCLUDED
#define AUDIOHELPERS_H_INCLUDED

#pragma once
#include "../JuceHeaderAudio.h"
#pragma warning( disable : 4244 )

// type to use for clock
// can't be unsigned to allow negative countdowns
typedef int sample_clk_t;
constexpr sample_clk_t MAX_NUMSAMPLES = std::numeric_limits<sample_clk_t>::max();

#define DB0_FOR_01 (0.8f)
#define MIN_DB (-70.0f)

// create a gain value for a float  between 0 and 1
// DB0_FOR_01   -> 0dB
// 1            -> +6dB

inline float float01ToGain (float f)
{
    if (f == 0)return 0;

    float minus6Pos = (1 - 2 * (1 - DB0_FOR_01));

    if (f > minus6Pos) return Decibels::decibelsToGain (jmap<float> (f, DB0_FOR_01, 1.0f, 0.0f, 6.0f));

    return Decibels::decibelsToGain (jmap<float> (f, 0, minus6Pos, MIN_DB, -6.0f));
};

inline float rmsToDB_6dBHR (float rms)
{
    // alows a +6db headroom for rms=1
    return Decibels::gainToDecibels (rms / 0.5f);
}


// helper for handling sample level fading in and out, autoCrossFade relaunches a fadeIn whenFadeOutEnds
class FadeInOut
{
public:
    FadeInOut (int _fadeInSamples, int _fadeOutSamples, bool autoCrossFade = false, double _skew = 1): fadeInNumSamples (_fadeInSamples), fadeOutNumSamples (_fadeOutSamples), crossFade (autoCrossFade), skew (_skew)
    {
        fadeOutCount = 0;
        fadeInCount = -1;
    }

    explicit FadeInOut (int numSample): fadeInNumSamples (numSample), fadeOutNumSamples (numSample), crossFade (false), skew (1.0)
    {

    }

    double getCurrentFade()
    {
        if (fadeInCount >= 0)
        {
            lastFade =  getFade (fadeOutNumSamples - fadeInCount, fadeInNumSamples);
            return lastFade;
        }

        if (fadeOutCount >= 0)
        {
            lastFade =  getFade (fadeOutCount, fadeOutNumSamples);
            return lastFade;
        }

        jassertfalse;
        return 0.0;

    }
    double  getLastFade() const
    {
        return lastFade;
    }
    bool  isMuted() {
        return (getCurrentFade()==0) && getLastFade()==0;
    }
    double  getFade (int cur, int max) const
    {
        if (skew == 1)return cur * 1.0 / max;

        return pow (cur * 1.0 / max, skew);
    }


    void startFadeOut()
    {
        if (fadeOutCount >= 0)return;

        fadeOutCount = (int) (fadeInCount > 0 ? fadeInCount * fadeOutNumSamples * 1.0 / fadeInNumSamples : fadeOutNumSamples);
        fadeInCount = -1;
    }
    void setFadedOut()
    {
        fadeOutCount = 0;
        fadeInCount = -1;
        lastFade = 0.0;
    }

    void setFadedIn()
    {
        fadeInCount = 0;
        fadeOutCount = -1;
        lastFade = 1.0;
    }


    void startFadeIn()
    {
        if (fadeInCount >= 0)return;

        fadeInCount = (int) (fadeOutCount > 0 ? fadeOutCount * fadeInNumSamples * 1.0 / fadeOutNumSamples : fadeInNumSamples);
        fadeOutCount = -1;
    }
    // should be called at each sample to compute resulting fade
    // usually called before getLastFade to ensure that first getCurrentFade calls after setFadedX contains the new value
    void incrementFade (int i = 1)
    {
        if (fadeOutCount > 0)
        {
            fadeOutCount -= i;
            fadeOutCount = jmax (0, fadeOutCount);

            if (crossFade && fadeOutCount <= 0)
            {
                fadeInCount = fadeInNumSamples;
            }
        }
        else if (fadeInCount > 0)
        {
            fadeInCount -= i;
            fadeInCount = jmax (0, fadeInCount);
        }
    }
    bool isFadingIn()const {return fadeInCount >= 0;}
    bool isFadingOut()const {return fadeOutCount >= 0;}

    const int fadeInNumSamples;
    const int fadeOutNumSamples;
    int fadeInCount{}, fadeOutCount{};
    double lastFade{};
    double skew;
    const bool crossFade;

};

// dumb implementation of smoothing
// unconsistent with samplerate / blockSize but simple and safe
template<typename floatType>
class SmoothedValue
{
public:
    SmoothedValue (floatType init, floatType _alpha): current (init), target (init), alpha (_alpha)
    {
        jassert (alpha <= 1);
        jassert (alpha >= 0);
    };
    void update()
    {
        if (target != current)
        {
            current += (target - current) * alpha;
        }
    }
    floatType get() const {return current;}
    void set (float t) {target = t;}
    floatType current;
    floatType target;
    floatType alpha;
};




inline void updateRMS (const AudioBuffer<float>& buffer,
                       float& targetRmsValue,
                       Array<float>& targetRMSChannelValues,
                       int numChannels,
                       bool skipChannelComputation = true)
{
    int numSamples = buffer.getNumSamples();

    if (targetRMSChannelValues.size() != numChannels)
    {
        int oldSize = targetRMSChannelValues.size();
        targetRMSChannelValues.resize (numChannels);

        for (int i = oldSize ; i < numChannels ; i++)
        {
            targetRMSChannelValues.set (i, 0);
        }

    }

    // faster implementation taken from juce Device Settings input meter

    float globalS = 0;

    // @ben we need that (window of 64 sample cannot describe any accurate RMS level alone thus decay factor)
    const double decayFactor = 0.9;
    const float lowThresh = 0.0001f;
    const int downSample = 2;
    const float factor = .9 * downSample * 1.0 / numSamples;

    if (buffer.hasBeenCleared())
    {
        globalS = 0;
    }
    else
    {
        if (skipChannelComputation)
        {
            for (int i = numChannels - 1; i >= 0; --i)
            {
                // this is very intensive so aproximate RMS by max value

                int n = numSamples;
                float s = 0;

                while (n -= downSample)
                    s += fabsf (buffer.getSample (i, n));

                globalS = jmax (globalS, s * factor);

                //      globalS = jmax(globalS,buffer.getMagnitude(i, 0,numSamples)*.7f);

                //      globalS = jmax(globalS,FloatVectorOperations::findMaximum(buffer.getReadPointer(i), numSamples))*.7f;
            }
        }
        else
        {

            for (int i = numChannels - 1; i >= 0; --i)
            {
                int n = numSamples;
                float s = 0;

                while (n -= downSample)
                    s += fabsf (buffer.getSample (i, n));

                s *= factor;
                //      s/=numSamples
                //      float s =  buffer.getMagnitude(i, 0,numSamples)*.7f;
                //      float s = FloatVectorOperations::findMaximum(buffer.getReadPointer(i), numSamples)*.7f;
                targetRMSChannelValues.set (i, (s > targetRMSChannelValues.getReference (i)) ? s :
                                            s > lowThresh ? targetRMSChannelValues.getReference (i) * (float)decayFactor :
                                            0);

                globalS = jmax (s, globalS);
            }
        }

    }

    if (globalS > targetRmsValue)
        targetRmsValue = globalS;
    else if (targetRmsValue > lowThresh)
        targetRmsValue *= (float)decayFactor;
    else
        targetRmsValue = 0;




}




#endif  // AUDIOHELPERS_H_INCLUDED
