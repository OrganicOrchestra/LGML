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


#ifndef LOOPERNODEUI_H_INCLUDED
#define LOOPERNODEUI_H_INCLUDED

#include "../../Controllable/Parameter/UI/TriggerBlinkUI.h"
#include "../../Controllable/Parameter/UI/SliderUI.h"
#include "../../Controllable/Parameter/UI/EnumParameterUI.h"
#include "LooperNode.h"
#include "../UI/ConnectableNodeContentUI.h"

class LooperNodeContentUI: public ConnectableNodeContentUI, public LooperNode::LooperListener
{
public:

    ScopedPointer<ParameterUI>   recPlaySelectedButton;
    ScopedPointer<ParameterUI>   clearSelectedButton;
    ScopedPointer<ParameterUI>   stopSelectedButton;
    ScopedPointer<ParameterUI>    volumeSelectedSlider;
    ScopedPointer<ParameterUI>   clearAllButton;
    ScopedPointer<ParameterUI>   stopAllButton;
    ScopedPointer<ParameterUI>     monitoringButton;


    LooperNodeContentUI();
    ~LooperNodeContentUI();

    Component trackContainer;
    Component headerContainer;
    void init() override;

    class TrackUI :
        public LooperTrack::Listener,
        public InspectableComponent
    {
    public:

        TrackUI (LooperTrack* track);

        ~TrackUI();

        void paint (Graphics& g)override;
        void paintOverChildren (Graphics& g) override;
        void mouseUp (const MouseEvent&) override {track->askForSelection (true);}

        void resized()override;
        void trackSelectedAsync (bool _isSelected)override { isSelected = _isSelected; repaint();}

        void trackStateChangedAsync (const LooperTrack::TrackState& /*state*/)override {};

        LooperTrack* track;

        ScopedPointer<ParameterUI> recPlayButton;
        ScopedPointer<ParameterUI> clearButton;
        ScopedPointer<ParameterUI> stopButton;

        ScopedPointer<ParameterUI> muteButton;
        ScopedPointer<ParameterUI> soloButton;
        ScopedPointer<ParameterUI> selectMeButton;
        ScopedPointer<EnumParameterUI> sampleChoiceDDL;


        class TimeStateUI : public juce::Component, public LooperTrack::Listener
        {
        public:
            TimeStateUI (LooperTrack* _track);
            ~TimeStateUI();
            void paint (Graphics& g)override;
            void trackStateChangedAsync (const LooperTrack::TrackState& state) override;
            void trackTimeChangedAsync (double position)override;
            LooperTrack* track;
            Colour mainColour;
        };

        TimeStateUI timeStateUI;

        float headerHackHeight = .2f;
        float volumeWidth = .2f;
        ScopedPointer<FloatSliderUI> volumeSlider;
        bool isSelected;
    };


    void trackNumChanged (int num) override;
    void resized() override;
    void reLayoutTracks();
    void reLayoutHeader();


    OwnedArray<TrackUI> tracksUI;
    LooperNode* looperNode;

};


#endif  // LOOPERNODEUI_H_INCLUDED
