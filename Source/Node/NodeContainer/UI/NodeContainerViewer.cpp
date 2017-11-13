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


#include "NodeContainerViewer.h"
#include "../../Connection/UI/NodeConnectionUI.h"
#include "../../Connection/UI/NodeConnectionEditor.h"
#include "../../../UI/Inspector/Inspector.h"
#include "../../../Utils/FactoryUIHelpers.h"
#include "../../UI/NodeUIFactory.h"
#include "../../UI/ConnectableNodeHeaderUI.h"



NodeContainerViewer::NodeContainerViewer (NodeContainer* container,ParameterContainer * uiP) :
InspectableComponent (container, "node"),
nodeContainer (container),
editingConnection (nullptr),
uiParams(uiP),
ParameterContainer(container->getNiceName())
{
    selectedItems.addChangeListener(this);
    setInterceptsMouseClicks (true, true);
    nodesLayer.addMouseListener(this, true);
    nodeContainer->addNodeContainerListener (this);

    canInspectChildContainersBeyondRecursion = false;

    minimizeAll = addNewParameter<BoolParameter>("minimizeAll", "minimize all visible nodes", false);

    addAndMakeVisible(nodesLayer);
    nodesLayer.setInterceptsMouseClicks(false,true);
    for (auto& n : nodeContainer->nodes)
    {
        addNodeUI (n);
    }

    for (auto& c : nodeContainer->connections)
    {
        addConnectionUI (c);
    }


    addAndMakeVisible(lassoSelectionComponent);


    resizeToFitNodes();


}

NodeContainerViewer::~NodeContainerViewer()
{
    nodeContainer->removeNodeContainerListener (this);
    clear();

}

void NodeContainerViewer::clear()
{
    if (editingConnection != nullptr)
    {
        editingConnection->getBaseConnector()->removeMouseListener (this);
        delete editingConnection;
        editingConnection = nullptr;
    }

    while (connectionsUI.size() > 0)
    {
        removeConnectionUI (connectionsUI[0]->connection);
    }

    while (nodesUI.size() > 0)
    {
        removeNodeUI (nodesUI[0]->connectableNode);
    }

}


void NodeContainerViewer::resized()
{
    nodesLayer.setBounds(getLocalBounds());
    //    auto area = getLocalBounds();
    //    auto header  =area.removeFromTop(20);
    //

}

void NodeContainerViewer::nodeAdded (ConnectableNode* node)
{
    addNodeUI (node);
}

void NodeContainerViewer::nodeRemoved (ConnectableNode* node)
{
    removeNodeUI (node);
}


void NodeContainerViewer::connectionAdded (NodeConnection* connection)
{
    addConnectionUI (connection);
}

void NodeContainerViewer::connectionRemoved (NodeConnection* connection)
{
    removeConnectionUI (connection);
}

void NodeContainerViewer::addNodeUI (ConnectableNode* node)
{
    if (getUIForNode (node) == nullptr)
    {

        ConnectableNodeUI* nui =
        NodeUIFactory::createDefaultUI (node,
                                        dynamic_cast<ConnectableNodeUIParams*>(uiParams->getControllableContainerByName(node->shortName)));



        nodesUI.add (nui);
        addChildControllableContainer(nui);
        nui->setMiniMode(minimizeAll->boolValue());
        nodesLayer.addAndMakeVisible (nui);

    }
    else
    {
        jassertfalse;
        //ui for this node already in list
    }
}



void NodeContainerViewer::removeNodeUI (ConnectableNode* node)
{
    //DBG("Remove NodeUI");
    ConnectableNodeUI* nui = getUIForNode (node);

    if (nui != nullptr)
    {
        nodesLayer.removeChildComponent (nui);
        removeChildControllableContainer(nui);
        nodesUI.removeObject (nui);



    }
    else
    {
        //ConnectableNodeUI isn't in list
    }
}


ConnectableNodeUI* NodeContainerViewer::getUIForNode (ConnectableNode* node)
{
    for (int i = nodesUI.size(); --i >= 0;)
    {
        ConnectableNodeUI* nui = nodesUI.getUnchecked (i);

        if (nui->connectableNode == node) return nui;
    }

    return nullptr;
}

