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

#include "NodeManager.h"
#include "ShapeShifterContent.h"
#include "NodeContainerViewer.h"
#include "Style.h"


//==============================================================================
/*
 Draw all connected Nodes and Connections
 */
class NodeManagerUI :
public juce::Component,
public NodeManager::NodeManagerListener
{
public:

  NodeManagerUI(NodeManager * nodeManager);
  ~NodeManagerUI();

  NodeManager * nodeManager;
  ScopedPointer<NodeContainerViewer> currentViewer;

  void clear();

  void resized() override;

  int getContentWidth();
  int getContentHeight();

  void managerCleared() override;

  void setCurrentViewedContainer(NodeContainer * c);

  void childBoundsChanged(Component * c)override;

  class  NodeManagerUIListener
  {
  public:
    /** Destructor. */
    virtual ~NodeManagerUIListener() {}
    virtual void currentViewedContainerChanged() {};
  };

  ListenerList<NodeManagerUIListener> nodeManagerUIListeners;
  void addNodeManagerUIListener(NodeManagerUIListener* newListener) { nodeManagerUIListeners.add(newListener); }
  void removeNodeManagerUIListener(NodeManagerUIListener* listener) { nodeManagerUIListeners.remove(listener); }
  bool keyPressed(const KeyPress & key)override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NodeManagerUI)

};

class NodeManagerUIViewport :
public ShapeShifterContentComponent,
public NodeManagerUI::NodeManagerUIListener,
public ButtonListener
{
  public :
  NodeManagerUIViewport(const String &contentName, NodeManagerUI * _nmui):nmui(_nmui),ShapeShifterContentComponent(contentName)
  {
    vp.setViewedComponent(nmui,true);
    vp.setScrollBarsShown(true, true);
    vp.setScrollOnDragEnabled(false);
    vp.addMouseListener(this, true);
    contentIsFlexible = true;
    addAndMakeVisible(vp);
    vp.setScrollBarThickness(10);
    nmui->addNodeManagerUIListener(this);
    reconstructViewerPath();
    setWantsKeyboardFocus(true);
    nmui->setWantsKeyboardFocus(true);
//    setOpaque(true);

  }

  virtual ~NodeManagerUIViewport()
  {
    nmui->removeNodeManagerUIListener(this);

  }
  void mouseMove(const MouseEvent & me) override{
    vp.setScrollOnDragEnabled(me.originalComponent==nmui->currentViewer);
  }


  OwnedArray<TextButton> pathButtons;

  void reconstructViewerPath()
  {
    for (auto &b : pathButtons)
    {
      removeChildComponent(b);
      b->removeListener(this);
    }

    pathButtons.clear();

    NodeContainer * c = nmui->currentViewer->nodeContainer;

    while (c != nullptr)
    {
      TextButton * b = new TextButton(c->getNiceName());
      if (c == nmui->currentViewer->nodeContainer) b->setEnabled(false);

      pathButtons.insert(0, b);
      addAndMakeVisible(b);
      b->addListener(this);

      c = c->parentNodeContainer;

    }


    resized();
  }

  void paint(Graphics &g) override
  {
    g.setColour(BG_COLOR.darker(.2f));
    g.fillRect(getLocalBounds().removeFromTop(30));
    const int grid = 100;
    auto area = vp.getViewArea();
    g.setColour(Colours::white.withAlpha(0.03f));
    auto dest = vp.getBoundsInParent();
    for(int x = -area.getX()%grid ; x < area.getWidth(); x+=grid){

      g.drawVerticalLine(x+dest.getX(), dest.getY(), dest.getBottom());
    }
    for(int y = -area.getY()%grid ; y < area.getHeight(); y+=grid){
      g.drawHorizontalLine( y+dest.getY(), dest.getX()  , dest.getRight());
    }

  }

  void resized() override{

    Rectangle<int> r = getLocalBounds();

    Rectangle<int> buttonR = r.removeFromTop(30).reduced(5);

    for (auto & b : pathButtons)
    {
      b->setBounds(buttonR.removeFromLeft(100));
      buttonR.removeFromLeft(5);
    }

    r.removeFromTop(2);

    vp.setBounds(r);

    nmui->setTopLeftPosition(r.getTopLeft());
    nmui->setSize(jmax<int>(r.getWidth(), nmui->getContentWidth()), jmax<int>(r.getHeight(), nmui->getContentHeight()));

  }

  void currentViewedContainerChanged()override
  {
    reconstructViewerPath();
    //nmui->setBounds(getLocalBounds().withTop(30));
    resized();


  }

  void buttonClicked(Button * b)override
  {
    int bIndex = pathButtons.indexOf((TextButton *)b);
    if (bIndex == -1)
    {
      DBG("WTF ?");
    }
    NodeContainer * c = nmui->currentViewer->nodeContainer;
    for (int i = pathButtons.size() - 1; i > bIndex; i--)
    {
      c = c->parentNodeContainer;
    }
    
    nmui->setCurrentViewedContainer(c);
    
  }
  
  Viewport vp;
  NodeManagerUI * nmui;
};

#endif  // NODEMANAGERUI_H_INCLUDED
