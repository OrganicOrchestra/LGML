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

//#include "../../../UI/Style.h"
//#include "NodeConnectionEditorLink.h"

//==============================================================================
NodeConnectionEditorLink::NodeConnectionEditorLink (NodeConnectionEditorDataSlot* _outSlot, NodeConnectionEditorDataSlot* _inSlot) :
    outSlot (_outSlot), inSlot (_inSlot), candidateDropSlot (nullptr), isSelected (false)
{
    isEditing = (outSlot != nullptr && inSlot == nullptr) || (outSlot == nullptr && inSlot != nullptr);
    setTooltip (juce::translate("Double click to delete"));
    setWantsKeyboardFocus (true);
}

NodeConnectionEditorLink::~NodeConnectionEditorLink()
{

}

bool NodeConnectionEditorLink::setCandidateDropSlot (NodeConnectionEditorDataSlot* slot)
{
    //check if connector can accept data
    NodeConnectionEditorDataSlot* baseSlot = getBaseSlot();

    if (baseSlot == slot) return false;

    if (baseSlot->ioType == slot->ioType) return false;

    if (baseSlot->connectionType != slot->connectionType) return false;


    if (baseSlot->isConnectedTo (slot)) return false;

    candidateDropSlot = slot;

    return true;
}

void NodeConnectionEditorLink::cancelCandidateDropSlot()
{
    candidateDropSlot = nullptr;
}

bool NodeConnectionEditorLink::finishEditing()
{
    bool result = false;

    bool hasCandidate = candidateDropSlot != nullptr;

    if (hasCandidate)
    {
        if (outSlot == nullptr)
        {
            outSlot = candidateDropSlot;
        }
        else
        {
            inSlot = candidateDropSlot;
        }

        result = true;
    }

    candidateDropSlot = nullptr;
    setEditing (false);

    return result;
}


void NodeConnectionEditorLink::paint (Graphics& g)
{
    bool baseIsOutput = getBaseSlot()->ioType == NodeConnectionEditorDataSlot::IOType::OUTPUT;

    Point<float> sourcePos;
    Point<float> endPos;

    if (isEditing)
    {
        sourcePos = getLocalPoint (getBaseSlot(), getBaseSlot()->getLocalBounds().getRelativePoint (baseIsOutput ? 1.f : 0.f, .5f)).toFloat();
        endPos = (candidateDropSlot != nullptr) ?
                 getLocalPoint (candidateDropSlot, candidateDropSlot->getLocalBounds().getRelativePoint (baseIsOutput ? 0.f : 1.f, .5f)).toFloat() :
                 getMouseXYRelative().toFloat();
    }
    else
    {
        sourcePos = getLocalPoint (outSlot, outSlot->getLocalBounds().getRelativePoint (1.f, .5f)).toFloat();
        endPos = getLocalPoint (inSlot, inSlot->getLocalBounds().getRelativePoint (0.f, .5f)).toFloat();
    }

    Point<float> midPoint = (sourcePos + endPos) / 2;


    int hitMargin = 10;
    hitPath.clear();
    hitPath.startNewSubPath (sourcePos.x, sourcePos.y - hitMargin);
    hitPath.cubicTo (midPoint.x, sourcePos.y - hitMargin, midPoint.x, endPos.y - hitMargin, endPos.x, endPos.y - hitMargin);
    hitPath.lineTo (endPos.x, endPos.y + hitMargin);
    hitPath.cubicTo (endPos.x, endPos.y + hitMargin, sourcePos.x, midPoint.y + hitMargin, sourcePos.x, sourcePos.y);
    hitPath.closeSubPath();

    Path p;
    p.startNewSubPath (sourcePos.x, sourcePos.y);
    p.cubicTo (midPoint.x, sourcePos.y, midPoint.x, endPos.y, endPos.x, endPos.y);

    Colour baseColor = findColour ((getBaseSlot()->isAudio()) ? LGMLColors::audioColor : LGMLColors::dataColor);
    g.setColour ((candidateDropSlot != nullptr) ? Colours::yellow : isSelected ? findColour (TextButton::buttonOnColourId) : (isEditing || isMouseOver()) ? Colours::red : baseColor);

    g.strokePath (p, PathStrokeType (2.0f));
}

void NodeConnectionEditorLink::resized()
{
    repaint();
}

void NodeConnectionEditorLink::mouseEnter (const MouseEvent&) { repaint(); }

void NodeConnectionEditorLink::mouseExit (const MouseEvent&) { repaint(); }

void NodeConnectionEditorLink::mouseDown (const MouseEvent&)
{
    listeners.call (&LinkListener::selectLink, this);
}

void NodeConnectionEditorLink::mouseDoubleClick (const MouseEvent&) { remove(); }

bool NodeConnectionEditorLink::keyPressed (const KeyPress& key)
{
    if (!isSelected) return false;

    if (key.getKeyCode() == KeyPress::deleteKey || key.getKeyCode() == KeyPress::backspaceKey)
    {
        remove();
        return true;
    }

    return false;
}

void NodeConnectionEditorLink::remove() { listeners.call (&LinkListener::askForRemoveLink, this); }

#endif
