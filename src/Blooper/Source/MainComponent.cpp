/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"

#include "NodeManager.h"
#include "NodeManagerUI.h"

#include "ControlManager.h"
#include "ControllerManagerUI.h"

#include "UIHelpers.h"
#include "TimeManagerUI.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/


ApplicationCommandManager& getCommandManager();
ApplicationProperties& getAppProperties();



class MainContentComponent   : public Component
{
public:
		
	ScopedPointer<NodeManager> nodeManager;
	ScopedPointer<NodeManagerUI> nodeManagerUI;
    ScopedPointer<TimeManagerUI> timeManagerUI;
    

	ScopedPointer<ControllerManager> controllerManager;
	ScopedPointer<ControllerManagerUI> controllerManagerUI;

    // Audio
    AudioDeviceManager deviceManager;
    AudioProcessorPlayer graphPlayer;
    //==============================================================================
    MainContentComponent()
    {		
		nodeManager = new NodeManager();
		controllerManager = new ControllerManager();

		initAudio();

		timeManagerUI = new TimeManagerUI();
		nodeManagerUI = new NodeManagerUI(nodeManager);
		controllerManagerUI = new ControllerManagerUI(controllerManager);
		
		
		addAndMakeVisible(timeManagerUI);
		addAndMakeVisible(nodeManagerUI);
		addAndMakeVisible(controllerManagerUI);
		
		nodeManagerUI->setSize(getWidth(),getHeight());
        
        // resize after contentCreated
         setSize (800,600);
    }

    ~MainContentComponent()
    {
        stopAudio();
		delete TimeManager::getInstance(); //TO PREVENT LEAK OF SINGLETON
    }
    
    
    void initAudio(){
        graphPlayer.setProcessor(&nodeManager->audioGraph);
        
        ScopedPointer<XmlElement> savedAudioState (getAppProperties().getUserSettings()
                                                   ->getXmlValue ("audioDeviceState"));
        deviceManager.initialise (256, 256, savedAudioState, true);
        deviceManager.addAudioCallback (&graphPlayer);
        deviceManager.addAudioCallback(TimeManager::getInstance());
    }
    void stopAudio(){
        deviceManager.removeAudioCallback (&graphPlayer);
        deviceManager.closeAudioDevice();
    }

    //=======================================================================
    void paint (Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (Colours::black);


        // You can add your drawing code here!
    }


    void resized() override
    {
		Rectangle<int> r = getLocalBounds();
		timeManagerUI->setBounds(r.removeFromTop(50));
		controllerManagerUI->setBounds(r.removeFromLeft(300));
		nodeManagerUI->setBounds(r);
    }

private:
    //==============================================================================

    // Your private member variables go here...

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }



#endif  // MAINCOMPONENT_H_INCLUDED
