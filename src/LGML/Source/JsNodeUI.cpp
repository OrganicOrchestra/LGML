/*
  ==============================================================================

    JsNodeEditor.cpp
    Created: 28 May 2016 2:18:27pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "JsNodeUI.h"

#include "JsNode.h"
void JsNodeUI::init(){
    JsNode * jsEnv = (JsNode*) node;
    jsUI = new JsEnvironmentUI((JsEnvironment*)jsEnv);
    addAndMakeVisible(jsUI);


};