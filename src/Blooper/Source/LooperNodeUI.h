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

class LooperNodeUI: public NodeBaseUI , public LooperNode::Looper::Listener{
public:
    LooperNodeUI(LooperNode * looperNode):NodeBaseUI(looperNode),looperNode(looperNode){
        trackNumChanged(looperNode->looper->tracks.size());
        looperNode->looper->addListener(this);
    }
    
    void paint(Graphics & g) override{
        g.setColour(Colours::pink);
        g.fillRect(getContentContainer()->getLocalBounds());
    }
    class TrackUI : public Component{
    public:
        
        TrackUI(LooperNode::Looper::Track * track):recButton(&track->shouldRecordTrig){
            addAndMakeVisible(recButton);
        }
        void paint(Graphics & g) override{
            g.fillAll(Colours::red.withAlpha(0.2f));
        }
        void resized()override{
            Rectangle<int> area = getLocalBounds();
            recButton.setBounds(area.removeFromTop(area.getHeight()/2));
        }
        
        TriggerButtonUI recButton;
        
    };
    void trackNumChanged(int num) {
        
        if(num < tracksUI.size()){
            tracksUI.removeRange(num, tracksUI.size() - num);
        }
        else{
            for(int i = tracksUI.size() ; i < num ; i++){
                TrackUI * t = new TrackUI(looperNode->looper->tracks.getUnchecked(i));
                tracksUI.add(t);
                getContentContainer()->addAndMakeVisible(t);
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
        int width = getContentContainer()->getLocalBounds().getWidth()/numCol;
        int height = getContentContainer()->getLocalBounds().getHeight()/numRow;
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
