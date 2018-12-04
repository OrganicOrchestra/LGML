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


#ifndef MOVABLEPANEL_H_INCLUDED
#define MOVABLEPANEL_H_INCLUDED

#include "ShapeShifter.h"
class ShapeShifterPanelTab;
#include "ShapeShifterContent.h"
#include "ShapeShifterPanelHeader.h"


class ShapeShifterPanel : public ShapeShifter, public ShapeShifterPanelHeader::Listener
{
public:
    enum AttachZone { NONE, TOP, BOTTOM, LEFT, RIGHT, CENTER };

    explicit ShapeShifterPanel (ShapeShifterContent* innerComponent = nullptr, ShapeShifterPanelTab* sourceTab = nullptr);
    virtual ~ShapeShifterPanel();

    static constexpr int headerHeight = 20;
    ShapeShifterPanelHeader header;

    OwnedArray<ShapeShifterContent> contents;

    bool transparentBackground;
    bool targetMode;

    Point<float> candidateTargetPoint;
    AttachZone candidateZone;


    ShapeShifterContent* currentContent;
    void setCurrentContent (ShapeShifterContent* content);
    void setCurrentContent (const String& name);


    void paint (Graphics& g) override;
    void paintOverChildren (Graphics& g) override;
    void resized() override;


    void setTargetMode (bool value);

    void setTransparentBackground (bool value);

    void attachTab (ShapeShifterPanelTab*);
    void detachTab (ShapeShifterPanelTab*, bool createNewPanel);
    void removeTab (ShapeShifterPanelTab*);

    bool attachPanel (ShapeShifterPanel*); //attach all tabs of a panel to this panel, or attach a panel to the parent container depending on attachZone

    void addContent (ShapeShifterContent* content, bool setCurrent = true);

    bool hasContent (ShapeShifterContent* content);
    bool hasContent (const String& name);
    ShapeShifterContent* getContentForName (const String& name);

    bool isFlexible() override;

    //Attach helpers

    AttachZone checkAttachZone(ShapeShifterPanel *source, Point<float> relativePoint);
    void setCandidateZone (AttachZone zone);

    virtual var getCurrentLayout() override;
    virtual void loadLayoutInternal (var layout) override;

    virtual void tabDrag (ShapeShifterPanelTab*) override;
    virtual void tabSelect (ShapeShifterPanelTab*) override;
    virtual void askForRemoveTab (ShapeShifterPanelTab*) override;
    virtual void headerDrag() override;

    //Listener
    class Listener
    {
    public:
        virtual ~Listener() {};
        virtual void panelDetach (ShapeShifterPanel*) {}
        virtual void panelEmptied (ShapeShifterPanel*) {};
        virtual void panelDestroyed (ShapeShifterPanel*) {};

        virtual void headerDrag (ShapeShifterPanel*) {}
        virtual void tabDrag (ShapeShifterPanel*) {};
        virtual void contentRemoved (ShapeShifterContent*) {}
    };

    ListenerList<Listener> listeners;
    void addShapeShifterPanelListener (Listener* newListener)
    {
        if (header.tabs.size() > 0){
//            DBG ("Panel, addListener (" << header.tabs[0]->content->contentName << ")");
        }

        listeners.add (newListener);
    }
    void removeShapeShifterPanelListener (Listener* listener) { listeners.remove (listener); }

    bool isMini();
    void setMini(bool m);
    void setFlexibleDimension(int d);
    int getFlexibleDimension();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShapeShifterPanel)

    

private:
    WeakReference<ShapeShifterPanel>::Master masterReference;
    friend class WeakReference<ShapeShifterPanel>;

};


#endif  // MOVABLEPANEL_H_INCLUDED
