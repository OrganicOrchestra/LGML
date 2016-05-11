/*
  ==============================================================================

    NodeConnectionEditor.cpp
    Created: 29 Mar 2016 6:43:49pm
    Author:  bkupe

  ==============================================================================
*/


#include "NodeConnectionEditor.h"
#include "NodeBase.h"

#include "Style.h"



//==============================================================================
NodeConnectionEditor::NodeConnectionEditor(NodeConnectionUI * nodeConnectionUI) :
	CustomEditor(nodeConnectionUI),
	currentConnection(nullptr),
	editingLink(nullptr)
{
    addAndMakeVisible(&outputsContainer);
    addAndMakeVisible(&inputsContainer);
    addAndMakeVisible(&linksContainer);

	setCurrentConnection(nodeConnectionUI->connection);
}

NodeConnectionEditor::~NodeConnectionEditor()
{
    setCurrentConnection(nullptr);
}

void NodeConnectionEditor::setCurrentConnection(NodeConnection * _connection)
{
    if (currentConnection == _connection) return;

    if (currentConnection != nullptr)
    {
        currentConnection->removeConnectionListener(this);
        clearContent();
    }

    currentConnection = _connection;

    if (currentConnection != nullptr)
    {
        currentConnection->addConnectionListener(this);

        if (currentConnection->isAudio()) generateContentForAudio();
        else generateContentForData();
    }
}

void NodeConnectionEditor::resized()
{
	int panelWidth = (int)(getWidth() / 3.f);

    Rectangle<int> r = getLocalBounds();
    outputsContainer.setBounds(r.removeFromLeft(panelWidth));
    inputsContainer.setBounds(r.removeFromRight(panelWidth));
    linksContainer.setBounds(r);

    r = outputsContainer.getLocalBounds().withTrimmedTop(5).withTrimmedLeft(5).withTrimmedBottom(5);

    for (int i = 0; i < outputsContainer.getNumChildComponents();i++)
    {
        Component * c = outputsContainer.getChildComponent(i);
        c->setBounds(r.removeFromTop(30));
        r.removeFromTop(5);
    }

    r = inputsContainer.getLocalBounds().withTrimmedTop(5).withTrimmedRight(5).withTrimmedBottom(5);
    for (int i = 0; i < inputsContainer.getNumChildComponents(); i++)
    {
        Component * c = inputsContainer.getChildComponent(i);
        c->setBounds(r.removeFromTop(30));
        r.removeFromTop(5);
    }

    r = linksContainer.getLocalBounds();
    for (int i = 0; i < linksContainer.getNumChildComponents(); i++)
    {
        Component * c = linksContainer.getChildComponent(i);
        c->setBounds(r);
    }
}

void NodeConnectionEditor::mouseEnter(const MouseEvent &)
{
    //DBG("Editor mouse enter " << e.eventComponent->getName());
}

void NodeConnectionEditor::mouseExit(const MouseEvent &)
{

}

void NodeConnectionEditor::clearContent()
{
    for (auto &s : inputSlots)
    {
        s->removeSlotListener(this);
    }

    for (auto &s : outputSlots)
    {
        s->removeSlotListener(this);
    }

    inputSlots.clear();
    outputSlots.clear();
    links.clear();

    inputsContainer.removeAllChildren();
    outputsContainer.removeAllChildren();
    linksContainer.removeAllChildren();
}

