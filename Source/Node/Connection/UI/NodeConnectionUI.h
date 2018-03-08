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
    public NodeConnection::Listener,
    public TooltipClient

{
public:
    typedef ConnectorComponent Connector;

    NodeConnectionUI (NodeConnection* connection = nullptr, Connector* sourceConnector = nullptr, Connector* destConnector = nullptr);
    ~NodeConnectionUI();

    NodeConnection* connection;

    class Anchor : public DrawableRectangle
    {
    public:
        Anchor()
        {
            const int anchorSize = 6;
            setRectangle (Rectangle<float> (0, 0, anchorSize, anchorSize));
            setFill (Colours::white.withAlpha (0.1f));
            setStrokeFill (Colours::white);
            setStrokeThickness (1);

        }
    };
    Anchor anchorSource;
    Anchor anchorDest;


    Connector* sourceConnector;
    Connector* destConnector;
    Connector* candidateDropConnector;

    void setSourceConnector (Connector* c);
    void setDestConnector (Connector* c);


    Path path;
    Path hitPath;

    void paint (Graphics&)override;
    void resized()override;

    void buildPath();
    void buildHitPath (Array<Point<float>> points);

    void updateBoundsFromNodes();
    virtual bool hitTest (int x, int y) override { return hitPath.contains ((float)x, (float)y); }

    //interaction
    void mouseDown (const MouseEvent& e) override;
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

    bool finishEditing();


    // Inherited via Listener
    virtual void connectionDataLinkAdded (DataProcessorGraph::Connection* dataConnection) override;
    virtual void connectionDataLinkRemoved (DataProcessorGraph::Connection* dataConnection) override;
    virtual void connectionAudioLinkAdded (const std::pair<int, int>& audioConnection) override;
    virtual void connectionAudioLinkRemoved (const std::pair<int, int>& audioConnection) override;


    Component* getNodeManagerUI() { return (Component*)findParentComponentOfClass<NodeManagerUI>(); }
    String getTooltip() override;
    InspectorEditor* createEditor() override;
    void handleCommandMessage (int cId)override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeConnectionUI);



};


#endif  // NODECONNECTIONUI_H_INCLUDED
