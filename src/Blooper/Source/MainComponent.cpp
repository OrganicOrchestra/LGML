/*
 ==============================================================================

 MainComponent.cpp
 Created: 25 Mar 2016 6:11:02pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "MainComponent.h"


// (This function is called by the app startup code to create our main component)
MainContentComponent* createMainContentComponent(Engine * e)     { return new MainContentComponent(e); }


MainContentComponent::MainContentComponent(Engine * e):engine(e)
{
    
    setLookAndFeel(lookAndFeelOO = new LookAndFeelOO);

    DBG("Application Start");


    



    timeManagerUI = new TimeManagerUI(TimeManager::getInstance());
    nodeManagerUI = new NodeManagerUI(NodeManager::getInstance());
    nodeManagerUIViewport=new NodeManagerUIViewport(nodeManagerUI);


    addAndMakeVisible(timeManagerUI);
    addAndMakeVisible(nodeManagerUIViewport);



    controllerManagerViewport = new ControllerManagerViewport(engine->controllerManager);
    addAndMakeVisible(controllerManagerViewport);


    controllableInspector = new ControllableInspector(nodeManagerUI);
    controllableInspectorViewPort = new ControllableInspectorViewPort(controllableInspector);
    addAndMakeVisible(controllableInspectorViewPort);

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

    e->createNewGraph();


}



MainContentComponent::~MainContentComponent(){

#if JUCE_MAC
    setMacMainMenu (nullptr);
#else
    //setMenuBar (nullptr);

#endif
//    LookAndFeelOO::deleteInstance();
}



void MainContentComponent::resized()
{
    Rectangle<int> r = getLocalBounds();
    timeManagerUI->setBounds(r.removeFromTop(20));
    controllerManagerViewport->setBounds(r.removeFromLeft(300));
    
    
    controllableInspector->setSize(300,controllableInspector->getHeight());
    controllableInspectorViewPort->setBounds(r.removeFromRight(300));
    nodeManagerUIViewport->setBounds(r);
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
