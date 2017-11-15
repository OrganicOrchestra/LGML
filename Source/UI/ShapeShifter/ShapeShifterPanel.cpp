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


#include "ShapeShifterPanel.h"
#include "../Style.h"
#include "ShapeShifterManager.h"
#include "ShapeShifterFactory.h"

constexpr float panelRelativeAttachSize = 0.2f;


ShapeShifterPanel::ShapeShifterPanel (ShapeShifterContent* _content, ShapeShifterPanelTab* sourceTab) :
    ShapeShifter (ShapeShifter::PANEL),
    currentContent (nullptr),
    transparentBackground (false),
    targetMode (false),
    candidateZone (NONE),
    candidateTargetPoint (Point<float>())
{
    Component::setName("Panel");
    addAndMakeVisible (header);
    header.addHeaderListener (this);

    if (_content != nullptr)
    {
        if (sourceTab == nullptr)
        {
            addContent (_content);
        }
        else
        {
            attachTab (sourceTab);
        }
    }

      setOpaque(true);

}

ShapeShifterPanel::~ShapeShifterPanel()
{

    for (auto& c : contents) c->contentIsShown = false;

    header.removeHeaderListener (this);
    listeners.call (&Listener::panelDestroyed, this);
    masterReference.clear();
}


void ShapeShifterPanel::setCurrentContent (ShapeShifterContent* _content)
{
    if (_content == currentContent){
        return;
    }

    if (currentContent != nullptr)
    {
        ShapeShifterPanelTab* tab = header.getTabForContent (currentContent);

        if (tab != nullptr) tab->setSelected (false);

        removeChildComponent (currentContent->contentComponent);
        currentContent->contentIsShown = false;
    }


    currentContent = _content;

    if (currentContent != nullptr)
    {
        ShapeShifterPanelTab* tab = header.getTabForContent (currentContent);

        if (tab != nullptr) tab->setSelected (true);

        addAndMakeVisible (currentContent->contentComponent);

        currentContent->contentIsShown = true;
    }

    resized();
}

void ShapeShifterPanel::setCurrentContent (const String& name)
{
    ShapeShifterContent* c = getContentForName (name);

    if (c != nullptr) setCurrentContent (c);
}

void ShapeShifterPanel::setTargetMode (bool value)
{
    if (targetMode == value) return;

    targetMode = value;
    repaint();
}

void ShapeShifterPanel::paint (Graphics& g)
{
    g.setColour (findColour (ResizableWindow::backgroundColourId).withAlpha (transparentBackground ? .3f : 1));
    g.fillRect (getLocalBounds().withTrimmedTop (headerHeight));
}

void ShapeShifterPanel::paintOverChildren (Graphics& g)
{
    if (!targetMode) return;

    Rectangle<float> r = getLocalBounds().toFloat();

    Colour hc = findColour (TextButton::buttonOnColourId).withAlpha (.5f);
    Colour nc = findColour (TextButton::buttonColourId).withAlpha (.3f);

    static float sideMinSize(10);
    float zoneHeight =jmax<float> (sideMinSize, r.getHeight() * panelRelativeAttachSize);
    float zoneWidth = jmax<float> (sideMinSize, r.getWidth() * panelRelativeAttachSize);
    static constexpr float pixGap = 10.0f;
    float scaleX = 1.0f - jmax<float>(0.f,pixGap/r.getWidth());
    float scaleY =  1.0f - jmax<float>(0.f,pixGap/r.getHeight());
    if (!isDetached())
    {
        auto symetryTransform = AffineTransform::rotation(juce::float_Pi, (float)r.getCentreX(),(float)r.getCentreY());
        g.setColour (candidateZone == AttachZone::TOP ? hc : nc);
        Path pt ;
        pt.addQuadrilateral(0.0f,0.0f,
                            zoneWidth, zoneHeight,
                            r.getWidth()-zoneWidth, zoneHeight,
                            r.getWidth(),0.0f);
        pt.applyTransform(AffineTransform::scale(scaleX ,scaleY, (float)r.getCentreX(), 0.0f));
        g.fillPath(pt);

        g.setColour (candidateZone == AttachZone::BOTTOM ? hc : nc);
        pt.applyTransform(symetryTransform);
        g.fillPath(pt);


        g.setColour (candidateZone == AttachZone::LEFT ? hc : nc);
        Path pl ;
        pl.addQuadrilateral(0.0f, 0.0f,
                            zoneWidth, zoneHeight,
                            zoneWidth, r.getHeight()-zoneHeight,
                            0.0f, r.getHeight());
        pl.applyTransform(AffineTransform::scale(scaleX ,scaleY, 0,r.getCentreY()));
        g.fillPath(pl);

        g.setColour (candidateZone == AttachZone::RIGHT ? hc : nc);
        pl.applyTransform(symetryTransform);
        g.fillPath(pl);

    }

    g.setColour (candidateZone == AttachZone::CENTER ? hc : nc);
    g.fillRect(r.reduced(zoneWidth, zoneHeight).reduced((1.0f-scaleX)*0.5*r.getWidth(), (1.0f-scaleY)*0.5*r.getHeight()));


}

