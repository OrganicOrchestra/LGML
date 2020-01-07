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

#pragma once

#include "../Audio/AudioHelpers.h" // for sample_clk_t....
 class TimeManagerListener
    {
    public:
        virtual ~TimeManagerListener() {}
        virtual void BPMChanged (double /*BPM*/) {}
        virtual void timeJumped (sample_clk_t /*time*/) {}
        virtual void playStop (bool /*playStop*/) {}
        // info for stopping manager if needed;
        virtual bool isBoundToTime() = 0;



    };


