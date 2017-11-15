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

#include "JsEnvironmentUI.h"
#include "../../Controllable/Parameter/UI/ParameterUIFactory.h"
#include "../../Controllable/Parameter/UI/StringParameterUI.h"


JsEnvironmentUI::JsEnvironmentUI (JSEnvContainer* _cont) : cont (_cont)
{
    env = cont->jsEnv;
    jassert (env);
    env->addListener (this);
    loadFileB = ParameterUIFactory::createDefaultUI (cont->loadT);
    addAndMakeVisible (loadFileB);


    reloadB = ParameterUIFactory::createDefaultUI (cont->reloadT);
    addAndMakeVisible (reloadB);


    openB = ParameterUIFactory::createDefaultUI (cont->showT);
    addAndMakeVisible (openB);

    watchT = ParameterUIFactory::createDefaultUI (cont->autoWatch);
    addAndMakeVisible (watchT);
    logEnvB = ParameterUIFactory::createDefaultUI (cont->logT);
    addAndMakeVisible (logEnvB);


    path = new StringParameterUI (cont->scriptPath);
    path->setNameLabelVisible (true);
    addAndMakeVisible (path);

    newJsFileLoaded (env->hasValidJsFile());
    addAndMakeVisible (validJsLed);


}
JsEnvironmentUI::~JsEnvironmentUI()
{if(cont.get()){
    env->removeListener (this);
}
}

void JsEnvironmentUI::resized()
{
    Rectangle<int> area = getLocalBounds().reduced (1);
    const int logEnvSize = 40;
    path->setBounds (area.removeFromTop (area.getHeight() / 2).reduced (2));
    const int ledSize = area.getHeight() / 2;
    const int step = (area.getWidth() - logEnvSize - ledSize) / 4;
    buildLed (ledSize);

    area.reduce (0, 2);
    validJsLed.setBounds (area.removeFromLeft (ledSize).reduced (0, (area.getHeight() - ledSize) / 2));
    loadFileB->setBounds (area.removeFromLeft (step).reduced (2, 0));
    reloadB->setBounds (area.removeFromLeft (step).reduced (2, 0));
    openB->setBounds (area.removeFromLeft (step).reduced (2, 0));
    watchT->setBounds (area.removeFromLeft (step).reduced (2, 0));
    logEnvB->setBounds (area.removeFromLeft (logEnvSize).reduced (2, 0));

}

void JsEnvironmentUI::buildLed (int size)
{
    Path circle;
    circle.addEllipse (Rectangle<float> (0, 0, (float)size, (float)size));
    validJsLed.setPath (circle);
}

void JsEnvironmentUI::newJsFileLoaded (bool)
{
    validJsLed.setFill (FillType ((env->hasValidJsFile() && env->isInSyncWithLGML()) ? Colours::green :
                                  (env->hasValidJsFile() ? Colours::orange :
                                   Colours::red)));
}
