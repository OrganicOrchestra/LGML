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

#ifndef FASTMAP_H_INCLUDED
#define FASTMAP_H_INCLUDED

#include "../Controllable/Parameter/ParameterContainer.h"
#include "../Controllable/Parameter/ParameterProxy.h"


struct Smoother;

class FastMap :
    public ParameterContainer,
    public ParameterProxy::ParameterProxyListener
{
public:
    FastMap();
    virtual ~FastMap();

    BoolParameter* enabledParam;
    BoolParameter* fullSync;

    RangeParameter* inputRange;
    RangeParameter* outputRange;

    BoolParameter* invertParam,*toggleParam;
    FloatParameter *smoothTimeIn,*smoothTimeOut;

    ParameterProxy* referenceIn;
    ParameterProxy*   referenceOut;

    ParameterProxy * getProxyForParameter(ParameterBase*,bool recursive=false) const;


private:
    void onContainerParameterChanged ( ParameterBase*)override;

    
    std::unique_ptr<Smoother> smoother;
    bool smoothingEnabled();
    
    bool isInRange; //memory for triggering
    bool fastMapIsProcessing;

    void process (bool toReferenceOut = true,bool sourceHasChanged = true);
    // inherited from proxy listener
    void linkedParamValueChanged (ParameterProxy*,ParameterBase::Listener *) override;
    void linkedParamRangeChanged(ParameterProxy*) override;
    void linkedParamChanged (ParameterProxy*) override;
private:
    WeakReference< FastMap >::Master masterReference;
    friend class WeakReference<FastMap>;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FastMap)
};


#endif  // FASTMAP_H_INCLUDED
