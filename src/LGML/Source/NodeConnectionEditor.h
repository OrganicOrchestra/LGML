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
class NodeConnectionEditor    : public DocumentWindow
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
    void setCurrentConnection(NodeConnection * _connection);
    void editConnection(NodeConnection * _connection);



    void resized();
    void closeButtonPressed() override;

    void clearContent();
    void generateContent();

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeConnectionEditor)
};


#endif  // NODECONNECTIONEDITOR_H_INCLUDED
