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

DataProcessor::DataType DataProcessor::getInputDataType(String dataName, String elementName)
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

}

DataProcessor::DataType DataProcessor::getOutputDataType(String dataName, String elementName)
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
}
