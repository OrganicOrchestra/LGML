/*
 ==============================================================================

 NodeManagerUI.cpp
 Created: 3 Mar 2016 10:38:22pm
 Author:  bkupe

 ==============================================================================
 */

#include "NodeManagerUI.h"
#include "NodeConnectionUI.h"
#include "NodeConnectionEditor.h"

#include "MainComponent.h"


//==============================================================================
NodeManagerUI::NodeManagerUI(NodeManager * nodeManager) :
nodeManager(nodeManager),
editingConnection(nullptr),
isSelectingNodes(false)
{

    nodeManager->addNodeManagerListener(this);
    setInterceptsMouseClicks(true, true);
    addAndMakeVisible(selectingBounds);

}

NodeManagerUI::~NodeManagerUI()
{
    nodeManager->removeNodeManagerListener(this);
    if (editingConnection != nullptr)
    {
        editingConnection->getBaseConnector()->removeMouseListener(this);
        delete editingConnection;
        editingConnection = nullptr;
    }
}

void NodeManagerUI::clear()
{
    nodesUI.clear();
}


void NodeManagerUI::resized()
{

}

void NodeManagerUI::nodeAdded(NodeBase * node)
{
    addNodeUI(node);
}

void NodeManagerUI::nodeRemoved(NodeBase * node)
{
    removeNodeUI(node);
}


void NodeManagerUI::connectionAdded(NodeConnection * connection)
{
    addConnectionUI(connection);
}

void NodeManagerUI::connectionRemoved(NodeConnection * connection)
{
    removeConnectionUI(connection);
}

void NodeManagerUI::inspectableSelectionChanged(InspectableComponent * c)
{
	if (c->isSelected) c->toFront(true);
}

void NodeManagerUI::addNodeUI(NodeBase * node)
{

    if (getUIForNode(node) == nullptr)
    {
        NodeBaseUI * nui = node->createUI();
        nodesUI.add(nui);
        addAndMakeVisible(nui);
		nui->addInspectableListener(this);
    }
    else
    {
        //ui for this node already in list
    }
}



void NodeManagerUI::removeNodeUI(NodeBase * node)
{
    //DBG("Remove NodeUI");
    NodeBaseUI * nui = getUIForNode(node);
    if (nui != nullptr)
    {
        nodesUI.removeObject(nui);
        removeChildComponent(nui);
		nui->removeInspectableListener(this);
    }
    else
    {
        //nodeBaseUI isn't in list
    }
}


NodeBaseUI * NodeManagerUI::getUIForNode(NodeBase * node)
{
    for (int i = nodesUI.size(); --i >= 0;)
    {
        NodeBaseUI * nui = nodesUI.getUnchecked(i);
        if (nui->node == node) return nui;
    }

    return nullptr;
}

void NodeManagerUI::addConnectionUI(NodeConnection * connection)
{
    DBG("NMUI :: addConnectionUI From NodeManagerListener");

    if (getUIForConnection(connection) != nullptr)
    {
        DBG("AddConnectionUI :: already exists");
        return;
    }

    NodeBaseUI * n1 = getUIForNode(connection->sourceNode);
    NodeBaseUI * n2 = getUIForNode(connection->destNode);

    ConnectorComponent * c1 = (n1 != nullptr) ? n1->getFirstConnector(connection->connectionType,ConnectorComponent::OUTPUT): nullptr;
    ConnectorComponent * c2 = (n2 != nullptr) ? n2->getFirstConnector(connection->connectionType,ConnectorComponent::INPUT) : nullptr;


    NodeConnectionUI * cui = new NodeConnectionUI(connection, c1,c2);
    connectionsUI.add(cui);

    DBG("Add And MakeVisible connection");
    addAndMakeVisible(cui,0);
}

void NodeManagerUI::removeConnectionUI(NodeConnection * connection)
{
    NodeConnectionUI * nui = getUIForConnection(connection);
    if (nui == nullptr)
    {
        DBG("RemoveConnectionUI :: not exists");
        return;
    }

    connectionsUI.removeObject(nui);
    removeChildComponent(nui);

    if (NodeConnectionEditor::getInstance()->currentConnection == connection)
    {
        NodeConnectionEditor::getInstance()->closeWindow();
    }
}

NodeConnectionUI * NodeManagerUI::getUIForConnection(NodeConnection* connection)
{
    for (int i = connectionsUI.size(); --i >= 0;)
    {
        NodeConnectionUI * cui = connectionsUI.getUnchecked(i);
        if (cui->connection == connection) return cui;
    }

    return nullptr;
}


