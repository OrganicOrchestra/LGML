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

#include "NodeManagerUI.h"
#include "../NodeManager.h"
#include "../../../Controllable/Parameter/ParameterContainerSync.h"
#include "../../../Controllable/ControllableUIHelpers.h"

class UISync:public ParameterContainerSync{
public:
    UISync(const String &n,ParameterContainer * c,ParameterContainer *p) : ParameterContainerSync(n,p){
        setRoot(c);
    }

    ParameterContainer* createContainerFromContainer(ParameterContainer * o) override{
        ParameterContainer* res (nullptr);
        if(dynamic_cast<ConnectableNode*>(o)){
            auto pc = new ConnectableNodeUIParams(o->shortName);
//            all.add(pc);
            res = pc;
        }
        return res;
    }
//    OwnedArray<ConnectableNodeUIParams> all;


};


class NodeManagerUIFactory : public ParameterContainer{
public:
    NodeManagerUIFactory():ParameterContainer("NodesUI"){
        canHaveUserDefinedContainers = true;
        nameParam->setInternalOnlyFlags(true,false);

    }

    ParameterContainer* addContainerFromObject (const String& name, DynamicObject*   data) override{
        ParameterContainer * res;
        if(auto c = getControllableContainerByName("NodeManagerUI")){
            res = dynamic_cast<NodeManagerUI*>(c);
        }
        else{
            res= new NodeManagerUI(NodeManager::getInstance());
            addChildControllableContainer(res);
        }
        if(res){
            res->configureFromObject(data);
        }
        return res;
    };
};

//==============================================================================
juce_ImplementSingleton(NodeManagerUI);
NodeManagerUI::NodeManagerUI (NodeManager* _nodeManager) :
    nodeManager (_nodeManager),
    currentViewer (nullptr),
ParameterContainer("NodeManagerUI"),
isMiniMode(false)
{
    canHaveUserDefinedContainers = true;
    
    uiSync = new UISync("UI",nodeManager,this);
    auto p =getRoot(true)->getControllableContainerByName("NodesUI");
    if(!p){
        NodeManagerUIFactory * np = new NodeManagerUIFactory();
        p = getRoot(true)->addChildControllableContainer(np);
    }
    p->addChildControllableContainer(this);
    nodeManager->addNodeManagerListener (this);
    execOrDefer([this](){setCurrentViewedContainer (nodeManager);});
    gridSize = addNewParameter<IntParameter>("gridSize","grid size to snap elements to",10,0,100);
//    nodeManager->parentContainer->addChildControllableContainer(uiSync->getSlaveContainer());

}

NodeManagerUI::~NodeManagerUI()
{

    
    nodeManager->removeNodeManagerListener (this);
    clear();
    setCurrentViewedContainer (nullptr);

    if(auto root = getRoot(true)){

    auto p = root->getControllableContainerByName("NodesUI");
    if(p){
        p->removeChildControllableContainer(this);
        root->removeChildControllableContainer(p);
        p->clearContainer(true);
        delete p;
    }
    else jassertfalse;
    }


    clearSingletonInstance();
}

void NodeManagerUI::clear()
{
    setCurrentViewedContainer (nullptr);
}

void NodeManagerUI::resized()
{



    if (currentViewer != nullptr)
    {
        Rectangle<int> r = getLocalBounds();
        currentViewer->setBounds(r);
        //currentViewer->setTopLeftPosition (0, 0);
        //currentViewer->setSize (jmax<int> (getWidth(), currentViewer->getWidth()), jmax<int> (getHeight(), currentViewer->getHeight()));
    }
}

int NodeManagerUI::getContentWidth() const
{
    return (currentViewer == nullptr) ? 0 : currentViewer->getWidth();
}

int NodeManagerUI::getContentHeight() const
{
    return (currentViewer == nullptr) ? 0 : currentViewer->getHeight();
}

void NodeManagerUI::managerCleared()
{
//    execOrDefer([this](){clear();});
    execOrDefer([this](){setCurrentViewedContainer(nodeManager);});
}

void NodeManagerUI::managerEndedLoading()
{

}
ParameterContainer * NodeManagerUI::addContainerFromObject(const String &s ,DynamicObject * d) {
    ParameterContainer* res = dynamic_cast<ParameterContainer*>(getControllableContainerByName(s));
    if(!res){
        ParameterContainer * newP =  new ConnectableNodeUIParams(s);
        addChildControllableContainer(newP);
        newP->configureFromObject(d);
    }
    else{
        res->configureFromObject(d);
    }

    return res;
};

void NodeManagerUI::setCurrentViewedContainer (NodeContainer* c)
{
    if (currentViewer != nullptr)
    {
        isMiniMode = currentViewer->minimizeAll->boolValue();
        if (currentViewer->nodeContainer == c) return;

        removeChildComponent (currentViewer);
        currentViewer = nullptr;
    }

    if (c != nullptr)
    {
        auto p =dynamic_cast<ParameterContainer*>(getMirroredContainer(c,nodeManager));
        if(!p){
            p = new ParameterContainer();
            addChildControllableContainer(p);
            jassertfalse;
        }
        currentViewer = new NodeContainerViewer (c,p);
        currentViewer->minimizeAll->setValue(isMiniMode);
        
        addAndMakeVisible (currentViewer);
        currentViewer->setMouseCursor(MouseCursor::CrosshairCursor);
        currentViewer->setTopLeftPosition (0, 0);
        currentViewer->selectThis();
        setSize (0, 0);
        resized();

    }


    nodeManagerUIListeners.call (&NodeManagerUIListener::currentViewedContainerChanged);

}
void NodeManagerUI::childBoundsChanged (Component* )
{
    if (currentViewer != nullptr)
    {
        setSize (currentViewer->getWidth(), currentViewer->getHeight());
    }
}


void NodeManagerUI::alignOnGrid(Point<int> &toAlign){
    if(gridSize->intValue()>0){
        int g = gridSize->intValue();
        toAlign.x -=toAlign.x%g;
        toAlign.y -=toAlign.y%g;
    }

}
bool NodeManagerUI::keyPressed (const KeyPress& key)
{
    if (key.getModifiers().isCommandDown() && key.getKeyCode() == KeyPress::upKey)
    {
        if (NodeContainer* c = (currentViewer->nodeContainer->getParentNodeContainer()))
        {
            setCurrentViewedContainer (c);
            return true;

        }
    }

    return false;
}


#endif
