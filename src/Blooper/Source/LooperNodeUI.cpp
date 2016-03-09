/*
  ==============================================================================

    LooperNodeUI.cpp
    Created: 8 Mar 2016 12:01:53pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "LooperNodeUI.h"

void LooperNodeUI::TrackUI::trackStateChangedAsync(const LooperNode::Looper::Track::TrackState & state) {
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

    repaint();
}




void LooperNodeUI::reLayoutTracks(){
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

void LooperNodeUI::trackNumChanged(int num) {
    
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