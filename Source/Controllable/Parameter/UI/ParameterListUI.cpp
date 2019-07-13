/*
  ==============================================================================

    ParameterListUI.cpp
    Created: 24 Mar 2018 5:00:25pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "ParameterListUI.h"

template<class T>
ParameterListUI<T>::ParameterListUI(ParameterList<T> *p):ParameterUI(p){
    for(int i = 0 ; i < p->size() ; i++){
        uiElems.add(new SliderUI<T>(p->paramsList[i]));
        addAndMakeVisible(uiElems.getLast());
        
    }
    
}


template<class T>
void ParameterListUI<T>::resized(){
    if(uiElems.size()==0) return;
    auto b = getLocalBounds();
    constexpr int gap = 2;
    float step = b.getWidth()/uiElems.size();
    for(auto s:uiElems){
        s->setBounds(b.removeFromLeft(step).reduced(gap,0));
    }

}

template class ParameterListUI<int>;
template class ParameterListUI<floatParamType>;
