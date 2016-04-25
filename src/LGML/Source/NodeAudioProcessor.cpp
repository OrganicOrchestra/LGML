/*
  ==============================================================================

    NodeAudioProcessor.cpp
    Created: 24 Apr 2016 1:32:40pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "NodeAudioProcessor.h"



// =====================

// NodeAudioProcessor

void NodeAudioProcessor::processBlock(AudioBuffer<float>& buffer,
                                                MidiBuffer& midiMessages) {

    if(isSuspended()){
        if(!wasSuspended){buffer.applyGainRamp(0, buffer.getNumSamples(), 1, 0);wasSuspended = true;}
        return;
    }
    else if(wasSuspended){buffer.applyGainRamp(0, buffer.getNumSamples(), 0, 1);wasSuspended=false;}

    processBlockInternal(buffer, midiMessages);

    if(rmsListeners.size() ){
        updateRMS(buffer);
        curSamplesForRMSUpdate+= buffer.getNumSamples();

        if(curSamplesForRMSUpdate>=samplesBeforeRMSUpdate){
            triggerAsyncUpdate();
            curSamplesForRMSUpdate = 0;
        }
    }


};



bool NodeAudioProcessor::setPreferedNumAudioInput(int num){
    setPlayConfigDetails(num,getTotalNumOutputChannels(),
                         getSampleRate(),
                         getBlockSize());

    nodeAudioProcessorListeners.call(&NodeAudioProcessorListener::numAudioInputChanged,num);
    return true;
}
bool NodeAudioProcessor::setPreferedNumAudioOutput(int num){
    setPlayConfigDetails(getTotalNumInputChannels(),num,
                         getSampleRate(),
                         getBlockSize());
    nodeAudioProcessorListeners.call(&NodeAudioProcessorListener::numAudioOutputChanged,num);
    return true;
}

void NodeAudioProcessor::updateRMS(const AudioBuffer<float>& buffer){
    int numSamples = buffer.getNumSamples();
    int numChannels = buffer.getNumChannels();
#ifdef HIGH_ACCURACY_RMS
    for(int i = numSamples-64; i>=0 ; i-=64){
        rmsValue += alphaRMS * (buffer.getRMSLevel(0, i, 64) - rmsValue);
    }
#else
    // faster implementation taken from juce Device Settings input meter
    for (int j = 0; j <numSamples; ++j)
    {
        float s = 0;
        for (int i = numChannels-1; i >0; --i)
            s = jmax(s, std::abs (buffer.getSample(i, j)));


        const double decayFactor = 0.99992;
        if (s > rmsValue)
            rmsValue = s;
        else if (rmsValue > 0.001f)
            rmsValue *= (float)decayFactor;
        else
            rmsValue = 0;
    }
#endif
    //            rmsValue = alphaRMS * buffer.getRMSLevel(0, 0, buffer.getNumSamples()) + (1.0-alphaRMS) * rmsValue;

}
