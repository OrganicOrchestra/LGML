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


#ifndef NODEMANAGERUI_H_INCLUDED
#define NODEMANAGERUI_H_INCLUDED
#pragma once

#include "../NodeManager.h"
#include "../../../UI/ShapeShifter/ShapeShifterContent.h"
#include "../../NodeContainer/UI/NodeContainerViewer.h"
#include "../../../UI/Style.h"
#include "../NodeFactory.h"
#include "../../../Utils/FactoryUIHelpers.h"
#include "../../../Controllable/Parameter/UI/ParameterUIFactory.h"

class UISync;

//==============================================================================
/*
 Draw all connected Nodes and Connections
 */
class NodeManagerUI :
    public juce::Component,
    public NodeManager::NodeManagerListener,
    public ParameterContainer,
    private DeletedAtShutdown 
{
public:
    juce_DeclareSingleton(NodeManagerUI, false);
    NodeManagerUI (NodeManager* nodeManager=NodeManager::getInstance());
    ~NodeManagerUI();

    NodeManager* nodeManager;
    ScopedPointer<NodeContainerViewer> currentViewer;

    void clear();

    void resized() override;
    void paint(Graphics & g)override;
    int getContentWidth() const;
    int getContentHeight() const;

    void managerCleared() override;
    void managerEndedLoading() override;

    void setCurrentViewedContainer (NodeContainer* c);

    void childBoundsChanged (Component* c)override;

    class  NodeManagerUIListener
    {
    public:
        /** Destructor. */
        virtual ~NodeManagerUIListener() {}
        virtual void currentViewedContainerChanged() {};
    };

    ListenerList<NodeManagerUIListener> nodeManagerUIListeners;
    void addNodeManagerUIListener (NodeManagerUIListener* newListener) { nodeManagerUIListeners.add (newListener); }
    void removeNodeManagerUIListener (NodeManagerUIListener* listener) { nodeManagerUIListeners.remove (listener); }
    bool keyPressed (const KeyPress& key)override;


    void alignOnGrid(Point<int> &toAlign);
    IntParameter * gridSize;
    
    ScopedPointer<UISync> uiSync;
    bool isMiniMode;
    ParameterContainer * addContainerFromObject(const String &,DynamicObject * d) override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeManagerUI)

};

