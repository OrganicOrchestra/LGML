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

#include "TimeMasterCandidate.h"
#include "ControllableContainer.h"

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

    void setSampleRate(int sr);
    double setBPMForLoopLength(uint64 time,int granularity=0);


	void jump(int amount);
  void goToTime(uint64 time);
  void advanceTime(uint64 );

    // used when triggering multiple change
    void lockTime(bool );
    bool isLocked();


    void togglePlay();

    int getBeatInt();
    double getBeat();
    uint64 getNextGlobalQuantifiedTime();
    uint64 getNextQuantifiedTime(int barFraction);
    uint64 getTimeForNextBeats(int beats);
    uint64 getTimeInSample();
    uint64 getNextTimeInSample();
    double getBeatInSamples(int numSampleToAdd);


    //return percent in beat
    double getBeatPercent();
    int getBar();

	/*
    ListenerList<Listener> listeners;
    void addTimeManagerListener(Listener* newListener) { listeners.add(newListener); }
    void removeTimeManagerListener(Listener* listener) { listeners.remove(listener); }
	*/

    uint beatTimeInSample;
    int sampleRate;

    Array<TimeMasterCandidate *>  potentialTimeMasterCandidate;
    bool isMasterCandidate(TimeMasterCandidate * n);
    bool hasMasterCandidate();
    void releaseMasterCandidate(TimeMasterCandidate * n);
    bool askForBeingMasterCandidate(TimeMasterCandidate * n);
    void audioDeviceIOCallback (const float** inputChannelData,int numInputChannels,float** outputChannelData,int numOutputChannels,int numSamples) override;


    bool getCurrentPosition (CurrentPositionInfo& result)override;

#if !LGML_UNIT_TESTS
private:
#endif

    struct TimeState{
        TimeState():isJumping(false),nextTime((uint64)-1),isPlaying(false),time((uint64)0){}
        bool isPlaying;
        void jumpTo(uint64 t){
            isJumping = true;
            nextTime = t;
        }
        bool isJumping;
        uint64 nextTime;
        uint64 time;
    };

    TimeState timeState,desiredTimeState;
    
    void shouldStop();
    void shouldPlay();
    void shouldRestart(bool );
    void shouldGoToZero();
    
    void updateState();
    void incrementClock(int time);

    void onContainerParameterChanged(Parameter * )override;
    void onContainerTriggerTriggered(Trigger * ) override;


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
