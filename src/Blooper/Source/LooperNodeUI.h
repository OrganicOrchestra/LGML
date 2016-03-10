/*
 ==============================================================================
 
 LooperNodeUI.h
 Created: 8 Mar 2016 12:01:53pm
 Author:  Martin Hermant
 
 ==============================================================================
 */

#ifndef LOOPERNODEUI_H_INCLUDED
#define LOOPERNODEUI_H_INCLUDED

#include "NodeBaseUI.h"

#include "TriggerBlinkUI.h"
#include "FloatSliderUI.h"

class LooperNodeUI: public NodeBaseContentUI , public LooperNode::Looper::Listener{
public:
    LooperNodeUI(){
    }
    
    void init() override{
		
        looperNode = dynamic_cast<LooperNode*> (node);
        trackNumChanged(looperNode->looper->tracks.size());
        looperNode->looper->addListener(this);
        setSize(300,300);
    }
    
    class TrackUI : public Component , public LooperNode::Looper::Track::Listener{
    public:
        
        TrackUI(LooperNode::Looper::Track * track):track(track),
        recPlayButton(track->recPlayTrig),
        clearButton(track->clearTrig),
        stopButton(track->stopTrig)
        {
            track->addListener(this);
            mainColour = Colours::black;
            addAndMakeVisible(recPlayButton);
            addAndMakeVisible(clearButton);
            volumeSlider = track->volume->createSlider();
            volumeSlider->orientation = FloatSliderUI::VERTICAL;
            addAndMakeVisible(volumeSlider);
            
            addAndMakeVisible(stopButton);
        }
        void paint(Graphics & g) override{
            g.fillAll(mainColour.withAlpha(0.7f));
        }
        void resized()override{
            Rectangle<int> area = getLocalBounds();
            volumeSlider->setBounds(area.removeFromRight(10));
            recPlayButton.setBounds(area.removeFromTop(area.getHeight()/2));
            stopButton.setBounds(area.removeFromLeft(area.getWidth()/2));
            clearButton.setBounds(area);
            
        }
        
        void trackStateChangedAsync(const LooperNode::Looper::Track::TrackState & state)override;
        LooperNode::Looper::Track * track;
        Colour mainColour;
        TriggerBlinkUI recPlayButton;
        TriggerBlinkUI clearButton;
        TriggerBlinkUI stopButton;
        ScopedPointer<FloatSliderUI> volumeSlider;
        
    };
    
    
    void trackNumChanged(int num) override;
    
    void resized() override{
        reLayoutTracks();
    }

    void reLayoutTracks();
    
    OwnedArray<TrackUI> tracksUI;
    
    LooperNode * looperNode;
    
};


#endif  // LOOPERNODEUI_H_INCLUDED
