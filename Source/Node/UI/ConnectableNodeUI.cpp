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

#include "ConnectableNodeUI.h"
#include "ConnectableNodeHeaderUI.h"
#include "../NodeContainer/NodeContainer.h"
#include "../Impl/ContainerInNode.h"
#include "../Impl/ContainerOutNode.h"


// needed because main component need to know it for its scopedPointer
#include "../../Controllable/Parameter/UI/SliderUI.h"


#include "../Manager/UI/NodeManagerUI.h"
#include "../../Controllable/Parameter/UndoableHelper.h"


ConnectableNodeUIParams::ConnectableNodeUIParams(ConnectableNodeUIParams * _origin):origin(_origin){

    for(auto &p:_origin->getAllParameters(false,true)){
        p->addParameterListener(this);
    }
}
void ConnectableNodeUIParams::initFromParams(){
    auto nui = static_cast<ConnectableNodeUIParams*> (origin.get());
    nodePosition = nui->nodePosition;
    nodeMinimizedPosition = nui->nodeMinimizedPosition;
    nodeSize = nui->nodeSize;
    miniModeParam = nui->miniModeParam;



}

void ConnectableNodeUIParams::notifyFromParams(){

    nodeMinimizedPosition->notifyValueChanged();
    nodePosition->notifyValueChanged();
    nodeSize->notifyValueChanged();
    miniModeParam->notifyValueChanged();
}
ConnectableNodeUIParams::~ConnectableNodeUIParams(){

    if(origin.get()){
    for(auto p:origin->getAllParameters(false,true)){
        if(p.get())
            p->removeParameterListener(this);

    }
    }
}
ConnectableNodeUIParams::ConnectableNodeUIParams(StringRef n): ParameterContainer(n){
    nodePosition=addNewParameter<Point2DParameter<int>> ("nodePosition", "position on canvas", 0, 0, Array<var> {0, 0});
    nodeMinimizedPosition= addNewParameter<Point2DParameter<int>> ("nodeMinimizedPosition", "position in minimode on canvas", 0, 0, Array<var> {0, 0});
    nodeSize=addNewParameter<Point2DParameter<int>> ("nodeSize", "Node Size", 180, 100, Array<var> {30, 30});
    miniModeParam=addNewParameter<BoolParameter> ("miniMode", "Mini Mode", false);
    nodePosition->isControllableExposed = false;
    nodeMinimizedPosition->isControllableExposed = false;
    nodeSize->isControllableExposed = false;


    nodePosition->isPresettable = false;
    nodeMinimizedPosition->isPresettable = false;
    nodeSize->isPresettable = false;

    nodePosition->isHidenInEditor = false;
    nodeMinimizedPosition->isHidenInEditor = false;
    nodeSize->isHidenInEditor = false;

}



//layout
const int connectorWidth = 10;

ConnectableNodeUI::ConnectableNodeUI (ConnectableNode* cn,ConnectableNodeUIParams*params, ConnectableNodeContentUI* contentUI, ConnectableNodeHeaderUI* headerUI) :
InspectableComponent (cn, "node"),
ConnectableNodeUIParams(params),
connectableNode (cn),
inputContainer (ConnectorComponent::ConnectorIOType::INPUT),
outputContainer (ConnectorComponent::ConnectorIOType::OUTPUT),
mainComponentContainer (this, contentUI, headerUI),
resizer (this, &constrainer),
isDraggingFromUI (false)
{
    

    constrainer.setMinimumWidth (50);
    constrainer.setMinimumHeight (50);

    addAndMakeVisible (mainComponentContainer);
    mainComponentContainer.setMouseCursor(MouseCursor::UpDownLeftRightResizeCursor);

    if (connectableNode->userCanAccessInputs)
    {
        inputContainer.setConnectorsFromNode (connectableNode);
        addAndMakeVisible (inputContainer);
    }

    if (connectableNode->userCanAccessOutputs)
    {
        outputContainer.setConnectorsFromNode (connectableNode);
        addAndMakeVisible (outputContainer);
    }

    getHeaderContainer()->addMouseListener (this, true); // (true, true);
    getContentContainer()->addMouseListener (this, false);

    initFromParams();
    mainComponentContainer.setNodeAndNodeUI (connectableNode, this);
    notifyFromParams();

    connectableNode->addConnectableNodeListener (this);
    



    addAndMakeVisible (&resizer);

    isMiniMode = miniModeParam->boolValue();
    //connectableNode->miniMode->isHidenInEditor = true;




}

