/*
 ==============================================================================

 JsNodeEditor.cpp
 Created: 28 May 2016 2:18:27pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "JsNodeUI.h"

#include "JsNode.h"

JsNodeUI::JsNodeUI(){};
JsNodeUI::~JsNodeUI(){
    if( node.get()){
        node->removeControllableContainerListener(this);
        ((JsNode*)node.get())->JsEnvironment::removeListener(this);
    };

};

void JsNodeUI::init(){
    JsNode * jsNode = (JsNode*) node.get();
    jsUI = new JsEnvironmentUI(((JsEnvironment*)jsNode)->jsParameters);
    jsNode->addControllableContainerListener(this);

    addAndMakeVisible(jsUI);


    jsNode->JsEnvironment::addListener(this);
    if(jsNode->hasValidJsFile()){
        newJsFileLoaded(true);
    }
  setDefaultSize(350, 150);

};

void JsNodeUI::newJsFileLoaded(bool v){
    JsNode * jsNode = (JsNode*) node.get();
    varUI.clear();
    if(v){
        for(auto &c:jsNode->jsParameters){
            controllableAdded(c);
        }
    }
};

void JsNodeUI::resized(){
    layoutUI();
}

void JsNodeUI::layoutUI(){
    Rectangle<int> area = getLocalBounds().reduced(2);
    jsUI->setBounds(area.removeFromTop(30));
    for(auto & comp:varUI){
        comp->setBounds( area.removeFromTop(20).reduced(2));
    }

}

void JsNodeUI::controllableAdded(Controllable * c) {
    JsNode * jsNode = (JsNode*) node.get();
    if(!jsNode->jsParameters.contains((Controllable*)c))return;
    ControllableUI * comp = new NamedControllableUI(c->createDefaultUI(), 100);
    varUI.add(comp);
    addAndMakeVisible(comp);
    if(varUI.size() * 20>getHeight()-jsUI->getHeight()){
        setSize( getWidth(), varUI.size() * 20 + 5 + jsUI->getHeight());
    }
    else{
        layoutUI();
    }

}
void JsNodeUI::controllableRemoved(Controllable * c) {

    for(auto & comp:varUI){
        if(comp->controllable == c){
            removeChildComponent(comp);
            varUI.removeObject(comp);
            break;
        }

    }


    layoutUI();


};