void ShapeShifterPanel::resized()
{
    Rectangle<int> r = getLocalBounds();
    header.setBounds (r.removeFromTop (headerHeight));

    if (currentContent != nullptr)
    {
        currentContent->contentComponent->setBounds (r);
    }
}

void ShapeShifterPanel::setTransparentBackground (bool value)
{
    if (transparentBackground == value) return;

    transparentBackground = value;
    repaint();
}

void ShapeShifterPanel::attachTab (ShapeShifterPanelTab* tab)
{

    if(tab && tab->content){
    Component::setName(Component::getName()+":"+tab->content->contentName);
    }
    header.attachTab (tab);

    contents.add (tab->content);
    setCurrentContent (tab->content);

}

void ShapeShifterPanel::detachTab (ShapeShifterPanelTab* tab, bool createNewPanel)
{
    if(tab && tab->content){
        Component::setName(Component::getName().replaceFirstOccurrenceOf(":"+tab->content->contentName, ""));
    }
    ShapeShifterContent* content = tab->content;

    header.removeTab (tab, false);
    contents.removeObject (content, false);

    if (createNewPanel)
    {
        Rectangle<int> r = getScreenBounds();
        ShapeShifterPanel* newPanel = ShapeShifterManager::getInstance()->createPanel (content, tab);
        ShapeShifterManager::getInstance()->showPanelWindow (newPanel, r);
    }

    if (currentContent == content)
    {
        if (contents.size() > 0)
        {
            setCurrentContent (contents[juce::jlimit<int> (0, contents.size() - 1, contents.indexOf (content))]);
        }
        else
        {
            //DBG("panel emtied, num listeners " << listeners.size());
            listeners.call (&Listener::panelEmptied, this);
        }
    }

}

void ShapeShifterPanel::addContent (ShapeShifterContent* content, bool setCurrent)
{
    if (content)
    {
        header.addTab (content);
        contents.add (content);

        if (setCurrent) setCurrentContent (content);
    }
    else
    {
        jassertfalse;
    }
}

bool ShapeShifterPanel::hasContent (ShapeShifterContent* content)
{
    for (auto& c : contents)
    {
        if (c == content) return true;
    }

    return false;
}


bool ShapeShifterPanel::hasContent (const String& name)
{
    return getContentForName (name) != nullptr;
}

ShapeShifterContent* ShapeShifterPanel::getContentForName (const String& name)
{
    for (auto& c : contents)
    {
        if (c == nullptr) continue;

        if (c->contentName == name) return c;
    }

    return nullptr;
}

bool ShapeShifterPanel::isFlexible()
{
    if (currentContent == nullptr) return false;

    return currentContent->contentIsFlexible;
}

void ShapeShifterPanel::removeTab (ShapeShifterPanelTab* tab)
{
    ShapeShifterContent* content = tab->content;
    contents.removeObject (content, false);
    header.removeTab (tab, true);

    if (currentContent == content)
    {
        if (contents.size() > 0)
        {
            setCurrentContent (contents[juce::jlimit<int> (0, contents.size() - 1, contents.indexOf (content) - 1)]);
        }
        else
        {
            listeners.call (&Listener::panelEmptied, this);
        }
    }

    listeners.call (&Listener::contentRemoved, content);
    delete content;

}

