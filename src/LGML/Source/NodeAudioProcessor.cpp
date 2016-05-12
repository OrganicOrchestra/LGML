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

NodeAudioProcessor::NodeAudioProcessor(const String Name) :AudioProcessor(), ControllableContainer(Name + "_audio") 
{
	outputVolume = addFloatParameter("masterVolume", "mester volume for this node", 1.);
	lastVolume = outputVolume->floatValue();
	bypass = addBoolParameter("Bypass", "by-pass current node, letting audio pass thru", false);
	skipControllableNameInAddress = true;

	setInputChannelName(0, "Main Left");
	setInputChannelName(1, "Main Right");
	setOutputChannelName(0, "Main Left");
	setOutputChannelName(1, "Main Right");
}


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


    if(!isSuspended())
    {
        if(!bypass->boolValue()){
            processBlockInternal(buffer, midiMessages);
            buffer.applyGainRamp(0, buffer.getNumSamples(), lastVolume, outputVolume->floatValue());
            lastVolume = outputVolume->floatValue();

            if(wasSuspended){
                buffer.applyGainRamp(0, buffer.getNumSamples(), 0, 1);
                wasSuspended=false;
            }
        }

        if(rmsListeners.size() ){
            updateRMS(buffer,rmsValueOut);
            curSamplesForRMSOutUpdate += buffer.getNumSamples();

            if(curSamplesForRMSOutUpdate>=samplesBeforeRMSOutUpdate){
                doUpdateRMSOut = true;
                curSamplesForRMSOutUpdate = 0;
            }
        }

        if(doUpdateRMSIn || doUpdateRMSOut) triggerAsyncUpdate();

    }

    else{
        if(!wasSuspended){
            buffer.applyGainRamp(0, buffer.getNumSamples(), 1, 0);wasSuspended = true;
        }
        else{
            buffer.clear();
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

void NodeAudioProcessor::setInputChannelNames(int startChannel, StringArray names)
{
	for (int i = startChannel; i < startChannel + names.size(); i++)
	{
		setInputChannelName(i, names[i]);
	}
}

void NodeAudioProcessor::setOutputChannelNames(int startChannel, StringArray names)
{
	for (int i = startChannel; i < startChannel + names.size(); i++)
	{
		setOutputChannelName(i, names[i]);
	}
}

void NodeAudioProcessor::setInputChannelName(int channelIndex, const String & name)
{
	while (inputChannelNames.size() < (channelIndex + 1))
	{
		inputChannelNames.add(String::empty);
	}

	inputChannelNames.set(channelIndex,name);
}

void NodeAudioProcessor::setOutputChannelName(int channelIndex, const String & name)
{
	while (outputChannelNames.size() < (channelIndex + 1))
	{
		outputChannelNames.add(String::empty);
	}

	outputChannelNames.set(channelIndex, name);
}

String NodeAudioProcessor::getInputChannelName(int channelIndex)
{
	String defaultName = "Input " + String(channelIndex);
	if (channelIndex < 0 || channelIndex >= inputChannelNames.size()) return defaultName;

	String s = inputChannelNames[channelIndex];
	if (s.isNotEmpty()) return s;
	return defaultName;
}

String NodeAudioProcessor::getOutputChannelName(int channelIndex)
{
	String defaultName = "Output " + String(channelIndex);
	if (channelIndex < 0 || channelIndex >= outputChannelNames.size()) return defaultName;

	String s = outputChannelNames[channelIndex];
	if (s.isNotEmpty()) return s;
	return defaultName;
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
