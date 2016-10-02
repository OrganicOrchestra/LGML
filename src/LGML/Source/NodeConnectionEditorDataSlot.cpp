/*
  ==============================================================================

    NodeConnectionEditorDataSlot.cpp
    Created: 29 Mar 2016 7:03:43pm
    Author:  bkupe

  ==============================================================================
*/


#include "NodeConnectionEditorDataSlot.h"
#include "Style.h"
//==============================================================================
NodeConnectionEditorDataSlot::NodeConnectionEditorDataSlot(String label, Data* data,
    NodeConnection::ConnectionType connectionType, IOType ioType) :label(label), data(data), channel(-1),
    connectionType(connectionType), ioType(ioType), currentEditingData(nullptr)
{


}

NodeConnectionEditorDataSlot::NodeConnectionEditorDataSlot(String label, int channel,
    NodeConnection::ConnectionType connectionType, IOType ioType) :label(label), data(nullptr), channel(channel),
connectionType(connectionType), ioType(ioType), currentEditingData(nullptr)
{
}

NodeConnectionEditorDataSlot::~NodeConnectionEditorDataSlot()
{
    data = nullptr;
}

void NodeConnectionEditorDataSlot::paint (Graphics& g)
{

    g.setColour (isMouseOver() ? HIGHLIGHT_COLOR:TEXTNAME_COLOR);
    g.setFont(12);

    g.drawText (label, getLocalBounds(),Justification::centred, true);   // draw some placeholder text

    Colour c = isMouseOver() ? HIGHLIGHT_COLOR : (isConnected() ? Colours::lightblue : NORMAL_COLOR);

    if (currentEditingData != nullptr)
    {
        if (currentEditingData->type == data->type) c = Colours::lightgreen;
        else if (currentEditingData->isTypeCompatible(data->type)) c = Colours::yellow;
        else c = NORMAL_COLOR;
    }
    g.setColour(c);

    Rectangle<int> connectorBounds = getLocalBounds();
    int connectorSize = 10;
    connectorBounds.setSize(connectorSize, connectorSize);
    int targetX = ioType == OUTPUT ? getLocalBounds().getRight() - connectorSize / 2 : connectorSize / 2;
    connectorBounds.setCentre(targetX, getLocalBounds().getCentreY());
    g.fillRoundedRectangle(connectorBounds.toFloat(),4);   // draw an outline around the component

}

void NodeConnectionEditorDataSlot::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void NodeConnectionEditorDataSlot::mouseDown(const MouseEvent &)
{
    listeners.call(&SlotListener::slotMouseDown,this);
}

void NodeConnectionEditorDataSlot::mouseEnter(const MouseEvent &)
{
    listeners.call(&SlotListener::slotMouseEnter, this);
    repaint();
}

void NodeConnectionEditorDataSlot::mouseExit(const MouseEvent &)
{
    listeners.call(&SlotListener::slotMouseExit, this);
    repaint();
}

void NodeConnectionEditorDataSlot::mouseMove(const MouseEvent &)
{
    listeners.call(&SlotListener::slotMouseMove, this);
}

void NodeConnectionEditorDataSlot::mouseUp(const MouseEvent &)
{
    listeners.call(&SlotListener::slotMouseUp, this);
}

void NodeConnectionEditorDataSlot::mouseDrag(const MouseEvent &)
{
    listeners.call(&SlotListener::slotMouseDrag, this);
}