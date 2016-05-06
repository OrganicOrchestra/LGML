/*
 ==============================================================================

 MainComponent.cpp
 Created: 25 Mar 2016 6:11:02pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "MainComponent.h"
#include "NodeConnectionEditor.h"
#include "DebugHelpers.h"

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
	ruleManagerUI = new ControlRuleManagerUI(ControlRuleManager::getInstance());
    lgmlLoggerUI = new LGMLLoggerUI(LGMLLogger::getInstance());

	ShapeShifterPanel * timeManagerPanel = ShapeShifterManager::getInstance()->createPanel(timeManagerUI);
	ShapeShifterPanel * nodeManagerPanel = ShapeShifterManager::getInstance()->createPanel(nodeManagerUI);
	ShapeShifterPanel * controllerManagerPanel = ShapeShifterManager::getInstance()->createPanel(controllerManagerUI);
	ShapeShifterPanel * inspectorPanel = ShapeShifterManager::getInstance()->createPanel(controllableInspector);
	ShapeShifterPanel * rulesPanel = ShapeShifterManager::getInstance()->createPanel(ruleManagerUI);
    ShapeShifterPanel * logPanel = ShapeShifterManager::getInstance()->createPanel(lgmlLoggerUI);

	timeManagerPanel->setPreferredHeight(50);

	shapeShifterManager.mainContainer.insertPanelAt(timeManagerPanel, 0);

	ShapeShifterContainer * c1 = shapeShifterManager.mainContainer.insertContainerAt(ShapeShifterContainer::Direction::HORIZONTAL,1);
	
	
	ShapeShifterContainer * vc = c1->insertContainerAt(ShapeShifterContainer::VERTICAL,0);
	vc->insertPanelAt(controllerManagerPanel, 0);
	vc->insertPanelAt(rulesPanel,1);
	vc->setPreferredWidth(300);

	c1->insertPanelAt(nodeManagerPanel, 1);

	ShapeShifterContainer * vc2 = c1->insertContainerAt(ShapeShifterContainer::VERTICAL,2);
	vc2->insertPanelAt(inspectorPanel, 0);
    vc2->insertPanelAt(logPanel, 1);
	vc2->setPreferredWidth(300);




	controllerManagerPanel->setPreferredWidth(300);
	inspectorPanel->setPreferredWidth(300);
	rulesPanel->setPreferredWidth(300);


    setSize(1200, 800);

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
    DBGLOG("LGMLv" <<ProjectInfo::versionString << "\n" << "by OrganicOrchestra");
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
