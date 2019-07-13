/*
  ==============================================================================

    ConnectorContainerComponent.h
    Created: 12 Jul 2019 12:42:15am
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once


class ConnectorContainerComponent :
public juce::Component,
public ConnectorComponent::ConnectorListener
{
public:
    OwnedArray<ConnectorComponent> connectors;

    ConnectorComponent::ConnectorDisplayLevel displayLevel;
    ConnectorComponent::ConnectorIOType type;

    explicit ConnectorContainerComponent (ConnectorComponent::ConnectorIOType type);

    void setConnectorsFromNode (ConnectableNode* node);
    void addConnector (ConnectorComponent::ConnectorIOType ioType, NodeConnection::ConnectionType dataType, ConnectableNode* node);
    void resized() override;

    void connectorVisibilityChanged (ConnectorComponent*) override;
    bool hitTest(int x, int y) override;

    ConnectorComponent* getFirstConnector (NodeConnection::ConnectionType dataType);



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ConnectorContainerComponent)
};



ConnectorContainerComponent::ConnectorContainerComponent (ConnectorComponent::ConnectorIOType type) : type (type), displayLevel (ConnectorComponent::MINIMAL)
{
    setInterceptsMouseClicks (false, true);
    setPaintingIsUnclipped(true);
}

void ConnectorContainerComponent::setConnectorsFromNode (ConnectableNode* _node)
{
    connectors.clear();

    ConnectableNode* targetNode = _node;

    if (targetNode == nullptr)
    {
        DBG ("Target Node nullptr !");
        return;
    }


    addConnector (type, NodeConnection::ConnectionType::AUDIO, targetNode);


    resized();

}

void ConnectorContainerComponent::addConnector (ConnectorComponent::ConnectorIOType ioType, NodeConnection::ConnectionType dataType, ConnectableNode* _node)
{
    ConnectorComponent* c = new ConnectorComponent (ioType, dataType, _node);
    connectors.add (c);
    addChildComponent (c);

    c->addConnectorListener (this);
}

void ConnectorContainerComponent::connectorVisibilityChanged (ConnectorComponent*)
{
    resized();
}


void ConnectorContainerComponent::resized()
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

ConnectorComponent* ConnectorContainerComponent::getFirstConnector (NodeConnection::ConnectionType dataType)
{
    for (int i = 0; i < connectors.size(); i++)
    {
        if (connectors.getUnchecked (i)->dataType == dataType) return connectors.getUnchecked (i);
    }

    return nullptr;
}

bool ConnectorContainerComponent::hitTest(int x, int y) {
    for(auto c:connectors){
        if(c->getBoundsInParent().contains(x,y))
            return true;
    }
    return false;
}

