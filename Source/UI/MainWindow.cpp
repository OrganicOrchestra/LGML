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

#if !ENGINE_HEADLESS

#include "MainWindow.h"
#include "MainComponent.h"
#include "../Engine.h"


#include "../Utils/AutoUpdater.h"


MainContentComponent* createMainContentComponent (Engine* e);
MainWindow::~MainWindow(){
    LGMLDragger::deleteInstance();
    stopTimer();
    latestVChecker = nullptr;
    getAppUndoManager().removeChangeListener(this);
}


MainWindow::MainWindow (String name, Engine* e)  :
DocumentWindow (name,Colours::lightgrey,DocumentWindow::allButtons),
latestVChecker(nullptr)
{

    getAppUndoManager().addChangeListener(this);
    mainComponent = createMainContentComponent (e);
    setContentOwned (mainComponent, false);
    // shapeshifter does'nt handle well if size are really small (under its min Size)
    setResizeLimits(200, 200, 4096, 4096);
    getConstrainer()->setMinimumOnscreenAmounts(100, 100, 100, 100);

#ifdef JUCE_LINUX
    /* lots of bug with nativetitlebar on ubuntu
     - no display
     - wrong rebuilding of windows position / size
     - double clicks sent to titlebar
     */
    setUsingNativeTitleBar (false);

#else
    setUsingNativeTitleBar (true);
#endif
    setResizable (true, false);

#if ! JUCE_MAC
    setMenuBar (mainComponent);
#endif

#if USE_GL
    openGLContext.setContinuousRepainting (false);
    openGLContext.attachTo (*getTopLevelComponent());
#endif


    // need to stay after ll init function for linux
#ifndef JUCE_LINUX
    String winSetting ( "fs 0 0 800 600");
#else
    // weird behaviour of fullscreen in ubuntu (can't get out of fs mode and wrong windows coords)
    String winSetting ( "0 0 800 600");
#endif


    if (auto prop = getAppProperties()->getCommonSettings (true))
    {
        winSetting = prop->getValue ("winSettings", winSetting);
    }

    ResizableWindow::restoreWindowStateFromString (winSetting);



    setVisible (true);



    ShapeShifterManager::getInstance()->loadLastSessionLayoutFile();
    LGMLDragger::getInstance()->setMainComponent (mainComponent);


    startTimer (4000);


}
void MainWindow::focusGained (FocusChangeType /*cause*/)
{
    //mainComponent->grabKeyboardFocus();

}

void MainWindow::changeListenerCallback (ChangeBroadcaster* /*source*/) {
    getEngine()->setChangedFlag(getAppUndoManager().canUndo());
};


void MainWindow::closeButtonPressed()
{
    // This is called when the user tries to close this window. Here, we'll just
    // ask the app to quit when this happens, but you can change this to do
    // whatever you need.

    if(getEngine()->hasChangedSinceSaved()){
        int result = AlertWindow::showYesNoCancelBox (AlertWindow::QuestionIcon, juce::translate("Save document"), juce::translate("Do you want to save the document before quitting ?"));

        if (result == 0)  return; //prevent exit

        if (result == 1)
        {
            juce::FileBasedDocument::SaveResult sr = getEngine()->save (true, true);

            switch (sr)
            {
                case juce::FileBasedDocument::SaveResult::userCancelledSave:
                case juce::FileBasedDocument::SaveResult::failedToWriteToFile:
                    return;

                case FileBasedDocument::SaveResult::savedOk:
                    break;
            }
        }
    }

    var boundsVar = var (new DynamicObject());
    Rectangle<int> r = getScreenBounds();

    getAppProperties()->getCommonSettings (true)->setValue ("winSettings", getWindowStateAsString());
    getAppProperties()->getCommonSettings (true)->saveIfNeeded();


#if USE_GL
    openGLContext.detach();
#endif
    JUCEApplication::getInstance()->systemRequestedQuit();

}

void MainWindow::timerCallback()
{
    const int timeToUpdate = getAppProperties()->getUserSettings()->getBoolValue("check for updates",true)?60000:-1;
    const int curT = getEngine()->getElapsedMillis();
    if(latestVChecker==nullptr && curT < timeToUpdate){
        latestVChecker = new LatestVersionChecker();
    }
    if(latestVChecker && curT>timeToUpdate){
        latestVChecker = nullptr;
    }
    setName (getEngine()->getDocumentTitle() + " : LGML "
             + Engine::versionString + String (" (CPU : ") +
             String ((int) getEngine()->engineStats->getAudioCPU()) + String ("%)"));
}



#endif
