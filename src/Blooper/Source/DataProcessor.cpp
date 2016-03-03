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

DataProcessor::DataType DataProcessor::getInputDataType(String dataName, String componentName)
{
	for (int i = inputDatas.size(); --i >= 0;)
	{
		Data* d = inputDatas.getUnchecked(i);

		if (d->name == dataName)
		{
			if (componentName.isEmpty())
			{
				return d->type;
			}
			else
			{
				DataComponent * comp = d->getComponent(componentName);
				if (comp == nullptr) return DataType::Unknown;
				return comp->type;
			}
		}
	}

}

DataProcessor::DataType DataProcessor::getOutputDataType(String dataName, String componentName)
{
	for (int i = outputDatas.size(); --i >= 0;)
	{
		Data* d = outputDatas.getUnchecked(i);

		if (d->name == dataName)
		{
			if (componentName.isEmpty())
			{
				return d->type;
			}
			else
			{
				DataComponent * comp = d->getComponent(componentName);
				if (comp == nullptr) return DataType::Unknown;
				return comp->type;
			}
		}
	}
}
