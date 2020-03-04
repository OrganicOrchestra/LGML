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


#ifndef NODECONNECTIONUI_H_INCLUDED
#define NODECONNECTIONUI_H_INCLUDED


#include "../../Manager/UI/NodeManagerUI.h"

//==============================================================================
/*


 */

class NodeConnectionUI :
    public InspectableComponent,
    public juce::ComponentListener,
    public NodeConnection::Listener

{
public:
    typedef ConnectorComponent Connector;

    NodeConnectionUI (NodeConnection* connection = nullptr, Connector* sourceConnector = nullptr, Connector* destConnector = nullptr);
    ~NodeConnectionUI();

    WeakReference<NodeConnection> connection;

    class Anchor : public DrawableRectangle
    {
    public:
        Anchor():isHovered(false)
        {
            setInterceptsMouseClicks(true, false);
            const int anchorSize = 6;
            setRectangle (Rectangle<float> (0, 0, anchorSize, anchorSize));
            setFill (Colours::white.withAlpha (0.1f));
            setStrokeFill (Colours::white);
            setStrokeThickness (1);

        }
        void mouseEnter(const MouseEvent & )final{
            isHovered = true;
            setFill (Colours::white.withAlpha (0.5f));
        }
        void mouseExit(const MouseEvent & )final{
            isHovered = false;
            setFill (Colours::white.withAlpha (0.1f));
        }
        void mouseDown(const MouseEvent & )final{
//            if(auto ncv = findParentComponentOfClass<NodeContainerViewer>()){
//
//            }
        }

        
        void mouseUp(const MouseEvent & )final{
            if(auto ncv = findParentComponentOfClass<NodeContainerViewer>()){
                ncv->finishEditingConnection();
            }
        }
        
        bool isHovered;
    };
    Anchor anchorSource;
    Anchor anchorDest;


    WeakReference<Connector> sourceConnector;
    WeakReference<Connector> destConnector;
    Connector* candidateDropConnector;

    void setSourceConnector (Connector* c);
    void setDestConnector (Connector* c);
    DynamicObject * createObject() override;

    Path path;
    Path hitPath;

    void paint (Graphics&)override;
    void resized()override;

    void buildPath();
    void buildHitPath (Array<Point<float>> points);
    void setSelectedInternal( bool v) final;
    void updateBoundsFromNodes();
    bool hitTest (int x, int y) final;

    //interaction
    void mouseDown (const MouseEvent& e) override;
    void mouseUp (const MouseEvent& e) final;
    void mouseDrag (const MouseEvent& e) override;
    void mouseEnter (const MouseEvent& e) override;
    void mouseExit (const MouseEvent& e) override;
    void mouseMove (const MouseEvent&)override;
    bool keyPressed (const KeyPress& key) override;

    void componentParentHierarchyChanged (Component&) override { removeComponentListener (this); updateBoundsFromNodes(); }

    Connector* getBaseConnector()
    {
        return sourceConnector != nullptr ? sourceConnector : destConnector;
    }

    Connector* getSecondConnector()
    {
        return sourceConnector != nullptr ? destConnector : sourceConnector;
    }

    //From Component Listener
    void componentMovedOrResized (Component& component, bool wasMoved, bool wasResize)override;


    bool isEditing()
    {
        return ((sourceConnector == nullptr && destConnector != nullptr) || (sourceConnector != nullptr && destConnector == nullptr));
    }


    bool setCandidateDropConnector (Connector* connector);
    void cancelCandidateDropConnector();
    void startEditing(bool editDest);
    bool finishEditing();


    // Inherited via Listener
    
    virtual void connectionAudioLinkAdded (const std::pair<int, int>& audioConnection) override;
    virtual void connectionAudioLinkRemoved (const std::pair<int, int>& audioConnection) override;


    Component* getNodeManagerUI() { return (Component*)findParentComponentOfClass<NodeManagerUI>(); }
    String getTooltip() override;
    std::unique_ptr<InspectorEditor> createEditor() override;
    void handleCommandMessage (int cId)override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeConnectionUI)

    bool _isMouseOver=false;

};


#endif  // NODECONNECTIONUI_H_INCLUDED
