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

class DataInNode : public NodeBase
{
public:
    class DataInNodeDataProcessor : public NodeBase::NodeDataProcessor
    {
    public:
        DataInNodeDataProcessor() :NodeBase::NodeDataProcessor() {

        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataInNodeDataProcessor)
    };


    DataInNode(NodeManager * nodeManager, uint32 nodeId);
    ~DataInNode();

    Array<Parameter *> dynamicParameters;

    FloatParameter * addFloatParamAndData(const String &name, float initialValue, float minVal, float maxVal)
    {
        FloatParameter * p = addFloatParameter(name, "OSC Control for " + name, initialValue, minVal, maxVal);
        dataProcessor->addOutputData(name, Data::DataType::Number);
        dynamicParameters.add(p);
        osc2DataListeners.call(&DataInNode::Listener::parameterAdded, p);
        return p;
    }

    void removeFloatParamAndData(FloatParameter * p)
    {
        removeControllable(p);
        dataProcessor->removeOutputData(p->niceName);
        osc2DataListeners.call(&DataInNode::Listener::parameterRemoved, p);
    }


    void onContainerParameterChanged(Parameter * p) override;

    virtual NodeBaseUI * createUI() override;

    //Listener
    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void parameterAdded(Parameter *) = 0;
        virtual void parameterRemoved(Parameter *) = 0;

    };

    ListenerList<Listener> osc2DataListeners;
    void addO2DListener(Listener* newListener) { osc2DataListeners.add(newListener); }
    void removeO2DListener(Listener* listener) { osc2DataListeners.remove(listener); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataInNode)
};



#endif  // DATAINNODE_H_INCLUDED
