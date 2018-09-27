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

#ifndef FASTMAPPER_H_INCLUDED
#define FASTMAPPER_H_INCLUDED

#include "../Controllable/Parameter/ParameterContainer.h"
#include "FastMap.h"

#if !ENGINE_HEADLESS
#include "../UI/LGMLDragger.h"
#include "../UI/Inspector/Inspector.h"
#endif

class FastMapper;


class FastMapper :
    public ParameterContainer,
#if !ENGINE_HEADLESS
    private LGMLDragger::Listener,
#endif
    private ParameterProxy::ParameterProxyListener

{
public:
    juce_DeclareSingleton (FastMapper, true)
    DECLARE_OBJ_TYPE (FastMapper,"map all parameters together in LGML")


    virtual ~FastMapper();

    OwnedArray<FastMap> maps;
    ParameterProxy* potentialIn;
    ParameterProxy* potentialOut;
    bool autoAddFastMaps;

    void clear();

    void setPotentialInput ( ParameterBase*);
    void setPotentialOutput ( ParameterBase*);

    FastMap* addFastMap();
    void removeFastmap (FastMap* f);


    ParameterContainer*   addContainerFromObject (const String& name, DynamicObject*   fData) override;




private:

#if !ENGINE_HEADLESS
    // LGMLDragger Listener
    void selectionChanged ( ParameterBase*) override;
    void mappingModeChanged(bool) override;
#endif
    uint32 lastFMAddedTime;

    // proxy listener
    void linkedParamChanged (ParameterProxy*) override;

    // ControllableContainer::Listener
    typedef  ControllableContainer::OwnedFeedbackListener<FastMapper> PSync;
    PSync pSync;
    friend class ControllableContainer::OwnedFeedbackListener<FastMapper>;

    void createNewFromPotentials();
    bool checkDuplicates (FastMap* f);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FastMapper)

};


#endif  // FASTMAPPER_H_INCLUDED
