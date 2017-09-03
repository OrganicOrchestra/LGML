/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/



#pragma warning( disable : 4244 )

#include "MainComponent.h"
#include "Engine.h"
#include "LGMLDragger.h"
#include "CommandLineElements.hpp"

MainContentComponent* createMainContentComponent(Engine* e);

//==============================================================================
class LGMLApplication : public JUCEApplication
{
public:
  //==============================================================================
  LGMLApplication() {}

  ApplicationCommandManager commandManager;
  ScopedPointer<ApplicationProperties> appProperties;
  AudioDeviceManager deviceManager;
  UndoManager undoManager;

  ScopedPointer<Engine> engine;


  const String getApplicationName() override       { return ProjectInfo::projectName; }
  const String getApplicationVersion() override    { return ProjectInfo::versionString; }
  bool moreThanOneInstanceAllowed() override       { return false; }

  //==============================================================================
  void initialise (const String& commandLine) override
  {
    // This method is where you should put your application's initialisation code..
    auto commandLinesElements = CommandLineElements::parseCommandLine(commandLine);
    if(commandLinesElements.containsCommand("v")){
      std::cout << ProjectInfo::versionString << std::endl;
      quit();
      return;
    }
    PropertiesFile::Options options;
    options.applicationName     = "LGML";
    options.filenameSuffix      = "settings";
    options.osxLibrarySubFolder = "Preferences";

    appProperties = new ApplicationProperties();
    appProperties->setStorageParameters (options);

	


	Process::setPriority (Process::HighPriority);

    engine = new Engine();
#if LGML_UNIT_TESTS

    UnitTestRunner tstRunner;
    CommandLineElements commandLineElements = CommandLineElements::parseCommandLine(commandLine);
    if(CommandLineElement elem = commandLineElements.getCommandLineElement("t","")){
      Array<UnitTest*> allTests = UnitTest::getAllTests();
      Array<UnitTest*> testsToRun ;
      for(auto &tName:elem.args){
        bool found = false;
        for(auto & t:allTests){
          if(tName==t->getName()){
            testsToRun.add(t);
            found = true;
            break;
          }
        }
        if(!found){
          DBG("no tests found for : "+tName);
        }
      }
      tstRunner.runTests(testsToRun);
    }
    else{
      tstRunner.runAllTests();
    }
    quit();
#else


    mainWindow = new MainWindow (getApplicationName(),engine);
      
    engine->parseCommandline(commandLinesElements);
    if(!engine->getFile().existsAsFile()){
      engine->createNewGraph();
      engine->setChangedFlag(false);
    }
#endif

  }

  void shutdown() override
  {
    // Add your application's shutdown code here..


    mainWindow = nullptr; // (deletes our window)
    engine = nullptr;
  }

  //==============================================================================
  void systemRequestedQuit() override
  {
    // This is called when the app is being asked to quit: you can ignore this
    // request and let the app carry on running, or call quit() to allow the app to close.
    quit();
  }

  void anotherInstanceStarted (const String& commandLine) override
  {
    // When another instance of the app is launched while this one is running,
    // this method is invoked, and the commandLine parameter tells you what
    // the other instance's command-line arguments were.

    DBG("Another instance started !");
    engine->parseCommandline(CommandLineElements::parseCommandLine(commandLine));


  }

