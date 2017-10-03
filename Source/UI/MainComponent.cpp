/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#include "MainComponent.h"
#include "AppPropertiesUI.h"

//#include "../Node/Manager/UI/NodeManagerUI.h"
//#include "../Controller/UI/ControllerManagerUI.h"


// (This function is called by the app startup code to create our main component)
MainContentComponent* createMainContentComponent (Engine* e)
{
    return new MainContentComponent (e);
}


MainContentComponent::MainContentComponent (Engine* e):
    engine (e)
{
    engine->addEngineListener (this);



    addAndMakeVisible (&ShapeShifterManager::getInstance()->mainShifterContainer);


    (&getCommandManager())->registerAllCommandsForTarget (this);
    (&getCommandManager())-> setFirstCommandTarget (this);

    (&getCommandManager())->getKeyMappings()->resetToDefaultMappings();
    //    (&getCommandManager())->getKeyMappings()->restoreFromXml (lastSavedKeyMappingsXML);
    addKeyListener ((&getCommandManager())->getKeyMappings());
#if JUCE_MAC
    setMacMainMenu (this, nullptr, "Open recent file");
#else
    //setMenu (this); //done in Main.cpp as it's a method of DocumentWindow
#endif



    setWantsKeyboardFocus (true);
    setOpaque (true);
    tooltipWindow.setOpaque (false);


}



MainContentComponent::~MainContentComponent()
{

    AppPropertiesUI::closeAppSettings();
#if JUCE_MAC
    setMacMainMenu (nullptr);

#endif

    engine->removeEngineListener (this);
    ShapeShifterManager::deleteInstance();

}

void MainContentComponent::focusGained (FocusChangeType cause)
{

    ShapeShifterManager* sm = ShapeShifterManager::getInstanceWithoutCreating();

    if (sm)
    {
        ShapeShifterPanel* nm = ShapeShifterManager::getInstance()->getPanelForContent (ShapeShifterManager::getInstance()->getContentForName (NodeManagerPanel));

        if (nm)
        {
            nm->grabKeyboardFocus();
        }
    }
}


void MainContentComponent::resized()
{
    Rectangle<int> r = getLocalBounds();
    //timeManagerUI->setBounds(r.removeFromTop(25));
    //DBG("Resized in main component :" << getLocalBounds().toString());

    ShapeShifterManager::getInstance()->mainShifterContainer.setBounds (r);

}


void MainContentComponent::paintOverChildren (Graphics& g)
{
    /*
     if(engine->isLoadingFile){
     g.setColour(Colours::black.withAlpha(0.4f));
     g.fillAll();

     int period = 4000.0;
     float time = (Time::currentTimeMillis()%period)*1.0/period;
     Point<int> center = getLocalBounds().getCentre();
     int numPoints = 10;
     float radius = 300;
     float pSize =40;
     float aStep = 2*float_Pi/numPoints ;
     for(int i = 0 ; i < numPoints ; i++){
     g.setColour(Colours::white.withAlpha(float(1.0+cos((time+i*0.25/numPoints)*2*float_Pi))/2.0f));
     g.fillEllipse(center.x + radius*cos(i*aStep),center.y+radius*sin(i*aStep),pSize,pSize);
     }
     }
     */
};

void MainContentComponent::paint (Graphics& g)
{
    g.fillAll (findColour (ResizableWindow::backgroundColourId).darker());
}

void MainContentComponent::startLoadFile()
{

    // clear graphics to gain time when deleting objects (and to ease thread safety))
    //  if(NodeManagerUIViewport * vp = (NodeManagerUIViewport*)ShapeShifterManager::getInstance()->getContentForName(NodeManagerPanel)){
    //    if(NodeContainerViewer *v = vp->nmui->currentViewer){
    //      v->clear();
    //    }
    //  }
    //  if(ControllerManagerUIViewport * cui = (ControllerManagerUIViewport*)ShapeShifterManager::getInstance()->getContentForName(ControllerPanel)){
    //    cui->controllerManagerUI->clear();
    //  }

    if (fileProgressWindow != nullptr)
    {
        removeChildComponent (fileProgressWindow);
        fileProgressWindow = nullptr;
    }

    fileProgressWindow = new ProgressWindow ("Loading File...", engine);
    addAndMakeVisible (fileProgressWindow);
    fileProgressWindow->setSize (getWidth(), getHeight());
    //  startTimerHz(10);
    //repaint();
}

void MainContentComponent::fileProgress (float percent, int state)
{
    // not implemented
    DBG ("File progress, " << percent);

    if (fileProgressWindow != nullptr)
    {
        fileProgressWindow->setProgress (percent);
    }
    else
    {
        DBG ("Window is null but still got progress");
    }
};

void MainContentComponent::endLoadFile()
{

    if (fileProgressWindow != nullptr)
    {
        removeChildComponent (fileProgressWindow);
        fileProgressWindow = nullptr;
    }

    //stopTimer();
    //repaint();
};

void MainContentComponent::timerCallback()
{
    //  if(!engine->isLoadingFile){
    //    stopTimer();
    //  }

    //repaint();
}
