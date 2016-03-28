/*
 ==============================================================================

 MainComponent.cpp
 Created: 25 Mar 2016 6:11:02pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "MainComponent.h"

// (This function is called by the app startup code to create our main component)
MainContentComponent* createMainContentComponent()     { return new MainContentComponent(); }


MainContentComponent::MainContentComponent():FileBasedDocument (filenameSuffix,
                                                                filenameWildcard,
                                                                "Load a filter graph",
                                                                "Save a filter graph")
{
    DBG("Application Start");

    controllerManager = new ControllerManager();

    initAudio();

    timeManagerUI = new TimeManagerUI();
    nodeManagerUI = new NodeManagerUI(NodeManager::getInstance());
    nodeManagerUIViewport=new NodeManagerUIViewport(nodeManagerUI);


    addAndMakeVisible(timeManagerUI);

    addAndMakeVisible(nodeManagerUIViewport);



    controllerManagerViewport = new ControllerManagerViewport(controllerManager);
    addAndMakeVisible(controllerManagerViewport);


    controllableInspector = new ControllableInspector(nodeManagerUI);
    addAndMakeVisible(controllableInspector);

    // resize after contentCreated

    setSize(1200, 600);

    (&getCommandManager())->registerAllCommandsForTarget (this);
    (&getCommandManager())-> setFirstCommandTarget(this);

    (&getCommandManager())->getKeyMappings()->resetToDefaultMappings();
//    (&getCommandManager())->getKeyMappings()->restoreFromXml (lastSavedKeyMappingsXML);
    addKeyListener ((&getCommandManager())->getKeyMappings());
#if JUCE_MAC
    setMacMainMenu (this);
#else
    //setMenu (this); //done in Main.cpp as it's a method of DocumentWindow
#endif

    createNewGraph();


}



MainContentComponent::~MainContentComponent(){
    stopAudio();
    TimeManager::deleteInstance(); //TO PREVENT LEAK OF SINGLETON
    NodeManager::deleteInstance();
    VSTManager::deleteInstance();
#if JUCE_MAC
    setMacMainMenu (nullptr);
#else
    //setMenuBar (nullptr);

#endif
}



void MainContentComponent::resized()
{
    Rectangle<int> r = getLocalBounds();
    timeManagerUI->setBounds(r.removeFromTop(20));
    controllerManagerViewport->setBounds(r.removeFromLeft(300));
    controllableInspector->setBounds(r.removeFromRight(300));
    nodeManagerUIViewport->setBounds(r);
}



void MainContentComponent::createNewGraph(){
    clear();
    NodeBase * node = NodeManager::getInstance()->addNode(NodeFactory::NodeType::AudioIn);
    node->xPosition->setValue(80);
    node->yPosition->setValue(50);
    node = NodeManager::getInstance()->addNode(NodeFactory::NodeType::AudioOut);
    node->xPosition->setValue(450);
    node->yPosition->setValue(50);
}




void MainContentComponent::initAudio(){
    graphPlayer.setProcessor(&NodeManager::getInstance()->audioGraph);
    ScopedPointer<XmlElement> savedAudioState (getAppProperties().getUserSettings()->getXmlValue ("audioDeviceState"));
    getAudioDeviceManager().initialise (256, 256, savedAudioState, true);
    getAudioDeviceManager().addAudioCallback (&graphPlayer);
    getAudioDeviceManager().addAudioCallback(TimeManager::getInstance());
}


void MainContentComponent::stopAudio(){
    getAudioDeviceManager().removeAudioCallback (&graphPlayer);
    getAudioDeviceManager().removeAudioCallback(TimeManager::getInstance());
    getAudioDeviceManager().closeAudioDevice();
}

void MainContentComponent::clear(){
//    do we need to stop audio?
//    stopAudio();
    TimeManager::getInstance()->stop();
    NodeManager::getInstance()->clear();


    changed();    //fileDocument
}

void MainContentComponent::showAudioSettings()
{
    AudioDeviceSelectorComponent audioSettingsComp (getAudioDeviceManager(),
                                                    0, 256,
                                                    0, 256,
                                                    true, true, true, false);

    audioSettingsComp.setSize (500, 450);

    DialogWindow::LaunchOptions o;
    o.content.setNonOwned (&audioSettingsComp);
    o.dialogTitle                   = "Audio Settings";
    o.componentToCentreAround       = this;
    o.dialogBackgroundColour        = Colours::azure;
    o.escapeKeyTriggersCloseButton  = true;
    o.useNativeTitleBar             = false;
    o.resizable                     = false;

    o.runModal();

    ScopedPointer<XmlElement> audioState (getAudioDeviceManager().createStateXml());

    getAppProperties().getUserSettings()->setValue ("audioDeviceState", audioState);
    getAppProperties().getUserSettings()->saveIfNeeded();

}
