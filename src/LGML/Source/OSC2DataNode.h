/*
  ==============================================================================

    OSC2DataNode.h
    Created: 19 Apr 2016 10:38:42am
    Author:  bkupe

  ==============================================================================
*/

#ifndef OSC2DATANODE_H_INCLUDED
#define OSC2DATANODE_H_INCLUDED

#include "NodeBase.h"

class OSC2DataNode : public NodeBase
{
public:
    class OSC2DataNodeDataProcessor : public NodeBase::NodeDataProcessor
    {
    public:
        OSC2DataNodeDataProcessor() :NodeBase::NodeDataProcessor() {

        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSC2DataNodeDataProcessor)
    };


    OSC2DataNode(NodeManager * nodeManager, uint32 nodeId);
    ~OSC2DataNode();

    Array<Parameter *> dynamicParameters;

    FloatParameter * addFloatParamAndData(const String &name, float initialValue, float minVal, float maxVal)
    {
        FloatParameter * p = addFloatParameter(name, "OSC Control for " + name, initialValue, minVal, maxVal);
        dataProcessor->addOutputData(name, DataProcessor::DataType::Number);
        dynamicParameters.add(p);
        osc2DataListeners.call(&OSC2DataNode::Listener::parameterAdded, p);
        return p;
    }

    void removeFloatParamAndData(FloatParameter * p)
    {
        removeControllable(p);
        dataProcessor->removeOutputData(p->niceName);
        osc2DataListeners.call(&OSC2DataNode::Listener::parameterRemoved, p);
    }


    void parameterValueChanged(Parameter * p) override;

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSC2DataNode)
};



#endif  // OSC2DATANODE_H_INCLUDED