void NodeConnectionEditor::generateContentForAudio()
{
    clearContent();

    int numOutputChannels = currentConnection->sourceNode->audioProcessor->getTotalNumOutputChannels();

    int numInputChannels = currentConnection->destNode->audioProcessor->getTotalNumInputChannels();

    DBG("generate content for audio : " << currentConnection->sourceNode->niceName << " : " << numOutputChannels << " / " << currentConnection->destNode->niceName << " : " << numInputChannels);
    for (int i = 0; i < numOutputChannels; i++)
    {
        NodeConnectionEditorDataSlot * s = new NodeConnectionEditorDataSlot("Output "+String(i+1),i,currentConnection->connectionType, NodeConnectionEditorDataSlot::IOType::OUTPUT);
        outputSlots.add(s);
        s->addSlotListener(this);
        s->setName("output" + String(i+1));
        outputsContainer.addAndMakeVisible(s);
        //DBG("Add output slot");
    }

    for (int i = 0; i < numInputChannels; i++)
    {
        NodeConnectionEditorDataSlot * s = new NodeConnectionEditorDataSlot("Input " +String(i + 1), i, currentConnection->connectionType, NodeConnectionEditorDataSlot::IOType::INPUT);
        inputSlots.add(s);
        s->addSlotListener(this);
        s->setName("input" + String(i + 1));
        inputsContainer.addAndMakeVisible(s);
        //DBG("Add input Slot");
    }

    //@Martin : How to define how many channels a node has (input and output), it seems that it's defaulted to 2,
    //but i can't find how to set it up anywhere. is it dynamically computed when sending or receiving the AudioBuffer in processBlock ?
    // should be fixed !!
    for (auto &pair : currentConnection->audioConnections)
    {
        addAudioLink(pair.first, pair.second);
    }


    resized();
}

void NodeConnectionEditor::generateContentForData()
{
    clearContent();
    //DBG("generate content for data");

    int numOutputData = currentConnection->sourceNode->dataProcessor->getTotalNumOutputData();
    int numInputData = currentConnection->destNode->dataProcessor->getTotalNumInputData();

    for (int i = 0; i < numOutputData; i++)
    {
        Data * data = currentConnection->sourceNode->dataProcessor->outputDatas[i];
        NodeConnectionEditorDataSlot * s = new NodeConnectionEditorDataSlot(data->name + " (" + data->getTypeString() + ")",data, currentConnection->connectionType, NodeConnectionEditorDataSlot::IOType::OUTPUT);
        s->setName("output"+data->name);
        s->addSlotListener(this);
        outputSlots.add(s);
        outputsContainer.addAndMakeVisible(s);
        //DBG("Add output slot");
    }

    for (int i = 0; i < numInputData; i++)
    {
        Data * data = currentConnection->destNode->dataProcessor->inputDatas[i];
        NodeConnectionEditorDataSlot * s = new NodeConnectionEditorDataSlot(data->name + " (" + data->getTypeString() + ")",data, currentConnection->connectionType, NodeConnectionEditorDataSlot::IOType::INPUT);
        s->setName("input" + data->name);
        s->addSlotListener(this);
        inputSlots.add(s);
        inputsContainer.addAndMakeVisible(s);
    }

    for (auto &connection : currentConnection->dataConnections)
    {
        addDataLink(connection->sourceData, connection->destData);
    }

    resized();
}

void NodeConnectionEditor::addAudioLink(int sourceChannel, int destChannel)
{
    NodeConnectionEditorDataSlot * os = outputSlots[sourceChannel];
    NodeConnectionEditorDataSlot * is = inputSlots[destChannel];
    NodeConnectionEditorLink * l = new NodeConnectionEditorLink(os, is);
    is->addConnectedSlot(os);
    os->addConnectedSlot(is);
    links.add(l);
    l->addLinkListener(this);
    linksContainer.addAndMakeVisible(l);
}

void NodeConnectionEditor::removeAudioLinkForChannels(int sourceChannel, int destChannel)
{
    //DBG("Remove audio Link for channels");
    NodeConnectionEditorLink * l = getLinkForChannels(sourceChannel, destChannel);
	if (l == nullptr) return;

    l->outSlot->removeConnectedSlot(l->inSlot);
    l->inSlot->removeConnectedSlot(l->outSlot);

    linksContainer.removeChildComponent(l);
    l->removeLinkListener(this);
    links.removeObject(l);
}

void NodeConnectionEditor::addDataLink(Data * sourceData, Data * destData)
{
    NodeConnectionEditorDataSlot * os = getOutputSlotForData(sourceData);
    NodeConnectionEditorDataSlot * is = getInputSlotForData(destData);
    NodeConnectionEditorLink * l = new NodeConnectionEditorLink(os, is);
    os->addConnectedSlot(is);
    is->addConnectedSlot(os);
    links.add(l);
    l->addLinkListener(this);
    linksContainer.addAndMakeVisible(l);
}

void NodeConnectionEditor::removeDataLinkForDatas(Data * sourceData, Data * destData)
{
    NodeConnectionEditorLink * l = getLinkForDatas(sourceData, destData);
    linksContainer.removeChildComponent(l);
    l->removeLinkListener(this);
    links.removeObject(l);
}