class NodeManagerUIViewport :
    public ShapeShifterContentComponent,
    public NodeManagerUI::NodeManagerUIListener,
    public Button::Listener
{
public :
    NodeManagerUIViewport (const String& contentName, NodeManagerUI* _nmui): nmui (_nmui), ShapeShifterContentComponent (contentName,"Patch your Audio here")
    {
        gridSizeUI =ParameterUIFactory::createDefaultUI(nmui->gridSize);
        addAndMakeVisible(gridSizeUI);
        vp.setViewedComponent (nmui, false);
        vp.setScrollBarsShown (true, true);
        vp.setScrollOnDragEnabled (false);
//        vp.addMouseListener (this, true);
        contentIsFlexible = true;
        addAndMakeVisible (vp);
        vp.setScrollBarThickness (10);
        nmui->addNodeManagerUIListener (this);
        reconstructViewerPath();
        setWantsKeyboardFocus (true);
        nmui->setWantsKeyboardFocus (true);
        addAndMakeVisible (addNodeBt);
        addNodeBt.addListener (this);
        addNodeBt.setTooltip (juce::translate("Add Node"));
            setOpaque(true);

        vp.addMouseListener(this,true);
        setPaintingIsUnclipped(true);
        vp.setPaintingIsUnclipped(true);

    }

    virtual ~NodeManagerUIViewport()
    {
        nmui->removeNodeManagerUIListener (this);

    }
//    void mouseMove (const MouseEvent& me) override
//    {
//
//    }

    ScopedPointer<ParameterUI> minimizeAllUI;
    ScopedPointer<ParameterUI> gridSizeUI;
    OwnedArray<TextButton> pathButtons;

    void reconstructViewerPath()
    {

        for (auto& b : pathButtons)
        {
            removeChildComponent (b);
            b->removeListener (this);
        }

        pathButtons.clear();
        if( nmui->currentViewer){
        NodeContainer* c = nmui->currentViewer->nodeContainer;
        minimizeAllUI = ParameterUIFactory::createDefaultUI(nmui->currentViewer->minimizeAll);

        addAndMakeVisible(minimizeAllUI);

            
        while (c != nullptr)
        {
            TextButton* b = new TextButton (c->getNiceName());
            LGMLUIUtils::optionallySetBufferedToImage(b);
            if (c == nmui->currentViewer->nodeContainer) b->setEnabled (false);

            pathButtons.insert (0, b);
            addAndMakeVisible (b);
            b->addListener (this);

            c = c->getParentNodeContainer();

        }
        }

        resized();
    }
    void mouseDown(const MouseEvent & e)override{
//        if(nmui->isParentOf(e.originalComponent)) {
//            beginDragAutoRepeat(40);
//        }
    }
    void mouseDrag(const MouseEvent & e)override{
        Component *c  = e.originalComponent;
        if(e.mouseWasDraggedSinceMouseDown() && nmui->currentViewer && nmui->currentViewer->isParentOf(c)){
            Rectangle<int> r = vp.getLocalArea(c->getParentComponent(),c->getBoundsInParent());
            Point<int> mouse = vp.getLocalPoint(e.originalComponent,e.position).toInt();
//            DBG(String(r.getRight()) +"::::" +String(mouse.x));
            vp.autoScroll(jmax(r.getRight(),mouse.x),jmax(r.getBottom(),mouse.y),140,10);
        }
    }
    void mouseUp(const MouseEvent & e)override{
        if(nmui->currentViewer) {
            auto nb = nmui->currentViewer->getNodesBoundingBox();
            auto maxP = vp.getLocalArea(nmui->currentViewer,nb);
            nmui->currentViewer->resizeToFitNodes(maxP.getTopLeft());
        }
//        beginDragAutoRepeat(-1);

    }


    void paint (Graphics& g) override
    {
        LGMLUIUtils::fillBackground(this, g);
        const int grid = 100;
        auto area = vp.getViewArea();
        g.setColour (Colours::white.withAlpha (0.03f));
        auto dest = vp.getBoundsInParent();

        for (int x = -area.getX() % grid ; x < area.getWidth(); x += grid)
        {

            g.drawVerticalLine (x + dest.getX(), dest.getY(), dest.getBottom());
        }

        for (int y = -area.getY() % grid ; y < area.getHeight(); y += grid)
        {
            g.drawHorizontalLine ( y + dest.getY(), dest.getX(), dest.getRight());
        }

    }

    void resized() override
    {
        ShapeShifterContentComponent::resized();
        Rectangle<int> r = getLocalBounds();

        Rectangle<int> buttonR = r.removeFromTop (30).reduced (5);
        
        minimizeAllUI->setBounds(buttonR.removeFromRight(100));
        gridSizeUI->setBounds(buttonR.removeFromRight(100));
        for (auto& b : pathButtons)
        {
            b->setBounds (buttonR.removeFromLeft (100));
            buttonR.removeFromLeft (5);
        }

        r.removeFromTop (2);

        vp.setBounds (r);

        nmui->setTopLeftPosition (r.getTopLeft());
        nmui->setSize (jmax<int> (r.getWidth(), nmui->getContentWidth()), jmax<int> (r.getHeight(), nmui->getContentHeight()));
        addNodeBt.setFromParentBounds (getLocalBounds());

    }

    void currentViewedContainerChanged()override
    {
        reconstructViewerPath();
        
        hideInfoLabelIfNeeded();

        //nmui->setBounds(getLocalBounds().withTop(30));
        resized();


    }
    void hideInfoLabelIfNeeded(){
        if( nmui->currentViewer){
            NodeContainer* c = nmui->currentViewer->nodeContainer;
            if(c){infoLabel.setVisible( c->getNumNodes()<=2);}
            else{jassertfalse;}
        }
    }

    void buttonClicked (Button* b)override
    {
        if (b == &addNodeBt)
        {
            
            ScopedPointer<PopupMenu> menu (FactoryUIHelpers::getFactoryTypesMenu<NodeFactory> ());

            int result = menu->show();
            Point<int> destPos = vp.getViewArea().getCentre();

            if (result > 0)
            {
                String tid(FactoryUIHelpers::getFactoryTypeNameFromMenuIdx<NodeFactory>(result));
                nmui->currentViewer->createNodeUndoable(tid, destPos);
            }

        }
        else
        {
            int bIndex = pathButtons.indexOf ((TextButton*)b);

            if (bIndex == -1)
            {
                DBG ("WTF ?");
            }

            NodeContainer* c = nmui->currentViewer->nodeContainer;

            for (int i = pathButtons.size() - 1; i > bIndex; i--)
            {
                c = c->getParentNodeContainer();
            }

            nmui->setCurrentViewedContainer (c);
        }

    }

    Viewport vp;
    AddElementButton addNodeBt;
    NodeManagerUI* nmui;
};

#endif  // NODEMANAGERUI_H_INCLUDED
