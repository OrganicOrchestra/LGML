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


#ifndef NODECONNECTIONEDITOR_H_INCLUDED
#define NODECONNECTIONEDITOR_H_INCLUDED



#include "NodeConnectionEditorDataSlot.hpp"
#include "NodeConnectionEditorLink.hpp"
#include "../../../UI/Inspector/InspectorEditor.h"
class NodeConnectionUI;
//==============================================================================
/*
*/
class NodeConnectionEditor : public InspectorEditor,
    NodeConnectionEditorDataSlot::SlotListener, NodeConnectionEditorLink::LinkListener, NodeConnection::Listener
{

public:
    NodeConnectionEditor (NodeConnectionUI* nodeConnectionUI);
    ~NodeConnectionEditor();

    OwnedArray<NodeConnectionEditorDataSlot> outputSlots;
    OwnedArray<NodeConnectionEditorDataSlot> inputSlots;
    OwnedArray<NodeConnectionEditorLink> links;


    Component mainContainer;

    Component inputsContainer;
    Component outputsContainer;
    Component linksContainer;

    NodeConnectionEditorLink* selectedLink;
    void setSelectedLink (NodeConnectionEditorLink*);

    NodeConnection* currentConnection;
    NodeConnection::ConnectionType editingType;

    void setCurrentConnection (NodeConnection* _connection);

    void resized()override;

    int getContentHeight() const override;

    std::unique_ptr<NodeConnectionEditorLink> editingLink;


    void mouseEnter (const MouseEvent&) override;
    void mouseExit (const MouseEvent&) override;

    void createEditingLink (NodeConnectionEditorDataSlot* baseSlot);

    void updateEditingLink();
    bool checkDropCandidates();
    void finishEditingLink();

    bool setCandidateDropSlot (NodeConnectionEditorDataSlot* slot);
    void cancelCandidateDropSlot();

    void clearContent();
    void generateContentForAudio();


    void addAudioLink (int sourceChannel, int destChannel);
    void removeAudioLinkForChannels (int sourceChannel, int destChannel);
    


    NodeConnectionEditorLink* getLinkForSlots (NodeConnectionEditorDataSlot* outSlot, NodeConnectionEditorDataSlot* inSlot);
    NodeConnectionEditorLink* getLinkForChannels (int sourceChannel, int destChannel);
    

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeConnectionEditor)

    // Inherited via SlotListener
    virtual void slotMouseEnter (NodeConnectionEditorDataSlot* target) override;
    virtual void slotMouseExit (NodeConnectionEditorDataSlot* target) override;
    virtual void slotMouseDown (NodeConnectionEditorDataSlot* target) override;
    virtual void slotMouseMove (NodeConnectionEditorDataSlot* target) override;
    virtual void slotMouseDrag (NodeConnectionEditorDataSlot* target) override;
    virtual void slotMouseUp (NodeConnectionEditorDataSlot* target) override;

    // Inherited via LinkListener
    virtual void askForRemoveLink (NodeConnectionEditorLink* target) override;
    virtual void selectLink (NodeConnectionEditorLink* target) override;

    // Inherited via Listener
    virtual void connectionRemoved (NodeConnection*) override {setCurrentConnection ( nullptr);};
    virtual void connectionAudioLinkAdded (const NodeConnection::AudioConnection& audioConnection) override;
    virtual void connectionAudioLinkRemoved (const NodeConnection::AudioConnection& audioConnection) override;
};

#include "NodeConnectionEditor.ipp"

#endif  // NODECONNECTIONEDITOR_H_INCLUDED
