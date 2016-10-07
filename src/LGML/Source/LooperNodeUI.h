/*
 ==============================================================================

 LooperNodeUI.h
 Created: 8 Mar 2016 12:01:53pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef LOOPERNODEUI_H_INCLUDED
#define LOOPERNODEUI_H_INCLUDED

#include "TriggerBlinkUI.h"
#include "FloatSliderUI.h"
#include "LooperNode.h"
#include "NodeBaseContentUI.h"

class LooperNodeContentUI: public NodeBaseContentUI , public LooperNode::LooperListener
{
public:

  ScopedPointer<TriggerBlinkUI>   recPlaySelectedButton;
  ScopedPointer<TriggerBlinkUI>   clearSelectedButton;
  ScopedPointer<TriggerBlinkUI>   stopSelectedButton;
  ScopedPointer<FloatSliderUI>    volumeSelectedSlider;
  ScopedPointer<TriggerBlinkUI>   clearAllButton;
  ScopedPointer<TriggerBlinkUI>   stopAllButton;
  ScopedPointer<BoolToggleUI>     monitoringButton;


  LooperNodeContentUI();
  ~LooperNodeContentUI();

  Component trackContainer;
  Component headerContainer;
  void init() override;

  class TrackUI :
  public Component ,
  public LooperTrack::Listener
  {
  public:

    TrackUI(LooperTrack * track);

    ~TrackUI();

    void paint(Graphics & g)override;
    void paintOverChildren(Graphics & g) override;
    void mouseUp(const MouseEvent &) override{track->askForSelection(true);}

    void resized()override;
    void trackSelectedAsync(bool _isSelected)override{ isSelected = _isSelected;repaint();}
    void trackStateChangedAsync(const LooperTrack::TrackState &)override{};
    LooperTrack * track;
    
    ScopedPointer<TriggerBlinkUI> recPlayButton;
    ScopedPointer<TriggerBlinkUI> clearButton;
    ScopedPointer<TriggerBlinkUI> stopButton;

    ScopedPointer<BoolToggleUI> muteButton;
    ScopedPointer<BoolToggleUI> soloButton;

    class TimeStateUI : public Component,public LooperTrack::Listener{
    public:
      TimeStateUI(LooperTrack * _track);
      ~TimeStateUI();
      void paint(Graphics &g)override;
      void trackStateChangedAsync(const LooperTrack::TrackState & state) override;
      void trackTimeChangedAsync(double position)override;
      LooperTrack * track;
      Colour mainColour;
    };

    TimeStateUI timeStateUI;

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
