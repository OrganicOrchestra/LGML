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
        bpmSlider->displayBar = false;
        addAndMakeVisible(bpmSlider);
        
    }
    
    
    void resized()override{
        Rectangle<int> area = getLocalBounds();
        timeBar.setBounds(area.removeFromRight(area.getWidth()/2));
        bpmSlider->setBounds(area.removeFromRight(area.getWidth()/2));
    }
    
    
    void async_isSettingTempo( bool b) override{
        timeBar.isSettingTempo = b;
        timeBar.showBeatComponents(!b);
        timeBar.repaint();
    }
    
    class TimeBar : public Component,public TimeManager::Listener,public Timer{
        public :
        
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
            
            void paint(Graphics & g)override{
                Rectangle<int> area = getLocalBounds();
                static int beatBarWidth  =2;
                g.setColour(Colours::grey);
                g.fillRect(area.removeFromLeft(beatBarWidth));
                g.fillRect(area.removeFromRight(beatBarWidth));
                
                
                if(percentDone >= 1){
                    g.setColour(Colours::green);
                    g.fillRect(area);
                }
                else{
                    g.setColour(Colours::orange);
                    g.fillRect(area.removeFromLeft(percentDone*area.getWidth()));
                    g.setColour(Colours::black);
                    g.fillRect(area);
                    
                }
                
                
                
                
            }
        };
        
        OwnedArray<BeatComponent> beatComponents;
        TimeBar(){
            TimeManager::getInstance()->addTimeManagerListener(this);
            initComponentsForNumBeats(TimeManager::getInstance()->beatPerBar);
        }
        void initComponentsForNumBeats(int nb){
            beatComponents.clear();
            int beatPerBar =TimeManager::getInstance()->beatPerBar;
            for(int i = 0 ;i <beatPerBar ; i++){
                BeatComponent * bc=new BeatComponent();
                addAndMakeVisible(bc);
                beatComponents.add(bc);
            }
            resized();
            
        }
        
        void resized() override{
            Rectangle<int> area = getLocalBounds();
            int beatPerBar =beatComponents.size();
            int beatWidth = area.getWidth()/beatPerBar;
            for(int i = 0 ; i < beatPerBar ; i++){beatComponents.getUnchecked(i)->setBounds(area.removeFromLeft(beatWidth));}
        }
        void  async_play()override{
            blinkCount = 0;
            zeroOutBeatComponents();
            startTimerHz(refreshHz);
        }
        void async_stop()override{
            zeroOutBeatComponents();
            repaint();
            stopTimer();
        }
        void async_newBeat( int b)override{
            if(b%beatComponents.size()==0){zeroOutBeatComponents();}
        }
        void async_beatPerBarChanged(int bpb)override{
            initComponentsForNumBeats(bpb);
        }
        
        void zeroOutBeatComponents(){
            for(int i = 0 ; i< beatComponents.size() ; i++){
                BeatComponent * bc = beatComponents.getUnchecked(i);
                bc->percentDone = 0;
                bc->repaint();
                
            }
            
        }
        
        void showBeatComponents(bool show){
            for(int i = 0 ; i< beatComponents.size() ; i++){beatComponents.getUnchecked(i)->setVisible(show);}
        }
        void timerCallback()override{
            if(isSettingTempo){repaint();}
            else{
                int lastBeat =TimeManager::getInstance()->getBeat()%beatComponents.size();
                for(int i = 0 ; i< beatComponents.size() ; i++){
                    BeatComponent * bc = beatComponents.getUnchecked(i);
                    if(i==lastBeat){
                        bc->percentDone = TimeManager::getInstance()->getBeatPercent();
                        bc->repaint();
                    }
                    // ensure old beats are filled
                    else if (i<lastBeat){
                        if(bc->percentDone!=1){
                            bc->percentDone = 1;
                            bc->repaint();
                        }
                    }
                }
            }
        }
        

        void paint(Graphics & g) override{

            if(isSettingTempo){
                // called only if setting tempo
                Rectangle<int> area = getLocalBounds();
                blinkCount+=blinkHz*1.0/refreshHz;
                if(blinkCount>1){
                    blinkCount-=1;
                }
                
                g.setColour(Colours::red.brighter(1-sin(2.0*double_Pi*blinkCount)));
                g.fillRect(area);
            }
        }
        
        
        
        
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeBar)
        
    };
    
    ScopedPointer<FloatSliderUI>  bpmSlider;
    
    TimeBar timeBar;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeManagerUI)
};


#endif  // TIMEMANAGERUI_H_INCLUDED
