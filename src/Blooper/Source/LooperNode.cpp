/*
  ==============================================================================

    LooperNode.cpp
    Created: 3 Mar 2016 10:32:16pm
    Author:  bkupe

  ==============================================================================
*/

#include "LooperNode.h"


void LooperNode::Looper::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer &midiMessages){

    AudioBuffer<float> bufferIn;
    bufferIn.copyFrom(0,0,buffer,0,0,buffer.getNumSamples());
    AudioBuffer<float>bufferOut;
    
    
    for( auto & t:tracks){
        t->processBlock(buffer,midiMessages);
        
        bufferOut.addFrom(0,0,buffer,0,0,buffer.getNumSamples());
        buffer.copyFrom(0,0,bufferIn,0,0,buffer.getNumSamples());
    }
    globalSampleTime+=buffer.getNumSamples();
    if(globalSampleTime>maxLoopLength){
        globalSampleTime -=maxLoopLength  ;
    }
    
}

void LooperNode::Looper::Track::processBlock(AudioBuffer<float>& buffer, MidiBuffer &midi){
    
    
    //        updatePendingLooperState(OOServer::getInstance()->looperState.timeInSamples);
    //        OOServer::getInstance()->updateGlobalTime(this,buffer.getNumSamples());
    
    // RECORDING
    if (*isRecording)
    {
        if(recordNeedle.get() + buffer.getNumSamples()> parentLooper->getSampleRate() * MAX_LOOP_LENGTH_S){
            *shouldRecord = false;
            *isRecording = false;
        };
        monoLoopSample.copyFrom(0, recordNeedle.get(), buffer, 0, 0, buffer.getNumSamples());
        recordNeedle += buffer.getNumSamples();

        
        
    }
#if STREAM_BIPARTITE
    else{
        streamBipBuffer.writeBlock(buffer);
    }
#endif
    // PLAYING
    // allow circular reading , although not sure that overflow need to be handled as its written with same block sizes than read
    // we may need it if we start to use a different clock  than looperState in OOServer that has a granularity of blockSize
    // or if we dynamicly change blockSize
    if (*isPlaying && recordNeedle.get()>0 && monoLoopSample.getNumSamples())
    {
        if ( (playNeedle + buffer.getNumSamples()) > recordNeedle.get())
        {
            
            //assert false for now see above
            //            jassert(false);
            int firstSegmentLength = recordNeedle.get() - playNeedle;
            int secondSegmentLength = buffer.getNumSamples() - firstSegmentLength;
            buffer.copyFrom(0, 0, monoLoopSample, 0, playNeedle, firstSegmentLength);
            buffer.copyFrom(0, 0, monoLoopSample, 0, 0, secondSegmentLength);
            playNeedle = secondSegmentLength;
            
        }else{
            buffer.copyFrom(0, 0, monoLoopSample, 0, playNeedle, buffer.getNumSamples());
            playNeedle += buffer.getNumSamples();
            playNeedle %= recordNeedle.get();
        }
        buffer.applyGainRamp(0, 0, buffer.getNumSamples(), lastVolume,*volume);
        lastVolume = *volume;
        
        
    }
    else{
        // silence output buffer
        buffer.applyGain(0, 0, buffer.getNumSamples(), 0);
    }
    

}


void LooperNode::Looper::setNumTracks(int numTracks){
    int oldSize = tracks.size();
    if(numTracks>oldSize){
        for(int i = oldSize ; i< numTracks ; i++){
        tracks.add(new Track(this,i));
        }
    }
    else{
    tracks.removeRange(oldSize,oldSize - numTracks );
    }
}