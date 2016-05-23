/*
  ==============================================================================

    ConnectableNodeUI.cpp
    Created: 18 May 2016 11:34:23pm
    Author:  bkupe

  ==============================================================================
*/

#include "ConnectableNodeUI.h"
#include "NodeContainer.h"
#include "ContainerInNode.h"
#include "ContainerOutNode.h"


// needed because main component need to know it for its scopedPointer
#include "FloatSliderUI.h"


ConnectableNodeUI::ConnectableNodeUI(ConnectableNode * cn, ConnectableNodeContentUI * contentUI, ConnectableNodeHeaderUI * headerUI) :
	InspectableComponent(cn),
	connectableNode(cn),
	inputContainer(ConnectorComponent::ConnectorIOType::INPUT),
	outputContainer(ConnectorComponent::ConnectorIOType::OUTPUT),
	mainContainer(this, contentUI, headerUI),
	dragIsLocked(false),
	miniMode(false)
{
	 connectorWidth = 10;

	 addAndMakeVisible(mainContainer);

	 if (connectableNode->userCanAccessInputs)
	 {
		 inputContainer.setConnectorsFromNode(connectableNode);
		 addAndMakeVisible(inputContainer);
	 }

	 if (connectableNode->userCanAccessOutputs)
	 {
		 outputContainer.setConnectorsFromNode(connectableNode);
		 addAndMakeVisible(outputContainer);
	 }

	 getHeaderContainer()->addMouseListener(this, true);// (true, true);

	 mainContainer.setNodeAndNodeUI(connectableNode, this);
	 if (getWidth() == 0 || getHeight() == 0) setSize(180, 100);

	 connectableNode->xPosition->addParameterListener(this);
	 connectableNode->yPosition->addParameterListener(this);
	 connectableNode->xPosition->hideInEditor = true;
	 connectableNode->yPosition->hideInEditor = true;

	 connectableNode->enabledParam->addParameterListener(this);

}

ConnectableNodeUI::~ConnectableNodeUI()
{
	connectableNode->xPosition->removeParameterListener(this);
	connectableNode->yPosition->removeParameterListener(this);
	connectableNode->enabledParam->removeParameterListener(this);
}



void ConnectableNodeUI::moved() {

	if (connectableNode->xPosition->intValue() != getBounds().getCentreX() || connectableNode->yPosition->intValue() != getBounds().getCentreY()) {
		connectableNode->xPosition->setValue((float)getBounds().getCentreX());
		connectableNode->yPosition->setValue((float)getBounds().getCentreY());
	}
}


void ConnectableNodeUI::setMiniMode(bool value)
{
	if (miniMode == value) return;
	DBG("CUI set miniMode");
	miniMode = value;

	mainContainer.setMiniMode(miniMode);
	setSize(getMiniModeWidth(miniMode),getMiniModeHeight(miniMode));
}

int ConnectableNodeUI::getMiniModeWidth(bool forMiniMode)
{
	return forMiniMode ? 180 : (getContentContainer()->getWidth() + inputContainer.getWidth()+outputContainer.getWidth() + (mainContainer.audioCtlUIContainer?mainContainer.audioCtlUIContainer->getWidth()+mainContainer.audioCtlContainerPadRight:0));
}

int ConnectableNodeUI::getMiniModeHeight(bool forMiniMode)
{
	return getHeaderContainer()->getBottom() + (forMiniMode?10:getContentContainer()->getHeight());
}

void ConnectableNodeUI::paint(Graphics&)
{

}

void ConnectableNodeUI::resized()
{
	Rectangle<int> r = getLocalBounds();
	Rectangle<int> inputBounds = r.removeFromLeft(connectorWidth);
	Rectangle<int> outputBounds = r.removeFromRight(connectorWidth);

	if (connectableNode->userCanAccessInputs)
	{
		inputContainer.setBounds(inputBounds);
	}

	if (connectableNode->userCanAccessOutputs)
	{
		outputContainer.setBounds(outputBounds);
	}

	mainContainer.setBounds(r);
}

void ConnectableNodeUI::parameterValueChanged(Parameter * p) {

	if (p == connectableNode->xPosition || p == connectableNode->yPosition) {
		setCentrePosition((int)connectableNode->xPosition->value, (int)connectableNode->yPosition->value);
	}
	else if (p == connectableNode->enabledParam)
	{
		mainContainer.repaint();
	}
}



