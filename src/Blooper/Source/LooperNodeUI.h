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

class LooperNodeUI: public NodeBaseContentUI , public LooperNode::Looper::Listener
{
public:

    ScopedPointer<TriggerBlinkUI>   recPlaySelectedButton;
    ScopedPointer<TriggerBlinkUI>   clearSelectedButton;
    ScopedPointer<TriggerBlinkUI>   stopSelectedButton;
    ScopedPointer<FloatSliderUI>    volumeSelectedSlider;
    ScopedPointer<TriggerBlinkUI>   clearAllButton;
    ScopedPointer<TriggerBlinkUI>   stopAllButton;
    ScopedPointer<BoolToggleUI> monitoringButton;

    LooperNodeUI();


    Component trackContainer;
    Component headerContainer;
    void init() override;

    class TrackUI :
    public Component ,
    public LooperNode::Looper::Track::Listener,
    public Trigger::Listener
    {
    public:

        TrackUI(LooperNode::Looper::Track * track):track(track),
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
            track->recPlayTrig->addTriggerListener(this);
            track->clearTrig->addTriggerListener(this);
            track->stopTrig->addTriggerListener(this);




        }
		~TrackUI(){
			track->removeTrackListener(this);
		}
		
		
        void paint(Graphics & g) override{
            if(isSelected){
                g.setColour(Colours::white);
                g.drawRect(getLocalBounds());
            }
            g.setColour(mainColour.withAlpha(0.7f));
            g.fillRoundedRectangle(getLocalBounds().
                       removeFromTop((int)(headerHackHeight*getLocalBounds().getHeight()))
                                   .withWidth((int)((1-volumeWidth)*getLocalBounds().getWidth()))
                                   .reduced(1).toFloat(),2);

        }
        void mouseUp(const MouseEvent &) override{track->askForSelection(true);}

        void triggerTriggered(Trigger *)override{track->askForSelection(true);}
        void resized()override{
            // RelativeLayout
            float pad = 0.01f;
            volumeSlider->setBoundsRelative(    1-volumeWidth+pad,             0+pad,
                                            volumeWidth - 2*pad,    1-2*pad);

            recPlayButton.setBoundsRelative(pad,headerHackHeight+pad,
                                           1.f-volumeWidth-2.f*pad, .8f-headerHackHeight-2.f*pad);
            stopButton.setBoundsRelative(0+pad,      .8f+pad,
                                         .4f-2*pad,    .2f-2*pad);
            clearButton.setBoundsRelative(.4f+pad,     .8f+pad,
                                          .4f-2*pad,   .2f-2*pad);
            //            Rectangle<int> area = getLocalBounds();
            //            area.reduce(5,5);
            //            volumeSlider->setBounds(area.removeFromRight(10));
            //            recPlayButton.setBounds(area.removeFromTop(area.getHeight()/2));
            //            stopButton.setBounds(area.removeFromLeft(area.getWidth()/2));
            //            clearButton.setBounds(area);
            //
        }
        void trackSelected(bool _isSelected)override{ isSelected = _isSelected;repaint();}
        void trackStateChangedAsync(const LooperNode::Looper::Track::TrackState & state)override;
        LooperNode::Looper::Track * track;
        Colour mainColour;
        TriggerBlinkUI recPlayButton;
        TriggerBlinkUI clearButton;
        TriggerBlinkUI stopButton;
        float headerHackHeight = .2f;
        float volumeWidth = .2f;
        ScopedPointer<FloatSliderUI> volumeSlider;
        bool isSelected;
    };


    void trackNumChanged(int num) override;
    void resized() override;
    void reLayoutTracks();
    void reLayoutHeader();

    OwnedArray<TrackUI> tracksUI;
    LooperNode * looperNode;

};


#endif  // LOOPERNODEUI_H_INCLUDED