ConnectableNodeUI::~ConnectableNodeUI()
{

    connectableNode->removeConnectableNodeListener (this);
    masterReference.clear();
}


Point2DParameter<int>* ConnectableNodeUI::getCurrentPositionParam(){
    auto ncv = findParentComponentOfClass<NodeContainerViewer>();
    return ncv && ncv->minimizeAll->boolValue()?nodeMinimizedPosition:nodePosition;
}

void ConnectableNodeUI::moved()
{
    bool wasDraggingFromUI = isDraggingFromUI;
    isDraggingFromUI = true;
    auto p = getPosition();
    auto pp = getCurrentPositionParam();
    if(pp){
        auto stp=pp->getPoint();
        if(stp!=p){
            Array<var> v  ({p.x,p.y});
            UndoableHelpers::setValueUndoable(pp,v);
            //else{pp->setValue(v);}
        }
    }
    isDraggingFromUI = wasDraggingFromUI;
}


void ConnectableNodeUI::setMiniMode (bool value)
{
    // update parameter only if in non minimized mode
    if(auto ncv = findParentComponentOfClass<NodeContainerViewer>())
        if(!ncv->minimizeAll->boolValue())
            if( value!=miniModeParam->boolValue()){
                miniModeParam->setValue(value);
                return;
            }
    mainComponentContainer.setMiniMode (value);
    isMiniMode = value;
    auto nodeP = getCurrentPositionParam();
    setBounds(nodeP->getX(), nodeP->getY() ,
              getMiniModeWidth (value), getMiniModeHeight (value));
}

int ConnectableNodeUI::getMiniModeWidth (bool forMiniMode)
{
    return forMiniMode ? 130 : (getContentContainer()->getWidth() +
                                inputContainer.isVisible()*inputContainer.getWidth() +
                                outputContainer.isVisible()*outputContainer.getWidth() +
                                (mainComponentContainer.audioCtlUIContainer ? mainComponentContainer.audioCtlUIContainer->getWidth() + mainComponentContainer.audioCtlContainerPadRight : 0));
}

int ConnectableNodeUI::getMiniModeHeight (bool forMiniMode)
{
    return getHeaderContainer()->getBottom() + (forMiniMode ? 10 : getContentContainer()->getHeight());
}

void ConnectableNodeUI::paint (Graphics&)
{

}

void ConnectableNodeUI::resized()
{

    Rectangle<int> r = getLocalBounds();



    if (connectableNode->userCanAccessInputs)
    {
        jassert(inputContainer.isVisible());
        Rectangle<int> inputBounds = r.removeFromLeft (connectorWidth);
        inputContainer.setBounds (inputBounds);
    }

    if (connectableNode->userCanAccessOutputs)
    {
        jassert(outputContainer.isVisible());
        Rectangle<int> outputBounds = r.removeFromRight (connectorWidth);
        outputContainer.setBounds (outputBounds);
    }


    mainComponentContainer.setBounds(r);
    resizer.setBounds (r.removeFromRight (10).removeFromBottom (10));

    isDraggingFromUI = true;
    nodeSize->setPoint(mainComponentContainer.contentContainer->getWidth(),mainComponentContainer.contentContainer->getHeight() );
    isDraggingFromUI = false;
}

void ConnectableNodeUI::onContainerParameterChanged(Parameter *p){

        if (p == nodePosition )
        {
            if (!isDraggingFromUI )
                postOrHandleCommandMessage (posChangedId);
        }
        else if(p== nodeMinimizedPosition){
            if (!isDraggingFromUI )
                postOrHandleCommandMessage (posChangedId);
        }
        else if ( p == nodeSize)
        {
            if (!isDraggingFromUI){
                postOrHandleCommandMessage (sizeChangedId);

            }
        }
        else if (p == miniModeParam)
        {
            postOrHandleCommandMessage (setMiniModeId);
        }

}

