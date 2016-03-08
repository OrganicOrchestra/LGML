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

#include "UIHelpers.h"
#include "TimeManager.h"

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
    
    // Audio
    AudioDeviceManager deviceManager;
    AudioProcessorPlayer graphPlayer;
    //==============================================================================
    MainContentComponent()
    {
        setSize (1280,800);

        // specify the number of input and output channels that we want to open
        //setAudioChannels (2, 2);

		nodeManager = new NodeManager();
        
        initAudio();

        
        
		nodeManagerUI = new NodeManagerUI(nodeManager);
		addAndMakeVisible(nodeManagerUI);
		nodeManagerUI->setSize(getWidth(),getHeight());
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
        // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.
    }

private:
    //==============================================================================

    // Your private member variables go here...


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }



#endif  // MAINCOMPONENT_H_INCLUDED
