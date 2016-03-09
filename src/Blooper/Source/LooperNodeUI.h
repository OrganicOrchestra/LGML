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

class LooperNodeUI: public NodeBaseContentUI , public LooperNode::Looper::Listener{
public:
    LooperNodeUI(){
    }
    
    void init() override{
		
        looperNode = dynamic_cast<LooperNode*> (node);
        trackNumChanged(looperNode->looper->tracks.size());
        looperNode->looper->addListener(this);
    }
    
    class TrackUI : public Component , public LooperNode::Looper::Track::Listener{
    public:
        
        TrackUI(LooperNode::Looper::Track * track):track(track),recPlayButton(track->recPlayTrig){
            track->addListener(this);
            mainColour = Colours::black;
            addAndMakeVisible(recPlayButton);
        }
        void paint(Graphics & g) override{
            g.fillAll(mainColour.withAlpha(0.7f));
        }
        void resized()override{
            Rectangle<int> area = getLocalBounds();
            recPlayButton.setBounds(area.removeFromTop(area.getHeight()/2));
        }
        
        void trackStateChangedAsync(const LooperNode::Looper::Track::TrackState & state)override;
        LooperNode::Looper::Track * track;
        Colour mainColour;
        TriggerBlinkUI recPlayButton;
        
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
