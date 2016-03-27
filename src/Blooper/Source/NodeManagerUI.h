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

class NodeConnectionUI;

//==============================================================================
/*
 Draw all connected Nodes and Connections
 */
class NodeManagerUI : public Component, public NodeManager::Listener,public SelectableComponent::Listener
{
public:
    NodeManagerUI(NodeManager * nodeManager);
    ~NodeManagerUI();

    NodeManager * nodeManager;



    OwnedArray<NodeBaseUI> nodesUI;
    OwnedArray<NodeConnectionUI>  connectionsUI;
    NodeConnectionUI * editingConnection;
    String editingDataName;
    String editingElementName;
    DataProcessor::DataType editingDataType;

    uint32 editingChannel;


    void clear();

    void paint (Graphics&) override;
    void resized() override;

    // Inherited via Listener
    virtual void nodeAdded(NodeBase *) override;
    virtual void nodeRemoved(NodeBase *) override;
    virtual void connectionAdded(NodeConnection *) override;
    virtual void connectionEdited(NodeConnection * ) override;
    virtual void connectionRemoved(NodeConnection *) override;


    void addNodeUI(NodeBase * node);
    void removeNodeUI(NodeBase * node);
    NodeBaseUI * getUIForNode(NodeBase * node);


    void addConnectionUI(NodeConnection * connection);
    void removeConnectionUI(NodeConnection * connection);
    NodeConnectionUI * getUIForConnection(NodeConnection * connection);

    //connection creation / editing
    typedef ConnectorComponent Connector;
    void createDataConnectionFromConnector(Connector * baseConnector, const String &dataName, const String &elementName, DataProcessor::DataType editingDataType = DataProcessor::DataType::Unknown);
    void createAudioConnectionFromConnector(Connector * baseConnector, uint32 channel);

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


    //
    class  SelectedNodesListener
    {
    public:
        /** Destructor. */
        virtual ~SelectedNodesListener() {}
        virtual void selectedNodeEvent(NodeBaseUI*  selectedNodes,bool isSelected) = 0;
    };

    void addSelectedNodesListener(SelectedNodesListener* newListener) { selectedNodeListeners.add(newListener); }
    void removeSelectedNodesListener(SelectedNodesListener* listener) { selectedNodeListeners.remove(listener); }
    ListenerList<SelectedNodesListener> selectedNodeListeners;



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
    Array<NodeBaseUI*> selectedNodes;

    //recieve info if node ask for selection or if its triggered by multiselection
    void selectableSelected(SelectableComponent * c,bool state)override {
        NodeBaseUI* n = dynamic_cast<NodeBaseUI*>(c);
        if(n){
            selectedNodeListeners.call(&SelectedNodesListener::selectedNodeEvent,n, state);
        }
        else{
            jassertfalse;
        }

    }




    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeManagerUI)

};

class NodeManagerUIViewport : public Viewport{
public:
    NodeManagerUIViewport(NodeManagerUI * _nmui):Viewport("NodeManagerViewPort"),nmui(_nmui){
        setScrollBarsShown(true,true);
        //    setSize(500,500);
        setViewedComponent(nmui,false);
    }
    void visibleAreaChanged (const Rectangle<int>& newVisibleArea)override{
        Point <int> mouse = getMouseXYRelative();
        autoScroll(mouse.x, mouse.y, 100, 10);

    }
    void resized() override{
        if(getLocalBounds().contains(nmui->getLocalBounds())){
            nmui->minBounds = getLocalBounds();
        }
    }

    NodeManagerUI * nmui;
};


#endif  // NODEMANAGERUI_H_INCLUDED
