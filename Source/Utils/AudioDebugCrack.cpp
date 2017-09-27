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

#include "AudioDebugCrack.h"
#if !FORCE_DISABLE_CRACK
Array<AudioDebugCrack*> AudioDebugCrack::allAudioDebugCracks;



AudioDebugCrack::AudioDebugCrack (const String& n): name (n), hasCrack (false), lastCrack (0), sampleCount (0)
{
    allAudioDebugCracks.add (this);
}

AudioDebugCrack* AudioDebugCrack::getOrCreateDetector (const juce::String& name)
{
    for (auto& c : allAudioDebugCracks)
    {
        if (c->name == name)return c;
    }

    return new AudioDebugCrack (name);
}


void AudioDebugCrack::processBuffer (const AudioBuffer<float>& b)
{

    for (int i = 0 ; i < b.getNumSamples() ; i++)
    {
        float curValue = 0;

        for (int c = 0 ; c < b.getNumChannels() ; c++)
        {
            curValue += b.getSample (c, i);
        }

        if (std::abs (curValue - lastValue) > threshold)
        {
            notifyCrack (b, i);

        }

        float derivative = std::abs (curValue - lastValue);

        if (std::abs (derivative - lastDerivative) > derivativeThreshold)
        {
            notifyCrack (b, i);
        }


        lastValue = curValue;
        const float alpha = 0.3f;
        lastDerivative += alpha * (derivative - lastDerivative);
        sampleCount++;

        if (sampleCount - lastCrack > debounceSample)
        {
            hasCrack = false;
        }
    }
}
void AudioDebugCrack::notifyCrack (const AudioBuffer<float>& /*b*/, int /*i*/)
{
    //    jassertfalse;

    if (sampleCount - lastCrack > debounceSample)
    {
        if (!hasCrack)
        {
            lastCrack = sampleCount;
        }

        hasCrack = true;

        //    DBG("crack for " <<name<<" at " << i);
        int dbg = 0;
        dbg++;
    }
}


void AudioDebugCrack::deleteInstanciated()
{
    for (auto& c : allAudioDebugCracks)
    {
        delete c;
    }

    allAudioDebugCracks.clear();
}
#endif
