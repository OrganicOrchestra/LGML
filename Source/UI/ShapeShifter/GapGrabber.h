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


#ifndef GAPGRABBER_H_INCLUDED
#define GAPGRABBER_H_INCLUDED

#include "../../JuceHeaderUI.h"//keep

class GapGrabber : public juce::Component
{
public:
    enum Direction { HORIZONTAL, VERTICAL };

    GapGrabber (Direction _direction);
    virtual ~GapGrabber();

    void paint (Graphics& g) override;
    void mouseEnter (const MouseEvent& e) override;
    void mouseExit (const MouseEvent& e) override;
    void mouseDrag (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) override;

    Direction direction;

    //Listener
    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void grabberGrabUpdate (GapGrabber*, int relativeDist) = 0;
    };

    ListenerList<Listener> listeners;
    void addGrabberListener (Listener* newListener) { listeners.add (newListener); }
    void removeGrabberListener (Listener* listener) { listeners.remove (listener); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GapGrabber)
};

#endif  // GAPGRABBER_H_INCLUDED
