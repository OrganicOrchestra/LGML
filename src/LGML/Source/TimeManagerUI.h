/*
 ==============================================================================

 TimeManagerUI.h
 Created: 8 Mar 2016 11:06:41pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef TIMEMANAGERUI_H_INCLUDED
#define TIMEMANAGERUI_H_INCLUDED

#include "TimeManager.h"
#include "FloatStepperUI.h"
#include "TriggerBlinkUI.h"

class TimeManagerUI : public Component,public Parameter::AsyncListener{

    public :
    TimeManagerUI(TimeManager * _timeManager);
    ~TimeManagerUI();

	void paint(Graphics &g) override;
    void resized()override;

    void asyncParameterValueChanged(Parameter* p ,var & v) override;


    class TimeBar : public Component,public Timer{
        public :
        TimeBar(TimeManager *);

        bool isSettingTempo = false;
        int refreshHz = 60;
        float blinkHz = 1;
        double blinkCount = 0;


        // handle only one beat area
        // allowing to redraw only concerned zone
        // should optimize things out but not explicitly validated
        class BeatComponent : public Component{
        public:
            float percentDone=0;
            void paint(Graphics & g)override;
        };

        OwnedArray<BeatComponent> beatComponents;

        void initComponentsForNumBeats(int nb);
        void resized() override;
        void async_play();
        void async_stop();
        void async_newBeat( int b);
        void async_beatPerBarChanged(int bpb);
        void async_isSettingTempo( bool b) ;


        void zeroOutBeatComponents();

        void showBeatComponents(bool show);
        void timerCallback()override;

        void paint(Graphics & g) override;

        TimeManager * timeManager;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeBar);

    };

	ScopedPointer<FloatStepperUI>  bpmStepper;
    ScopedPointer<TriggerBlinkUI> playTrig,stopTrig;


    TimeBar timeBar;
    TimeManager * timeManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeManagerUI);
};


#endif  // TIMEMANAGERUI_H_INCLUDED
