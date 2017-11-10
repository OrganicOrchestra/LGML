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

#ifndef FASTMAPPER_H_INCLUDED
#define FASTMAPPER_H_INCLUDED

#include "../Controllable/Parameter/ParameterContainer.h"
#include "FastMap.h"

#include "../UI/LGMLDragger.h"
#include "../UI/Inspector/Inspector.h"


class FastMapper;


class FastMapper :
    public ParameterContainer,
    private LGMLDragger::Listener

{
public:
    juce_DeclareSingleton (FastMapper, true)
    DECLARE_OBJ_TYPE (FastMapper)


    virtual ~FastMapper();

    OwnedArray<FastMap> maps;
    ParameterProxy* potentialIn;
    ParameterProxy* potentialOut;
    bool autoAddFastMaps;

    void clear();

    void setPotentialInput (Parameter*);
    void setPotentialOutput (Parameter*);

    FastMap* addFastMap();
    void removeFastmap (FastMap* f);


    ParameterContainer*   addContainerFromObject (const String& name, DynamicObject*   fData) override;




private:

    // LGMLDragger Listener
    void selectionChanged (Parameter*) override;
    void mappingModeChanged(bool) override;

    uint32 lastFMAddedTime;


    // ControllableContainer::Listener
    void controllableFeedbackUpdate (ControllableContainer*, Controllable*) override;
    void createNewFromPotentials();
    bool checkDuplicates (FastMap* f);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FastMapper)

};


#endif  // FASTMAPPER_H_INCLUDED
