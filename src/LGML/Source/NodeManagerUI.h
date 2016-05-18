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
//#include "SelectableComponentHandler.h"
#include "ShapeShifterContent.h"
#include "InspectableComponent.h"

class NodeConnectionUI;

//==============================================================================
/*
 Draw all connected Nodes and Connections
 */
class NodeManagerUI : public Component, public NodeManager::Listener, public InspectableComponent::InspectableListener
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
    void paint(Graphics&) override;

    // Inherited via Listener
    virtual void nodeAdded(NodeBase *) override;
    virtual void nodeRemoved(NodeBase *) override;
    virtual void connectionAdded(NodeConnection *) override;
    virtual void connectionRemoved(NodeConnection *) override;

    //NodeUI Listener
    //virtual void componentSelected(SelectableComponent *) override;
    //virtual void componentDeselected(SelectableComponent *) override;
    virtual void inspectableSelectionChanged(InspectableComponent *) override;

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
    static void createNodeFromIndexAtPos(int modalResult,Component * thisComponent,int  maxResult);
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

class NodeManagerUIViewport: public ShapeShifterContent{
    public :
    NodeManagerUIViewport(const String &contentName, NodeManagerUI * _nmui):nmui(_nmui),ShapeShifterContent(contentName)
	{
        vp.setViewedComponent(nmui,true);
        vp.setScrollBarsShown(true, true);
        vp.setScrollOnDragEnabled(false);
        contentIsFlexible = true;
        addAndMakeVisible(vp);
		vp.setScrollBarThickness(10);

    }
    void resized() override{
        vp.setSize(getWidth(), getHeight());
        nmui->setSize(jmax(getWidth(),nmui->getWidth()),
                      jmax(getHeight(),nmui->getHeight()));
        nmui->minBounds.setSize(getWidth(), getHeight());

    }
    Viewport vp;
    NodeManagerUI * nmui;
};

#endif  // NODEMANAGERUI_H_INCLUDED
