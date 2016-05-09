/*
 ==============================================================================

 NodeManagerUI.h
 Created: 3 Mar 2016 10:38:22pm
 Author:  bkupe

 ==============================================================================
 */

#ifndef NODEMANAGERUI_H_INCLUDED
#define NODEMANAGERUI_H_INCLUDED


#include "NodeManager.h"
#include "NodeBaseUI.h"
#include "SelectableComponentHandler.h"
#include "ShapeShifterContent.h"

class NodeConnectionUI;

//==============================================================================
/*
 Draw all connected Nodes and Connections
 */
class NodeManagerUI : public ShapeShifterContent, public NodeManager::Listener, public SelectableComponent::SelectableListener
{
public:
    NodeManagerUI(NodeManager * nodeManager);
    ~NodeManagerUI();

    NodeManager * nodeManager;

    OwnedArray<NodeBaseUI> nodesUI;
    OwnedArray<NodeConnectionUI>  connectionsUI;
    NodeConnectionUI * editingConnection;

    void clear();

    void resized() override;

    // Inherited via Listener
    virtual void nodeAdded(NodeBase *) override;
    virtual void nodeRemoved(NodeBase *) override;
    virtual void connectionAdded(NodeConnection *) override;
    virtual void connectionRemoved(NodeConnection *) override;

	//NodeUI Listener
	virtual void componentSelected(SelectableComponent *) override;
	virtual void componentDeselected(SelectableComponent *) override;

    void addNodeUI(NodeBase * node);
    void removeNodeUI(NodeBase * node);
    NodeBaseUI * getUIForNode(NodeBase * node);


    void addConnectionUI(NodeConnection * connection);
    void removeConnectionUI(NodeConnection * connection);
    NodeConnectionUI * getUIForConnection(NodeConnection * connection);

    //connection creation / editing
    typedef ConnectorComponent Connector;
    void createDataConnectionFromConnector(Connector * baseConnector);
    void createAudioConnectionFromConnector(Connector * baseConnector);

    void updateEditingConnection();
    bool isEditingConnection() { return editingConnection != nullptr; }
    bool checkDropCandidates();
    bool setCandidateDropConnector(Connector * c);
    void cancelCandidateDropConnector();
    void finishEditingConnection();



    //Mouse event
    void mouseDown(const MouseEvent& event) override;
    void mouseMove(const MouseEvent& event) override;
    void mouseDrag(const MouseEvent& event) override;
    void mouseUp(const MouseEvent& event) override;

    void childBoundsChanged(Component * )override;

    void setAllNodesToStartAtZero();
    void resizeToFitNodes();
    static void createNodeFromIndexAtPos(int modalResult,Viewport * c,int  maxResult);
    Rectangle<int> minBounds;

private:
    bool isSelectingNodes;
    class SelectingRect :public Component{
    public:
        void paint(Graphics & g) override{
            g.setColour(Colours::whitesmoke.withAlpha(.1f));
            g.fillRect(getLocalBounds());
        }
    };
    SelectingRect selectingBounds;
    void checkSelected();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeManagerUI)

};

#endif  // NODEMANAGERUI_H_INCLUDED
