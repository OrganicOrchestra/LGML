/*
  ==============================================================================

    JsNode.cpp
    Created: 28 May 2016 2:00:46pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "JsNode.h"

#include "JsNodeUI.h"
#include "Engine.h"
Engine & getEngine();

ConnectableNodeUI * JsNode::createUI()
{

    NodeBaseUI * ui = new NodeBaseUI(this,new JsNodeUI());
    return ui;

}



void JsNode::newJsFileLoaded(){
    
    String relativePath = currentFile.getRelativePathFrom(File::getCurrentWorkingDirectory());//currentFile.getFullPathName()
    scriptPath->setValue(relativePath);
}