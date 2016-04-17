/*
  ==============================================================================

    NodeConnectionEditorLink.cpp
    Created: 29 Mar 2016 7:00:07pm
    Author:  bkupe

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "NodeConnectionEditorLink.h"

#include "Style.h"
#include "UIHelpers.h"

//==============================================================================
NodeConnectionEditorLink::NodeConnectionEditorLink(NodeConnectionEditorDataSlot * outSlot, NodeConnectionEditorDataSlot * inSlot) :
	outSlot(outSlot), inSlot(inSlot), candidateDropSlot(nullptr)
{
	isEditing = (outSlot != nullptr && inSlot == nullptr) || (outSlot == nullptr && inSlot != nullptr);
}

NodeConnectionEditorLink::~NodeConnectionEditorLink()
{

}

bool NodeConnectionEditorLink::setCandidateDropSlot(NodeConnectionEditorDataSlot * slot)
{
	//check if connector can accept data
	NodeConnectionEditorDataSlot * baseSlot = getBaseSlot();
	if (baseSlot == slot) return false;
	if (baseSlot->ioType == slot->ioType) return false;
	if (baseSlot->connectionType != slot->connectionType) return false;
	
	if (baseSlot->connectionType == NodeConnection::ConnectionType::DATA && !(baseSlot->data->isTypeCompatible(slot->data->type))) return false;

	candidateDropSlot = slot;

	return true;
}

void NodeConnectionEditorLink::cancelCandidateDropSlot()
{
	candidateDropSlot = nullptr;
}

bool NodeConnectionEditorLink::finishEditing()
{
	bool success = candidateDropSlot != nullptr;

	if (success)
	{
		if (outSlot == nullptr)
		{
			outSlot = candidateDropSlot;
		}
		else
		{
			inSlot = candidateDropSlot;

		}
	}

	candidateDropSlot = nullptr;
	setEditing(false);

	return success;
}


void NodeConnectionEditorLink::paint (Graphics& g)
{

    
	bool baseIsOutput = getBaseSlot()->ioType == NodeConnectionEditorDataSlot::IOType::OUTPUT;

	Point<float> sourcePos;
	Point<float> endPos;

	if (isEditing)
	{
		sourcePos = getLocalPoint(getBaseSlot(), getBaseSlot()->getLocalBounds().getRelativePoint(baseIsOutput?1:0,.5f)).toFloat();
		endPos = (candidateDropSlot != nullptr) ?
			getLocalPoint(candidateDropSlot, candidateDropSlot->getLocalBounds().getRelativePoint(baseIsOutput ?0:1, .5f)).toFloat() :
			getMouseXYRelative().toFloat();
	}
	else
	{
		sourcePos = getLocalPoint(outSlot, outSlot->getLocalBounds().getRelativePoint(1, .5f)).toFloat();
		endPos = getLocalPoint(inSlot, inSlot->getLocalBounds().getRelativePoint(0, .5f)).toFloat();
	}

	g.setColour(Colours::lightgreen);
	g.drawLine(sourcePos.x, sourcePos.y, endPos.x, endPos.y);


	/*
	Point<float> midPoint = (sourcePos + endPos) / 2;

	float smoothBigConnector = 1 + .01f*(jmax<float>(10, std::abs(endPos.x - sourcePos.x)) - 10);
	float anchorOffset = (endPos.x - sourcePos.x) / (2 * smoothBigConnector);
	float sourceAnchorX = sourcePos.x + anchorOffset;
	float endAnchorX = endPos.x - anchorOffset;


	int hitMargin = 10;
	hitPath.clear();
	hitPath.startNewSubPath(sourcePos.x, sourcePos.y - hitMargin);
	hitPath.quadraticTo(sourceAnchorX, sourcePos.y - hitMargin, midPoint.x, midPoint.y - hitMargin);
	hitPath.quadraticTo(endAnchorX, endPos.y - hitMargin, endPos.x, endPos.y - hitMargin);
	hitPath.lineTo(endPos.x, endPos.y + hitMargin);
	hitPath.quadraticTo(endAnchorX, endPos.y + hitMargin, midPoint.x, midPoint.y + hitMargin);
	hitPath.quadraticTo(sourceAnchorX, sourcePos.y + hitMargin, sourcePos.x, sourcePos.y + hitMargin);
	hitPath.closeSubPath();

	Path p;
	p.startNewSubPath(sourcePos.x, sourcePos.y);
	p.quadraticTo(sourceAnchorX, sourcePos.y, midPoint.x, midPoint.y);
	p.quadraticTo(endAnchorX, endPos.y, endPos.x, endPos.y);

	Colour baseColor = getBaseSlot()->connectionType == NodeConnection::ConnectionType::AUDIO ? AUDIO_COLOR : DATA_COLOR;
	g.setColour((candidateDropSlot != nullptr) ? Colours::yellow : isMouseOver() ? Colours::orange : baseColor);
	g.strokePath(p, PathStrokeType(2.0f));
	*/
}

void NodeConnectionEditorLink::resized()
{
	repaint();
}