// allow to react to custom mainContainer.contentContainer
void ConnectableNodeUI::childBoundsChanged(Component* c) {
	// if changes in this layout take care to update  childBounds changed to update when child resize itself (ConnectableNodeContentUI::init()
	if (c == &mainContainer) {
		int destWidth = mainContainer.getWidth() + 2 * connectorWidth;
		int destHeight = mainContainer.getHeight();
		if (getWidth() != destWidth ||
			destHeight != getHeight()) {
			setSize(destWidth, destHeight);
		}
	}
}


#pragma warning( disable : 4100 ) //still don't understand why this is generating a warning if not disabled by pragma.
void ConnectableNodeUI::mouseDown(const juce::MouseEvent &e)
{
	if (e.eventComponent != &mainContainer.headerContainer->grabber) return;

	nodeInitPos = getBounds().getCentre();

	/*
	// don't want to drag if over volume
	if(ConnectableNodeAudioCtlUI * ctlUI = mainContainer.audioCtlUIContainer){
	Point<int> mouse = getMouseXYRelative();
	Component * found = getComponentAt(mouse.x,mouse.y);

	dragIsLocked = (dynamic_cast<ConnectableNodeAudioCtlUI *>(found) == ctlUI);
	}
	*/
}
#pragma warning( default : 4100 )


void ConnectableNodeUI::mouseUp(const juce::MouseEvent &) {
	selectThis();
}

void ConnectableNodeUI::mouseDrag(const MouseEvent & e)
{
	if (e.eventComponent != &mainContainer.headerContainer->grabber) return;
	//if(dragIsLocked) return;

	Point<int> diff = Point<int>(e.getPosition() - e.getMouseDownPosition());
	Point <int> newPos = nodeInitPos + diff;
	newPos.x = jmax<int>(newPos.x, 0);
	newPos.y = jmax<int>(newPos.y, 60); //find a way to top to 0

	connectableNode->xPosition->setValue((float)newPos.x);
	connectableNode->yPosition->setValue((float)newPos.y);
}

bool ConnectableNodeUI::keyPressed(const KeyPress & key)
{
	if (!isSelected) return false;
	if (key.getKeyCode() == KeyPress::deleteKey || key.getKeyCode() == KeyPress::backspaceKey)
	{
		connectableNode->remove();
		return true;
	}

	return false;
}

////////////    MAIN CONTAINER


ConnectableNodeUI::MainContainer::MainContainer(ConnectableNodeUI * _nodeUI, ConnectableNodeContentUI * content, ConnectableNodeHeaderUI * header) :
	connectableNodeUI(_nodeUI),
	headerContainer(header),
	contentContainer(content),
	audioCtlUIContainer(nullptr),
	miniMode(false)
{

	if (headerContainer == nullptr) headerContainer = new ConnectableNodeHeaderUI();
	if (contentContainer == nullptr) contentContainer = new ConnectableNodeContentUI();


	addAndMakeVisible(headerContainer);
	addAndMakeVisible(contentContainer);

}

void ConnectableNodeUI::MainContainer::setNodeAndNodeUI(ConnectableNode * _node, ConnectableNodeUI * _nodeUI)
{
	if (_node->hasAudioOutputs() && _node->hasMainAudioControl) {
		jassert(audioCtlUIContainer == nullptr);
		audioCtlUIContainer = new ConnectableNodeAudioCtlUI();
		addAndMakeVisible(audioCtlUIContainer);
		audioCtlUIContainer->setNodeAndNodeUI(_node, _nodeUI);
	}

	headerContainer->setNodeAndNodeUI(_node, _nodeUI);
	contentContainer->setNodeAndNodeUI(_node, _nodeUI);

	resized();
}

void ConnectableNodeUI::MainContainer::paint(Graphics & g)
{
	g.setColour(connectableNodeUI->connectableNode->enabledParam->boolValue() ? PANEL_COLOR : PANEL_COLOR.darker(.7f));
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 4);

	g.setColour(connectableNodeUI->isSelected ? HIGHLIGHT_COLOR : LIGHTCONTOUR_COLOR);
	g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1), 4.f, connectableNodeUI->isSelected ? 2.f : .5f);
}


void ConnectableNodeUI::MainContainer::resized()
{

	// if changes in this layout take care to update  childBounds changed to update when child resize itself (ConnectableNodeContentUI::init()
	Rectangle<int> r = getLocalBounds();
	if (r.getWidth() == 0 || r.getHeight() == 0)return;

	Rectangle<int> headerBounds = r.removeFromTop(headerContainer->getHeight());
	headerContainer->setBounds(headerBounds);

	if (!miniMode)
	{
		if (audioCtlUIContainer) {
			r.removeFromRight(audioCtlContainerPadRight);
			audioCtlUIContainer->setBounds(r.removeFromRight(10).reduced(0, 4));
		}
		contentContainer->setBounds(r);
	}
}

