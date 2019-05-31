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


#ifndef SHAPESHIFTERMANAGER_H_INCLUDED
#define SHAPESHIFTERMANAGER_H_INCLUDED

#include "ShapeShifterContainer.h"
#include "ShapeShifterWindow.h"
#include "ShapeShifterFactory.h"
#include "../../Logger/LGMLLogger.h"


class ShapeShifterManager :
    public ShapeShifterPanel::Listener,
    private LGMLLogger::CoalescedListener
{
public:
    juce_DeclareSingleton (ShapeShifterManager, true);
    ShapeShifterManager();
    virtual ~ShapeShifterManager();

    ShapeShifterContainer mainShifterContainer;

    File lastFile;
    File defaultFolder;

    template<typename T>
    Array<Component::SafePointer<T> >getAllSPanelsOfType(bool onlyVisible=true,ShapeShifterContainer * root=nullptr){
        Array<Component::SafePointer<T> > res;
        if(!root) root = &mainShifterContainer;
        for(auto & p:root->shifters){
            if(auto c = dynamic_cast<ShapeShifterContainer*>(p)){
                res.addArray(getAllSPanelsOfType<T>(onlyVisible,c));
            }
            else if(auto pa = dynamic_cast<ShapeShifterPanel*>(p)){
                for( auto co:pa->contents){
                    if (!onlyVisible || co->contentIsShown)
                        if(auto rco = dynamic_cast<T*>(co))
                            res.add(rco);
                }
            }
            
        }
        return res;
    }

    OwnedArray<ShapeShifterPanel> openedPanels;
    OwnedArray<ShapeShifterWindow> openedWindows;

    ShapeShifterPanel* currentCandidatePanel;
    void setCurrentCandidatePanel (ShapeShifterPanel*);

    PanelName getPanelNameForContentName (const String& name);
    String getContentNameForPanelName (PanelName panelName);

    ShapeShifterPanel* getPanelForContent (ShapeShifterContent* content);
    ShapeShifterPanel* getPanelForContentName (const String& name);
    ShapeShifterPanelTab* getTabForContentName (const String& name);

    ShapeShifterPanel* createPanel (ShapeShifterContent* content, ShapeShifterPanelTab* sourceTab = nullptr);
    void removePanel (ShapeShifterPanel* panel);

    ShapeShifterWindow* showPanelWindow (ShapeShifterPanel* _panel, Rectangle<int> bounds);
    ShapeShifterWindow* showPanelWindowForContent (PanelName panelName);
    void showContent (String contentName);


    void closePanelWindow (ShapeShifterWindow* window, bool doRemovePanel);

    ShapeShifterContent* getContentForName (PanelName contentName);

    ShapeShifterPanel *checkCandidateTargetForPanel(ShapeShifterPanel *panel, Point<float> relativeMousePoint);
    bool checkDropOnCandidateTarget (WeakReference<ShapeShifterPanel> panel);

    ShapeShifterWindow* getWindowForPanel (ShapeShifterPanel* panel);

    void loadLayout (var layoutObject);
    var getCurrentLayout();
    void loadLayoutFromFile (int fileIndexInLayoutFolder = -1);
    void loadLayoutFromFile (const File& fromFile);
    void loadLastSessionLayoutFile();
    void loadDefaultLayoutFile();
    void saveCurrentLayout();
    void saveCurrentLayoutToFile (const File& toFile);
    Array<File> getLayoutFiles();

    void clearAllPanelsAndWindows();

    const int baseMenuCommandID = 0x31000;
    const int baseSpecialMenuCommandID = 0x32000;
    PopupMenu getPanelsMenu();

    void handleMenuPanelCommand (int commandID);

    //LGML Logger to blink tabs
    void newMessages(int from,int to)final;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShapeShifterManager)
};

#endif  // SHAPESHIFTERMANAGER_H_INCLUDED
