/*
  ==============================================================================

    SelectableComponentHandler.cpp
    Created: 28 Mar 2016 3:10:48pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "SelectableComponentHandler.h"
#include "SelectableComponent.h"



void SelectableComponentHandler::internalSelected(SelectableComponent * c,bool state,bool unique){

    if(unique || c==nullptr){
        for(auto & cc:selected){
            cc->askForSelection(false,false);
            selectableHandlerListeners.call(&SelectableHandlerListener::selectableChanged,cc, false);

        }
        selected.clear();
    }
    if(c){
        c->internalSetSelected(state);
        if(state)selected.add(c);
        else selected.removeFirstMatchingValue(c);
        selectableHandlerListeners.call(&SelectableHandlerListener::selectableChanged,c, state);
    }


}
