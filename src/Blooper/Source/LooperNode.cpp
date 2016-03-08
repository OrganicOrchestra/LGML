/*
 ==============================================================================
 
 LooperNode.cpp
 Created: 3 Mar 2016 10:32:16pm
 Author:  bkupe
 
 ==============================================================================
 */

#include "LooperNode.h"
#include "TimeManager.h"

#include "LooperNodeUI.h"

void LooperNode::Looper::processBlockInternal(AudioBuffer<float>& buffer, MidiBuffer &midiMessages){
    
    // TODO check if we can optimize copies
    // handle multiples channels
    bufferIn.makeCopyOf(buffer);
    bufferOut.setSize(bufferIn.getNumChannels(),bufferIn.getNumSamples());
    bufferOut.clear();
    for( auto & t:tracks){
        t->processBlock(buffer,midiMessages);
        
        bufferOut.addFrom(0,0,buffer,0,0,buffer.getNumSamples());
        buffer.copyFrom(0,0,bufferIn,0,0,buffer.getNumSamples());
    }
    
    buffer.makeCopyOf( bufferOut);
}

void LooperNode::Looper::Track::processBlock(AudioBuffer<float>& buffer, MidiBuffer &midi){
    
    
    updatePendingLooperTrackState(TimeManager::getInstance()->timeInSample);
    
    
    // RECORDING
    if (trackState == RECORDING)
    {
        if(recordNeedle + buffer.getNumSamples()> parentLooper->getSampleRate() * MAX_LOOP_LENGTH_S){
            setTrackState(STOPPED);
        };
        monoLoopSample.copyFrom(0, recordNeedle, buffer, 0, 0, buffer.getNumSamples());
        recordNeedle += buffer.getNumSamples();

    }
    
    else{
        streamBipBuffer.writeBlock(buffer);
    }
    
    // PLAYING
    // allow circular reading , although not sure that overflow need to be handled as its written with same block sizes than read
    // we may need it if we start to use a different clock  than looperState in OOServer that has a granularity of blockSize
    // or if we dynamicly change blockSize
    if (trackState==PLAYING && recordNeedle>0 && monoLoopSample.getNumSamples())
    {
        if ( (playNeedle + buffer.getNumSamples()) > recordNeedle)
        {
            
            //assert false for now see above
            //            jassert(false);
            int firstSegmentLength = recordNeedle - playNeedle;
            int secondSegmentLength = buffer.getNumSamples() - firstSegmentLength;
            buffer.copyFrom(0, 0, monoLoopSample, 0, playNeedle, firstSegmentLength);
            buffer.copyFrom(0, 0, monoLoopSample, 0, 0, secondSegmentLength);
            playNeedle = secondSegmentLength;
            
        }else{
            buffer.copyFrom(0, 0, monoLoopSample, 0, playNeedle, buffer.getNumSamples());
            playNeedle += buffer.getNumSamples();
            playNeedle %= recordNeedle;
        }
        buffer.applyGainRamp(0, 0, buffer.getNumSamples(), lastVolume,volume.value);
        lastVolume = volume.value;
        
        
    }
    else{
        // silence output buffer
        buffer.applyGain(0, 0, buffer.getNumSamples(), 0);
    }
    
    
}
void LooperNode::Looper::Track::updatePendingLooperTrackState(int64 curTime){
    
    if(quantizedRecordStart>0){
        if(curTime>quantizedRecordStart){
            preDelayMs.setValue( 0);
            setTrackState(RECORDING);
            
        }
        
    }
    else if( quantizedRecordEnd>0){
        if(curTime>quantizedRecordEnd){
            preDelayMs.setValue(0);
            setTrackState(PLAYING);
        }
    }
    else if(trackState==SHOULD_RECORD){
        setTrackState(RECORDING);
    }

    
    
    if(quantizedPlayStart>0){
        if(curTime>quantizedPlayStart){
            setTrackState(PLAYING);
        }
    }
    else if( quantizedPlayEnd>0){
        if(curTime>quantizedPlayEnd){
            setTrackState(STOPPED);
        }
    }
    else if(trackState == SHOULD_PLAY){
        setTrackState(PLAYING);
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
    
    listeners.call(&Listener::trackNumChanged,numTracks);
}


void LooperNode::Looper::Track::triggerTriggered(Trigger * t){
    if(t == &shouldRecordTrig){
        setTrackState(SHOULD_RECORD);
    }
    else if(t == &shouldPlayTrig){
        setTrackState(SHOULD_PLAY);
    }
    else if(t== &shouldClearTrig){
        setTrackState(SHOULD_CLEAR);
    }
}


void LooperNode::Looper::Track::setTrackState(TrackState newState){
    
    if(newState == RECORDING){
        quantizedRecordStart = -1;
        if(preDelayMs.value>0){
            monoLoopSample.copyFrom(0,0,streamBipBuffer.getLastBlock(preDelayMs.value),preDelayMs.value);
            recordNeedle = preDelayMs.value;
        }
        else{
            preDelayMs.setValue( 0);
            recordNeedle = 0;
        }
    }
    else if(trackState == RECORDING && newState!=trackState){
        {
            
            
            recordNeedle-=preDelayMs.value;
            // 22 ms if 44100
            int fadeNumSaples = 10;
            if(recordNeedle>2*fadeNumSaples){
                monoLoopSample.applyGainRamp(0, 0, fadeNumSaples, 0, 1);
                monoLoopSample.applyGainRamp(0,recordNeedle - fadeNumSaples, fadeNumSaples, 1, 0);
            }
            
            quantizedRecordEnd = -1;
        }
    }
    else if(newState ==PLAYING){
        quantizedPlayStart = -1;
        playNeedle = 0;
    }
    else if (trackState== PLAYING && newState!=PLAYING){
        quantizedPlayEnd = -1;
    }
    
    else if(newState == SHOULD_CLEAR){
        recordNeedle = 0;
        playNeedle = 0;
        quantizedPlayEnd = -1;
        quantizedPlayStart = -1;
        quantizedRecordEnd = -1;
        quantizedRecordStart = -1;
        
    }
    
    trackState = newState;
    listeners.call(&LooperNode::Looper::Track::Listener::internalTrackStateChanged,trackState);
};

NodeBaseUI * LooperNode::createUI(){return new NodeBaseUI(this, new LooperNodeUI);}