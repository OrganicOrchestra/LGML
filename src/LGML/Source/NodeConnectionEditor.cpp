/*
  ==============================================================================

    NodeConnectionEditor.cpp
    Created: 29 Mar 2016 6:43:49pm
    Author:  bkupe

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "NodeConnectionEditor.h"
#include "NodeBase.h"

#include "Style.h"

juce_ImplementSingleton(NodeConnectionEditor);


//==============================================================================
NodeConnectionEditor::NodeConnectionEditor() : DocumentWindow("Connection Editor",BG_COLOR,DocumentWindow::closeButton,false)
{
    currentConnection = nullptr;
	editingLink = nullptr;

    setSize(500,500);
    setResizable(true, false);
    setUsingNativeTitleBar(true);
    mainContainer.setBounds(0, 0, 500, 500);
    setContentOwned(&mainContainer,true);

	mainContainer.addAndMakeVisible(&outputsContainer);
	mainContainer.addAndMakeVisible(&inputsContainer);
	mainContainer.addAndMakeVisible(&linksContainer);

}

NodeConnectionEditor::~NodeConnectionEditor()
{
    setCurrentConnection(nullptr);
    setVisible(false);
    removeFromDesktop();
}

void NodeConnectionEditor::setCurrentConnection(NodeConnection * _connection)
{
    if (currentConnection == _connection) return;

    if (_connection != nullptr)
    {
        clearContent();
    }

    currentConnection = _connection;

    if (currentConnection != nullptr)
    {
		DBG("Set current connection, type = " << currentConnection->connectionType);
		if (currentConnection->connectionType == NodeConnection::ConnectionType::AUDIO) generateContentForAudio();
		else generateContentForData();
    }
}

void NodeConnectionEditor::editConnection(NodeConnection * _connection)
{
    addToDesktop();
    setTopLeftPosition(200, 200);
    setVisible(true);
	toFront(true);
	//setAlwaysOnTop(true);
	
	DBG("Edit connection " << String(_connection != nullptr));
	setCurrentConnection(_connection);
}

void NodeConnectionEditor::resized()
{
    DBG("editor resized " << mainContainer.getBounds().toString());
	mainContainer.setBounds(0,0,getWidth(),getHeight());

	float panelWidth = jlimit<float>(150, 400, getWidth() / 3.f);

	Rectangle<int> r = mainContainer.getLocalBounds();
	outputsContainer.setBounds(r.removeFromLeft(panelWidth));
	inputsContainer.setBounds(r.removeFromRight(panelWidth));
	linksContainer.setBounds(r); 
	
	r = outputsContainer.getLocalBounds().withTrimmedTop(5).withTrimmedLeft(5).withTrimmedBottom(5);
	
	for (int i = 0; i < outputsContainer.getNumChildComponents();i++)
	{
		Component * c = outputsContainer.getChildComponent(i);
		c->setBounds(r.removeFromTop(50));
		r.removeFromTop(10);
	}

	r = inputsContainer.getLocalBounds().withTrimmedTop(5).withTrimmedRight(5).withTrimmedBottom(5);
	for (int i = 0; i < inputsContainer.getNumChildComponents(); i++)
	{
		Component * c = inputsContainer.getChildComponent(i);
		c->setBounds(r.removeFromTop(50));
		r.removeFromTop(10);
	}

	r = linksContainer.getLocalBounds();
	for (int i = 0; i < linksContainer.getNumChildComponents(); i++)
	{
		Component * c = linksContainer.getChildComponent(i);
		c->setBounds(r);
	}
}

void NodeConnectionEditor::closeButtonPressed()
{
    setCurrentConnection(nullptr);
	removeFromDesktop();
}

void NodeConnectionEditor::mouseEnter(const MouseEvent &e)
{
	//DBG("Editor mouse enter " << e.eventComponent->getName());

	
}

void NodeConnectionEditor::mouseExit(const MouseEvent &e)
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

	//DBG("generate content for audio " << numOutputChannels << " / " << numInputChannels);
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

	for (auto &pair : currentConnection->audioConnections)
	{
		NodeConnectionEditorLink * l = new NodeConnectionEditorLink(inputSlots[pair.first],outputSlots[pair.second]);
		inputSlots[pair.first]->setConnected(true);
		outputSlots[pair.second]->setConnected(true);
		links.add(l);
		linksContainer.addAndMakeVisible(l);
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
		DataProcessor::Data * data = currentConnection->sourceNode->dataProcessor->outputDatas[i];
		NodeConnectionEditorDataSlot * s = new NodeConnectionEditorDataSlot(data->name + " (" + data->getTypeString() + ")",data, currentConnection->connectionType, NodeConnectionEditorDataSlot::IOType::OUTPUT);
		s->setName("output"+data->name);
		s->addSlotListener(this);
		outputSlots.add(s);
		outputsContainer.addAndMakeVisible(s);
		//DBG("Add output slot");
	}

	for (int i = 0; i < numInputData; i++)
	{
		DataProcessor::Data * data = currentConnection->destNode->dataProcessor->inputDatas[i];
		NodeConnectionEditorDataSlot * s = new NodeConnectionEditorDataSlot(data->name + " (" + data->getTypeString() + ")",data, currentConnection->connectionType, NodeConnectionEditorDataSlot::IOType::INPUT);
		s->setName("input" + data->name);
		s->addSlotListener(this);
		inputSlots.add(s);
		inputsContainer.addAndMakeVisible(s);
		//DBG("Add input Slot");
	}

	//@Martin : How to define how many channels a node has (input and output), it seems that it's defaulted to 2, 
	//but i can't find how to set it up anywhere. is it dynamically computed when sending or receiving the AudioBuffer in processBlock ?

	for (auto &connection : currentConnection->dataConnections)
	{
		NodeConnectionEditorDataSlot * os = getOutputSlotForData(connection->sourceData);
		NodeConnectionEditorDataSlot * is = getInputSlotForData(connection->destData);
		NodeConnectionEditorLink * l = new NodeConnectionEditorLink(os,is);
		os->setConnected(true);
		is->setConnected(true);
		links.add(l);
		linksContainer.addAndMakeVisible(l);
	}

	resized();
}

NodeConnectionEditorDataSlot * NodeConnectionEditor::getOutputSlotForData( DataProcessor::Data * data)
{
	for (auto &slot : outputSlots)
	{
		if (slot->data == data) return slot;
	}

	return nullptr;
}

NodeConnectionEditorDataSlot * NodeConnectionEditor::getInputSlotForData(DataProcessor::Data * data)
{
	for (auto &slot : inputSlots)
	{
		if (slot->data == data) return slot;
	}

	return nullptr;
}



void NodeConnectionEditor::createEditingLink(NodeConnectionEditorDataSlot * baseSlot)
{
	DBG("Create Link from connector");

	if (editingLink != nullptr)
	{
		DBG("Already editing a connection !");
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

	mainContainer.addAndMakeVisible(editingLink);
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
		if (slot->connectionType == NodeConnection::ConnectionType::AUDIO || (slot->data != nullptr && slot->data->isTypeCompatible(baseSlot->data->type)))
		{
			if (targetIsInput && slot->connectionType == NodeConnection::ConnectionType::DATA && slot->data->numConnections >= 1)
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

	DBG("Finish Editing, sucess ? " + String(success));
	mainContainer.removeChildComponent(editingLink);
	delete editingLink;
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


void NodeConnectionEditor::slotMouseUp(NodeConnectionEditorDataSlot * target)
{
	finishEditingLink();
}

void NodeConnectionEditor::slotMouseExit(NodeConnectionEditorDataSlot * target)
{
	for (auto &s : outputSlots) s->setCurrentEditingData(nullptr);
	for (auto &s : inputSlots) s->setCurrentEditingData(nullptr);
	
}

void NodeConnectionEditor::slotMouseDown(NodeConnectionEditorDataSlot * target)
{
	createEditingLink(target);
}

void NodeConnectionEditor::slotMouseMove(NodeConnectionEditorDataSlot * target)
{
	//updateEditingLink();
}

void NodeConnectionEditor::slotMouseDrag(NodeConnectionEditorDataSlot * target)
{
	updateEditingLink();
}

