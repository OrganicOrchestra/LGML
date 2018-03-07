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

#ifndef VUMETER_H_INCLUDED
#define VUMETER_H_INCLUDED
#pragma once

#include "../Node/NodeBase.h"
#include "../Audio/AudioHelpers.h"

//TODO, move to more common place for use in other components
class VuMeter : public juce::Component, public NodeBase::RMSListener, public NodeBase::RMSChannelListener, Timer
{
public:

    enum Type { IN, OUT};

    int targetChannel;

    float voldB;
    Type type;

    bool volChanged;
    bool isActive;
    Colour colorHigh;
    Colour colorLow;

    VuMeter (Type _type) : type (_type)
    {
        targetChannel = -1;
        setSize (8, 20);
        voldB = 0.f;
        volChanged = true;
        startTimer (1000 / 30);
        colorHigh = Colours::red;
        colorLow = Colours::lightgreen;
        isActive = true;
        setOpaque (true);
        setInterceptsMouseClicks(false, false);
    }


    ~VuMeter()
    {

    }

    void paint (Graphics& g)override
    {

        Rectangle<int> r = getLocalBounds();


        g.setColour (findColour (TextButton::ColourIds::buttonColourId));
        //    g.fillRoundedRectangle(r.toFloat(), 2);
        g.fillRect (r.toFloat());

        if (voldB > 0)
        {
            g.setGradientFill (ColourGradient (colorLow, 0.f, r.getHeight()*.5f, colorHigh, 0.f, r.getHeight() * 0.1f, false));
            //      g.fillRoundedRectangle(r.removeFromBottom((int)(r.getHeight()*(voldB))).toFloat(), 2.f);
            g.fillRect (r.removeFromBottom ((int) (r.getHeight() * (voldB))).toFloat());
        }
    }


    void updateValue (float value)
    {
        if (!isActive) {setVoldB (0); return;}

        // allow a +6dB headRoom
        float newVoldB = rmsToDB_6dBHR (value); //jmap<float>(20.0f*log10(value / 0.74f), 0.0f, 6.0f, 0.85f, 1.0f);
        newVoldB = jmap<float> (newVoldB, -70.0f, 6.0f, 0.0f, 1.0f);

        if ((newVoldB >= 0 || voldB != 0) && std::abs (newVoldB - voldB) > 0.02f)
        {
            setVoldB (jmax (0.0f, newVoldB));
        }
    }

    void RMSChanged ( float rmsIn, float rmsOut) override
    {
        if (targetChannel > -1) return;

        float rms = (type == Type::IN) ? rmsIn : rmsOut;
        updateValue (rms);
    };

    void channelRMSInChanged (ConnectableNode*, float rms, int channel) override
    {
        if (targetChannel == channel && type == Type::IN) updateValue (rms);
    }
    void channelRMSOutChanged (ConnectableNode*, float rms, int channel) override
    {
        if (targetChannel == channel && type == Type::OUT) updateValue (rms);
    }

    void setVoldB (float value)
    {
        if (voldB == value) return;

        voldB = value;
        volChanged = true;
    }

    void timerCallback()override
    {
        if (volChanged) repaint();

        volChanged = false;


    }
};



#endif  // VUMETER_H_INCLUDED
