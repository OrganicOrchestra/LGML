/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "JuceHeader.h"

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
		
	TooltipWindow tooltipWindow; // to add tooltips to an application, you
								 // just need to create one of these and leave it
								 // there to do its work..

	ScopedPointer<NodeManagerUI> nodeManagerUI;
    ScopedPointer<TimeManagerUI> timeManagerUI;
    

	ScopedPointer<ControllerManager> controllerManager;
	
	ScopedPointer<ControllerManagerViewport> controllerManagerViewport;

	
    // Audio
    AudioDeviceManager deviceManager;
    AudioProcessorPlayer graphPlayer;
    //==============================================================================
    MainContentComponent()
	{
		DBG("Application Start");

		controllerManager = new ControllerManager();

		initAudio();

		timeManagerUI = new TimeManagerUI();
		nodeManagerUI = new NodeManagerUI(NodeManager::getInstance());


		addAndMakeVisible(timeManagerUI);
		addAndMakeVisible(nodeManagerUI);

		controllerManagerViewport = new ControllerManagerViewport(controllerManager);
		addAndMakeVisible(controllerManagerViewport);

		nodeManagerUI->setSize(getWidth(), getHeight());

		// resize after contentCreated
		setSize(1200, 600);
	}


    ~MainContentComponent()
    {
        stopAudio();
		TimeManager::deleteInstance(); //TO PREVENT LEAK OF SINGLETON
		NodeManager::deleteInstance();
    }
    
     
    void initAudio(){
        graphPlayer.setProcessor(&NodeManager::getInstance()->audioGraph);
        
        ScopedPointer<XmlElement> savedAudioState (getAppProperties().getUserSettings()
                                                   ->getXmlValue ("audioDeviceState"));
        deviceManager.initialise (256, 256, savedAudioState, true);
        deviceManager.addAudioCallback (&graphPlayer);
        deviceManager.addAudioCallback(TimeManager::getInstance());
    }
    void stopAudio(){
        deviceManager.removeAudioCallback (&graphPlayer);
        deviceManager.removeAudioCallback(TimeManager::getInstance());
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
		timeManagerUI->setBounds(r.removeFromTop(20));
		controllerManagerViewport->setBounds(r.removeFromLeft(300));
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