void NodeContainerViewer::addConnectionUI (NodeConnection* connection)
{

    if (getUIForConnection (connection) != nullptr)
    {
        DBG ("AddConnectionUI :: already exists");
        return;
    }

    ConnectableNode* sourceNode = (ConnectableNode*)connection->sourceNode;
    ConnectableNode* destNode = (ConnectableNode*)connection->destNode;

    if (auto s = dynamic_cast<ContainerOutNode*> (sourceNode)) sourceNode = s->getParentNodeContainer();

    if (auto s = dynamic_cast<ContainerInNode*> (destNode)) destNode = s->getParentNodeContainer();

    ConnectableNodeUI* n1 = getUIForNode (sourceNode);
    ConnectableNodeUI* n2 = getUIForNode (destNode);

    ConnectorComponent* c1 = (n1 != nullptr) ? n1->getFirstConnector (connection->connectionType, ConnectorComponent::OUTPUT) : nullptr;
    ConnectorComponent* c2 = (n2 != nullptr) ? n2->getFirstConnector (connection->connectionType, ConnectorComponent::INPUT) : nullptr;


    NodeConnectionUI* cui = new NodeConnectionUI (connection, c1, c2);
    connectionsUI.add (cui);

    nodesLayer.addAndMakeVisible (cui, 0);
    cui->resized();
}

void NodeContainerViewer::removeConnectionUI (NodeConnection* connection)
{
    NodeConnectionUI* nui = getUIForConnection (connection);

    if (nui == nullptr)
    {
        DBG ("RemoveConnectionUI :: not exists");
        return;
    }

    connectionsUI.removeObject (nui);
    nodesLayer.removeChildComponent (nui);
}

NodeConnectionUI* NodeContainerViewer::getUIForConnection (NodeConnection* connection)
{
    for (int i = connectionsUI.size(); --i >= 0;)
    {
        NodeConnectionUI* cui = connectionsUI.getUnchecked (i);

        if (cui->connection == connection) return cui;
    }

    return nullptr;
}


void NodeContainerViewer::createDataConnectionFromConnector (Connector* baseConnector)
{

    //DBG("Create Data connection from connector");

    if (editingConnection != nullptr)
    {
        DBG ("Already editing a connection !");
        return;
    }

    bool isOutputConnector = baseConnector->ioType == Connector::ConnectorIOType::OUTPUT;


    if (isOutputConnector)
    {
        editingConnection = new NodeConnectionUI (nullptr, baseConnector, nullptr);
    }
    else
    {
        editingConnection = new NodeConnectionUI (nullptr, nullptr, baseConnector);
    }

    nodesLayer.addAndMakeVisible (editingConnection);

    baseConnector->addMouseListener (this, false);
}

void NodeContainerViewer::createAudioConnectionFromConnector (Connector* baseConnector, NodeConnection* root)
{
    if (editingConnection != nullptr)
    {
        DBG ("Already editing a connection !");
        return;
    }

    bool isOutputConnector = baseConnector->ioType == Connector::ConnectorIOType::OUTPUT;
    editingModel = root ? new NodeConnection::Model (root->model) : nullptr;

    if (isOutputConnector)
    {
        editingConnection = new NodeConnectionUI (nullptr, baseConnector, nullptr);
    }
    else
    {
        editingConnection = new NodeConnectionUI (nullptr, nullptr, baseConnector);
    }

    // editingChannel = (uint32)-1; //temp, will be able to select which channel later

    baseConnector->addMouseListener (this, false);

    nodesLayer.addAndMakeVisible (editingConnection);
}

void NodeContainerViewer::updateEditingConnection()
{
    if (editingConnection == nullptr) return;

    Point<int> cPos = getLocalPoint (editingConnection->getBaseConnector(), editingConnection->getBaseConnector()->getLocalBounds().getCentre());
    Point<int> mPos = getMouseXYRelative();
    int minX = jmin<int> (cPos.x, mPos.x);
    int minY = jmin<int> (cPos.y, mPos.y);
    int tw = abs (cPos.x - mPos.x);
    int th = abs (cPos.y - mPos.y);
    int margin = 50;

    checkDropCandidates();

    editingConnection->setBounds (minX - margin, minY - margin, tw + margin * 2, th + margin * 2);
}

bool NodeContainerViewer::checkDropCandidates()
{
    //  Connector * candidate = nullptr;
    for (int i = 0; i < nodesUI.size(); i++)
    {
        Array<Connector*> compConnectors = nodesUI.getUnchecked (i)->getComplementaryConnectors (editingConnection->getBaseConnector());

        for (int j = 0; j < compConnectors.size(); j++)
        {
            Connector* c = compConnectors.getUnchecked (j);
            float dist = (float) (c->getMouseXYRelative().getDistanceFromOrigin());

            if (dist < 20)
            {
                //        candidate = c;
                setCandidateDropConnector (c);
                return true;
            }
        }
    }

    cancelCandidateDropConnector();
    return false;
}

bool NodeContainerViewer::setCandidateDropConnector (Connector* connector)
{
    if (!isEditingConnection()) return false;

    bool result = editingConnection->setCandidateDropConnector (connector);
    editingConnection->candidateDropConnector->addMouseListener (this, false);
    return result;
}

