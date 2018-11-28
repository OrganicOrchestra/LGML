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

    void setValueUndoable ( ParameterBase* p,const var & _value, bool silentSet = false, bool force = false);
    void resetValueUndoable ( ParameterBase* p,bool silentSet=false);
    void startNewTransaction( ParameterBase*p,bool force=false);
    void setParameterCoalesced(bool t);
    
}
