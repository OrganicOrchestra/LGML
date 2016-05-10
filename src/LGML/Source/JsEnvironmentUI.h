/*
  ==============================================================================

    JsEnvironmentUI.h
    Created: 10 May 2016 9:33:22am
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef JSENVIRONMENTUI_H_INCLUDED
#define JSENVIRONMENTUI_H_INCLUDED


#include "DebugHelpers.h"
#include "JsEnvironment.h"

class JsEnvironmentUI : public Component,public Button::Listener,public JsEnvironment::Listener{
public:
    JsEnvironmentUI(JsEnvironment * _env):env(_env){
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

        watchT.setButtonText("autoWatch");
        watchT.setClickingTogglesState(true);
        addAndMakeVisible(watchT);
        watchT.addListener(this);

        validJsLed.setFill(FillType(Colours::red));
        addAndMakeVisible(validJsLed);

    }
    ~JsEnvironmentUI(){
        env->removeListener(this);
    }

    TextButton loadFileB;
    TextButton reloadB;
    TextButton openB;
    TextButton logEnvB;
    TextButton watchT;
    DrawablePath validJsLed;




    void resized()override{
        Rectangle<int> area = getLocalBounds().reduced(2);
        const int logEnvSize = 30;
        const int ledSize = 10;
        const int step = (area.getWidth()- logEnvSize-ledSize)/4 ;
        buildLed(ledSize);
        validJsLed.setBounds(area.removeFromLeft(ledSize).reduced(0, (area.getHeight()-ledSize)/2));
        loadFileB.setBounds(area.removeFromLeft(step).reduced(2));
        reloadB.setBounds(area.removeFromLeft(step).reduced(2));
        openB.setBounds(area.removeFromLeft(step).reduced(2));
        watchT.setBounds(area.removeFromLeft(step).reduced(2));
        logEnvB.setBounds(area.removeFromLeft(logEnvSize).reduced(2));

    }

    void buildLed( int size){
        Path circle;
        circle.addEllipse(Rectangle<float>(0,0,(float)size, (float)size));
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
        else if(b==&watchT){
            env->setAutoWatch(watchT.getToggleState());
        }
    };
    JsEnvironment * env;
};



#endif  // JSENVIRONMENTUI_H_INCLUDED