void NodeContainerViewer::cancelCandidateDropConnector()
{
    if (!isEditingConnection()) return;

    if (editingConnection->candidateDropConnector != nullptr) editingConnection->candidateDropConnector->removeMouseListener (this);

    editingConnection->cancelCandidateDropConnector();


}

void NodeContainerViewer::finishEditingConnection()
{
    //DBG("Finish Editing connection");
    if (!isEditingConnection()) return;

    //bool isEditingDataOutput = editingConnection->getBaseConnector()->ioType == Connector::ConnectorIOType::OUTPUT;
    editingConnection->getBaseConnector()->removeMouseListener (this);

    if (editingConnection->candidateDropConnector != nullptr) editingConnection->candidateDropConnector->removeMouseListener (this);

    bool isDataConnection = editingConnection->getBaseConnector()->dataType == NodeConnection::ConnectionType::DATA;

    if (isDataConnection) //DATA
    {
        bool success = editingConnection->finishEditing();

        if (success)
        {
            nodeContainer->addConnection (editingConnection->sourceConnector->node, editingConnection->destConnector->node, editingConnection->getBaseConnector()->dataType, editingModel);
        }
    }
    else //AUDIO
    {
        bool success = editingConnection->finishEditing();

        if (success)
        {
            nodeContainer->addConnection (editingConnection->sourceConnector->node, editingConnection->destConnector->node, editingConnection->getBaseConnector()->dataType, editingModel);
        }
    }

    nodesLayer.removeChildComponent (editingConnection);
    delete editingConnection;
    editingConnection = nullptr;

}

ConnectableNodeUI * getRelatedConnectableNodeUIForDrag(Component * c){

    // dont drag if comming from param or text editor

    if( auto nui = dynamic_cast<ConnectableNodeUI*>(c)){
        return nui;
    }

    if(auto cont = dynamic_cast<ConnectableNodeContentUI*>(c)){
        return cont->nodeUI;
    }

    if(auto header = dynamic_cast<ConnectableNodeHeaderUI*>(c)){
        return header->nodeUI;
    }
    if(auto label = dynamic_cast<Label*>(c)){
        return label->findParentComponentOfClass<ConnectableNodeUI>();
    }
    //    if(auto vumeter = dynamic_cast<VuMeter*>(c)){
    //        return vumeter->findParentComponentOfClass<ConnectableNodeUI>();
    //    }
    return nullptr;

}
//Interaction Events
void NodeContainerViewer::mouseDown (const MouseEvent& event)
{
    if (event.eventComponent == this)
    {
        if (event.mods.isRightButtonDown())
        {

            Point<int> mousePos = getMouseXYRelative();
            PopupMenu   menu;//(new PopupMenu());
            static Array<String> filt  {"t_ContainerInNode", "t_ContainerOutNode"};
            ScopedPointer<PopupMenu> addNodeMenu (FactoryUIHelpers::createFactoryTypesMenuFilter<NodeFactory> (filt));

            menu.addSubMenu ("Add Node", *addNodeMenu);
            int result = menu.show();

            if (result > 0)
            {
                if (auto c =  FactoryUIHelpers::createFromMenuIdx<NodeBase> (result))
                {
                    ConnectableNode* n = (ConnectableNode*)nodeContainer->addNode (c);
                    jassert (n != nullptr);
                    if(auto m = getUIForNode(n)){
                        m->nodePosition->setPoint (mousePos - m->nodeSize->getPoint() / 2);
                        m->nodeMinimizedPosition->setPoint (mousePos - m->nodeSize->getPoint() / 2);
                    }

                }
                else
                {
                    jassertfalse;
                }
            }
        }
        else{
            lassoSelectionComponent.beginLasso(event, this);

        }
    }

    else
    {
        hasDraggedDuringClick = false;

        if(auto nui=getRelatedConnectableNodeUIForDrag(event.eventComponent)){

            nui->selectThis();
            resultOfMouseDownSelectMethod = selectedItems.addToSelectionOnMouseDown(nui, event.mods);
            selectedInitBounds.clear();
            for(auto s: selectedItems){
                if(s.get()){
                    Component * tSize = s->mainComponentContainer.contentContainer;
                    selectedInitBounds.set(s, s->getBoundsInParent().withSize(tSize->getWidth(), tSize->getHeight()));
                }

            }
        }


    }



}



void NodeContainerViewer::mouseMove (const MouseEvent& e)
{
    if (editingConnection != nullptr)
    {
        //DBG("NMUI mouse mouve while editing connection");
        updateEditingConnection();
    }

}