void ConnectableNodeUI::nodeParameterChanged (ConnectableNode*, Parameter* p)
{

    if (p == connectableNode->enabledParam)
    {
        postOrHandleCommandMessage (repaintId);
    }

}
void ConnectableNodeUI::postOrHandleCommandMessage (int id)
{
    if (MessageManager::getInstance()->isThisTheMessageThread())
        handleCommandMessage (id);
    else
        postCommandMessage (id);
}
void ConnectableNodeUI::handleCommandMessage (int commandId)
{
    switch (commandId)
    {
        case repaintId:
            repaint();
            break;

        case setMiniModeId:
            setMiniMode (miniModeParam->boolValue());
            break;

        case posChangedId:
            setTopLeftPosition(getCurrentPositionParam()->getPoint());
            if(!nodeMinimizedPosition->isOverriden){
                nodeMinimizedPosition->setPoint(nodePosition->getPoint());
            }
            break;

        case sizeChangedId:
            mainComponentContainer.contentContainer->setSize (nodeSize->getX(), nodeSize->getY());
//                        childBoundsChanged(&mainComponentContainer);
            //            resized();
            break;

        default:
            break;
    }
}

//// allow to react to custom MainComponentContainer.contentContainer
void ConnectableNodeUI::childBoundsChanged (Component* c)
{
    // if changes in this layout take care to update  childBounds changed to update when child resize itself (ConnectableNodeContentUI::init()
    //    if (c == &mainComponentContainer)
    //    {
    int destWidth = mainComponentContainer.getWidth() + ((inputContainer.isVisible()?1:0) +
                                                         (outputContainer.isVisible()?1:0))* connectorWidth;
    int destHeight = mainComponentContainer.getHeight();

    if (getWidth() != destWidth ||
        destHeight != getHeight())
    {

        setSize (destWidth, destHeight);
        nodeSize->setPoint(mainComponentContainer.contentContainer->getWidth(),mainComponentContainer.contentContainer->getHeight() );
    }
    //    }
}


void ConnectableNodeUI::mouseDown (const juce::MouseEvent& /*e*/)
{
UndoableHelpers::startNewTransaction(getCurrentPositionParam(),true);
    UndoableHelpers::setParameterCoalesced(true);

}

void ConnectableNodeUI::mouseUp (const juce::MouseEvent&)
{
    isDraggingFromUI = false;
    UndoableHelpers::setParameterCoalesced(false);
}

void ConnectableNodeUI::mouseDrag (const MouseEvent& e)
{

    isDraggingFromUI = true;

}
void ConnectableNodeUI::mouseEnter (const MouseEvent& e)  {

};
void ConnectableNodeUI::mouseExit (const MouseEvent& e)  {
    
};

bool ConnectableNodeUI::keyPressed (const KeyPress& key)
{
    if (!isSelected) return false;

    if (key.getKeyCode() == KeyPress::deleteKey || key.getKeyCode() == KeyPress::backspaceKey)
    {
        if (connectableNode->canBeRemovedByUser)
        {
            connectableNode->remove();
        }

        return true;
    }
    else if (key.getModifiers().isCommandDown() && key.getKeyCode() == KeyPress::downKey)
    {
        if (NodeContainer* c = dynamic_cast<NodeContainer* > (connectableNode.get()))
        {
            if (NodeManagerUI* manager = findParentComponentOfClass<NodeManagerUI>())
            {
                manager->setCurrentViewedContainer (c);
                return true;
            }
        }
    }

    return false;
}

////////////    MAIN CONTAINER


