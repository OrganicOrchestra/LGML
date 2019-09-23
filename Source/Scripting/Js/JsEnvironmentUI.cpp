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

#if !ENGINE_HEADLESS

#include "JsEnvironmentUI.h"
#include "../../Controllable/Parameter/UI/ParameterUIFactory.h"
#include "../../Controllable/Parameter/UI/StringParameterUI.h"
#include "../../Controllable/Parameter/FileParameter.h"

JsEnvironmentUI::JsEnvironmentUI (JSEnvContainer* _cont) : cont (_cont)
{
    env = cont->jsEnv;
    jassert (env);

    

    
    logEnvB = ParameterUIFactory::createDefaultUI (cont->logT);
    addAndMakeVisible (logEnvB.get());

    filePathUI = ParameterUIFactory::createDefaultUI (cont->scriptPath);
    addAndMakeVisible (filePathUI.get());


}
JsEnvironmentUI::~JsEnvironmentUI()
{
}

void JsEnvironmentUI::resized()
{
    Rectangle<int> area = getLocalBounds().reduced (2);
    const int logEnvSize = 40;
    if(auto * fp = dynamic_cast<FileParameter*>(filePathUI->parameter.get())){
        if(fp->hasValidPath(false)){
            logEnvB->setVisible(true);
            logEnvB->setBounds (area.removeFromLeft (logEnvSize).reduced (2, 0));
        }
        else{
            logEnvB->setVisible(false);
        }
    }
    filePathUI->setBounds (area.reduced (2,0));



}


#endif
