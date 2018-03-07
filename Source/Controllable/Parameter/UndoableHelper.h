/*
  ==============================================================================

    UndoableHelper.h
    Created: 6 Mar 2018 7:46:16pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once
#include "Parameter.h"
namespace UndoableHelpers{

    void setValueUndoable (Parameter* p,const var & _value, bool silentSet = false, bool force = false);
    
    void startNewTransaction(Parameter *p,bool force=false);
    
    
}
