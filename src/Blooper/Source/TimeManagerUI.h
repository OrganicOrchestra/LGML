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

class TimeManagerUI : public Component, public TimeManager::Listener{
    
    public :
    TimeManagerUI(){
        TimeManager::getInstance()->addTimeManagerListener(this);
        addAndMakeVisible(timeBar);
        bpmSlider = TimeManager::getInstance()->BPM->createSlider();
        bpmSlider->displayText = true;
        addAndMakeVisible(bpmSlider);
        
    }
    
    
    void resized()override{
        Rectangle<int> area = getLocalBounds();
        timeBar.setBounds(area.removeFromRight(area.getWidth()/2));
        bpmSlider->setBounds(area.removeFromRight(area.getWidth()/2));
    }
    
    
    void async_isSettingTempo( bool b) override{
        timeBar.isSettingTempo = b;
        timeBar.repaint();
    }
    
    class TimeBar : public Component,public TimeManager::Listener,public Timer{
        public :
        
        bool isSettingTempo = false;
        int refreshHz = 30;
        float blinkHz = 1;
        double blinkCount = 0;
        TimeBar(){
            TimeManager::getInstance()->addTimeManagerListener(this);
        }
        
        void  async_play()override{
            blinkCount = 0;
            startTimerHz(refreshHz);
        }
        void async_stop()override{
            repaint();
            stopTimer();
        }
        void timerCallback()override{
            repaint();
        }
        void paint(Graphics & g) override{
            Rectangle<int> area = getLocalBounds();
            int beatPerBar =TimeManager::getInstance()->beatPerBar;
            int beatWidth = area.getWidth()/beatPerBar;
            
            if(isSettingTempo){
                blinkCount+=blinkHz*1.0/refreshHz;
                if(blinkCount>1){
                    blinkCount-=1;
                }
                
                g.setColour(Colours::red.brighter(1-sin(2.0*double_Pi*blinkCount)));
                g.fillRect(area);
            }
            else{
                int lastBeat =TimeManager::getInstance()->getBeat()%beatPerBar;
                float widthCurrentPart = TimeManager::getInstance()->getBeatPercent();
                g.setColour(Colours::green);
                g.fillRect(area.withWidth(beatWidth*lastBeat));
                g.setColour(Colours::orange);
                g.fillRect(area.withLeft(beatWidth*lastBeat).withWidth(widthCurrentPart*beatWidth));
               
            }
            int beatBarWidth  =4;
            g.setColour(Colours::grey);
            for(int i = 0 ; i < beatPerBar+1  ; i++){
                g.fillRect(area.withLeft(i*beatWidth - beatBarWidth/2).withWidth(beatBarWidth));
            }
        }
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeBar)
        
    };
    
    ScopedPointer<FloatSliderUI>  bpmSlider;
    
    TimeBar timeBar;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeManagerUI)
};


#endif  // TIMEMANAGERUI_H_INCLUDED
