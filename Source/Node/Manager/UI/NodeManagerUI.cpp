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
            all.add(pc);
            res = pc;
        }
        return res;
    }
    OwnedArray<ConnectableNodeUIParams> all;


};


class NodeManagerUIFactory : public ParameterContainer{
public:
    NodeManagerUIFactory():ParameterContainer("NodesUI"){}

    ParameterContainer* addContainerFromObject (const String& name, DynamicObject*   data) override{
        if(auto c = getControllableContainerByName("NodeManagerUI")){
            return dynamic_cast<NodeManagerUI*>(c);
        }
        return new NodeManagerUI(NodeManager::getInstance());
    };
};

//==============================================================================
juce_ImplementSingleton(NodeManagerUI);
NodeManagerUI::NodeManagerUI (NodeManager* _nodeManager) :
    nodeManager (_nodeManager),
    currentViewer (nullptr),
ParameterContainer("NodeManagerUI"),isMiniMode(false)
{

    
    uiSync = new UISync("UI",nodeManager,this);
    auto p =getRoot(true)->getControllableContainerByName("NodesUI");
    if(!p){
        NodeManagerUIFactory * np = new NodeManagerUIFactory();
        np->nameParam->isEditable = false;
        p = getRoot(true)->addChildControllableContainer(np);
    }
    p->addChildControllableContainer(this);
    nodeManager->addNodeManagerListener (this);
    execOrDefer([this](){setCurrentViewedContainer (nodeManager);});
    
//    nodeManager->parentContainer->addChildControllableContainer(uiSync->getSlaveContainer());

}

NodeManagerUI::~NodeManagerUI()
{

    
    nodeManager->removeNodeManagerListener (this);
    clear();
    setCurrentViewedContainer (nullptr);

    auto p =getRoot(true)->getControllableContainerByName("NodesUI");
    if(p){
        p->removeChildControllableContainer(this);
    }
    else jassertfalse;
}

void NodeManagerUI::clear()
{
    setCurrentViewedContainer (nullptr);
}

void NodeManagerUI::resized()
{

    Rectangle<int> r = getLocalBounds();

    if (currentViewer != nullptr)
    {
        currentViewer->setTopLeftPosition (0, 0);
        currentViewer->setSize (jmax<int> (getWidth(), currentViewer->getWidth()), jmax<int> (getHeight(), currentViewer->getHeight()));
    }
}

int NodeManagerUI::getContentWidth()
{
    return (currentViewer == nullptr) ? 0 : currentViewer->getWidth();
}

int NodeManagerUI::getContentHeight()
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
    ParameterContainer* existing = dynamic_cast<ParameterContainer*>(getControllableContainerByName(s));
    if(existing){
        return existing;
    }
    
    ParameterContainer * newP =  new ConnectableNodeUIParams(s);
    return newP;
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
        currentViewer->setTopLeftPosition (0, 0);
        currentViewer->setSelected (true);
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


