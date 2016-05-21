/*
 ==============================================================================

 LooperNodeUI.cpp
 Created: 8 Mar 2016 12:01:53pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "LooperNodeUI.h"

LooperNodeContentUI::LooperNodeContentUI(){

}

LooperNodeContentUI::~LooperNodeContentUI(){
    looperNode->removeLooperListener(this);
}

void LooperNodeContentUI::init(){

    looperNode = dynamic_cast<LooperNode*> (node);
    looperNode->addLooperListener(this);

    recPlaySelectedButton = looperNode->recPlaySelectedTrig->createBlinkUI();
    clearSelectedButton = looperNode->clearSelectedTrig->createBlinkUI();
    stopSelectedButton = looperNode->stopSelectedTrig->createBlinkUI();

    clearAllButton = looperNode->clearAllTrig->createBlinkUI();
    stopAllButton = looperNode->stopAllTrig->createBlinkUI();

    volumeSelectedSlider = looperNode->volumeSelected->createSlider();
    monitoringButton = looperNode->isMonitoring->createToggle();

    headerContainer.addAndMakeVisible(recPlaySelectedButton);
    headerContainer.addAndMakeVisible(clearSelectedButton);
    headerContainer.addAndMakeVisible(stopSelectedButton);
    headerContainer.addAndMakeVisible(clearAllButton);
    headerContainer.addAndMakeVisible(stopAllButton);
    headerContainer.addAndMakeVisible(monitoringButton);


    addAndMakeVisible(headerContainer);
    addAndMakeVisible(trackContainer);

    setSize(650,180);


    trackNumChanged(looperNode->trackGroup.tracks.size());
    looperNode->trackGroup.tracks.getUnchecked(looperNode->selectTrack->intValue())->setSelected(true);
}

void LooperNodeContentUI::resized(){
    Rectangle<int> area = getLocalBounds();
    headerContainer.setBounds(area.removeFromTop(50));
    trackContainer.setBounds(area);
    reLayoutHeader();
    reLayoutTracks();

}


void LooperNodeContentUI::reLayoutHeader(){

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
void LooperNodeContentUI::reLayoutTracks(){
    if (tracksUI.size() == 0) return;

    int numCol = 8;
    int numRow = (int)ceil((tracksUI.size())*1.f / numCol);

    float trackWidth =  40;// trackContainer.getWidth() / numCol;
    float trackHeight = 130;// 1.0f / numRow;
    float gap = 5;
    float margin = 5;

    float targetWidth = (trackWidth + gap)*numCol - gap + margin*2;
    float targetHeight = (trackHeight + gap) * numRow - gap+margin*2;

    if (trackContainer.getWidth() != targetWidth || trackContainer.getHeight() != targetHeight)
    {
        trackContainer.setSize((int)targetWidth, (int)targetHeight);
        setSize(trackContainer.getBounds().getBottomRight().x, trackContainer.getBounds().getBottomRight().y);
        return;
    }

    Rectangle<int> r = trackContainer.getLocalBounds().reduced((int)margin);

    int trackIndex = 0;
    for(int j = 0 ; j < numRow ; j++){

        Rectangle<int> rowRect = r.removeFromTop((int)trackHeight);

        for(int i = 0 ; i < numCol ;i++){

            if (trackIndex >= tracksUI.size()) break;
            tracksUI.getUnchecked(trackIndex)->setBounds(rowRect.removeFromLeft((int)trackWidth));
            rowRect.removeFromLeft((int)gap);
            trackIndex++;

        }

        if (j < numRow - 1) r.removeFromTop((int)gap);

    }
}

void LooperNodeContentUI::trackNumChanged(int num) {

    if(num < tracksUI.size()){
        tracksUI.removeRange(num, tracksUI.size() - num);
    }
    else{
        for(int i = tracksUI.size() ; i < num ; i++){
            TrackUI * t = new TrackUI(looperNode->trackGroup.tracks.getUnchecked(i));
            tracksUI.add(t);
            trackContainer.addAndMakeVisible(t);
        }
    }
    resized();
};



//////////////
// Track UI
////////////////


LooperNodeContentUI::TrackUI::TrackUI(LooperTrack * track) :track(track),
isSelected(false)
{
    recPlayButton = track->recPlayTrig->createBlinkUI();
    clearButton = track->clearTrig->createBlinkUI();
    stopButton = track->stopTrig->createBlinkUI();
    muteButton = track->mute->createToggle();
    muteButton->invertVisuals = true;
    soloButton = track->solo->createToggle();

    track->addTrackListener(this);
    trackStateChangedAsync(track->trackState);
    addAndMakeVisible(recPlayButton);
    addAndMakeVisible(clearButton);
    volumeSlider = track->volume->createSlider();
    volumeSlider->orientation = FloatSliderUI::VERTICAL;
    addAndMakeVisible(volumeSlider);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(muteButton);
    addAndMakeVisible(soloButton);

}

LooperNodeContentUI::TrackUI::~TrackUI() {
    track->removeTrackListener(this);
}

void LooperNodeContentUI::TrackUI::paint(Graphics & g) {

    g.setColour(PANEL_COLOR.brighter());
    g.fillRoundedRectangle(getLocalBounds().toFloat(),2.f);

    g.setColour(mainColour);
    g.fillRoundedRectangle(5, 5, 10, 10, 2);

    g.setColour(Colours::white);
    g.setFont(12);
    g.drawText("#" + String(track->trackIdx), getLocalBounds().withHeight(20).reduced(2) , Justification::right);

    if (isSelected) {
        g.setColour(Colours::yellow);
        g.drawRoundedRectangle(getLocalBounds().reduced(1).toFloat(), 2.f, 1.f);
    }
}

void LooperNodeContentUI::TrackUI::resized() {
    Rectangle<int> r = getLocalBounds().reduced(2);
    r.removeFromTop(25);//header
    int gap = 5;
    int step = r.getHeight()/6 - gap;
    volumeSlider->setBounds(r.removeFromRight(step));
    r.reduce(5,0);
    muteButton->setBounds(r.removeFromTop(step));
    r.removeFromTop(gap);
    soloButton->setBounds(r.removeFromTop(step));
    r.removeFromTop(gap);
    clearButton->setBounds(r.removeFromTop(step));
    r.removeFromTop(gap);
    stopButton->setBounds(r.removeFromTop(step));
    r.removeFromTop(gap);
    recPlayButton->setBounds(r);
    r.removeFromTop(gap);

}

void LooperNodeContentUI::TrackUI::trackStateChangedAsync(const LooperTrack::TrackState & state) {
    switch(state){
        case LooperTrack::TrackState::RECORDING:
            mainColour = Colours::red;
            break;

        case LooperTrack::TrackState::PLAYING:
            mainColour = Colours::green.brighter(.3f);
            break;

        case LooperTrack::TrackState::SHOULD_RECORD:
            mainColour = Colours::orange;
            break;

        case LooperTrack::TrackState::SHOULD_PLAY:
            mainColour = Colours::cadetblue;
            break;

        case LooperTrack::TrackState::SHOULD_CLEAR:
        case LooperTrack::TrackState::CLEARED:
            mainColour = Colours::grey;
            break;

        case LooperTrack::TrackState::STOPPED:
        case LooperTrack::TrackState::SHOULD_STOP:
            mainColour = Colours::grey.darker();
            break;

        default:
            jassertfalse;
            break;
    }

    repaint();
}
