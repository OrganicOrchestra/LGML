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


#ifndef GENERICParameterCONTAINEREDITOR_H_INCLUDED
#define GENERICParameterCONTAINEREDITOR_H_INCLUDED

#include "../../../UI/Inspector/InspectorEditor.h"
#include "../ParameterContainer.h"
#include "ParameterUI.h"

class PresetChooserUI;
class GenericParameterContainerEditor;

class CCInnerContainerUI :
    public juce::Component,
    private ControllableContainer::Listener,
    public ButtonListener
{
public:
    class CCLinkBT : public TextButton
    {
    public:
        CCLinkBT (ParameterContainer* targetContainer);
        ParameterContainer* targetContainer;
    };


    CCInnerContainerUI (GenericParameterContainerEditor* editor, ParameterContainer* container, int level, int maxLevel, bool canAccessLowerContainers);
    virtual ~CCInnerContainerUI();

    Label containerLabel;

    ParameterContainer* container;
    ScopedPointer<PresetChooserUI> presetChooser;

    OwnedArray<NamedParameterUI > parametersUI;
    OwnedArray<CCInnerContainerUI> innerContainers;
    ScopedPointer<Component> customEditor;
    OwnedArray<Component> lowerContainerLinks;

    GenericParameterContainerEditor* editor;

    int level;
    int maxLevel;
    bool canAccessLowerContainers;

    void paint (Graphics& g) override;
    void resized() override;
    void clear();
    void rebuild();

    void addParameterUI (Parameter* c);
    void removeParameterUI (Parameter* c);

    void addCCInnerUI (ParameterContainer* cc);
    void removeCCInnerUI (ParameterContainer* cc);

    void addCCLink (ParameterContainer* cc);
    void removeCCLink (ParameterContainer* cc);

    int getContentHeight();

    NamedParameterUI* getUIForParameter (Parameter* c);
    CCInnerContainerUI* getInnerContainerForCC (ParameterContainer* cc);
    CCLinkBT* getCCLinkForCC (ParameterContainer* cc);

    void controllableAdded (ControllableContainer*, Controllable*)override;
    void controllableRemoved (ControllableContainer*, Controllable*)override;
    void controllableContainerAdded (ControllableContainer*, ControllableContainer*)override;
    void controllableContainerRemoved (ControllableContainer*, ControllableContainer*)override;
    void childStructureChanged (ControllableContainer*, ControllableContainer*,bool isAdded)override;

    void buttonClicked (Button* b)override;
};


class GenericParameterContainerEditor : public InspectorEditor, public ButtonListener, private ControllableContainer::Listener, Timer
{
public :
    GenericParameterContainerEditor (ParameterContainer* sourceComponent);
    virtual ~GenericParameterContainerEditor();

    TextButton parentBT;

    WeakReference<ParameterContainer> sourceContainer;
    ScopedPointer<CCInnerContainerUI> innerContainer;

    void setCurrentInspectedContainer (ParameterContainer*, bool forceUpdate = false, int recursiveInspectionLevel = 0, bool canInspectChildContainersBeyondRecursion = true);

    virtual int getContentHeight() override;

    void resized() override;
    void clear() override;

    void buttonClicked (Button*  b)override;

    void childStructureChanged (ControllableContainer*, ControllableContainer*,bool isAdded) override;

    void handleCommandMessage (int cID)override;

    void timerCallback()override;

    enum
    {
        CHILD_STRUCTURE_CHANGED = 0
    } commandMessageIDs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GenericParameterContainerEditor)
};


#endif  // GENERICParameterCONTAINEREDITOR_H_INCLUDED
