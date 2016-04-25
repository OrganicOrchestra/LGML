/*
 ==============================================================================

 DataProcessor.cpp
 Created: 3 Mar 2016 1:53:08pm
 Author:  bkupe

 ==============================================================================
 */

#include "DataProcessor.h"

typedef Data::DataType DataType;

DataProcessor::DataProcessor()
{
	
}

DataProcessor::~DataProcessor()
{
    inputDatas.clear();
    outputDatas.clear();
}

Data * DataProcessor::addInputData(const String & name, Data::DataType type)
{
	Data *d = new Data(this, name, type);
	inputDatas.add(d);

	d->addDataListener(this);

	listeners.call(&DataProcessor::Listener::inputAdded, d);

	return d;
}

Data * DataProcessor::addOutputData(const String & name, DataType type)
{
	Data * d = new Data(this, name, type);
	outputDatas.add(d);

	listeners.call(&DataProcessor::Listener::outputAdded, d);

	return d;
}

void DataProcessor::removeInputData(const String & name)
{
	Data * d = getInputDataByName(name);
	if (d == nullptr) return;

	listeners.call(&DataProcessor::Listener::inputRemoved, d);
	inputDatas.removeObject(d, true);
}

void DataProcessor::removeOutputData(const String & name)
{
	Data * d = getOutputDataByName(name);
	if (d == nullptr) return;
	listeners.call(&DataProcessor::Listener::ouputRemoved, d);
	outputDatas.removeObject(d, true);
}

void DataProcessor::updateOutputData(String & dataName, const float & value1, const float & value2, const float & value3)
{
	Data * d = getOutputDataByName(dataName);
	if (d != nullptr) d->update(value1, value2, value3);
}
 
StringArray DataProcessor::getInputDataInfos()
{
	StringArray dataInfos;
	for (auto &d : inputDatas) dataInfos.add(d->name + " (" + d->getTypeString() + ")");
	return dataInfos;
}

StringArray DataProcessor::getOutputDataInfos()
{
	StringArray dataInfos;
	for (auto &d : outputDatas) dataInfos.add(d->name + " (" + d->getTypeString() + ")");
	return dataInfos;
}

DataType DataProcessor::getInputDataType(const String &dataName, const String &elementName)
{
    for (int i = inputDatas.size(); --i >= 0;)
    {
        Data* d = inputDatas.getUnchecked(i);

        if (d->name == dataName)
        {
            if (elementName.isEmpty())
            {
                return d->type;
            }
            else
            {
                DataElement * e = d->getElement(elementName);
                if (e == nullptr) return DataType::Unknown;
                return e->type;
            }
        }
    }

    return DataType::Unknown;
}

DataType DataProcessor::getOutputDataType(const String &dataName, const String &elementName)
{
    for (int i = outputDatas.size(); --i >= 0;)
    {
        Data* d = outputDatas.getUnchecked(i);

        if (d->name == dataName)
        {
            if (elementName.isEmpty())
            {
                return d->type;
            }
            else
            {
                DataElement * e = d->getElement(elementName);
                if (e == nullptr) return DataType::Unknown;
                return e->type;
            }
        }
    }

    return DataType::Unknown;

}

Data * DataProcessor::getOutputDataByName(const String & dataName)
{
	for (auto &d : outputDatas)
	{
		if (d->name == dataName) return d;
	}

	return nullptr;
}

Data * DataProcessor::getInputDataByName(const String & dataName)
{
	for (auto &d : inputDatas)
	{
		if (d->name == dataName) return d;
	}

	return nullptr;
}

void DataProcessor::dataChanged(Data * d)
{
	listeners.call(&DataProcessor::Listener::inputDataChanged, d);
}
