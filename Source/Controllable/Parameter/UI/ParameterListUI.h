/*
  ==============================================================================

    Point2DParameterUI.h
    Created: 23 Mar 2018 12:28:02pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once
#include "ParameterUI.h"
#include "../ParameterList.h"
#include "SliderUI.h"
template<class T>
class ParameterListUI : public ParameterUI{
public:
    ParameterListUI(ParameterList<T> *p);
    OwnedArray<ParameterUI> uiElems;


    void resized()override;

    
};
