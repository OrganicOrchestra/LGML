/*
 ==============================================================================

 JavascriptControllerUI.h
 Created: 7 May 2016 7:28:40am
 Author:  Martin Hermant

 ==============================================================================
 */

#ifndef JAVASCRIPTCONTROLLERUI_H_INCLUDED
#define JAVASCRIPTCONTROLLERUI_H_INCLUDED
#include "JavascriptController.h"
#include "OSCDirectControllerContentUI.h"
#include "DebugHelpers.h"

class JavascriptControllerUI : public ControllerContentUI{
public:

    JavascriptControllerUI(JsEnvironment * env){
        oscUI = new OSCDirectControllerContentUI();

        scriptUI = new ScriptUI(env);
        addAndMakeVisible(oscUI);
        addAndMakeVisible(scriptUI);
    }

    void init() override{
        oscUI->controller = controller;
        oscUI->cui = cui;
        oscUI->init();
    }
    class ScriptUI : public Component,public Button::Listener,public JsEnvironment::Listener{
    public:
        ScriptUI(JsEnvironment * _env):env(_env){
            env->addListener(this);
            loadFileB.setButtonText("Load");
            addAndMakeVisible(loadFileB);
            loadFileB.addListener(this);

            reloadB.setButtonText("Reload");
            addAndMakeVisible(reloadB);
            reloadB.addListener(this);

            openB.setButtonText("Show");
            addAndMakeVisible(openB);
            openB.addListener(this);

            logEnvB.setButtonText("Log");
            addAndMakeVisible(logEnvB);
            logEnvB.addListener(this);


            validJsLed.setFill(FillType(Colours::red));
            addAndMakeVisible(validJsLed);

        }
        ~ScriptUI(){
            env->removeListener(this);
        }

        TextButton loadFileB;
        TextButton reloadB;
        TextButton openB;
        TextButton logEnvB;
        DrawablePath validJsLed;




        void resized()override{
            Rectangle<int> area = getLocalBounds().reduced(2);
            const int logEnvSize = 30;
            const int ledSize = 10;
            const int step = (area.getWidth()- logEnvSize-ledSize)/3 ;
            buildLed(ledSize);
            validJsLed.setBounds(area.removeFromLeft(ledSize).reduced(0, (area.getHeight()-ledSize)/2));
            loadFileB.setBounds(area.removeFromLeft(step).reduced(2));
            reloadB.setBounds(area.removeFromLeft(step).reduced(2));
            openB.setBounds(area.removeFromLeft(step).reduced(2));
            logEnvB.setBounds(area.removeFromLeft(logEnvSize).reduced(2));

        }

        void buildLed( int size){
            Path circle;
            circle.addEllipse(Rectangle<float>(0,0,size,size));
            validJsLed.setPath(circle);
        }

        void newJsFileLoaded(bool s) override{
            validJsLed.setFill(FillType(s?Colours::green:Colours::red));
        }

        void buttonClicked (Button* b) override {
            if(b== &loadFileB){
                FileChooser myChooser ("Please select the script you want to load...",
                                       File::getSpecialLocation (File::userHomeDirectory),
                                       "*.js");

                if (myChooser.browseForFileToOpen())
                {
                    File script (myChooser.getResult());
                    env->loadFile(script);
                }
            }
            else if (b == &openB){
                env->showFile();
            }
            else if (b== &reloadB){
                env->reloadFile();
            }
            else if(b==&logEnvB){
                LOG(env->printAllNamespace());
            }
        };
        JsEnvironment * env;
    };


    void resized()override{
        Rectangle<int> area = getLocalBounds();
        scriptUI->setBounds(area.removeFromTop(30));
        oscUI->setBounds(area);
    }
    ScopedPointer<OSCDirectControllerContentUI> oscUI;
    ScopedPointer<ScriptUI> scriptUI;
    
    
};




#endif  // JAVASCRIPTCONTROLLERUI_H_INCLUDED
