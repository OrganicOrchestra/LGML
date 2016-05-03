/*
 ==============================================================================

 MainComponent.cpp
 Created: 25 Mar 2016 6:11:02pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "MainComponent.h"
#include "NodeConnectionEditor.h"

// (This function is called by the app startup code to create our main component)
MainContentComponent* createMainContentComponent(Engine * e)
{
    return new MainContentComponent(e);
}


MainContentComponent::MainContentComponent(Engine * e):
    engine(e),
    audioSettingsComp(getAudioDeviceManager(),
        0, 256,
        0, 256,
        false,false, false,false)
{

    setLookAndFeel(lookAndFeelOO = new LookAndFeelOO);

    DBG("Application Start");

	addAndMakeVisible(shapeShifterManager.mainContainer);

    timeManagerUI = new TimeManagerUI(TimeManager::getInstance());
    nodeManagerUI = new NodeManagerUI(NodeManager::getInstance());
	controllerManagerUI = new ControllerManagerUI(ControllerManager::getInstance());

	controllableInspector = new ControllableInspector(nodeManagerUI); //Needs to be abstracted from NodeManager, and be able to inspect any ControllableContainer
	
	//Shape Shifter initialization
	ShapeShifterContainer * c1 = shapeShifterManager.mainContainer.insertContainerAt(0,ShapeShifterContainer::ContentType::PANELS,ShapeShifterContainer::Direction::HORIZONTAL);
	c1->setPreferredHeight(45);

	ShapeShifterContainer * c2 = shapeShifterManager.mainContainer.insertContainerAt(1, ShapeShifterContainer::ContentType::PANELS, ShapeShifterContainer::Direction::HORIZONTAL);

	ShapeShifterPanel * timeManagerPanel = ShapeShifterManager::getInstance()->createPanel(timeManagerUI);
	ShapeShifterPanel * nodeManagerPanel = ShapeShifterManager::getInstance()->createPanel(nodeManagerUI);

	ShapeShifterPanel * controllerManagerPanel = ShapeShifterManager::getInstance()->createPanel(controllerManagerUI);
	controllerManagerPanel->setPreferredWidth(300);

	controllerManagerPanel->addContent(controllableInspector);
	//ShapeShifterPanel * controllableInspectorPanel = ShapeShifterManager::getInstance()->createPanel(controllableInspector);
	//controllableInspectorPanel->setPreferredWidth(200);


	c1->insertPanelAt(timeManagerPanel, 0);
	c2->insertPanelAt(controllerManagerPanel, 0);
	c2->insertPanelAt(nodeManagerPanel, 1);
	//c2->insertPanelAt(controllableInspectorPanel, 2);

    // resize after contentCreated

    setSize(1500, 750);

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
    //e->initAudio();

}



MainContentComponent::~MainContentComponent(){

#if JUCE_MAC
    setMacMainMenu (nullptr);
#else
    //setMenuBar (nullptr);

#endif
//    LookAndFeelOO::deleteInstance();


    NodeConnectionEditor::deleteInstance();
	ShapeShifterManager::deleteInstance();
}



void MainContentComponent::resized()
{
	Rectangle<int> r = getLocalBounds();
	//timeManagerUI->setBounds(r.removeFromTop(25));
	shapeShifterManager.mainContainer.setBounds(r);
}

void MainContentComponent::showAudioSettings()
{
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
