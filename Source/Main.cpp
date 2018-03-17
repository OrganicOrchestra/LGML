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


#include "Utils/CommandLineElements.hpp"

#if !ENGINE_HEADLESS
#include "UI/LookAndFeelOO.h"
#include "UI/MainWindow.h"
#endif



class CrashHandler{
public:
    static bool saveToFile;
    static void init(){
        if(getCrashFile().existsAsFile()){
            ScopedPointer<InputStream> in = getCrashFile().createInputStream();
            if(in){
                String bt = in->readString();
                LOG(bt);
                #if !ENGINE_HEADLESS
                AlertWindow aw("lgml crashed last time","error message : ",AlertWindow::AlertIconType::WarningIcon);
                aw.addTextBlock(bt);
                aw.addButton("Ok", 1);
                aw.addButton("Copy to clipboard", 2);
                auto res = aw.runModalLoop();
                if(res==2){
                    SystemClipboard::copyTextToClipboard(bt);
                }
                #endif
            }
            getCrashFile().deleteFile();
        }
    }

    static File & getCrashFile(){
        static auto exportFolder = File::getSpecialLocation(File::SpecialLocationType::tempDirectory);
        static File crashLog =exportFolder.getChildFile("lgml_error.log");
        if(crashLog.existsAsFile() && !crashLog.hasWriteAccess()){
            LOGE(juce::translate("can't write to log ") << crashLog.getFullPathName());
            jassertfalse;

        }
        return crashLog;
    }
    static void triggerCrash(){abort();}

    static void report(void *p){

        auto bt = SystemStats::getStackBacktrace();
        if(saveToFile){
        ScopedPointer<OutputStream> out ( getCrashFile().createOutputStream());
        if(out){
            if(!out->writeString(bt))
                LOGE(juce::translate("Crash Reporter can't write to file"));
        }
        }
        LOG(bt);


    }
};
bool CrashHandler::saveToFile = true;


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


    const String getApplicationName() override       { return Engine::projectName; }
    const String getApplicationVersion() override    { return Engine::versionString; }
    bool moreThanOneInstanceAllowed() override       { return false; }

    //==============================================================================
    void initialise (const String& commandLine) override
    {
        CrashHandler::init();
        SystemStats::setApplicationCrashHandler(&CrashHandler::report);

        // This method is where you should put your application's initialisation code..
        auto commandLinesElements = CommandLineElements::parseCommandLine (commandLine);

        if (commandLinesElements.containsCommand ("v"))
        {
            std::cout << Engine::versionString << std::endl;
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

#if !ENGINE_HEADLESS
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
                CrashHandler::saveToFile = false;
#if !ENGINE_HEADLESS
                mainWindow = nullptr; // (deletes our window)
                if(LGMLDragger::getInstanceWithoutCreating()){LGMLDragger::deleteInstance();}
#endif
                engine = nullptr;
            }

            //==============================================================================
            void systemRequestedQuit() override
            {
                // This is called when the app is being asked to quit: you can ignore this
                // request and let the app carry on running, or call quit() to allow the app to close.
                CrashHandler::saveToFile = false;
                quit();
            }

            void anotherInstanceStarted (const String& commandLine) override
            {
                // When another instance of the app is launched while this one is running,
                // this method is invoked, and the commandLine parameter tells you what
                // the other instance's command-line arguments were.

                bool isMacFileCallback =
                        commandLine.startsWithChar (File::getSeparatorChar())
                        && File(commandLine).existsAsFile();
                
                
                if(!isMacFileCallback){
                    DBG ("Log recieved command args : "<<commandLine);
                }
                    engine->parseCommandline (CommandLineElements::parseCommandLine (commandLine));
//                }
//                else{

#if !ENGINE_HEADLESS
//                    AlertWindow::showMessageBox(AlertWindow::AlertIconType::WarningIcon,"other instance started", commandLine);
#endif
//                }


            }

#if !ENGINE_HEADLESS
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
            
