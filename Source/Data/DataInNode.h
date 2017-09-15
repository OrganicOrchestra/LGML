/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#ifndef DATAINNODE_H_INCLUDED
#define DATAINNODE_H_INCLUDED

#include "../Node/NodeBase.h"

class DataInNode :
	public NodeBase
{
public:
    DECLARE_OBJ_TYPE(DataInNode)
    ~DataInNode();

    Array<Parameter *> dynamicParameters;

	FloatParameter * addFloatParamAndData(const String &name, float initialValue, float minVal, float maxVal);
	void removeFloatParamAndData(FloatParameter * p);


    void onContainerParameterChanged(Parameter * p) override;

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
