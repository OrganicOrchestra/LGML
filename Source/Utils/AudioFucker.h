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

#ifndef AUDIOFUCKER_H_INCLUDED
#define AUDIOFUCKER_H_INCLUDED
#pragma once
#include "../JuceHeaderAudio.h"//keep

class AudioFucker : public AudioIODeviceCallback
{

public:
    AudioFucker (AudioDeviceManager* _manager, double _targetCPU = 100): manager (_manager), targetCPU (_targetCPU)
    {
        iterationCount = 100;
        adaptationSpeed = .7;
    }
    ~AudioFucker()
    {
        manager->removeAudioCallback (this);
        int a = 0;
        a++;
    }

    AudioDeviceManager* manager;
    double targetCPU;
    double adaptationSpeed;
    int iterationCount;
    virtual void audioDeviceIOCallback (const float** /*inputChannelData*/,
                                        int /*numInputChannels*/,
                                        float** /*outputChannelData*/,
                                        int /*numOutputChannels*/,
                                        int /*numSamples*/)
    {

        double currentCPU = manager->getCpuUsage() * 100.0;
        iterationCount += (int) (adaptationSpeed * (targetCPU - currentCPU));
        iterationCount = jmax (0, iterationCount);
        //        DBG(iterationCount);
        sample_clk_t  i = 0 ;

        while (i < iterationCount)
        {
            doStupidThings();
            i++;
        }
    }

    void doStupidThings()
    {
        float j = 1;

        for (int i = 0 ; i < 100 ; i++)
        {
            j *= 2;
            j /= 3;
        }
    }
    virtual void audioDeviceAboutToStart (AudioIODevice* /*device*/) {}

    /** Called to indicate that the device has stopped. */
    virtual void audioDeviceStopped() {}
};





#endif  // AUDIOFUCKER_H_INCLUDED
