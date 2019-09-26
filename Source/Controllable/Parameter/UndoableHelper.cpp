/*
  ==============================================================================

    UndoableHelper.cpp
    Created: 6 Mar 2018 7:46:53pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "UndoableHelper.h"
#include "../../Engine.h"

extern UndoManager& getAppUndoManager();
namespace UndoableHelpers{

class UndoableSetValueAction : public UndoableAction{
public:
    UndoableSetValueAction( ParameterBase* p,const var & _value, bool _silentSet, bool _force,bool _isResetAction=false):
    parameter(p),
    silentSet(_silentSet),
    force(_force),
    isResetAction(_isResetAction)
    {
        if(parameter.get()){
            lastValue = parameter->value.clone();
        }
        value = _value.clone();
    }



    bool perform() override{
        if(parameter.get()){
            if(isResetAction){
                parameter->resetValue();
            }
            else{
                parameter->setValue(value,silentSet,force);
            }
            return true;
        }
        return false;

    };


    bool undo() override{
        if(parameter.get()){
            parameter->setValue(lastValue,silentSet,force);
            return true;
        }
        return false;
    };

    UndoableAction* createCoalescedAction (UndoableAction* nextAction) override {

        if(auto n = dynamic_cast<UndoableSetValueAction*>(nextAction)){
            if(n->parameter==parameter){
                auto na = new UndoableSetValueAction(parameter,n->value,n->silentSet,n->force,n->isResetAction);
                na->lastValue = lastValue.clone();
                return na;
            }

        }
        return nullptr;
        }

    WeakReference<ParameterBase> parameter;
    bool silentSet,force;
    var value,lastValue;
    bool isResetAction;
};
    bool isCoalescing = false;
    void setParameterCoalesced(bool t){
        isCoalescing  = t;
    }
    void setValueUndoable ( ParameterBase* p,const var & _value, bool silentSet , bool force ){
        if(p){
            if(!isCoalescing)startNewTransaction(p);
            getAppUndoManager().perform(new UndoableSetValueAction(p,_value,silentSet,force));
        }
    }
    void resetValueUndoable ( ParameterBase* p, bool silentSet ){
        if(p){
            if(!isCoalescing)startNewTransaction(p);
            getAppUndoManager().perform(new UndoableSetValueAction(p,var(),silentSet,false,true));
        }
    }

    void startNewTransaction( ParameterBase*p,bool force){
        String tname ("set " + p->niceName + (p->parentContainer?("from : " + p->parentContainer->getNiceName()):""));
        if(force || tname!=getAppUndoManager().getCurrentTransactionName()){
            getAppUndoManager().beginNewTransaction (tname);
        }
    }


}
