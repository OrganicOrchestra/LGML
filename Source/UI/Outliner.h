/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#pragma once

#include "ShapeShifter/ShapeShifterContent.h"
#include "../Controllable/Parameter/ParameterContainer.h"
#include "Inspector/Inspector.h"


class OutlinerItem;
class ParameterUI;

class OutlinerItemComponent :
public  InspectableComponent,
public SettableTooltipClient,
private Button::Listener,
private Label::Listener,
private Controllable::Listener, // for name of controllables
private ParameterBase::Listener // for nameParam(container)
{
public:
    explicit OutlinerItemComponent (OutlinerItem* item);
    ~OutlinerItemComponent();
    WeakReference<OutlinerItem> item;

    Label label;
    std::unique_ptr<ParameterUI> paramUI;
    std::unique_ptr<Button> addUserParamBt;
    std::unique_ptr<Button> removeMeBt;
    void paint (Graphics& g) override;
    void mouseDown (const MouseEvent& e) override;
    void resized()override;
    void buttonClicked (Button*) override;
    void labelTextChanged (Label* labelThatHasChanged) override;
    // ControllableListener
    void controllableNameChanged (Controllable* ) override;
    // ParamListener
    void newMessage(const ParameterBase::ParamWithValue &pv)override;

private:
    void updateLabelText( );
    int labelWidth;

};

class OutlinerItem : public TreeViewItem,ControllableContainer::Listener
{
public:
    OutlinerItem (ParameterContainer* container,bool generateSubTree);
    OutlinerItem ( ParameterBase* controllable,bool generateSubTree);
    ~OutlinerItem();

    bool isContainer;

    WeakReference<ParameterContainer> container;
    WeakReference<ParameterBase> parameter;
    WeakReference<Component> currentDisplayedComponent;

    String getUniqueName() const override;
    void controllableContainerAdded(ControllableContainer * notif,ControllableContainer * ori)override;
    void controllableContainerRemoved(ControllableContainer * notif,ControllableContainer * ori)override;
    void childControllableAdded (ControllableContainer*, Controllable*) override;
    void childControllableRemoved (ControllableContainer*, Controllable*)override;
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
TextEditor::Listener,
Button::Listener,
Inspector::InspectorListener

{
public:

    explicit Outliner (const String& contentName,ParameterContainer* root=nullptr,bool showFilterText = true);
    ~Outliner();
    void clear();


    TreeView treeView;
    std::unique_ptr<OutlinerItem> rootItem;
    WeakReference<ParameterContainer> root,baseRoot;

    TextEditor filterTextEditor;
    TextButton linkToSelected;
    TextButton showOnlyUserContainersB;
    String nameFilter;

    

    void resized() override;

    int getTargetHeight();
    void paint (Graphics& g) override;

    void textEditorTextChanged (TextEditor&)override;

    void rebuildTree();
    void buildTree (OutlinerItem* parentItem, ParameterContainer* parentContainer, bool shouldFilter = true);

    void childStructureChanged (ControllableContainer*, ControllableContainer*,bool isAdded) override;
    void containerWillClear (ControllableContainer* origin) override;
    void handleAsyncUpdate()override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Outliner)

    void setRoot(ParameterContainer *,bool saveOpenness=true );
    bool showOnlyUserContainers;

private:

    void selectionChanged (Inspector *)override;
    void buttonClicked(Button *b) override;
    void saveCurrentOpenChilds();
    void restoreCurrentOpenChilds();
    class XmlElementCounted : public ReferenceCountedObject{
    public:
        XmlElementCounted(std::unique_ptr<XmlElement> e):xml(std::move(e)){};
        std::unique_ptr<XmlElement> xml;
        using Ptr = ReferenceCountedObjectPtr<XmlElementCounted>;
    };
    // we should be using RAII like scoped pointer, but hashmap accessors, cant contains Scoped
    HashMap<WeakReference<ParameterContainer>,XmlElementCounted::Ptr> opennessStates;
    static Identifier blockSelectionPropagationId;
    friend class OutlinerItem;


};