ConnectableNodeUI::MainComponentContainer::MainComponentContainer (ConnectableNodeUI* _nodeUI, ConnectableNodeContentUI* content, ConnectableNodeHeaderUI* header) :
connectableNodeUI (_nodeUI),
headerContainer (header),
contentContainer (content),
audioCtlUIContainer (nullptr),
miniMode (false)
{

    if (headerContainer == nullptr) headerContainer = new ConnectableNodeHeaderUI();

    if (contentContainer == nullptr) contentContainer = new ConnectableNodeContentUI();


    addAndMakeVisible (headerContainer);
    addAndMakeVisible (contentContainer);

}

void ConnectableNodeUI::MainComponentContainer::setNodeAndNodeUI (ConnectableNode* _node, ConnectableNodeUI* _nodeUI)
{
    if (_node->hasAudioOutputs() && _node->hasMainAudioControl)
    {
        jassert (audioCtlUIContainer == nullptr);
        audioCtlUIContainer = new ConnectableNodeAudioCtlUI();
        addAndMakeVisible (audioCtlUIContainer);
        audioCtlUIContainer->setNodeAndNodeUI (_node, _nodeUI);
    }

    headerContainer->setNodeAndNodeUI (_node, _nodeUI);
    contentContainer->setNodeAndNodeUI (_node, _nodeUI);

    resized();
}

void ConnectableNodeUI::MainComponentContainer::paint (Graphics& g)
{
    if (!connectableNodeUI->connectableNode.get())return;

    g.setColour (connectableNodeUI->connectableNode->enabledParam->boolValue() ? findColour (LGMLColors::elementBackground) : findColour (LGMLColors::elementBackground).darker (.7f));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 4);

    g.setColour (connectableNodeUI->isSelected ? findColour (TextButton::buttonOnColourId) : findColour (TextButton::textColourOffId));
    g.drawRoundedRectangle (getLocalBounds().toFloat().reduced (1), 4.f, connectableNodeUI->isSelected ? 2.f : .5f);
}


void ConnectableNodeUI::MainComponentContainer::resized()
{

    // if changes in this layout take care to update  childBounds changed to update when child resize itself (ConnectableNodeContentUI::init()
    Rectangle<int> r = getLocalBounds();

    if (r.getWidth() == 0 || r.getHeight() == 0)return;

    Rectangle<int> headerBounds = r.removeFromTop (headerContainer->getHeight());
    headerContainer->setBounds (headerBounds);

    if (!miniMode)
    {

        if (audioCtlUIContainer)
        {
            r.removeFromRight (audioCtlContainerPadRight);
            audioCtlUIContainer->setBounds (r.removeFromRight (audioCtlContainerWidth).reduced (0, 4));
        }

        connectableNodeUI->nodeSize->setValue (Array<var> {r.getWidth(), r.getHeight()}, true);
        contentContainer->setBounds (r);
    }
}

void ConnectableNodeUI::MainComponentContainer::setMiniMode (bool value)
{
    if (miniMode == value) return;

    miniMode = value;

    if (miniMode)
    {
        removeChildComponent (contentContainer);

        if (audioCtlUIContainer) removeChildComponent (audioCtlUIContainer);
    }
    else
    {
        addChildComponent (contentContainer);

        if (audioCtlUIContainer) addChildComponent (audioCtlUIContainer);
    }

    headerContainer->setMiniMode (miniMode);

}

void ConnectableNodeUI::MainComponentContainer::childBoundsChanged (Component* c)
{
    if (c == contentContainer || c == audioCtlUIContainer)
    {
        int destWidth = contentContainer->getWidth() +
        (audioCtlUIContainer ? (audioCtlContainerWidth + audioCtlContainerPadRight ) : 0);
        int destHeight = contentContainer->getHeight() + headerContainer->getHeight();

        if (getWidth() != destWidth ||
            getHeight() != destHeight)
        {
            setSize (destWidth, destHeight);
        }
    }
}

String ConnectableNodeUI::MainComponentContainer::getTooltip() {
    if(auto cn = connectableNodeUI){
        return cn->getTooltip();
    }
    return String::empty;
}







