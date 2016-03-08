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
<<<<<<< HEAD
      
		
=======


        // specify the number of input and output channels that we want to open
        //setAudioChannels (2, 2);
>>>>>>> 287d44fb80b3b07c52c9ff485d3d2d9721f5db39

		
		nodeManager = new NodeManager();
<<<<<<< HEAD
		controllerManager = new ControllerManager();

		initAudio();
=======
        initAudio();
>>>>>>> 287d44fb80b3b07c52c9ff485d3d2d9721f5db39

		nodeManagerUI = new NodeManagerUI(nodeManager);
		controllerManagerUI = new ControllerManagerUI(controllerManager);
		addAndMakeVisible(controllerManagerUI);
		addAndMakeVisible(nodeManagerUI);
<<<<<<< HEAD

		setSize(1200, 600);

=======
		nodeManagerUI->setSize(getWidth(),getHeight());
        
        
        timeManagerUI = new TimeManagerUI();
        addAndMakeVisible(timeManagerUI);
        
        // resize after contentCreated
                setSize (800,600);
>>>>>>> 287d44fb80b3b07c52c9ff485d3d2d9721f5db39
    }

    ~MainContentComponent()
    {
        stopAudio();
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
<<<<<<< HEAD
		Rectangle<int> r = getLocalBounds();
		controllerManagerUI->setBounds(r.removeFromLeft(300));
		nodeManagerUI->setBounds(r);
=======
        // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.
        
        Rectangle<int> area = getLocalBounds();
        timeManagerUI->setBounds(area.removeFromTop(50));
        nodeManagerUI->setBounds(area);
>>>>>>> 287d44fb80b3b07c52c9ff485d3d2d9721f5db39
    }

private:
    //==============================================================================

    // Your private member variables go here...

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }



#endif  // MAINCOMPONENT_H_INCLUDED
