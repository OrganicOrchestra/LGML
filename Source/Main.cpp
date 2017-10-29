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

#pragma warning( disable : 4244 )



#include "Engine.h"
#include "JuceHeader.h" // for project info

#include "Utils/CommandLineElements.hpp"

#if ENGINE_WITH_UI
    #include "UI/LookAndFeelOO.h"
    #include "UI/MainWindow.h"
#endif


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
        auto commandLinesElements = CommandLineElements::parseCommandLine (commandLine);

        if (commandLinesElements.containsCommand ("v"))
        {
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
        CommandLineElements commandLineElements = CommandLineElements::parseCommandLine (commandLine);

        if (CommandLineElement elem = commandLineElements.getCommandLineElement ("t", ""))
        {
            Array<UnitTest*> allTests = UnitTest::getAllTests();
            Array<UnitTest*> testsToRun ;

            for (auto& tName : elem.args)
            {
                bool found = false;

                for (auto& t : allTests)
                {
                    if (tName == t->getName())
                    {
                        testsToRun.add (t);
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    DBG ("no tests found for : " + tName);
                }
            }

            tstRunner.runTests (testsToRun);
        }
        else
        {
            tstRunner.runAllTests();
        }

        quit();
#else

#if ENGINE_WITH_UI
        LookAndFeel::setDefaultLookAndFeel (lookAndFeelOO = new LookAndFeelOO);
        mainWindow = new MainWindow (getApplicationName(), engine);
#endif
        engine->parseCommandline (commandLinesElements);

        if (!engine->getFile().existsAsFile())
        {
            engine->createNewGraph();
            engine->setChangedFlag (false);
        }

#endif

    }

    void shutdown() override
    {
        // Add your application's shutdown code here..
        
#if ENGINE_WITH_UI
        mainWindow = nullptr; // (deletes our window)
#endif
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

        DBG ("Another instance started !");
        AlertWindow("other instance started", "other instance started", juce::AlertWindow::AlertIconType::WarningIcon);
        engine->parseCommandline (CommandLineElements::parseCommandLine (commandLine));


    }

#if ENGINE_WITH_UI
    ScopedPointer<MainWindow> mainWindow;
    ScopedPointer<LookAndFeel> lookAndFeelOO;
#endif

};


static LGMLApplication* getApp()                 { return dynamic_cast<LGMLApplication*> (JUCEApplication::getInstance()); }
ApplicationCommandManager& getCommandManager()      { return getApp()->commandManager; }
ApplicationProperties * getAppProperties()           { return getApp()->appProperties; }
AudioDeviceManager& getAudioDeviceManager()        { return getApp()->deviceManager;}
UndoManager& getAppUndoManager()                      { return getApp()->undoManager;}
Engine* getEngine()                              { return getApp()->engine;}
ThreadPool* getEngineThreadPool()                              { return &getEngine()->threadPool;}
bool  isEngineLoadingFile()                            {if (getEngine()) {return getEngine()->isLoadingFile;} else {return false;}}
//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (LGMLApplication)

