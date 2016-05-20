#include "ConnectorComponent.h"
#include "NodeContainerViewer.h"
#include "NodeFactory.h"
#include "Style.h"

ConnectorComponent::ConnectorComponent(ConnectorIOType _ioType, NodeConnection::ConnectionType _dataType, ConnectableNode * _node) :
    ioType(_ioType), 
	dataType(_dataType), 
	node(_node),
	isHovered(false)
{
    boxColor = dataType == NodeConnection::ConnectionType::AUDIO ? AUDIO_COLOR : DATA_COLOR;
    setSize(10,10);

    if(node->type == NodeType::ContainerType){
		if(ioType == ConnectorIOType::INPUT) ((NodeContainer *)node)->containerInNode->addNodeAudioProcessorListener(this);
		else ((NodeContainer *)node)->containerOutNode->addNodeAudioProcessorListener(this);
	}
	else
	{
		((NodeBase *)node)->addNodeAudioProcessorListener(this);
	}
    generateToolTip();
}

ConnectorComponent::~ConnectorComponent(){
	if (node->type == NodeType::ContainerType) {
		if (ioType == ConnectorIOType::INPUT)((NodeContainer *)node)->containerInNode->removeNodeAudioProcessorListener(this);
		else ((NodeContainer *)node)->containerOutNode->removeNodeAudioProcessorListener(this);
	}
	else
	{
		((NodeBase *)node)->removeNodeAudioProcessorListener(this);
	}
}

void ConnectorComponent::generateToolTip(){
    String tooltip;
    tooltip += dataType == NodeConnection::ConnectionType::AUDIO?"Audio\n":"Data\n";
    if (dataType == NodeConnection::ConnectionType::AUDIO)
    {
		bool isInput = ioType == ConnectorIOType::INPUT;
		AudioProcessorGraph::Node * tAudioNode = node->getAudioNode(isInput);
		if (tAudioNode != nullptr)
		{
			tooltip += isInput? tAudioNode->getProcessor()->getTotalNumInputChannels() : tAudioNode->getProcessor()->getTotalNumOutputChannels();
			tooltip += " channels";
		}
		else
		{
			tooltip = "[Error accessing audio processor]";
		}
    }
    else
    {
        StringArray dataInfos = ioType == ConnectorIOType::INPUT ? node->getInputDataInfos() : node->getOutputDataInfos();
        tooltip += dataInfos.joinIntoString("\n");
    }
      setTooltip(tooltip);
}
void ConnectorComponent::paint(Graphics & g)
{
    g.setGradientFill(ColourGradient(isHovered?boxColor.brighter(5.f):boxColor, (float)(getLocalBounds().getCentreY()),(float)(getLocalBounds().getCentreY()), boxColor.darker(), 0.f,0.f, true));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 2);
}

void ConnectorComponent::mouseDown(const MouseEvent &)
{
    NodeContainerViewer * containerViewer = getNodeContainerViewer();

    if (dataType == NodeConnection::ConnectionType::DATA)
    {

		containerViewer->createDataConnectionFromConnector(this);
    }
    else
    {
		containerViewer->createAudioConnectionFromConnector(this);
    }
}

void ConnectorComponent::mouseEnter (const MouseEvent&){
    isHovered = true;
    repaint();
}
void ConnectorComponent::mouseExit  (const MouseEvent&){
    isHovered = false;
    repaint();
}

void ConnectorComponent::numAudioInputChanged(NodeBase *, int){
	generateToolTip();
}
void ConnectorComponent::numAudioOutputChanged(NodeBase *, int){
	generateToolTip();
}
 
NodeContainerViewer * ConnectorComponent::getNodeContainerViewer() const noexcept
{
    return findParentComponentOfClass<NodeContainerViewer>();
}

ConnectableNodeUI * ConnectorComponent::getNodeUI() const noexcept
{
    return findParentComponentOfClass<ConnectableNodeUI>();
}
