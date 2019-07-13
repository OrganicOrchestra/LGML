/*
  ==============================================================================

    ParameterUIHelpers.cpp
    Created: 13 Jul 2019 5:22:35pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "ParameterUIHelpers.h"


void AllParamType::add(ParameterUI* p){
    jassert(p->parameter.get());

    container.getReference(p->parameter.get()).add(p);
    allPs.set(p,p->parameter.get());
}
void AllParamType::removeAllInstancesOf(ParameterUI* p){
    auto paramPtr = p->parameter.get();
    if(!paramPtr){
        paramPtr = allPs[p];
    }
    jassert(paramPtr);
    container.getReference(paramPtr).removeAllInstancesOf(p);
    allPs.remove(p);
}

AllParamType::ArrayType AllParamType::getForParameter(ParameterBase *p) const{
    jassert(p);

    return container[p];
}
