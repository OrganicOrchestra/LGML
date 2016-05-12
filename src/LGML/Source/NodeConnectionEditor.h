/*
  ==============================================================================

    NodeConnectionEditor.h
    Created: 29 Mar 2016 6:43:49pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODECONNECTIONEDITOR_H_INCLUDED
#define NODECONNECTIONEDITOR_H_INCLUDED



#include "NodeConnectionEditorDataSlot.h"
#include "NodeConnectionEditorLink.h"
#include "CustomEditor.h"
#include "NodeConnectionUI.h"

//==============================================================================
/*
*/
class NodeConnectionEditor : public CustomEditor,
    NodeConnectionEditorDataSlot::SlotListener, NodeConnectionEditorLink::LinkListener, NodeConnection::Listener
{

public:
    NodeConnectionEditor(NodeConnectionUI * nodeConnectionUI);
    ~NodeConnectionEditor();

    OwnedArray<NodeConnectionEditorDataSlot> outputSlots;
    OwnedArray<NodeConnectionEditorDataSlot> inputSlots;
    OwnedArray<NodeConnectionEditorLink> links;

    Component mainContainer;

    Component inputsContainer;
    Component outputsContainer;
    Component linksContainer;

    NodeConnection * currentConnection;
    NodeConnection::ConnectionType editingType;

    void setCurrentConnection(NodeConnection * _connection);

    void resized()override;

	int getContentHeight() override;

    ScopedPointer<NodeConnectionEditorLink> editingLink;


    void mouseEnter(const MouseEvent &) override;
    void mouseExit(const MouseEvent &) override;

    void createEditingLink(NodeConnectionEditorDataSlot * baseSlot);

    void updateEditingLink();
    bool checkDropCandidates();
    void finishEditingLink();

    bool setCandidateDropSlot(NodeConnectionEditorDataSlot * slot);
    void cancelCandidateDropSlot();

    void clearContent();
    void generateContentForAudio();
    void generateContentForData();

    void addAudioLink(int sourceChannel, int destChannel);
    void removeAudioLinkForChannels(int sourceChannel, int destChannel);
    void addDataLink(Data * sourceData, Data * destData);
    void removeDataLinkForDatas(Data * sourceData, Data * destData);


    NodeConnectionEditorLink * getLinkForSlots(NodeConnectionEditorDataSlot * outSlot, NodeConnectionEditorDataSlot * inSlot);
    NodeConnectionEditorLink * getLinkForChannels(int sourceChannel, int destChannel);
    NodeConnectionEditorLink * getLinkForDatas(Data * sourceData, Data * destData);

    NodeConnectionEditorDataSlot * getOutputSlotForData(Data * data);
    NodeConnectionEditorDataSlot * getInputSlotForData(Data * data);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeConnectionEditor)

    // Inherited via SlotListener
    virtual void slotMouseEnter(NodeConnectionEditorDataSlot * target) override;
    virtual void slotMouseExit(NodeConnectionEditorDataSlot * target) override;
    virtual void slotMouseDown(NodeConnectionEditorDataSlot * target) override;
    virtual void slotMouseMove(NodeConnectionEditorDataSlot * target) override;
    virtual void slotMouseDrag(NodeConnectionEditorDataSlot * target) override;
    virtual void slotMouseUp(NodeConnectionEditorDataSlot * target) override;

    // Inherited via LinkListener
    virtual void askForRemoveLink(NodeConnectionEditorLink * target) override;

    // Inherited via Listener
    virtual void askForRemoveConnection(NodeConnection *) override {};
    virtual void connectionDataLinkAdded(DataProcessorGraph::Connection * dataConnection) override;
    virtual void connectionDataLinkRemoved(DataProcessorGraph::Connection * dataConnection) override;
    virtual void connectionAudioLinkAdded(const NodeConnection::AudioConnection & audioConnection) override;
    virtual void connectionAudioLinkRemoved(const NodeConnection::AudioConnection & audioConnection) override;
};


#endif  // NODECONNECTIONEDITOR_H_INCLUDED
