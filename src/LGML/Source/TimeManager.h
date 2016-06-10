/*
 ==============================================================================

 TimeManager.h
 Created: 2 Mar 2016 8:33:44pm
 Author:  bkupe

 ==============================================================================
 */

#ifndef TIMEMANAGER_H_INCLUDED
#define TIMEMANAGER_H_INCLUDED



/*
 This singleton handle time at sample Level
 then can dispatch synchronous or asynchronous event via TimeManager::Listener
 */

#include "NodeBase.h"

#include "TimeMasterCandidate.h"

class TimeManager : public AudioIODeviceCallback ,public ControllableContainer,public AudioPlayHead,
public TimeMasterCandidate
{


    public :

    // TODO check if we can use SingleThread Singleton for fast access in processAdio
    juce_DeclareSingleton(TimeManager, true);

    TimeManager();
    ~TimeManager();


    BoolParameter * playState;
    Trigger * playTrigger;
    Trigger * stopTrigger;
    BoolParameter * isSettingTempo;
    FloatParameter *  BPM;
    IntParameter * currentBeat;
    IntParameter * currentBar;
    IntParameter * beatPerBar;


    IntParameter * quantizedBarFraction;
    void incrementClock(int time);

    void onContainerParameterChanged(Parameter * )override;
    void onContainerTriggerTriggered(Trigger * ) override;
    void setSampleRate(int sr);

    int setBPMForLoopLength(int time);
	
	
	void jump(int amount);

    // used when triggering multiple change
    void lockTime(bool );
    bool isLocked();
    void togglePlay();

    int getBeat();
    int getNextGlobalQuantifiedTime();
    int getNextQuantifiedTime(int barFraction);
    uint64 getTimeForNextBeats(int beats);


    //return percent in beat
    double getBeatPercent();
    int getBar();

	/*
    ListenerList<Listener> listeners;
    void addTimeManagerListener(Listener* newListener) { listeners.add(newListener); }
    void removeTimeManagerListener(Listener* listener) { listeners.remove(listener); }
	*/

    Atomic<uint64> timeInSample;
    int beatTimeInSample;
    int sampleRate;
    Array<TimeMasterCandidate *>  potentialTimeMasterCandidate;
    bool isMasterCandidate(TimeMasterCandidate * n);
    bool hasMasterCandidate();
    void releaseMasterCandidate(TimeMasterCandidate * n);
    bool askForBeingMasterCandidate(TimeMasterCandidate * n);
    void audioDeviceIOCallback (const float** inputChannelData,int numInputChannels,float** outputChannelData,int numOutputChannels,int numSamples) override;


    bool getCurrentPosition (CurrentPositionInfo& result)override;

private:
    void setBPMInternal(double BPM);

    virtual void audioDeviceAboutToStart (AudioIODevice* device)override {
        setSampleRate((int)device->getCurrentSampleRate());
        // should we notify blockSize?
    };

    /** Called to indicate that the device has stopped. */
    virtual void audioDeviceStopped() override{

    };
    bool _isLocked;
    void updateCurrentPositionInfo();

    CurrentPositionInfo currentPositionInfo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimeManager)

};



#endif  // TIMEMANAGER_H_INCLUDED
