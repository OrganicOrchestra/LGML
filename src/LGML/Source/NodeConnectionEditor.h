/*
  ==============================================================================

    NodeConnectionEditor.h
    Created: 29 Mar 2016 6:43:49pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef NODECONNECTIONEDITOR_H_INCLUDED
#define NODECONNECTIONEDITOR_H_INCLUDED

#include "JuceHeader.h"

#include "NodeConnection.h"
#include "NodeConnectionEditorDataSlot.h"
#include "NodeConnectionEditorLink.h"

//==============================================================================
/*
*/
class NodeConnectionEditor    : public DocumentWindow, NodeConnectionEditorDataSlot::SlotListener
{

public:
    NodeConnectionEditor();
    ~NodeConnectionEditor();

    juce_DeclareSingleton(NodeConnectionEditor, true);

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
    void editConnection(NodeConnection * _connection);

	

    void resized();
    void closeButtonPressed() override;

	NodeConnectionEditorLink * editingLink;
	

	void mouseEnter(const MouseEvent &) override;
	void mouseExit(const MouseEvent &) override;
	void mouseMove(const MouseEvent &) override
	{
		DBG("Editor mouseMove");
	}

	void createEditingLink(NodeConnectionEditorDataSlot * baseSlot);

	void updateEditingLink();
	bool checkDropCandidates();
	void finishEditingLink();
	
	bool setCandidateDropSlot(NodeConnectionEditorDataSlot * slot);
	void cancelCandidateDropSlot();

    void clearContent();
    void generateContentForAudio();
	void generateContentForData();

	NodeConnectionEditorDataSlot * getOutputSlotForData(DataProcessor::Data * data);
	NodeConnectionEditorDataSlot * getInputSlotForData(DataProcessor::Data * data);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeConnectionEditor)

	// Inherited via SlotListener
	virtual void slotMouseEnter(NodeConnectionEditorDataSlot * target) override;
	virtual void slotMouseExit(NodeConnectionEditorDataSlot * target) override;
	virtual void slotMouseDown(NodeConnectionEditorDataSlot * target) override;
	virtual void slotMouseMove(NodeConnectionEditorDataSlot * target) override;
	virtual void slotMouseDrag(NodeConnectionEditorDataSlot * target) override;
	virtual void slotMouseUp(NodeConnectionEditorDataSlot * target) override;
};


#endif  // NODECONNECTIONEDITOR_H_INCLUDED
