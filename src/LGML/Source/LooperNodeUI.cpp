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
        int numCol = (tracksUI.size()+1)/numRow ;
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


LooperNodeUI::TrackUI::TrackUI(LooperTrack * track) :track(track),
recPlayButton(track->recPlayTrig),
clearButton(track->clearTrig),
stopButton(track->stopTrig),
isSelected(false)
{
	track->addTrackListener(this);
	trackStateChangedAsync(track->trackState);
	addAndMakeVisible(recPlayButton);
	addAndMakeVisible(clearButton);
	volumeSlider = track->volume->createSlider();
	volumeSlider->orientation = FloatSliderUI::VERTICAL;
	//            volumeSlider->displayText=false;
	addAndMakeVisible(volumeSlider);
	addAndMakeVisible(stopButton);

	//select on trigger activations
	// track->recPlayTrig->addTriggerListener(this);
	// track->clearTrig->addTriggerListener(this);
	// track->stopTrig->addTriggerListener(this);
}

LooperNodeUI::TrackUI::~TrackUI() {
	track->removeTrackListener(this);
}

void LooperNodeUI::TrackUI::paint(Graphics & g) {
	if (isSelected) {
		g.setColour(Colours::white);
		g.drawRect(getLocalBounds());
	}
	g.setColour(mainColour.withAlpha(0.7f));
	g.fillRoundedRectangle(getLocalBounds().
		removeFromTop((int)(headerHackHeight*getLocalBounds().getHeight()))
		.withWidth((int)((1 - volumeWidth)*getLocalBounds().getWidth()))
		.reduced(1).toFloat(), 2);

}

void LooperNodeUI::TrackUI::resized() {
	// RelativeLayout
	float pad = 0.01f;
	volumeSlider->setBoundsRelative(1 - volumeWidth + pad, 0 + pad,
		volumeWidth - 2 * pad, 1 - 2 * pad);

	recPlayButton.setBoundsRelative(pad, headerHackHeight + pad,
		1.f - volumeWidth - 2.f*pad, .8f - headerHackHeight - 2.f*pad);
	stopButton.setBoundsRelative(0 + pad, .8f + pad,
		.4f - 2 * pad, .2f - 2 * pad);
	clearButton.setBoundsRelative(.4f + pad, .8f + pad,
		.4f - 2 * pad, .2f - 2 * pad);
	//            Rectangle<int> area = getLocalBounds();
	//            area.reduce(5,5);
	//            volumeSlider->setBounds(area.removeFromRight(10));
	//            recPlayButton.setBounds(area.removeFromTop(area.getHeight()/2));
	//            stopButton.setBounds(area.removeFromLeft(area.getWidth()/2));
	//            clearButton.setBounds(area);
	//
}

void LooperNodeUI::TrackUI::trackStateChangedAsync(const LooperTrack::TrackState & state) {
    switch(state){
		case LooperTrack::TrackState::RECORDING:
            mainColour = Colours::red;
            break;

        case LooperTrack::TrackState::PLAYING:
            mainColour = Colours::green;
            break;

        case LooperTrack::TrackState::SHOULD_RECORD:
            mainColour = Colours::yellow;
            break;

        case LooperTrack::TrackState::SHOULD_PLAY:
            mainColour = Colours::yellow;
            break;

        case LooperTrack::TrackState::SHOULD_CLEAR:
        case LooperTrack::TrackState::CLEARED:
            mainColour = Colours::grey;
            break;

        case LooperTrack::TrackState::STOPPED:
            mainColour = Colours::black;
            break;

        default:
            jassertfalse;
            break;
    }

    repaint();
}
