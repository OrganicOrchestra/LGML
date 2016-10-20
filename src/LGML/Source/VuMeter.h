/*
 ==============================================================================

 VuMeter.h
 Created: 8 Mar 2016 6:28:57pm
 Author:  bkupe

 ==============================================================================
 */

#ifndef VUMETER_H_INCLUDED
#define VUMETER_H_INCLUDED
#pragma once

#include "NodeBase.h"
#include "Style.h"//keep
#include "AudioHelpers.h"

//TODO, move to more common place for use in other components
class VuMeter : public Component, public NodeBase::RMSListener ,public NodeBase::RMSChannelListener,Timer{
public:

  enum Type { IN,OUT};

  int targetChannel;

  float voldB;
  Type type;

  bool volChanged;
  bool isActive;
  Colour colorHigh;
  Colour colorLow;

  VuMeter(Type _type) : type(_type)
  {
    targetChannel = -1;
    setSize(8, 20);
    voldB = 0.f;
    volChanged = true;
    startTimer(1000/40);
    colorHigh = Colours::red;
    colorLow = Colours::lightgreen;
    isActive = true;
  }

  ~VuMeter(){

  }

  void paint(Graphics &g)override {

    Rectangle<int> r = getLocalBounds();
    /*
     if (targetChannel != -1)
     {
     g.setColour(TEXTNAME_COLOR);
     g.drawFittedText(String(targetChannel + 1), r.removeFromBottom(10), Justification::centred, 1);
     r.removeFromBottom(2);
     }
     */

    g.setColour(NORMAL_COLOR);
    g.fillRoundedRectangle(r.toFloat(), 2);
    if (voldB > 0)
    {
      g.setGradientFill(ColourGradient(colorLow, 0.f, r.getHeight()*.5f, colorHigh, 0.f, r.getHeight()*0.1f, false));
      g.fillRoundedRectangle(r.removeFromBottom((int)(r.getHeight()*(voldB))).toFloat(), 2.f);
    }
  }


  void updateValue(float value)
  {
    if(!isActive){setVoldB(0);return;}
    // allow a +6dB headRoom
    float newVoldB = rmsToDB_6dBHR(value);//jmap<float>(20.0f*log10(value / 0.74f), 0.0f, 6.0f, 0.85f, 1.0f);
    newVoldB = jmap<float> (newVoldB,-70.0f,6.0f,0.0f,1.0f);

    if ((newVoldB >= 0 || voldB!=0) && std::abs(newVoldB - voldB)>0.02f) {
      setVoldB(jmax(0.0f,newVoldB));
    }
  }

  void RMSChanged(ConnectableNode *, float rmsIn,float rmsOut) override {

    if (targetChannel > -1) return;

    float rms = (type == Type::IN) ? rmsIn : rmsOut;

    updateValue(rms);
  };

  void channelRMSInChanged(ConnectableNode *, float rmsIn, int channel) override
  {
    if (targetChannel == channel && type == Type::IN) updateValue(rmsIn);
  }
  void channelRMSOutChanged(ConnectableNode *, float rmsOut, int channel) override
  {
    if (targetChannel == channel && type == Type::OUT) updateValue(rmsOut);
  }

  void setVoldB(float value)
  {
    if (voldB == value) return;
    voldB = value;
    volChanged = true;
  }

  void timerCallback()override{
    if (volChanged) repaint();
    volChanged = false;


  }
};



#endif  // VUMETER_H_INCLUDED
