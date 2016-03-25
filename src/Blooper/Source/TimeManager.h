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

#include "NodeBase.h"

class TimeManager : public AudioIODeviceCallback ,public ControllableContainer{
    
    
    public :
    
    // TODO check if we can use SingleThread Singleton for fast access in processAdio
    juce_DeclareSingleton(TimeManager, true);
    
    TimeManager();
	~TimeManager();
    
    int beatPerQuantizedTime;
    void incrementClock(int time);
    
    
    void stop();
    void setPlayState(bool s,bool isSettingTempo = false);
    void setSampleRate(int sr);
    void setBPM(double BPM);
    int setBPMForLoopLength(int time);
    double getBPM();
    FloatParameter *  BPM;
    
    void setBeatPerBar(int bpb);
    int getBeat();
    int getNextQuantifiedTime();
    void setNumBeatForQuantification(int n);
    //return percent in beat
    double getBeatPercent();
    int getBar();
    bool isSettingTempo;
    bool getIsSettingTempo(){return isSettingTempo;};
// these Macros helps to declare synchronous and asynchronous Methods for listeners
    //declares internal function as internal_"name" then dispatch sync and async messages
#define METHOD_SYNC_ASYNC(x)                \
    bool needToCallAsync_##x = false;       \
    virtual void x() {};                    \
    virtual void async_##x() {};            \
    void internal_##x(){                    \
        x();needToCallAsync_##x = true;triggerAsyncUpdate();}

#define CHECK_ASYNC(x)          \
    if(needToCallAsync_##x){    \
        async_##x();            \
        needToCallAsync_##x = false;}

#define METHOD_SYNC_ASYNC1(x,type)              \
    bool needToCallAsync_##x = false;           \
    type value_##x;                             \
    virtual void x(type param_##x) {};          \
    virtual void async_##x(type param_##x) {};  \
    void internal_##x(type param_##x){x(param_##x);needToCallAsync_##x = true;value_##x = param_##x;triggerAsyncUpdate();}

#define CHECK_ASYNC1(x,type)            \
    if(needToCallAsync_##x){            \
        async_##x(value_##x);           \
        needToCallAsync_##x = false;}   \

    //Listener
    
    // they should override "name" and async_"name"
    class  Listener : public AsyncUpdater
    {
    public:
        
        /** Destructor. */
        virtual ~Listener() {}
        
        
        METHOD_SYNC_ASYNC(stop)
        METHOD_SYNC_ASYNC(play)
        METHOD_SYNC_ASYNC1(isSettingTempo,bool)
        METHOD_SYNC_ASYNC1(newBar,int)
        METHOD_SYNC_ASYNC1(newBeat,int)
        METHOD_SYNC_ASYNC1(newBPM,double)
        METHOD_SYNC_ASYNC1(beatPerBarChanged,int)
        
        
        void handleAsyncUpdate()override {
            CHECK_ASYNC(stop);
            CHECK_ASYNC(play);
            CHECK_ASYNC1(newBar,int);
            CHECK_ASYNC1(newBeat,int);
            CHECK_ASYNC1(newBPM,double);
            CHECK_ASYNC1(isSettingTempo,bool);
            CHECK_ASYNC1(beatPerBarChanged,int)
        }
    };
    
    bool playState;
    ListenerList<Listener> listeners;
    void addTimeManagerListener(Listener* newListener) { listeners.add(newListener); }
    void removeTimeManagerListener(Listener* listener) { listeners.remove(listener); }

    uint64 timeInSample;
    int beatTimeInSample;
    int sampleRate;
    int beatPerBar;
    NodeBase *  timeMasterNode;
    bool isMasterNode(NodeBase * n){return n==timeMasterNode;}
    bool hasMasterNode(){return timeMasterNode!=nullptr;}
    void removeIfMaster(NodeBase * n){if(n==timeMasterNode)timeMasterNode=nullptr;}
    bool askForBeingMasterNode(NodeBase * n);
     void audioDeviceIOCallback (const float** inputChannelData,int numInputChannels,float** outputChannelData,int numOutputChannels,int numSamples) ;
    

    virtual void audioDeviceAboutToStart (AudioIODevice* device) {
        setSampleRate(device->getCurrentSampleRate());
     // should we notify blockSize?
    };
    
    /** Called to indicate that the device has stopped. */
    virtual void audioDeviceStopped() {
    
    };
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeManager)

};



#endif  // TIMEMANAGER_H_INCLUDED
