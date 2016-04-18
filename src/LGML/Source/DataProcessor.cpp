/*
 ==============================================================================

 DataProcessor.cpp
 Created: 3 Mar 2016 1:53:08pm
 Author:  bkupe

 ==============================================================================
 */

#include "DataProcessor.h"

DataProcessor::DataProcessor()
{

}

DataProcessor::~DataProcessor()
{
	inputDatas.clear();
	outputDatas.clear();
}

DataProcessor::DataType DataProcessor::getInputDataType(const String &dataName, const String &elementName)
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

DataProcessor::DataType DataProcessor::getOutputDataType(const String &dataName, const String &elementName)
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
