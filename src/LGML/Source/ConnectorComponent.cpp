#include "ConnectorComponent.h"
#include "NodeContainerViewer.h"
#include "Style.h"

ConnectorComponent::ConnectorComponent(ConnectorIOType _ioType, NodeConnection::ConnectionType _dataType, NodeBase * _node) :
    ioType(_ioType), 
	dataType(_dataType), 
	node(_node),
	isHovered(false)
{
    boxColor = dataType == NodeConnection::ConnectionType::AUDIO ? AUDIO_COLOR : DATA_COLOR;
    setSize(10,10);
    if(node){
		node->addNodeAudioProcessorListener(this);
	}
    generateToolTip();
}

ConnectorComponent::~ConnectorComponent(){
    if(node){
		node->removeNodeAudioProcessorListener(this);
	}
}

void ConnectorComponent::generateToolTip(){
    String tooltip;
    tooltip += dataType == NodeConnection::ConnectionType::AUDIO?"Audio\n":"Data\n";
    if (dataType == NodeConnection::ConnectionType::AUDIO)
    {
        tooltip += ioType == ConnectorIOType::INPUT? node->getTotalNumInputChannels() : node->getTotalNumOutputChannels();
        tooltip += " channels";
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

void ConnectorComponent::numAudioInputChanged(int){generateToolTip();}
void ConnectorComponent::numAudioOutputChanged(int){generateToolTip();}
 
NodeContainerViewer * ConnectorComponent::getNodeContainerViewer() const noexcept
{
    return findParentComponentOfClass<NodeContainerViewer>();
}

ConnectableNodeUI * ConnectorComponent::getNodeUI() const noexcept
{
    return findParentComponentOfClass<ConnectableNodeUI>();
}
