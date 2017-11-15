/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#ifndef OUTLINER_H_INCLUDED
#define OUTLINER_H_INCLUDED


#include "ShapeShifter/ShapeShifterContent.h"
#include "../Controllable/Parameter/ParameterContainer.h"
#include "Inspector/Inspector.h"


class OutlinerItem;
class ParameterUI;

class OutlinerItemComponent :
public  InspectableComponent,
public SettableTooltipClient,
private Button::Listener,
private Label::Listener
{
public:
    OutlinerItemComponent (OutlinerItem* item);
    OutlinerItem* item;

    Label label;
    ScopedPointer<ParameterUI> paramUI;
    ScopedPointer<Button> addUserParamBt;
    ScopedPointer<Button> removeMeBt;
    void paint (Graphics& g) override;
    void mouseDown (const MouseEvent& e) override;
    void resized()override;
    void buttonClicked (Button*) override;
    void labelTextChanged (Label* labelThatHasChanged) override;
};

class OutlinerItem : public TreeViewItem,ControllableContainer::Listener
{
public:
    OutlinerItem (ParameterContainer* container,bool generateSubTree);
    OutlinerItem (Parameter* controllable,bool generateSubTree);
    ~OutlinerItem();

    bool isContainer;

    WeakReference<ParameterContainer> container;
    WeakReference<Parameter> parameter;
    WeakReference<Component> currentDisplayedComponent;

    String getUniqueName() const override;
    void controllableContainerAdded(ControllableContainer * notif,ControllableContainer * ori)override;
    void controllableContainerRemoved(ControllableContainer * notif,ControllableContainer * ori)override;
    void controllableAdded (ControllableContainer*, Controllable*) override;
    void controllableRemoved (ControllableContainer*, Controllable*)override;
    bool mightContainSubItems() override;
    void itemSelectionChanged (bool isNowSelected) override;
    Component* createItemComponent() override;

private:
    friend class WeakReference<OutlinerItem>;
    WeakReference<OutlinerItem>::Master masterReference;

    JUCE_LEAK_DETECTOR(OutlinerItem);
};

class Outliner : public ShapeShifterContentComponent,
private ControllableContainerListener, AsyncUpdater,
TextEditorListener,
Button::Listener,
Inspector::InspectorListener
{
public:

    Outliner (const String& contentName,ParameterContainer* root=nullptr,bool showFilterText = true);
    ~Outliner();
    void clear();

    TreeView treeView;
    ScopedPointer<OutlinerItem> rootItem;
    WeakReference<ParameterContainer> root,baseRoot;

    TextEditor filterTextEditor;
    TextButton linkToSelected;
    String nameFilter;

    bool showHiddenContainers; //include or exclude in treeview the "skipInAddress" containers (may be later exposed to user as an option)

    void resized() override;
    void paint (Graphics& g) override;

    void textEditorTextChanged (TextEditor&)override;

    void rebuildTree();
    void buildTree (OutlinerItem* parentItem, ParameterContainer* parentContainer, bool shouldFilter = true);

    void childStructureChanged (ControllableContainer*, ControllableContainer*,bool isAdded) override;
    void handleAsyncUpdate()override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Outliner)

    void setRoot(ParameterContainer * );
    bool showUserContainer;
private:
    void currentComponentChanged (Inspector *)override;
    void buttonClicked(Button *b) override;
    void saveCurrentOpenChilds();
    void restoreCurrentOpenChilds();
    // we should be using RAII like scoped pointer, but hashmap accessors, cant contains Scoped
    HashMap<WeakReference<ParameterContainer>,XmlElement*> opennessStates;


};

#endif  // OUTLINER_H_INCLUDED