NodeConnectionEditorLink * NodeConnectionEditor::getLinkForSlots(NodeConnectionEditorDataSlot * outSlot, NodeConnectionEditorDataSlot * inSlot)
{
    for (auto &l : links)
    {
        //DBG(String("check link ") << l->outSlot->getName() << String(" <> ") << l->inSlot->getName() << "// " << outSlot->getName() << " <> " << inSlot->getName());
        if (l->outSlot == outSlot && l->inSlot == inSlot) return l;
    }
    return nullptr;
}

NodeConnectionEditorLink * NodeConnectionEditor::getLinkForChannels(int sourceChannel, int destChannel)
{
    for (auto &l : links)
    {
        if (l->outSlot->channel == sourceChannel && l->inSlot->channel == destChannel) return l;
    }

    return nullptr;
}

NodeConnectionEditorLink * NodeConnectionEditor::getLinkForDatas(Data * sourceData, Data * destData)
{
    for (auto &l : links)
    {
        if (l->outSlot->data == sourceData && l->inSlot->data == destData) return l;
    }

    return nullptr;
}

NodeConnectionEditorDataSlot * NodeConnectionEditor::getOutputSlotForData( Data * data)
{
    for (auto &slot : outputSlots)
    {
        if (slot->data == data) return slot;
    }

    return nullptr;
}

NodeConnectionEditorDataSlot * NodeConnectionEditor::getInputSlotForData(Data * data)
{
    for (auto &slot : inputSlots)
    {
        if (slot->data == data) return slot;
    }

    return nullptr;
}



void NodeConnectionEditor::createEditingLink(NodeConnectionEditorDataSlot * baseSlot)
{
    //DBG("Create Link from connector");

    if (editingLink != nullptr)
    {
        //DBG("Already editing a connection !");
        return;
    }

    if (baseSlot->ioType == NodeConnectionEditorDataSlot::IOType::OUTPUT)
    {
        editingLink = new NodeConnectionEditorLink(baseSlot, nullptr);
    }
    else
    {
        editingLink = new NodeConnectionEditorLink(nullptr, baseSlot);
    }

    addAndMakeVisible(editingLink);
}

void NodeConnectionEditor::updateEditingLink()
{
    if (editingLink == nullptr) return;

    Point<int> cPos = getLocalPoint(editingLink->getBaseSlot(), editingLink->getBaseSlot()->getLocalBounds().getCentre());
    Point<int> mPos = getMouseXYRelative();
    int minX = jmin<int>(cPos.x, mPos.x);
    int minY = jmin<int>(cPos.y, mPos.y);
    int tw = abs(cPos.x - mPos.x);
    int th = abs(cPos.y - mPos.y);
    int margin = 50;

    checkDropCandidates();

    editingLink->setBounds(minX - margin, minY - margin, tw + margin * 2, th + margin * 2);
}


bool NodeConnectionEditor::checkDropCandidates()
{
    NodeConnectionEditorDataSlot * baseSlot = editingLink->getBaseSlot();
    bool targetIsInput = baseSlot->ioType == NodeConnectionEditorDataSlot::IOType::OUTPUT;
    OwnedArray<NodeConnectionEditorDataSlot> * targetArray = targetIsInput ? &inputSlots : &outputSlots;



    for(auto &slot : *targetArray)
    {
        if (slot->isAudio() || (slot->data != nullptr && slot->data->isTypeCompatible(baseSlot->data->type)))
        {
            if (targetIsInput && slot->isData() && slot->data->numConnections >= 1)
            {
                //TODO : implement way to replace a "taken" slot with the one we are editing (with a confirmation prompt)
            }
            else
            {
                float dist = (float)(slot->getMouseXYRelative().getDistanceFrom(slot->getLocalBounds().getRelativePoint(targetIsInput?0:1,.5f)));
                if (dist < 20)
                {
                    return setCandidateDropSlot(slot);
                }
            }
        }
    }

    cancelCandidateDropSlot();
    return false;
}

