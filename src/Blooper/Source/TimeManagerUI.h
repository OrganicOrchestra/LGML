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
#include "FloatSliderUI.h"

class TimeManagerUI : public Component{

    public :
    TimeManagerUI(TimeManager * _timeManager);


    void resized()override;



    class TimeBar : public Component,public TimeManager::Listener,public Timer{
        public :
        TimeBar(TimeManager *);

        bool isSettingTempo = false;
        int refreshHz = 30;
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
        void  async_play()override;
        void async_stop()override;
        void async_newBeat( int b)override;
        void async_beatPerBarChanged(int bpb)override;
        void async_isSettingTempo( bool b) override;


        void zeroOutBeatComponents();

        void showBeatComponents(bool show);
        void timerCallback()override;

        void paint(Graphics & g) override;

        TimeManager * timeManager;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeBar);

    };

    ScopedPointer<FloatSliderUI>  bpmSlider;

    TimeBar timeBar;
    TimeManager * timeManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeManagerUI);
};


#endif  // TIMEMANAGERUI_H_INCLUDED
