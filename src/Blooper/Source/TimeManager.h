/*
  ==============================================================================

    TimeManager.h
    Created: 2 Mar 2016 8:33:44pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef TIMEMANAGER_H_INCLUDED
#define TIMEMANAGER_H_INCLUDED


#include "JuceHeader.h"
/*
 This singleton handle time at sample Level
 then can dispatch synchronous or asynchronous event via TimeManager::Listener
 */

class TimeManager : public AudioIODeviceCallback{
    
    
    public :
    juce_DeclareSingleton(TimeManager, true);
    
    TimeManager():
    timeInSample(0),
    playState(false),
    beatTimeInSample(44100),
    beatPerBar(4),sampleRate(44100){
        
    };
    
    
    
    void incrementClock(int time){
        int lastBeat = getBeat();
        
        if(playState){
        timeInSample+=time;
        }
        int newBeat = getBeat();
        if(lastBeat!=newBeat){
            listeners.call(&Listener::internal_newBeat,newBeat);
            if(newBeat%beatPerBar == 0){
                listeners.call(&Listener::internal_newBar,getBar());
                
            }
        }
    }
    
    
    void stop(){
        setPlayState(false);
    }
    
    void setPlayState(bool s){
        playState = s;
        if(s){
         listeners.call(&Listener::internal_stop);
        }
        else{
         listeners.call(&Listener::internal_play);
        }
    }
    
    void setSampleRate(int sr){
        sampleRate = sr;
        // actualize beatTime in sample
        setBPM(getBPM());
        // shouldWe notify something here?
    }
    void setBPM(double BPM){
        beatTimeInSample = sampleRate*60.0/BPM;
        listeners.call(&Listener::newBPM,BPM);
    }
    double getBPM(){
        return sampleRate*60.0/beatTimeInSample;
    }
    
    int getBeat(){
        return timeInSample/beatTimeInSample;
    }
    
    //return percent in beat
    double getBeatPercent(){
        return timeInSample*1.0/beatTimeInSample-getBeat();
    }
    int getBar(){
        return getBeat()/beatPerBar;
    }
    
// these Macros helps to declare synchronous and asynchronous Methods for listeners
    //declares internal function as internal_"name" then dispatch sync and async messages
#define METHOD_SYNC_ASYNC(x) \
    bool needToCallAsync_##x = false;\
    virtual void x() {};\
    virtual void async_##x() {};\
    void internal_##x(){x();needToCallAsync_##x = true;triggerAsyncUpdate();}\

#define CHECK_ASYNC(x) \
    if(needToCallAsync_##x){\
        async_##x();\
        needToCallAsync_##x = false;\
    }\

#define METHOD_SYNC_ASYNC1(x,type) \
    bool needToCallAsync_##x = false;\
    type value_##x;\
    virtual void x(type param_##x) {};\
    virtual void async_##x(type param_##x) {};\
    void internal_##x(type param_##x){x(param_##x);needToCallAsync_##x = true;value_##x = param_##x;triggerAsyncUpdate();}\

#define CHECK_ASYNC1(x,type) \
    if(needToCallAsync_##x){\
        async_##x(value_##x);\
        needToCallAsync_##x = false;\
    }\

    //Listener
    
    // they should override "name" and async_"name"
    class  Listener : public AsyncUpdater
    {
    public:
        
        /** Destructor. */
        virtual ~Listener() {}
        
        
        METHOD_SYNC_ASYNC(stop)
        METHOD_SYNC_ASYNC(play)
        METHOD_SYNC_ASYNC1(newBar,int)
        METHOD_SYNC_ASYNC1(newBeat,int)
        METHOD_SYNC_ASYNC1(newBPM,double)
        
        
        void handleAsyncUpdate()override {
            CHECK_ASYNC(stop);
            CHECK_ASYNC(play);
            CHECK_ASYNC1(newBar,int);
            CHECK_ASYNC1(newBeat,int);
            CHECK_ASYNC1(newBPM,double);
        }
    };
    
    
    bool playState;
    ListenerList<Listener> listeners;
    void addListener(Listener* newListener) { listeners.add(newListener); }
    void removeListener(Listener* listener) { listeners.remove(listener); }

    int timeInSample;
    int beatTimeInSample;
    int sampleRate;
    int beatPerBar;
    
    
     void audioDeviceIOCallback (const float** inputChannelData,
                                        int numInputChannels,
                                        float** outputChannelData,
                                        int numOutputChannels,
                                 int numSamples) {
    incrementClock(numSamples);
         
         for (int i = 0; i < numOutputChannels; ++i)
             zeromem (outputChannelData[i], sizeof (float) * (size_t) numSamples);
     }
    
    /** Called to indicate that the device is about to start calling back.
     
     This will be called just before the audio callbacks begin, either when this
     callback has just been added to an audio device, or after the device has been
     restarted because of a sample-rate or block-size change.
     
     You can use this opportunity to find out the sample rate and block size
     that the device is going to use by calling the AudioIODevice::getCurrentSampleRate()
     and AudioIODevice::getCurrentBufferSizeSamples() on the supplied pointer.
     
     @param device       the audio IO device that will be used to drive the callback.
     Note that if you're going to store this this pointer, it is
     only valid until the next time that audioDeviceStopped is called.
     */
    virtual void audioDeviceAboutToStart (AudioIODevice* device) {
        setSampleRate(device->getCurrentSampleRate());
    
    };
    
    /** Called to indicate that the device has stopped. */
    virtual void audioDeviceStopped() {
    
    };
    

    

};



#endif  // TIMEMANAGER_H_INCLUDED
