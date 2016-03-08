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

#include "TriggerButtonUI.h"

class LooperNodeUI: public NodeBaseContentUI , public LooperNode::Looper::Listener{
public:
    LooperNodeUI(){
    }
    
    void setNodeAndNodeUI(NodeBase * node,NodeBaseUI * nodeUI)override{
        looperNode = dynamic_cast<LooperNode*> (node);
        trackNumChanged(looperNode->looper->tracks.size());
        looperNode->looper->addListener(this);
    }
    
    class TrackUI : public Component , public LooperNode::Looper::Track::Listener{
    public:
        
        TrackUI(LooperNode::Looper::Track * track):track(track),recPlayButton(&track->shouldRecordTrig){
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
        
        void trackStateChangedAsync(const LooperNode::Looper::Track::TrackState & state) override{
            switch(state){
                case LooperNode::Looper::Track::RECORDING:
                    mainColour = Colours::red;
                    break;
                    
                case LooperNode::Looper::Track::PLAYING:
                    mainColour = Colours::green;
                    break;
                
                case LooperNode::Looper::Track::SHOULD_RECORD:
                    mainColour = Colours::yellow;
                    break;
                
                case LooperNode::Looper::Track::SHOULD_PLAY:
                    mainColour = Colours::yellow;
                    break;
                
                case LooperNode::Looper::Track::SHOULD_CLEAR:
                case LooperNode::Looper::Track::CLEARED:
                    mainColour = Colours::black;
                    break;
                
                case LooperNode::Looper::Track::STOPPED:
                    mainColour = Colours::grey;
                    break;
                
                default:
                    jassertfalse;
                    break;
            }
            
            if(state == LooperNode::Looper::Track::RECORDING ||
               state == LooperNode::Looper::Track::STOPPED
               )
            {
                recPlayButton.setTriggerReference(&track->shouldPlayTrig);
            }
            else{
                recPlayButton.setTriggerReference(&track->shouldRecordTrig);
            }
            repaint();
        };
        
        LooperNode::Looper::Track * track;
        Colour mainColour;
        TriggerButtonUI recPlayButton;
        
    };
    void trackNumChanged(int num) {
        
        if(num < tracksUI.size()){
            tracksUI.removeRange(num, tracksUI.size() - num);
        }
        else{
            for(int i = tracksUI.size() ; i < num ; i++){
                TrackUI * t = new TrackUI(looperNode->looper->tracks.getUnchecked(i));
                tracksUI.add(t);
                addAndMakeVisible(t);
            }
        }
        
        resized();
    };
    
    
    void resized() override{
        reLayoutTracks();
    }
    void reLayoutTracks(){
        int numRow = 2;
        int numCol = tracksUI.size()/numRow ;
        int width = getLocalBounds().getWidth()/numCol;
        int height = getLocalBounds().getHeight()/numRow;
        int pad = 10;
        
        for(int j = 0 ; j < numRow ; j++){
            for(int i = 0 ; i < numCol ;i++){
                int idx = j*numCol + i;
                if(idx < tracksUI.size()){
                    tracksUI.getUnchecked(idx)->setBounds(i*width+pad,j*height+pad,width-2*pad,height-2*pad);
                }
            }
        }
        
    }
    
    OwnedArray<TrackUI> tracksUI;
    
    LooperNode * looperNode;
    
};


#endif  // LOOPERNODEUI_H_INCLUDED