void NodeContainerViewer::mouseDrag (const MouseEvent&  e)
{

    if (editingConnection != nullptr)
    {
        //    if (event.eventComponent == editingConnection->getBaseConnector())

        updateEditingConnection();
        return;

    }

    if (e.eventComponent == this){
        lassoSelectionComponent.dragLasso(e);
        lassoSelectionComponent.toFront(false);

    }

    else     {

        auto nui = getRelatedConnectableNodeUIForDrag(e.eventComponent);
        if(nui){
            bool isResizing = dynamic_cast<ResizableCornerComponent*>(e.eventComponent)!=nullptr;
            Point<int> diff = Point<int> (e.getPosition() - e.getMouseDownPosition());
            if(!isResizing){
                hasDraggedDuringClick = diff.getDistanceSquaredFromOrigin()>0;
                for(auto s: selectedItems){
                    if(s.get()){
                        if(selectedInitBounds.contains(s)){
                            Point <int> newPos = selectedInitBounds.getReference(s).getPosition() + diff;
                            auto nodeP = s->getCurrentPositionParam();
                            nodeP->setPoint (newPos);
                            s->setTopLeftPosition (nodeP->getPoint());
                        }
                        else{
                            jassertfalse;
                        }
                    }
                }
            }
            else if( !minimizeAll->boolValue()){
                for(auto s: selectedItems){
                    if(s.get()){
                        if(selectedInitBounds.contains(s)){
                            Point <int> newSize = selectedInitBounds.getReference(s).withPosition(Point<int>(0,0)).getBottomRight() + diff;
                            auto nodeS = s->nodeSize;
                            nodeS->setPoint(newSize);
                        }
                        else{
                            jassertfalse;
                        }
                    }
                }
            }
        }
    }


}

void NodeContainerViewer::mouseUp (const MouseEvent& e)
{
    
        if (isEditingConnection())
        {
            finishEditingConnection();
            return;
        }

    if (e.eventComponent == this){
            if(getLassoSelection().getItemArray().size()==0)
                selectThis();
    }
    else if(auto nui = getRelatedConnectableNodeUIForDrag(e.eventComponent)){
        selectedItems.addToSelectionOnMouseUp(nui, e.mods, hasDraggedDuringClick , resultOfMouseDownSelectMethod);
    }

    lassoSelectionComponent.endLasso();


}

bool NodeContainerViewer::keyPressed (const KeyPress& key)
{
    if (key.getTextCharacter() == 'a')
    {
        Point<int> mousePos = getMouseXYRelative();
        ScopedPointer<PopupMenu > menu = FactoryUIHelpers::getFactoryTypesMenu<NodeFactory>();


        int result = menu->show();


        if (result > 0 )
        {
            if (auto c = FactoryUIHelpers::createFromMenuIdx<NodeBase> (result))
            {

                auto* n = nodeContainer->addNode (c);
                jassert (n != nullptr);
                if(auto m = getUIForNode(n)){
                    m->nodePosition->setPoint (mousePos);
                    m->nodeMinimizedPosition->setPoint (mousePos);
                }

            }
            else
            {
                return false;
            }
        }

        return true;
    }

    return false;
};



void NodeContainerViewer::childBoundsChanged (Component*)
{
    resizeToFitNodes();
}

void NodeContainerViewer::onContainerParameterChanged(Parameter * p) {
    if(p==minimizeAll){
        for(auto n:nodesUI){
            n->miniMode->setValue(minimizeAll->boolValue(),false,true);
        }
    }

};

void NodeContainerViewer::resizeToFitNodes()
{


    Rectangle<int> _bounds (0, 0, 1, 1);

    if (auto* p = findParentComponentOfClass<Viewport>())
    {
        _bounds = p->getBounds();
    }

    for (auto& n : nodesUI)
    {
        _bounds = _bounds.getUnion (n->getBoundsInParent().withLeft (0).withTop (0));

    }

    setSize (_bounds.getWidth(), _bounds.getHeight());

}

void NodeContainerViewer::findLassoItemsInArea (Array<SelectedUIType>& itemsFound,const Rectangle<int>& area) {
    for(auto n : nodesUI){
        if(n->getBoundsInParent().intersects(area)){
            itemsFound.add(n);
        }
    }
    
    auto insp = Inspector::getInstance();
    if(itemsFound.size()){
        // unselect this if needed
        if(insp->getCurrentComponent()==this){
            insp->setCurrentComponent(nullptr);
        }
    }
    else{
        insp->setCurrentComponent(nullptr); 
    }
};

SelectedItemSet<SelectedUIType>& NodeContainerViewer::getLassoSelection() {
    return selectedItems;
};

void NodeContainerViewer::changeListenerCallback (ChangeBroadcaster* source){
    if(source== &selectedItems){
        for(auto n: nodesUI){
            n->setSelected(selectedItems.getItemArray().contains(n));
        }
    }
    
}