  //==============================================================================
  /*
   This class implements the desktop window that contains an instance of
   our MainContentComponent class.
   */
  class MainWindow    : public DocumentWindow, private Timer
  {
  public:
    MainWindow (String name,Engine * e)  : DocumentWindow (name,
                                                           Colours::lightgrey,
                                                           DocumentWindow::allButtons)
    {
      startTimer(1000);

      

	  mainComponent = createMainContentComponent(e);
	  setContentOwned(mainComponent, false);
	  setResizable(true, true);


      setUsingNativeTitleBar(true);

      
#if ! JUCE_MAC
      setMenuBar(mainComponent);
#endif

#if JUCE_OPENGL 
      openGLContext.setContinuousRepainting(false);
      openGLContext.attachTo(*getTopLevelComponent());
#endif


      // need to stay after ll init function for linux
#ifndef JUCE_LINUX
      String winSetting ( "fs 0 0 800 600");
#else
      // weird behaviour of fullscreen in linux (can't get out of fs mode)
      String winSetting ( "0 0 800 600");
#endif


      if(auto prop = getAppProperties().getCommonSettings(true)){
        winSetting = prop->getValue("winSettings",winSetting);
      }
      ResizableWindow::restoreWindowStateFromString(winSetting);


      
      setVisible (true);



	  ShapeShifterManager::getInstance()->loadLastSessionLayoutFile();
      LGMLDragger::getInstance()->setMainComponent(mainComponent,&mainComponent->tooltipWindow);
      

    }
    void focusGained(FocusChangeType cause)override{
		//mainComponent->grabKeyboardFocus();
    }

    void closeButtonPressed() override
    {
      // This is called when the user tries to close this window. Here, we'll just
      // ask the app to quit when this happens, but you can change this to do
      // whatever you need.

      //@martin added but commented for testing (relou behavior)
      int result = AlertWindow::showYesNoCancelBox(AlertWindow::QuestionIcon, "Save document", "Do you want to save the document before quitting ?");

       if (result == 0)  return; //prevent exit
       
	   if (result == 1)
	   {
		   juce::FileBasedDocument::SaveResult sr = ((LGMLApplication *)LGMLApplication::getInstance())->engine->save(true, true);
		   switch (sr)
		   {
		   case juce::FileBasedDocument::SaveResult::userCancelledSave:
		   case juce::FileBasedDocument::SaveResult::failedToWriteToFile:
				   return;

         case FileBasedDocument::SaveResult::savedOk:
           break;
		   }
	   }


	   var boundsVar = var(new DynamicObject());
	   Rectangle<int> r = getScreenBounds();

      getAppProperties().getCommonSettings(true)->setValue("winSettings",getWindowStateAsString());
	   getAppProperties().getCommonSettings(true)->saveIfNeeded();


#if JUCE_OPENGL 
	   openGLContext.detach();
#endif
       JUCEApplication::getInstance()->systemRequestedQuit();

    }

    void timerCallback() override;

    /* Note: Be careful if you override any DocumentWindow methods - the base
     class uses a lot of them, so by overriding you might break its functionality.
     It's best to do all your work in your content component instead, but if
     you really have to override any DocumentWindow methods, make sure your
     subclass also calls the superclass's method.
     */
    MainContentComponent * mainComponent;

#if JUCE_OPENGL 
	OpenGLContext openGLContext;
#endif

  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
  };

private:
		ScopedPointer<MainWindow> mainWindow;


};


static LGMLApplication* getApp()                 { return dynamic_cast<LGMLApplication*>(JUCEApplication::getInstance()); }
ApplicationCommandManager& getCommandManager()      { return getApp()->commandManager; }
ApplicationProperties& getAppProperties()           { return *getApp()->appProperties; }
AudioDeviceManager & getAudioDeviceManager()        { return getApp()->deviceManager;}
UndoManager & getAppUndoManager()                      { return getApp()->undoManager;}
Engine * getEngine()                              { return getApp()->engine;}
ThreadPool * getEngineThreadPool()                              { return &getApp()->engine->threadPool;}
bool  isEngineLoadingFile()                            {if(getEngine()) {return getEngine()->isLoadingFile;}else{return false;}}
//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (LGMLApplication)

void LGMLApplication::MainWindow::timerCallback()
{
  setName(getApp()->engine->getDocumentTitle() +" : LGML "
          + String(ProjectInfo::versionString)+String(" (CPU : ")+
          String((int)(getAudioDeviceManager().getCpuUsage() * 100))+String("%)"));
}