void NodeConnectionEditor::finishEditingLink()
{
    bool success = editingLink->finishEditing();

    //DBG("Finish Editing, sucess ? " + String(success));



    if (success)
    {
        bool doCreateNewLink = true;

        bool inputAlreadyConnected = editingLink->inSlot->isConnected();

        if(inputAlreadyConnected)
        {
            int alertResult = AlertWindow::showNativeDialogBox("Already connected", "This slot is already connected, choose Yes to replace the existing connection with this one",true);
            //DBG("Alert result " + String(alertResult));

            if (alertResult == 1)
            {
                NodeConnectionEditorLink * l = getLinkForSlots( editingLink->inSlot->getFirstConnectedSlot(), editingLink->inSlot);
                if (l != nullptr) l->remove();
            }
            else
            {
                doCreateNewLink = false;
            }
        }

        if (doCreateNewLink)
        {
            if (currentConnection->isAudio())
            {
                currentConnection->addAudioGraphConnection(editingLink->outSlot->channel, editingLink->inSlot->channel);
            }
            else
            {
                currentConnection->addDataGraphConnection(editingLink->outSlot->data, editingLink->inSlot->data);
            }
        }
    }

    removeChildComponent(editingLink);
    editingLink = nullptr;
}

bool NodeConnectionEditor::setCandidateDropSlot(NodeConnectionEditorDataSlot * slot)
{
    if (editingLink == nullptr) return false;

    bool result = editingLink->setCandidateDropSlot(slot);
    //editingLink->candidateDropSlot->addMouseListener(this, false);
    return result;
}

void NodeConnectionEditor::cancelCandidateDropSlot()
{
    if (editingLink == nullptr) return;
    //if (editingLink->candidateDropSlot != nullptr) editingLink->candidateDropSlot->removeMouseListener(this);
    editingLink->cancelCandidateDropSlot();


}


void NodeConnectionEditor::slotMouseEnter(NodeConnectionEditorDataSlot * target)
{
    if (target->ioType == NodeConnectionEditorDataSlot::IOType::INPUT)
    {
        for (auto &s : outputSlots) s->setCurrentEditingData(target->data);
    }
    else
    {
        for (auto &s : inputSlots) s->setCurrentEditingData(target->data);
    }
}


void NodeConnectionEditor::slotMouseUp(NodeConnectionEditorDataSlot *)
{
    finishEditingLink();
}

void NodeConnectionEditor::askForRemoveLink(NodeConnectionEditorLink * target)
{
    //DBG("Ask for remove link");
    if (currentConnection->isAudio())
    {
        currentConnection->removeAudioGraphConnection(target->outSlot->channel, target->inSlot->channel);
    }
    else
    {
        currentConnection->removeDataGraphConnection(target->outSlot->data, target->inSlot->data);
    }
}


void NodeConnectionEditor::connectionDataLinkAdded(DataProcessorGraph::Connection * dataConnection)
{
    addDataLink(dataConnection->sourceData, dataConnection->destData);
    resized();
}

void NodeConnectionEditor::connectionDataLinkRemoved(DataProcessorGraph::Connection * dataConnection)
{
    removeDataLinkForDatas(dataConnection->sourceData, dataConnection->destData);
}

void NodeConnectionEditor::connectionAudioLinkAdded(const NodeConnection::AudioConnection & audioConnection)
{
    //DBG("Audio link added " << audioConnection.first << " > " << audioConnection.second);
    addAudioLink(audioConnection.first, audioConnection.second);
    resized();
}

void NodeConnectionEditor::connectionAudioLinkRemoved(const NodeConnection::AudioConnection & audioConnection)
{
    //DBG("Audio link removed");
    removeAudioLinkForChannels(audioConnection.first, audioConnection.second);
}

void NodeConnectionEditor::slotMouseExit(NodeConnectionEditorDataSlot *)
{
    for (auto &s : outputSlots) s->setCurrentEditingData(nullptr);
    for (auto &s : inputSlots) s->setCurrentEditingData(nullptr);

}

void NodeConnectionEditor::slotMouseDown(NodeConnectionEditorDataSlot * target)
{
    createEditingLink(target);
}

void NodeConnectionEditor::slotMouseMove(NodeConnectionEditorDataSlot *)
{
    //updateEditingLink();
}

void NodeConnectionEditor::slotMouseDrag(NodeConnectionEditorDataSlot *)
{
    updateEditingLink();
}