bool ShapeShifterPanel::attachPanel (ShapeShifterPanel* panel)
{
    switch (candidateZone)
    {
        case LEFT:
        case RIGHT:
        case TOP:
        case BOTTOM:
            if (parentShifterContainer != nullptr) parentShifterContainer->insertPanelRelative (panel, this, candidateZone);
            else return false;

            break;

        case NONE:
            return false;

        case CENTER:

            int numTabs = panel->header.tabs.size();

            while (numTabs > 0)
            {
                ShapeShifterPanelTab* t = panel->header.tabs[0];
                panel->detachTab (t, false);
                attachTab (t);
                numTabs--;
            }

            ShapeShifterManager::getInstance()->removePanel (panel);

            break;


    }

    return true;
}



ShapeShifterPanel::AttachZone ShapeShifterPanel::checkAttachZone (ShapeShifterPanel* source)
{
    AttachZone z = AttachZone::NONE;

    candidateTargetPoint = getLocalPoint (source, Point<float>());

    double rx = candidateTargetPoint.x / getWidth() - 0.5;
    double ry = candidateTargetPoint.y / getHeight() - 0.5;

    if (fabs(rx) > 0.5 || fabs(ry) > 0.5 )
    {
        jassertfalse;
    }
    else
    {
        if( (fabs(rx) < 0.5f -panelRelativeAttachSize) && (fabs(ry) < 0.5f - panelRelativeAttachSize)){
            z = AttachZone::CENTER;
        }
        else{
            // project on diagonals
            float dx =rx - ry;
            float dy = rx + ry;
            if (dx>0){
                if( dy>0) z = AttachZone::RIGHT;
                else z= AttachZone::TOP;

            }
            else{
                if (dy >0) z = AttachZone::BOTTOM;
                else z = AttachZone::LEFT;
                
            }
        }
    }

    if (isDetached() && z != AttachZone::CENTER) z = AttachZone::NONE;

    setCandidateZone (z);
    return candidateZone;
}

void ShapeShifterPanel::setCandidateZone (AttachZone zone)
{
    if (candidateZone == zone) return;

    candidateZone = zone;
    repaint();
}


var ShapeShifterPanel::getCurrentLayout()
{
    var layout = ShapeShifter::getCurrentLayout();
    var tabData;

    for (auto& t : header.tabs)
    {
        if (t->content == nullptr) continue;

        var tData (new DynamicObject());
        tData.getDynamicObject()->setProperty ("name", t->content->contentName);
        tabData.append (tData);
    }

    if (currentContent != nullptr) layout.getDynamicObject()->setProperty ("currentContent", currentContent->contentName);

    layout.getDynamicObject()->setProperty ("tabs", tabData);

    return layout;
}

void ShapeShifterPanel::loadLayoutInternal (var layout)
{
    juce::Array<var>* tabData = layout.getDynamicObject()->getProperty ("tabs").getArray();

    if (tabData != nullptr)
    {
        for (auto& tData : *tabData)
        {
            String t = tData.getDynamicObject()->getProperty ("name").toString();
            ShapeShifterContent* c = ShapeShifterFactory::createContentForName (tData.getDynamicObject()->getProperty ("name"));
            addContent (c);
        }
    }

    if (layout.getDynamicObject()->hasProperty ("currentContent"))
    {
        setCurrentContent (layout.getDynamicObject()->getProperty ("currentContent").toString());
    }
}

void ShapeShifterPanel::tabDrag (ShapeShifterPanelTab* tab)
{
    if (!isDetached() ) detachTab (tab, true);
    else listeners.call (&Listener::tabDrag, this);
}

void ShapeShifterPanel::tabSelect (ShapeShifterPanelTab* tab)
{
    setCurrentContent (tab->content);
}

void ShapeShifterPanel::askForRemoveTab (ShapeShifterPanelTab* tab)
{
    removeTab (tab);
}

void ShapeShifterPanel::headerDrag()
{
    if (!isDetached())
    {
        listeners.call (&Listener::panelDetach, this);
    }
    else listeners.call (&Listener::headerDrag, this);
}