// ======= CONNECTOR CONTAINER AND CONNECTOR COMPONENT ===================
ConnectableNodeUI::ConnectorContainer::ConnectorContainer (ConnectorComponent::ConnectorIOType type) : type (type), displayLevel (ConnectorComponent::MINIMAL)
{
    setInterceptsMouseClicks (false, true);

}

void ConnectableNodeUI::ConnectorContainer::setConnectorsFromNode (ConnectableNode* _node)
{
    connectors.clear();

    ConnectableNode* targetNode = _node;

    if (targetNode == nullptr)
    {
        DBG ("Target Node nullptr !");
        return;
    }


    addConnector (type, NodeConnection::ConnectionType::AUDIO, targetNode);
    addConnector (type, NodeConnection::ConnectionType::DATA, targetNode);

    resized();

}

void ConnectableNodeUI::ConnectorContainer::addConnector (ConnectorComponent::ConnectorIOType ioType, NodeConnection::ConnectionType dataType, ConnectableNode* _node)
{
    ConnectorComponent* c = new ConnectorComponent (ioType, dataType, _node);
    connectors.add (c);
    addChildComponent (c);

    c->addConnectorListener (this);
}

void ConnectableNodeUI::ConnectorContainer::connectorVisibilityChanged (ConnectorComponent*)
{
    resized();
}


void ConnectableNodeUI::ConnectorContainer::resized()
{
    Rectangle<int> r = getLocalBounds();

    r.removeFromTop (10);

    for (auto& c : connectors)
    {
        if (!c->isVisible()) continue;

        c->setBounds (r.removeFromTop (r.getWidth()));
        r.removeFromTop (15);

    }
}

ConnectorComponent* ConnectableNodeUI::ConnectorContainer::getFirstConnector (NodeConnection::ConnectionType dataType)
{
    for (int i = 0; i < connectors.size(); i++)
    {
        if (connectors.getUnchecked (i)->dataType == dataType) return connectors.getUnchecked (i);
    }

    return nullptr;
}

bool ConnectableNodeUI::ConnectorContainer::hitTest(int x, int y) {
    for(auto c:connectors){
        if(c->getBoundsInParent().contains(x,y))
            return true;
    }
    return false;
}



Array<ConnectorComponent*> ConnectableNodeUI::getComplementaryConnectors (ConnectorComponent* baseConnector)
{
    Array<ConnectorComponent*> result;


    ConnectorContainer* checkSameCont = baseConnector->ioType == ConnectorComponent::ConnectorIOType::INPUT ? &inputContainer : &outputContainer;

    if (checkSameCont->getIndexOfChildComponent (baseConnector) != -1) return result;

    ConnectorContainer* complCont = checkSameCont == &inputContainer ? &outputContainer : &inputContainer;

    for (int i = 0; i < complCont->connectors.size(); i++)
    {
        ConnectorComponent* c = (ConnectorComponent*)complCont->getChildComponent (i);

        if (c->dataType == baseConnector->dataType)
        {
            result.add (c);
        }
    }

    return result;
}

//Need to clean out and decide whether there can be more than 1 data connector / audio connector on nodes

ConnectorComponent* ConnectableNodeUI::getFirstConnector (NodeConnection::ConnectionType connectionType, ConnectorComponent::ConnectorIOType ioType)
{
    if (ioType == ConnectorComponent::INPUT)
    {
        return inputContainer.getFirstConnector (connectionType);
    }
    else
    {
        return outputContainer.getFirstConnector (connectionType);
    }
}


bool ConnectableNodeUI::hitTest(int x, int y){
    if (mainComponentContainer.getBoundsInParent().contains(x,y))
        return true;

    if(outputContainer.isVisible() && outputContainer.getBoundsInParent().contains(x, y)){
        auto lp =outputContainer.getLocalPoint(this, Point<int> (x, y));
        return outputContainer.hitTest(lp.x,lp.y) ;
    }
    if(inputContainer.isVisible() &&inputContainer.getBoundsInParent().contains(x, y)){
        auto lp =inputContainer.getLocalPoint(this, Point<int> (x, y));
        return inputContainer.hitTest(lp.x,lp.y) ;
    }
    return false;
}

#endif
