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

#pragma once

#include "../../Controllable/Parameter/UI/TriggerBlinkUI.h"
#include "../../Controllable/Parameter/UI/SliderUI.h"
#include "../../Controllable/Parameter/UI/EnumParameterUI.h"
#include "LooperNode.h"
#include "../UI/ConnectableNodeContentUI.h"

class LooperNodeContentUI: public ConnectableNodeContentUI, public LooperNode::LooperListener,ParameterBase::Listener,public FileDragAndDropTarget
{
public:

    std::unique_ptr<ParameterUI>   recPlaySelectedButton;
    std::unique_ptr<ParameterUI>   clearSelectedButton;
    std::unique_ptr<ParameterUI>   stopSelectedButton;
    std::unique_ptr<ParameterUI>    volumeSelectedSlider;
    std::unique_ptr<ParameterUI>   clearAllButton;
    std::unique_ptr<ParameterUI>   stopAllButton;
    std::unique_ptr<ParameterUI>     monitoringButton;


    LooperNodeContentUI();
    ~LooperNodeContentUI();

    Component trackContainer;
    Component headerContainer;
    void init() override;

    class TrackUI :
        public InspectableComponent,
        public LooperTrack::Listener

    {
    public:

        TrackUI (LooperTrack* track);

        ~TrackUI();

        void paint (Graphics& g)override;
        void paintOverChildren (Graphics& g) override;


        void resized()override;
        void mouseUp (const MouseEvent&) override ;
        void trackSelectedAsync (bool _isSelected)override ;

        void trackStateChangedAsync (const LooperTrack::TrackState& /*state*/)override ;

        LooperTrack* track;

        std::unique_ptr<ParameterUI> recPlayButton;
        std::unique_ptr<ParameterUI> clearButton;
        std::unique_ptr<ParameterUI> stopButton;

        std::unique_ptr<ParameterUI> muteButton;
        std::unique_ptr<ParameterUI> soloButton;
        std::unique_ptr<ParameterUI> selectMeButton;
        std::unique_ptr<ParameterUI> sampleChoiceDDL;


        class TimeStateUI : public juce::Component, public LooperTrack::TrackTimeListener
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
        std::unique_ptr<FloatSliderUI> volumeSlider;
        bool isTrackSelected;
    };


    void trackNumChanged (int num) override;
    void resized() override;
    void reLayoutTracks();
    void reLayoutHeader();


    OwnedArray<TrackUI> tracksUI;
    LooperNode* looperNode;

private:
    void newMessage (const ParameterBase::ParamWithValue&) override;
    void checkSoloState();

    bool isInterestedInFileDrag (const StringArray& files) override;
    void fileDragEnter (const StringArray& files, int x, int y) override;
    void fileDragMove (const StringArray& files, int x, int y) override;
    void fileDragExit (const StringArray& files) override;
    void filesDropped (const StringArray& files, int x, int y) override;
    
};



#if NON_INCREMENTAL_COMPILATION
    #include "LooperNodeUI.cpp"
#endif