void NodeManagerUI::createDataConnectionFromConnector(Connector * baseConnector)
{

    //DBG("Create Data connection from connector");

    if (editingConnection != nullptr)
    {
        DBG("Already editing a connection !");
        return;
    }

    bool isOutputConnector = baseConnector->ioType == Connector::ConnectorIOType::OUTPUT;


    if (isOutputConnector)
    {
        editingConnection = new NodeConnectionUI(nullptr, baseConnector, nullptr);
    }else
    {
        editingConnection = new NodeConnectionUI(nullptr, nullptr, baseConnector);
    }

    addAndMakeVisible(editingConnection);

    //editingData = data;
    baseConnector->addMouseListener(this, false);
    /*
    if (data == nullptr)
    {

    }
    else
    {

    }
    */
}

void NodeManagerUI::createAudioConnectionFromConnector(Connector * baseConnector)
{
    //DBG("Create Audio Connection from connector " + String(channel));

    if (editingConnection != nullptr)
    {
        DBG("Already editing a connection !");
        return;
    }

    bool isOutputConnector = baseConnector->ioType == Connector::ConnectorIOType::OUTPUT;

    if (isOutputConnector)
    {
        editingConnection = new NodeConnectionUI(nullptr, baseConnector, nullptr);
    }
    else
    {
        editingConnection = new NodeConnectionUI(nullptr, nullptr, baseConnector);
    }

   // editingChannel = (uint32)-1; //temp, will be able to select which channel later

    baseConnector->addMouseListener(this, false);

    addAndMakeVisible(editingConnection);
}

void NodeManagerUI::updateEditingConnection()
{
    if (editingConnection == nullptr) return;

    Point<int> cPos = getLocalPoint(editingConnection->getBaseConnector(), editingConnection->getBaseConnector()->getLocalBounds().getCentre());
    Point<int> mPos = getMouseXYRelative();
    int minX = jmin<int>(cPos.x, mPos.x);
    int minY = jmin<int>(cPos.y, mPos.y);
    int tw = abs(cPos.x - mPos.x);
    int th = abs(cPos.y - mPos.y);
    int margin = 50;

    checkDropCandidates();

    editingConnection->setBounds(minX-margin, minY-margin, tw+margin*2, th+margin*2);
}

bool NodeManagerUI::checkDropCandidates()
{
    Connector * candidate = nullptr;
    for (int i = 0; i < nodesUI.size(); i++)
    {
        Array<Connector *> compConnectors = nodesUI.getUnchecked(i)->getComplementaryConnectors(editingConnection->getBaseConnector());

        for (int j = 0; j < compConnectors.size(); j++)
        {
            Connector * c = compConnectors.getUnchecked(j);
            float dist = (float)(c->getMouseXYRelative().getDistanceFromOrigin());
            if (dist < 20)
            {
                candidate = c;
                setCandidateDropConnector(c);
                return true;
            }
        }
    }

    cancelCandidateDropConnector();
    return false;
}

bool NodeManagerUI::setCandidateDropConnector(Connector * connector)
{
    if (!isEditingConnection()) return false;

    bool result = editingConnection->setCandidateDropConnector(connector);
    editingConnection->candidateDropConnector->addMouseListener(this,false);
    return result;
}

void NodeManagerUI::cancelCandidateDropConnector()
{
    if (!isEditingConnection()) return;
    if(editingConnection->candidateDropConnector != nullptr) editingConnection->candidateDropConnector->removeMouseListener(this);
    editingConnection->cancelCandidateDropConnector();


}

void NodeManagerUI::finishEditingConnection()
{
    //DBG("Finish Editing connection");
    if (!isEditingConnection()) return;

    //bool isEditingDataOutput = editingConnection->getBaseConnector()->ioType == Connector::ConnectorIOType::OUTPUT;
    editingConnection->getBaseConnector()->removeMouseListener(this);
    if(editingConnection->candidateDropConnector != nullptr) editingConnection->candidateDropConnector->removeMouseListener(this);

    bool isDataConnection = editingConnection->getBaseConnector()->dataType == NodeConnection::ConnectionType::DATA;

    if (isDataConnection) //DATA
    {
        bool success = editingConnection->finishEditing();
        if (success)
        {
            nodeManager->addConnection(editingConnection->sourceConnector->node, editingConnection->destConnector->node, editingConnection->getBaseConnector()->dataType);
        }
    }else //AUDIO
    {
        bool success = editingConnection->finishEditing();

        if (success)
        {
            nodeManager->addConnection(editingConnection->sourceConnector->node, editingConnection->destConnector->node, editingConnection->getBaseConnector()->dataType);
        }
    }

    removeChildComponent(editingConnection);
    delete editingConnection;
    editingConnection = nullptr;

}

