/*
  ==============================================================================

    AudioHelpers.h
    Created: 8 Mar 2016 12:33:13pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef AUDIOHELPERS_H_INCLUDED
#define AUDIOHELPERS_H_INCLUDED


#include "JuceHeader.h"


/*
 helper class for bipartite buffer
 allowing having constant access to contiguous memory in a circular buffer
 */
class BipBuffer{
    
public:
    BipBuffer(int size){
        phantomSize = size;
        buf.setSize(1,3*size);
        writeNeedle = 0;
    }
    
    void writeBlock(AudioSampleBuffer & newBuf){
        
        int toCopy = newBuf.getNumSamples();
        
        if( phantomSize+writeNeedle + toCopy > 3*phantomSize){
            int firstSeg = 3*phantomSize-(phantomSize+writeNeedle) ;
            safeCopy(newBuf.getReadPointer(0,0),firstSeg);
            safeCopy(newBuf.getReadPointer(0,firstSeg),toCopy-firstSeg);
        }
        else{
            safeCopy(newBuf.getReadPointer(0),toCopy);
        }
    }
    
    
    const float* getLastBlock(int num){
        jassert(num<phantomSize);
        return buf.getReadPointer(0, phantomSize + writeNeedle - num);
    }
    
    AudioSampleBuffer buf;
private:
    void safeCopy(const float * b,int s){
        buf.copyFrom(0, phantomSize+writeNeedle, b, s);
        
        if(writeNeedle>2*phantomSize){
            buf.copyFrom(0, writeNeedle-2*phantomSize,b,s);
        }
        writeNeedle+=s;
        writeNeedle%=2*phantomSize;
        
    }
    int writeNeedle;
    int phantomSize;
    
    
    
    
};






#endif  // AUDIOHELPERS_H_INCLUDED
