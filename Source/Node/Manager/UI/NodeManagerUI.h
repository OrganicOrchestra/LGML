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
    public ParameterContainer
{
public:
    juce_DeclareSingleton(NodeManagerUI, false);
    NodeManagerUI (NodeManager* nodeManager=NodeManager::getInstance());
    ~NodeManagerUI();

    NodeManager* nodeManager;
    ScopedPointer<NodeContainerViewer> currentViewer;

    void clear();

    void resized() override;

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

//    DynamicObject* getObject();
//    void setFromObject(const DynamicObject * obj);



    ScopedPointer<UISync> uiSync;
    bool isMiniMode;
    ParameterContainer * addContainerFromObject(const String &,DynamicObject * d) override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeManagerUI)

};

class NodeManagerUIViewport :
    public ShapeShifterContentComponent,
    public NodeManagerUI::NodeManagerUIListener,
    public ButtonListener
{
public :
    NodeManagerUIViewport (const String& contentName, NodeManagerUI* _nmui): nmui (_nmui), ShapeShifterContentComponent (contentName,"Patch your Audio here")
    {
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
        addNodeBt.setTooltip ("Add Node");
        //    setOpaque(true);
        vp.setScrollOnDragEnabled (false);

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

            if (c == nmui->currentViewer->nodeContainer) b->setEnabled (false);

            pathButtons.insert (0, b);
            addAndMakeVisible (b);
            b->addListener (this);

            c = c->getParentNodeContainer();

        }
        }

        resized();
    }

    void paint (Graphics& g) override
    {
//        g.setColour (findColour (ResizableWindow::backgroundColourId).darker (.2f));
//        g.fillRect (getLocalBounds().removeFromTop (30));
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
            static Array<String> filt  {"t_ContainerInNode", "t_ContainerOutNode"};
            ScopedPointer<PopupMenu> menu (FactoryUIHelpers::createFactoryTypesMenuFilter<NodeFactory> (filt));

            int result = menu->show();
            Point<int> mousePos = vp.getViewArea().getCentre();

            if (result > 0)
            {
                if (auto c =  FactoryUIHelpers::createFromMenuIdx<NodeBase> (result))
                {
                    ConnectableNode* n = nmui->currentViewer->nodeContainer->addNode (c);
                    jassert (n != nullptr);
                    if(auto ui= nmui->getControllableForAddress(c->getControlAddressArray())){
                        if(auto d = dynamic_cast<ConnectableNodeUIParams*>(ui)){
                            d->nodePosition->setPoint (mousePos);
                            d->nodeMinimizedPosition->setPoint (mousePos);
                        }
                    }
                }
                else
                {
                    jassertfalse;
                }
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
