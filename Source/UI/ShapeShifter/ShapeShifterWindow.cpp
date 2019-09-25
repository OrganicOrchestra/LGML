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

#if !ENGINE_HEADLESS

#include "ShapeShifterWindow.h"
#include "ShapeShifterManager.h"





extern ApplicationCommandManager& getCommandManager();


ShapeShifterWindow::ShapeShifterWindow (ShapeShifterPanel* _panel, Rectangle<int> bounds) :
ResizableWindow (_panel->currentContent->contentName, true),
dragMode (PANEL),
panel (_panel),
checking (false)
{
    setOpaque(false);
    setTopLeftPosition (bounds.getTopLeft());
    _panel->setBounds (bounds);

//    panel->setPreferredWidth (getWidth());
//    panel->setPreferredHeight (getHeight());

    setVisible (true);
    _panel->currentContent->contentComponent->setVisible(true);
    setContentNonOwned (_panel, true);
    jassert(panel->currentContent->contentComponent->isVisible());
    jassert(_panel->currentContent->contentComponent->isShowing());


    //DBG("window -> addShapeShifterListener " << panel->header.tabs[0]->content->contentName);
    panel->addShapeShifterPanelListener (this); //is it necessary ?


//    setBackgroundColour (
//                         Colours::transparentWhite
////                         findColour (ResizableWindow::backgroundColourId).darker (.1f).withAlpha (.3f)
//                         );
//    setColour(ResizableWindow::backgroundColourId, Colours::transparentWhite);


    setResizable (true, true);
    setResizeLimits(ShapeShifter::minSize,ShapeShifter::minSize, 4096, 4096);
    getConstrainer()->setMinimumOnscreenAmounts(0xffffff , 0xffffff, 100, 100);

    //setDraggable (true);

    //setUsingNativeTitleBar(true);
    toFront (true);

#if USE_GL_FOR_WINDOWS
    auto * rootShapeShifter = ShapeShifterManager::getInstance()->mainShifterContainer.getTopLevelComponent();
    OpenGLContext* context = OpenGLContext::getContextAttachedTo (*rootShapeShifter);

    if (context)
    {
        void * sharedCtxt = context->getRawContext();
        jassert(sharedCtxt);
        openGLContext.setNativeSharedContext (sharedCtxt);
    }
    else
    {
        jassertfalse;
    }

    openGLContext.setContinuousRepainting (true);
    auto * rootComponent = getTopLevelComponent();

    openGLContext.attachTo (*rootComponent);
#endif

    addMouseListener (this, true);

    addKeyListener ((&getCommandManager())->getKeyMappings());


}

ShapeShifterWindow::~ShapeShifterWindow()
{
#if USE_GL_FOR_WINDOWS
    openGLContext.detach();
#endif
    removeMouseListener (this);
    clear();

}

void ShapeShifterWindow::paintOverChildren (Graphics& )
{

}

void ShapeShifterWindow::resized()
{

    ResizableWindow::resized();

    if (panel == nullptr){
        jassertfalse;
        return;}

//    panel->setSize(getWidth(),getHeight());
    jassert(panel->currentContent->contentComponent->isShowing());
    panel->setPreferredWidth (getWidth());
    panel->setPreferredHeight (getHeight());

}

void ShapeShifterWindow::mouseDown (const MouseEvent& e)
{
    //ResizableWindow::mouseDown(e);
    jassert(panel->currentContent->contentComponent->isVisible());
    if (e.eventComponent == &panel->header || dynamic_cast<ShapeShifterPanelTab*> (e.eventComponent) != nullptr)
    {
        dragMode = e.eventComponent == &panel->header ? PANEL : TAB;
        dragger.startDraggingComponent(this, e);
    }
    else
    {
        dragMode = NONE;
    }
    setAlwaysOnTop(true);

}

void ShapeShifterWindow::mouseDrag (const MouseEvent& e)
{
    //ResizableWindow::mouseDrag(e);
    if (dragMode == NONE) return;
    jassert(panel->currentContent->contentComponent->isVisible());
    jassert(panel->currentContent->contentComponent->isShowing());
    panel->setTransparentBackground (true);

    ShapeShifterManager::getInstance()->checkCandidateTargetForPanel(panel, panel->getLocalPoint(e.originalComponent,
                                                                                                 e.position));

    dragger.dragComponent(this, e, nullptr);
    setTopLeftPosition(getPosition());
}

void ShapeShifterWindow::mouseUp(const MouseEvent &) {
    //ResizableWindow::mouseUp(e);

    panel->setTransparentBackground (false);
    setAlwaysOnTop(false);
    checking = true;
    bool found = ShapeShifterManager::getInstance()->checkDropOnCandidateTarget (panel);
    checking = false;

    if (found)
    {
        clear();
        ShapeShifterManager::getInstance()->closePanelWindow (this, false);
    }


}


void ShapeShifterWindow::clear()
{
    if (panel != nullptr)
    {
        panel->removeShapeShifterPanelListener (this);
        panel = nullptr;
    }
}

void ShapeShifterWindow::userTriedToCloseWindow()
{
    ShapeShifterManager::getInstance()->closePanelWindow (this, true);
}

var ShapeShifterWindow::getCurrentLayout()
{
    var data (new DynamicObject());
    data.getDynamicObject()->setProperty ("panel", panel->getCurrentLayout());
    data.getDynamicObject()->setProperty ("x", getBounds().getPosition().x);
    data.getDynamicObject()->setProperty ("y", getBounds().getPosition().y);
    data.getDynamicObject()->setProperty ("width", getWidth());
    data.getDynamicObject()->setProperty ("height", getHeight());
    return data;
}

void ShapeShifterWindow::panelEmptied (ShapeShifterPanel*)
{
    if (!checking) ShapeShifterManager::getInstance()->closePanelWindow (this, true);
}

#endif
