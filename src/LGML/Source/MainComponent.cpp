/*
 ==============================================================================

 MainComponent.cpp
 Created: 25 Mar 2016 6:11:02pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "MainComponent.h"
#include "Engine.h"



// (This function is called by the app startup code to create our main component)
MainContentComponent* createMainContentComponent(Engine * e)
{
    return new MainContentComponent(e);
}


MainContentComponent::MainContentComponent(Engine * e):
    engine(e)
{

    setLookAndFeel(lookAndFeelOO = new LookAndFeelOO);

	addAndMakeVisible(&ShapeShifterManager::getInstance()->mainContainer);

	ShapeShifterManager::getInstance()->loadLastSessionLayoutFile();

    (&getCommandManager())->registerAllCommandsForTarget (this);
    (&getCommandManager())-> setFirstCommandTarget(this);

    (&getCommandManager())->getKeyMappings()->resetToDefaultMappings();
//    (&getCommandManager())->getKeyMappings()->restoreFromXml (lastSavedKeyMappingsXML);
    addKeyListener ((&getCommandManager())->getKeyMappings());
#if JUCE_MAC
    setMacMainMenu (this,nullptr,"");
#else
    //setMenu (this); //done in Main.cpp as it's a method of DocumentWindow
#endif


    e->createNewGraph();
	e->setChangedFlag(false);

    //e->initAudio();

	setSize((int)(getParentMonitorArea().getWidth()*.9f), (int)(getParentMonitorArea().getHeight()*.6f));
}



MainContentComponent::~MainContentComponent(){

#if JUCE_MAC
    setMacMainMenu (nullptr);
#else
    //setMenuBar (nullptr);

#endif
//    LookAndFeelOO::deleteInstance();

	//DBG("Clear inspector");

	ShapeShifterManager::deleteInstance();
}



void MainContentComponent::resized()
{
	Rectangle<int> r = getLocalBounds();
	//timeManagerUI->setBounds(r.removeFromTop(25));
	//DBG("Resized in main component :" << getLocalBounds().toString());

	ShapeShifterManager::getInstance()->mainContainer.setBounds(r);
}

void MainContentComponent::showAudioSettings()
{
    AudioDeviceSelectorComponent audioSettingsComp(getAudioDeviceManager(),
                                                   0, 256,
                                                   0, 256,
                                                   true,true, false,false);
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

    engine->audioSettingsHandler.saveCurrent();


}
