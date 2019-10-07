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

#ifndef FASTMAPPERUI_H_INCLUDED
#define FASTMAPPERUI_H_INCLUDED

#include "../UI/ShapeShifter/ShapeShifterContent.h"
#include "FastMapUI.h"
#include "../UI/Inspector/Inspector.h"
#include "../UI/Style.h"
#include "../UI/LGMLDragger.h"
#include "../UI/StackedContainerUI.h"

#include "FastMapper.h"

class FastMapperUI :
    public InspectableComponent,
    public ShapeShifterContent,
    private ControllableContainerListener,
    private Button::Listener,
    private Inspector::InspectorListener,
    private LGMLDragger::Listener


{
public:
    typedef ParameterContainer ContainerType;
    typedef ParameterBase ControllableType;

    FastMapperUI (const String& contentName, FastMapper* fastMapper);
    virtual ~FastMapperUI();

    FastMapper* fastMapper;
    TextButton linkToSelection;
    Label candidateLabel;
    AddElementButton addFastMapButton;
    void addFastMapUndoable();
    
    std::unique_ptr<Component> potentialIn, potentialOut;
    StackedContainerViewport<FastMapUI,FastMap> mapsUI;

    Array<WeakReference<ContainerType> > viewFilterContainers;
    Array<WeakReference<ControllableType>> viewFilterControllables;

    void clear();

    void addFastMapUI (FastMap*);
    void removeFastMapUI (FastMapUI*);

    void resetAndUpdateView();
    void setViewFilter (const Array<WeakReference<ContainerType> > & filterContainer);
    void setViewFilter (const Array<WeakReference<ControllableType>> & filterControllable);
    void resetViewFilter();
    bool mapPassViewFilter (FastMap*);



    const int mapHeight = 35;
    const int gap = 5;
    int getContentHeight() const;

    void resized() override;
    void mouseDown (const MouseEvent& e) override;

    virtual void controllableContainerAdded (ControllableContainer*, ControllableContainer*) override;
    virtual void controllableContainerRemoved (ControllableContainer*, ControllableContainer*) override;

    
private:

    void buttonClicked (Button*) override;
    // Inspector Listener
    void selectionChanged (Inspector* ) override;

    // LGMLDrager
    void mappingModeChanged(bool) override;
    void selectionChanged ( ParameterBase*) override{}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FastMapperUI)

};



#endif  // FASTMAPPERUI_H_INCLUDED
