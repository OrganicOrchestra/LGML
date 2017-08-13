/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#include "DummyNode.h"
#include "NodeBaseUI.h"
#include "DummyNodeContentUI.h"
#include "TimeManager.h"

Identifier lrId("Left / Right");
Identifier rlId("Right / Left");
Identifier mixedId("Mixed");
Identifier clickId("Click");
Identifier sineId("Sine");


DummyNode::DummyNode() :
NodeBase("DummyNode",NodeType::DummyType),
clickFade(300,300)
{

  clickFade.setFadedOut();
  freq1Param = addNewParameter<FloatParameter>("Freq 1", "This is a test int slider",.23f);
  freq2Param = addNewParameter<FloatParameter>("Freq 2", "This is a test int slider", .55f);

  testTrigger =  addNewParameter<Trigger>("Test Trigger", "Youpi");

  enumParam = addNewParameter<EnumParameter>("Mode", "Enum Mode test");
  enumParam->addOption(lrId);
  enumParam->addOption(rlId);
  enumParam->addOption(mixedId);
  enumParam->addOption(clickId);
  enumParam->addOption(sineId);



  //DATA
  addInputData("IN Number", DataType::Number);
  addInputData("IN Orientation", DataType::Orientation);

  addOutputData("OUT Number", DataType::Number);
  outPosData = addOutputData("OUT Position", DataType::Position);

  pxParam = addNewParameter<FloatParameter>("PointX", "X", 0.f, 0.f, 1.f);
  pyParam = addNewParameter<FloatParameter>("PointY", "Y", 0.f, 0.f, 1.f);

  //AUDIO
  setPlayConfigDetails(2, 3, getSampleRate(), getBlockSize());


}

DummyNode::~DummyNode()
{
}

void DummyNode::onContainerParameterChanged(Parameter * p)
{
  NodeBase::onContainerParameterChanged(p);
  if (p == freq1Param)
  {
    //       ((DummyAudioProcessor*)audioProcessor)->amp = p->getNormalizedValue();
    period1 = (int)(44100.0f / (1.0f + 440.0f*p->getNormalizedValue()));
  }
  else if (p == freq2Param)
  {
    period2 = (int)(44100.0f / (1.0f + 440.0f*p->getNormalizedValue()));
	 } else if (p == enumParam)
   {
     //DBG("Enum param changed : " << enumParam->stringValue() << " / " << enumParam->getFirstSelected().toString());
   } else if (p == pxParam || p == pyParam)
   {
     outPosData->update(pxParam->floatValue(), pyParam->floatValue());
   }
}





void DummyNode::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer &) {
  Identifier outType = enumParam->getFirstSelectedId();
  if(outType==clickId ||  outType==sineId){
    TimeManager * tm = TimeManager::getInstance();
    if(tm->isJumping()){

    }
    if(tm->isPlaying()){
      clickFade.startFadeIn();
      int numSamples = buffer.getNumSamples();
      int numOutputChannels = buffer.getNumChannels();
      static sample_clk_t sinCount = 0;

      bool isFirstBeat = (tm->getClosestBeat()%tm->beatPerBar->intValue()) == 0;
      const int sinFreq = isFirstBeat?period1:period2;
      //    const int sinPeriod = sampleRate / sinFreq;
      const double k = 40.0;

      bool ADSREnv =  !( outType==sineId);
      for(int i = 0 ; i < numSamples;i++){

        double carg = sinCount*1.0/sinFreq;
        double env = 1.0;
        if(ADSREnv){
          double x = (tm->getBeatInNextSamples(i)-tm->getBeatInt() ) ;
          double h = k*fmod((double)x+1.0/k,1.0);
          //        double env = jmax(0.0,1.0 - x*4.0);
          env = jmax(0.0,h*exp(1.0-h));
        }
        clickFade.incrementFade();
        double fade = clickFade.getCurrentFade();
        float res = (float)(fade*(env* cos(2.0*float_Pi*carg)));

        for(int c = 0 ;c < numOutputChannels ; c++ ){buffer.setSample(c, i, res);}

        sinCount = (sinCount+1)%(sinFreq);


      }
    }
    else{
      clickFade.startFadeOut();
    }
  }
  else{

    if (outType == lrId)
    {
      for (int i = 0; i < buffer.getNumSamples(); i++) {
        buffer.addSample(0, i, (float)(amp*cos(2.0*double_Pi*step1*1.0 / period1)));
        buffer.addSample(1, i, (float)(amp*cos(2.0*double_Pi*step2*1.0 / period2)));
        step1++;
        step2++;
        if (step1>period1) { step1 = 0; }
        if (step2>period2) { step2 = 0; }
      }
    } else if (outType == rlId)
    {
      for (int i = 0; i < buffer.getNumSamples(); i++) {
        buffer.addSample(1, i, (float)(amp*cos(2.0*double_Pi*step1*1.0 / period1)));
        buffer.addSample(0, i, (float)(amp*cos(2.0*double_Pi*step2*1.0 / period2)));
        step1++;
        step2++;
        if (step1>period1) { step1 = 0; }
        if (step2>period2) { step2 = 0; }
      }
    } else if (outType == mixedId)
    {
      for (int i = 0; i < buffer.getNumSamples(); i++) {
        buffer.addSample(0, i, (float)(amp/2*cos(2.0*double_Pi*step1*1.0 / period1)));
        buffer.addSample(0, i, (float)(amp/2*cos(2.0*double_Pi*step2*1.0 / period2)));
        buffer.addSample(1, i, (float)(amp/2*cos(2.0*double_Pi*step1*1.0 / period1)));
        buffer.addSample(1, i, (float)(amp/2*cos(2.0*double_Pi*step2*1.0 / period2)));
        step1++;
        step2++;
        if (step1>period1) { step1 = 0; }
        if (step2>period2) { step2 = 0; }
      }
    }

  }

}




inline void DummyNode::processInputDataChanged(Data * d)
{
	 //DBG("DummyNode :: Input data changed " << d->name);

	 if (d->name == "IN Number")
   {
     amp = d->getElement("value")->value;
   }
}

ConnectableNodeUI * DummyNode::createUI()
{

  NodeBaseUI * ui = new NodeBaseUI(this,new DummyNodeContentUI());
  return ui;
  
}
