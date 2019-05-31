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

#if JUCE_DEBUG

#include "../../Time/TimeManager.h"

Identifier lrId ("Left / Right");
Identifier rlId ("Right / Left");
Identifier mixedId ("Mixed");
Identifier clickId ("Click");
Identifier sineId ("Sine");

REGISTER_NODE_TYPE (DummyNode)
DummyNode::DummyNode (StringRef name) :
    NodeBase (name),
    clickFade (300, 300)
{

    clickFade.setFadedOut();
    freq1Param = addNewParameter<FloatParameter> ("Freq 1", "This is a test int slider", .23f,20.f,4000.f);
    freq2Param = addNewParameter<FloatParameter> ("Freq 2", "This is a test int slider", .55f);

    testTrigger =  addNewParameter<Trigger> ("Test Trigger", "Youpi");

    enumParam = addNewParameter<EnumParameter> ("Mode", "Enum Mode test");
    enumParam->addOption (lrId,true);
    enumParam->addOption (rlId,true);
    enumParam->addOption (mixedId,true);
    enumParam->addOption (clickId,true);
    enumParam->addOption (sineId,true);

    

    pxParam = addNewParameter<FloatParameter> ("Point X", "X", 0.f, 0.f, 1.f);
    pyParam = addNewParameter<FloatParameter> ("Point Y", "Y", 0.f, 0.f, 1.f);

    //AUDIO
    setPlayConfigDetails (2, 3, getSampleRate(), getBlockSize());

    Thread::sleep(2000);
}

DummyNode::~DummyNode()
{
}

void DummyNode::onContainerParameterChanged ( ParameterBase* p)
{
    NodeBase::onContainerParameterChanged (p);

    if (p == freq1Param)
    {
        //       ((DummyAudioProcessor*)audioProcessor)->amp = p->getNormalizedValue();
        period1 = (int) (44100.0f / ( freq1Param->floatValue()));
    }
    else if (p == freq2Param)
    {
        period2 = (int) (44100.0f / (1.0f + 440.0f * freq2Param->getNormalizedValue()));
    }
    else if (p == enumParam)
    {
        //DBG("Enum param changed : " << enumParam->stringValue() << " / " << enumParam->getFirstSelected().toString());
    }

}

void DummyNode::prepareToPlay(double sr,int bs) {
    Thread::sleep(4000);
}



void DummyNode::processBlockInternal (AudioBuffer<float>& buffer, MidiBuffer&)
{
    Identifier outType = enumParam->getFirstSelectedId();

    if (outType == clickId ||  outType == sineId)
    {
        TimeManager* tm = TimeManager::getInstance();

        if (tm->isJumping())
        {

        }

        if (tm->isPlaying())
        {
            clickFade.startFadeIn();
            int numSamples = buffer.getNumSamples();
            int numOutputChannels = buffer.getNumChannels();
            static sample_clk_t sinCount = 0;

            bool isFirstBeat = (tm->getClosestBeat() % tm->beatPerBar->intValue()) == 0;
            const int sinFreq = isFirstBeat ? period1 : period2;
            //    const int sinPeriod = sampleRate / sinFreq;
            const double k = 40.0;

            bool ADSREnv =  ! ( outType == sineId);

            for (int i = 0 ; i < numSamples; i++)
            {

                double carg = sinCount * 1.0 / sinFreq;
                double env = 1.0;

                if (ADSREnv)
                {
                    double x = (tm->getBeatInNextSamples (i) - tm->getBeatInt() ) ;
                    double h = k * fmod ((double)x + 1.0 / k, 1.0);
                    //        double env = jmax(0.0,1.0 - x*4.0);
                    env = jmax (0.0, h * exp (1.0 - h));
                }

                clickFade.incrementFade();
                double fade = clickFade.getCurrentFade();
                float res = (float) (fade * (env * cos (2.0 * float_Pi * carg)));

                for (int c = 0 ; c < numOutputChannels ; c++ ) {buffer.setSample (c, i, res);}

                sinCount = (sinCount + 1) % (sinFreq);


            }
        }
        else
        {
            clickFade.startFadeOut();
        }
    }
    else
    {

        if (outType == lrId)
        {
            for (int i = 0; i < buffer.getNumSamples(); i++)
            {
                buffer.addSample (0, i, (float) (amp * cos (2.0 * double_Pi * step1 * 1.0 / period1)));
                buffer.addSample (1, i, (float) (amp * cos (2.0 * double_Pi * step2 * 1.0 / period2)));
                step1++;
                step2++;

                if (step1 > period1) { step1 = 0; }

                if (step2 > period2) { step2 = 0; }
            }
        }
        else if (outType == rlId)
        {
            for (int i = 0; i < buffer.getNumSamples(); i++)
            {
                buffer.addSample (1, i, (float) (amp * cos (2.0 * double_Pi * step1 * 1.0 / period1)));
                buffer.addSample (0, i, (float) (amp * cos (2.0 * double_Pi * step2 * 1.0 / period2)));
                step1++;
                step2++;

                if (step1 > period1) { step1 = 0; }

                if (step2 > period2) { step2 = 0; }
            }
        }
        else if (outType == mixedId)
        {
            for (int i = 0; i < buffer.getNumSamples(); i++)
            {
                buffer.addSample (0, i, (float) (amp / 2 * cos (2.0 * double_Pi * step1 * 1.0 / period1)));
                buffer.addSample (0, i, (float) (amp / 2 * cos (2.0 * double_Pi * step2 * 1.0 / period2)));
                buffer.addSample (1, i, (float) (amp / 2 * cos (2.0 * double_Pi * step1 * 1.0 / period1)));
                buffer.addSample (1, i, (float) (amp / 2 * cos (2.0 * double_Pi * step2 * 1.0 / period2)));
                step1++;
                step2++;

                if (step1 > period1) { step1 = 0; }

                if (step2 > period2) { step2 = 0; }
            }
        }

    }

}





#endif // JUCE DEBGUG