void ConnectableNodeUI::MainContainer::setMiniMode(bool value)
{
	if (miniMode == value) return;
	miniMode = value;

	if (miniMode)
	{
		removeChildComponent(contentContainer);
		if(audioCtlUIContainer) removeChildComponent(audioCtlUIContainer);
	}else
	{
		addChildComponent(contentContainer);
		if (audioCtlUIContainer) addChildComponent(audioCtlUIContainer);
	}

	headerContainer->setMiniMode(miniMode);

}

void ConnectableNodeUI::MainContainer::childBoundsChanged(Component* c) {
	if (c == contentContainer || c == audioCtlUIContainer) {
		int destWidth = contentContainer->getWidth() + (audioCtlUIContainer ? audioCtlUIContainer->getWidth() + audioCtlContainerPadRight : 0);
		int destHeight = contentContainer->getHeight() + headerContainer->getHeight();
		if (getWidth() != destWidth ||
			getHeight() != destHeight) {
			setSize(destWidth, destHeight);
		}
	}
}







// ======= CONNECTOR CONTAINER AND CONNECTOR COMPONENT ===================
ConnectableNodeUI::ConnectorContainer::ConnectorContainer(ConnectorComponent::ConnectorIOType type) : type(type), displayLevel(ConnectorComponent::MINIMAL)
{
	setInterceptsMouseClicks(false, true);

}

void ConnectableNodeUI::ConnectorContainer::setConnectorsFromNode(ConnectableNode * _node)
{
	connectors.clear();

	//If container, go for containerIn / containerOut
	ConnectableNode * targetNode = _node;
	if (_node->type == NodeType::ContainerType)
	{
		if (type == ConnectorComponent::INPUT)
		{
			targetNode = ((NodeContainer *)_node)->containerInNode;
		}
		else
		{
			targetNode = ((NodeContainer *)_node)->containerOutNode;
		}
	}

	if (targetNode == nullptr)
	{
		DBG("Target Node nullptr !");
		return;
	}


	addConnector(type, NodeConnection::ConnectionType::AUDIO, targetNode);
	addConnector(type, NodeConnection::ConnectionType::DATA, targetNode);

	resized();

}

void ConnectableNodeUI::ConnectorContainer::addConnector(ConnectorComponent::ConnectorIOType ioType, NodeConnection::ConnectionType dataType, ConnectableNode * _node)
{
	ConnectorComponent * c = new ConnectorComponent(ioType, dataType, _node);
	connectors.add(c);
	addChildComponent(c);

	c->addConnectorListener(this);
}

void ConnectableNodeUI::ConnectorContainer::connectorVisibilityChanged(ConnectorComponent *)
{
	resized();
}


void ConnectableNodeUI::ConnectorContainer::resized()
{
	Rectangle<int> r = getLocalBounds();

	r.removeFromTop(10);
	for (auto &c : connectors)
	{
		if (!c->isVisible()) continue;

		c->setBounds(r.removeFromTop(r.getWidth()));
		r.removeFromTop(15);

	}
}

ConnectorComponent * ConnectableNodeUI::ConnectorContainer::getFirstConnector(NodeConnection::ConnectionType dataType)
{
	for (int i = 0; i < connectors.size(); i++)
	{
		if (connectors.getUnchecked(i)->dataType == dataType) return connectors.getUnchecked(i);
	}

	return nullptr;
}



Array<ConnectorComponent*> ConnectableNodeUI::getComplementaryConnectors(ConnectorComponent * baseConnector)
{
	Array<ConnectorComponent *> result;


	ConnectorContainer * checkSameCont = baseConnector->ioType == ConnectorComponent::ConnectorIOType::INPUT ? &inputContainer : &outputContainer;
	if (checkSameCont->getIndexOfChildComponent(baseConnector) != -1) return result;

	ConnectorContainer * complCont = checkSameCont == &inputContainer ? &outputContainer : &inputContainer;
	for (int i = 0; i < complCont->connectors.size(); i++)
	{
		ConnectorComponent *c = (ConnectorComponent *)complCont->getChildComponent(i);
		if (c->dataType == baseConnector->dataType)
		{
			result.add(c);
		}
	}

	return result;
}

//Need to clean out and decide whether there can be more than 1 data connector / audio connector on nodes

ConnectorComponent * ConnectableNodeUI::getFirstConnector(NodeConnection::ConnectionType connectionType, ConnectorComponent::ConnectorIOType ioType)
{
	if (ioType == ConnectorComponent::INPUT)
	{
		return inputContainer.getFirstConnector(connectionType);
	}
	else
	{
		return outputContainer.getFirstConnector(connectionType);
	}
}
