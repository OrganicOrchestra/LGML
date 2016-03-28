/*
 ==============================================================================

 LooperNodeUI.cpp
 Created: 8 Mar 2016 12:01:53pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "LooperNodeUI.h"

LooperNodeUI::LooperNodeUI(){

}


void LooperNodeUI::init(){

    looperNode = dynamic_cast<LooperNode*> (node);
    looperNode->looper->addLooperListener(this);

    recPlaySelectedButton = new TriggerBlinkUI(looperNode->looper->recPlaySelectedTrig);
    clearSelectedButton = new TriggerBlinkUI(looperNode->looper->clearSelectedTrig);
    stopSelectedButton = new TriggerBlinkUI(looperNode->looper->stopSelectedTrig);

    clearAllButton = new TriggerBlinkUI(looperNode->looper->clearAllTrig);
    stopAllButton = new TriggerBlinkUI(looperNode->looper->stopAllTrig);

    volumeSelectedSlider = new FloatSliderUI(looperNode->looper->volumeSelected);

    monitoringButton = new BoolToggleUI(looperNode->looper->isMonitoring);

    headerContainer.addAndMakeVisible(recPlaySelectedButton);
    headerContainer.addAndMakeVisible(clearSelectedButton);
    headerContainer.addAndMakeVisible(stopSelectedButton);
    headerContainer.addAndMakeVisible(clearAllButton);
    headerContainer.addAndMakeVisible(stopAllButton);
    headerContainer.addAndMakeVisible(monitoringButton);


    addAndMakeVisible(headerContainer);
    addAndMakeVisible(trackContainer);

    setSize(300,200);


    trackNumChanged(looperNode->looper->tracks.size());
}

void LooperNodeUI::resized(){
    Rectangle<int> area = getLocalBounds();
    headerContainer.setBounds(area.removeFromTop(50));
    trackContainer.setBounds(area);
    reLayoutHeader();
    reLayoutTracks();

}


void LooperNodeUI::reLayoutHeader(){

    float selectedW = .4f;
    Rectangle<int> area = headerContainer.getBounds();
    int pad = 3;
    Rectangle<int> selTrackArea = area.removeFromLeft((int)(selectedW*area.getWidth()));
    recPlaySelectedButton->setBounds(selTrackArea.removeFromTop((int)(.6f*selTrackArea.getHeight())).reduced(pad));
    stopSelectedButton->setBounds(selTrackArea.removeFromLeft((int)(.5f*selTrackArea.getWidth())).reduced(pad));
    clearSelectedButton->setBounds(selTrackArea.reduced(pad));

    stopAllButton->setBounds(area.removeFromLeft(area.getWidth()/3).reduced(pad));
    clearAllButton->setBounds(area.removeFromLeft(area.getWidth()/2).reduced(pad));
    monitoringButton->setBounds(area.removeFromTop(area.getHeight()/3).reduced(pad));

}
void LooperNodeUI::reLayoutTracks(){
    if(tracksUI.size()){

        int numRow = 2;
        int numCol = tracksUI.size()/numRow ;
        float width = 1.0f/numCol;
        float height =1.0f/numRow;
        float pad = 0.02f;

        for(int j = 0 ; j < numRow ; j++){
            for(int i = 0 ; i < numCol ;i++){
                int idx = j*numCol + i;
                if(idx < tracksUI.size()){
                    tracksUI.getUnchecked(idx)->setBoundsRelative(i*1.0f/numCol + pad, j*1.0f/numRow + pad,
                                                                  width - 2.0f*pad, height - 2.0f*pad);
                }
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
            trackContainer.addAndMakeVisible(t);
        }
    }
    resized();
};



//////////////
// Track UI
////////////////


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
            mainColour = Colours::grey;
            break;

        case LooperNode::Looper::Track::STOPPED:
            mainColour = Colours::black;
            break;

        default:
            jassertfalse;
            break;
    }

    repaint();
}
