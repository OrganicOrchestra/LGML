/*
 ==============================================================================

 PlayableBuffer.h
 Created: 6 Jun 2016 7:45:50pm
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef PLAYABLEBUFFER_H_INCLUDED
#define PLAYABLEBUFFER_H_INCLUDED

#include "JuceHeader.h"




class PlayableBuffer {

    public :
    PlayableBuffer(int numChannels,int numSamples):
    loopSample(numChannels,numSamples),
    recordNeedle(0),
    playNeedle(0),isJumping(false),
    state(BUFFER_STOPPED),
    lastState(BUFFER_STOPPED)
    {

    }

    bool writeAudioBlock(const AudioBuffer<float> & buffer){

        if (recordNeedle + buffer.getNumSamples()> loopSample.getNumSamples()) {
            jassertfalse;
            return false;
        }
        else{
            for (int i = loopSample.getNumChannels() - 1; i >= 0; --i) {
                loopSample.copyFrom(i, recordNeedle, buffer, i, 0, buffer.getNumSamples());
            }
            recordNeedle += buffer.getNumSamples();
        }

        return true;
    }

    int getNumSamples(){return loopSample.getNumSamples();}

    void readNextBlock(AudioBuffer<float> & buffer){

        if ((playNeedle + buffer.getNumSamples()) > recordNeedle)
        {

            //assert false for now see above
            jassertfalse;
            //            // crop buffer to ensure not coming back
            //            recordNeedle = (playNeedle + buffer.getNumSamples());
            int firstSegmentLength = recordNeedle - playNeedle;
            int secondSegmentLength = buffer.getNumSamples() - firstSegmentLength;
            for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
                int maxChannelFromRecorded = jmin(loopSample.getNumChannels() - 1, i);
                buffer.copyFrom(i, 0, loopSample, maxChannelFromRecorded, playNeedle, firstSegmentLength);
                buffer.copyFrom(i, 0, loopSample, maxChannelFromRecorded, 0, secondSegmentLength);
            }
            playNeedle = secondSegmentLength;


        }

        // stitch audio jumps by quick fadeIn/Out
        if(isJumping){
            const int halfBlock =  buffer.getNumSamples()/2;
            for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
                int maxChannelFromRecorded = jmin(loopSample.getNumChannels() - 1, i);
                buffer.copyFrom(i, 0, loopSample, maxChannelFromRecorded, startJumpNeedle, halfBlock);
                buffer.applyGainRamp(i, 0, halfBlock, 1.0f, 0.0f);
                buffer.copyFrom(i, halfBlock, loopSample, maxChannelFromRecorded, playNeedle, halfBlock);
                buffer.applyGainRamp(i, 0, halfBlock, 0.0f, 1.0f);

            }

            isJumping = false;
        }
        else{
            for (int i = buffer.getNumChannels() - 1; i >= 0; --i) {
                int maxChannelFromRecorded = jmin(loopSample.getNumChannels() - 1, i);
                buffer.copyFrom(i, 0, loopSample, maxChannelFromRecorded, playNeedle, buffer.getNumSamples());
            }
        }
        playNeedle += buffer.getNumSamples();
        playNeedle %= recordNeedle;
        //        }




    }


    void setPlayNeedle(int n){
        if(playNeedle!=n){
            isJumping = true;
            startJumpNeedle = playNeedle;
        }

        playNeedle = n;


    }

    void cropEndOfRecording(int sampletoRemove){
        recordNeedle-=sampletoRemove;
    }

    void fadeInOut(int fadeNumSamples,double mingain){
        if (fadeNumSamples>0 && recordNeedle>2 * fadeNumSamples) {
            for (int i = loopSample.getNumChannels() - 1; i >= 0; --i) {
                loopSample.applyGainRamp(i, 0, fadeNumSamples, mingain, 1);
                loopSample.applyGainRamp(i, recordNeedle - fadeNumSamples, fadeNumSamples, 1, mingain);
            }
        }
    }
    bool isFirstPlayingFrameAfterRecord(){
        return lastState == BUFFER_RECORDING && state == BUFFER_PLAYING;
    }
    bool isStopping(){
        return (lastState == BUFFER_PLAYING ) && (state==BUFFER_STOPPED);
    }
    bool isRecording(){
        return state == BUFFER_RECORDING;
    }
    bool firstRecordedFrame(){
        return state == BUFFER_RECORDING && (lastState!=BUFFER_RECORDING);
    }
    void startRecord(){
        recordNeedle = 0;
    }

    void startPlay(){
        playNeedle = 0;
    }
    bool checkTimeAlignment(uint64 curTime){
        if(state == BUFFER_PLAYING && playNeedle>=0 && recordNeedle>0){
            //        const int blockSize = parentLooper->getBlockSize();
            const int minQuantifiedFraction = recordNeedle;//floor(TimeManager::getInstance()->beatTimeInSample / (16.0*blockSize))*blockSize;
			int globalPos =(curTime%minQuantifiedFraction);
			int localPos =(playNeedle%minQuantifiedFraction);
            if(globalPos!=localPos){
                isJumping = true;
                startJumpNeedle = playNeedle;
				playNeedle = (playNeedle - localPos) + globalPos;
				
            }
        }
        return !isJumping;
    }


    enum BufferState {
        BUFFER_STOPPED = 0,
        BUFFER_PLAYING,
        BUFFER_RECORDING

    };

    void setState(BufferState newState){
        //        lastState = state;
        stateChanged |=newState!=state;
        switch (newState){
            case BUFFER_RECORDING:
                recordNeedle = 0;
                playNeedle = -1;
                break;
            case BUFFER_PLAYING:
                playNeedle = 0;
                break;
            case BUFFER_STOPPED:
                break;
        }
        state = newState;

    }

    void endProcessBlock(){
        lastState = state;
		stateChanged =false;
    }

    BufferState getState(){
        return state;
    }

    BufferState getLastState(){
        return lastState;
    }

    bool isOrWasPlaying(){
        return (state==BUFFER_PLAYING || lastState==BUFFER_PLAYING) &&  recordNeedle>0 && loopSample.getNumSamples();
    }
    int getRecordedLength(){return recordNeedle+1;}

    int getPlayPos(){return playNeedle;}


    bool stateChanged;




private:

    BufferState state;
    BufferState lastState;
    bool isJumping;
    AudioSampleBuffer loopSample;

    int recordNeedle,playNeedle,startJumpNeedle;

};



#endif  // PLAYABLEBUFFER_H_INCLUDED
