/*
  ==============================================================================

    DataInNode.h
    Created: 19 Apr 2016 10:38:42am
    Author:  bkupe

  ==============================================================================
*/

#ifndef DATAINNODE_H_INCLUDED
#define DATAINNODE_H_INCLUDED

#include "NodeBase.h"

class DataInNode :
	public NodeBase
{
public:
    DataInNode(NodeManager * nodeManager, uint32 nodeId);
    ~DataInNode();

    Array<Parameter *> dynamicParameters;

    FloatParameter * addFloatParamAndData(const String &name, float initialValue, float minVal, float maxVal)
    {
        FloatParameter * p = addFloatParameter(name, "OSC Control for " + name, initialValue, minVal, maxVal);
        addOutputData(name, Data::DataType::Number);
        dynamicParameters.add(p);
		dataInListeners.call(&DataInListener::parameterAdded, p);
        return p;
    }

    void removeFloatParamAndData(FloatParameter * p)
    {
        removeControllable(p);
        removeOutputData(p->niceName);
		dataInListeners.call(&DataInListener::parameterRemoved, p);
    }


    void onContainerParameterChanged(Parameter * p) override;

    virtual NodeBaseUI * createUI() override;

    //Listener
    class DataInListener
    {
    public:
        virtual ~DataInListener() {}
        virtual void parameterAdded(Parameter *) = 0;
        virtual void parameterRemoved(Parameter *) = 0;

    };

    ListenerList<DataInListener> dataInListeners;
    void addDataInListener(DataInListener* newListener) { dataInListeners.add(newListener); }
    void removeDataInListener(DataInListener* listener) { dataInListeners.remove(listener); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataInNode)
};



#endif  // DATAINNODE_H_INCLUDED
