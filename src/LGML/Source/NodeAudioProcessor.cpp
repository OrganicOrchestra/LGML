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

	bool doUpdateRMSIn = false;
	bool doUpdateRMSOut = false;

	if (rmsListeners.size()) {
		updateRMS(buffer,rmsValueIn);
		curSamplesForRMSInUpdate += buffer.getNumSamples();

		if (curSamplesForRMSInUpdate >= samplesBeforeRMSInUpdate) {
			doUpdateRMSIn = true;
			curSamplesForRMSInUpdate = 0;
		}
	}

    if(isSuspended()){
        if(!wasSuspended){buffer.applyGainRamp(0, buffer.getNumSamples(), 1, 0);wasSuspended = true;}
        else{buffer.clear();}
        return;
    }
    else if(wasSuspended){buffer.applyGainRamp(0, buffer.getNumSamples(), 0, 1);wasSuspended=false;}

    processBlockInternal(buffer, midiMessages);

    buffer.applyGainRamp(0, buffer.getNumSamples(), lastVolume, outputVolume->floatValue());
    lastVolume = outputVolume->floatValue();

    if(rmsListeners.size() ){
        updateRMS(buffer,rmsValueOut);
        curSamplesForRMSOutUpdate += buffer.getNumSamples();

        if(curSamplesForRMSOutUpdate>=samplesBeforeRMSOutUpdate){
			doUpdateRMSOut = true;
            curSamplesForRMSOutUpdate = 0;
        }
    }

	if(doUpdateRMSIn || doUpdateRMSOut) triggerAsyncUpdate();

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

void NodeAudioProcessor::updateRMS(const AudioBuffer<float>& buffer, float &targetRmsValue){
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
        if (s > targetRmsValue)
			targetRmsValue = s;
        else if (targetRmsValue > 0.001f)
			targetRmsValue *= (float)decayFactor;
        else
			targetRmsValue = 0;
    }
#endif
    //            rmsValue = alphaRMS * buffer.getRMSLevel(0, 0, buffer.getNumSamples()) + (1.0-alphaRMS) * rmsValue;

}
