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

#ifndef TIMEMANAGERUI_H_INCLUDED
#define TIMEMANAGERUI_H_INCLUDED



#include "TimeManager.h"//keep
#include "../Controllable/Parameter/UI/SliderUI.h"
#include "../Controllable/Parameter/UI/TriggerBlinkUI.h"
#include "../Controllable/Parameter/UI/BoolToggleUI.h"
#include "../Controllable/Parameter/UI/StepperUI.h"
#include "../UI/ShapeShifter/ShapeShifterContent.h"


class TimeManagerUI :
    public InspectableComponent,
    public ShapeShifterContent,
    public ParameterBase::Listener

{

public :
    TimeManagerUI (const String& contentName, TimeManager* _timeManager);
    ~TimeManagerUI();

    
    void resized()override;

    void newMessage (const ParameterBase::ParamWithValue& pv) override;


    class TimeBar : public juce::Component, private Timer
    {
    public :
        TimeBar (TimeManager*);

        bool isSettingTempo = false;
        TimeManager* timeManager;
        int refreshHz ;
        float blinkHz;
        double blinkCount = 0;


        // handle only one beat area
        // allowing to redraw only concerned zone
        // should optimize things out but not explicitly validated
        class BeatComponent : public juce::Component
        {
        public:
            BeatComponent();
            float percentDone = 0;
            void paint (Graphics& g)override;
        };

        OwnedArray<BeatComponent> beatComponents;

        void initComponentsForNumBeats (int nb);
        void resized() override;
        void async_play();
        void async_stop();
        void async_newBeat ( int b);
        void async_beatPerBarChanged (int bpb);



        void zeroOutBeatComponents();

        void showBeatComponents (bool show);
        void timerCallback()override;

        void paint (Graphics& g) override;




        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeBar)

    };

    std::unique_ptr<ParameterUI>  bpmStepper;
    std::unique_ptr<ParameterUI> quantizStepper;
    std::unique_ptr<ParameterUI> playTrig, stopTrig, tapTempo;
    std::unique_ptr<ParameterUI> click;
    std::unique_ptr<ParameterUI> clickVolumeUI;


    std::unique_ptr<ParameterUI> linkEnabled;
    std::unique_ptr<ParameterUI> linkNumPeers;



    TimeBar timeBar;
    TimeManager* timeManager;

    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeManagerUI)
};


#endif  // TIMEMANAGERUI_H_INCLUDED
