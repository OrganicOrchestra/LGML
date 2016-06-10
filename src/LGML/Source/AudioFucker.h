/*
  ==============================================================================

    AudioFucker.h
    Created: 6 Jun 2016 7:45:08pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef AUDIOFUCKER_H_INCLUDED
#define AUDIOFUCKER_H_INCLUDED


class AudioFucker : public AudioIODeviceCallback{

public:
    AudioFucker(AudioDeviceManager *_manager,double _targetCPU = 100):manager(_manager),targetCPU(_targetCPU){
        iterationCount = 100;
        adaptationSpeed = .7;
    }
    ~AudioFucker(){
        manager->removeAudioCallback(this);
        int a = 0;
        a++;
    }

    AudioDeviceManager * manager;
    double targetCPU;
    double adaptationSpeed;
    int iterationCount;
    virtual void audioDeviceIOCallback (const float** /*inputChannelData*/,
                                        int /*numInputChannels*/,
                                        float** /*outputChannelData*/,
                                        int /*numOutputChannels*/,
                                        int /*numSamples*/) {

        double currentCPU = manager->getCpuUsage()*100.0;
        iterationCount += (int)(adaptationSpeed* (targetCPU -currentCPU));
        iterationCount = jmax(0,iterationCount);
        //        DBG(iterationCount);
        uint64  i = 0 ;
        while(i < iterationCount){
            doStupidThings();
            i++;
        }
    }

    void doStupidThings(){
        float j = 1;
        for(int i = 0 ; i < 100 ; i++){
            j*=2;
            j/=3;
        }
    }
    virtual void audioDeviceAboutToStart (AudioIODevice* /*device*/) {}

    /** Called to indicate that the device has stopped. */
    virtual void audioDeviceStopped() {}
};





#endif  // AUDIOFUCKER_H_INCLUDED