void NodeManagerUI::createNodeFromIndexAtPos(int modalResult, Component * c,int maxRes)
{
    if (modalResult >= 1 && modalResult <= maxRes)
    {
        NodeBase * n = NodeManager::getInstance()->addNode((NodeType)(modalResult ));

        Point<int> mousePos = c->getMouseXYRelative();
        n->xPosition->setValue((float)mousePos.x);
        n->yPosition->setValue((float)mousePos.y);
    }
}



//Interaction Events
void NodeManagerUI::mouseDown(const MouseEvent & event)
{
    if (event.eventComponent == this )
    {
        if (event.mods.isRightButtonDown())
        {

            PopupMenu   menu;//(new PopupMenu());
            ScopedPointer<PopupMenu> addNodeMenu(NodeFactory::getNodeTypesMenu(0));
            menu.addSubMenu("Add Node", *addNodeMenu);

            menu.show(0,0,0,0,ModalCallbackFunction::forComponent(&NodeManagerUI::createNodeFromIndexAtPos,(Component*)this,addNodeMenu->getNumItems()));
        }
        else
        {
            /*
            if (event.mods.isCtrlDown())
            {
                NodeBase * n = nodeManager->addNode(NodeType::Dummy);
                n->xPosition->setValue((float)getMouseXYRelative().x);
                n->yPosition->setValue((float)getMouseXYRelative().y);
            }
            */

            Point<int> mouse = getMouseXYRelative();
            selectingBounds.setTopLeftPosition(mouse.x,mouse.y);


        }
    }

}

void NodeManagerUI::mouseMove(const MouseEvent &)
{
    if (editingConnection != nullptr)
    {
        //DBG("NMUI mouse mouve while editing connection");
        updateEditingConnection();
    }
}

void NodeManagerUI::mouseDrag(const MouseEvent & event)
{
    if (editingConnection != nullptr)
    {
        if (event.eventComponent == editingConnection->getBaseConnector())
        {
            updateEditingConnection();
        }
    }
    else{

        if(isSelectingNodes){
            selectingBounds.setSize(std::abs(event.getDistanceFromDragStartX()),std::abs(event.getDistanceFromDragStartY()));
            selectingBounds.setTopLeftPosition(jmin(event.getMouseDownX(),getMouseXYRelative().x),
                                               jmin(event.getMouseDownY(),getMouseXYRelative().y));
            selectingBounds.toFront(false);
            checkSelected();
            repaint();
        }
        else if(event.getDistanceFromDragStart()>4){

            isSelectingNodes = true;
            selectingBounds.setVisible(true);
        }
    }
}

void NodeManagerUI::mouseUp(const MouseEvent &)
{
    if (isEditingConnection())
    {
        finishEditingConnection();
    }

    if(!isSelectingNodes){
		MainContentComponent::inspector.setCurrentComponent(nullptr);
	}

    isSelectingNodes = false;
    selectingBounds.setVisible(false);
    selectingBounds.setSize(0, 0);

}


void NodeManagerUI::checkSelected(){

    // multiple ones
	/*
    if(isSelectingNodes){
        Array<SelectableComponent*> currentOnes;
        for(auto &n:nodesUI){
            if(selectingBounds.getBounds().intersects(n->getBounds())){
                currentOnes.add(n);
            }
        }

        for(auto &n: MainContentComponent::mainSelectableHandler.selected){
            if(!currentOnes.contains(n)){
                n->askForSelection(false,false);
            }
        }

        for(auto &n:currentOnes){
            if(!MainContentComponent::mainSelectableHandler.selected.contains(n)){
                n->askForSelection(true,false);
            }
        }
    }
	*/

    // only one

}

void NodeManagerUI::setAllNodesToStartAtZero(){
    if(nodesUI.size()==0)return;

    Point<int> minPoint = nodesUI.getUnchecked(0)->getBounds().getTopLeft();
    for(auto &n:nodesUI){
        minPoint.x = jmin(n->getX(),minPoint.x);
        minPoint.y = jmin(n->getY(),minPoint.y);
    }

    for(auto &n:nodesUI){
        n->setTopLeftPosition(n->getX()-(minPoint.x<0?minPoint.x:0),
                              n->getY()-(minPoint.y<0?minPoint.y:0));
    }

}
void NodeManagerUI::childBoundsChanged(Component * ){
    resizeToFitNodes();
}


void NodeManagerUI::resizeToFitNodes(){
	
	Rectangle<int> _bounds(minBounds);
    
    for(auto &n:nodesUI){

        Rectangle<int> r = n->getBoundsInParent();
        _bounds = _bounds.getUnion(r);

    }
    setBounds(_bounds);

}

void NodeManagerUI::paint(Graphics&g) {
//    g.setColour(Colours::red);
//    g.fillAll();
};
