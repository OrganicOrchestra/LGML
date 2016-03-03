/*
  ==============================================================================

    DataProcessor.h
    Created: 3 Mar 2016 1:53:08pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef DATAPROCESSOR_H_INCLUDED
#define DATAPROCESSOR_H_INCLUDED


#include <JuceHeader.h>

class DataProcessor
{

protected:
	DataProcessor();

public :
	enum DataType
	{
		Unknown,Number,Boolean,Position,Orientation,Color
	};

	class DataComponent
	{
	public:
		DataComponent(String _name) : name(_name){}

		String name;
		DataType type;
		float value;
	};

	class Data
	{
	public:
		Data(String _name, DataType _type) : name(_name), type(_type)
		{
			switch (type)
			{
			case Number:
			case Boolean:
				addComponent("value");
				break;

			case Position:
				addComponent("x");
				addComponent("y");
				addComponent("z");
				break;

			case Orientation:
				addComponent("Yaw");
				addComponent("Pitch");
				addComponent("Roll");
				break;

			case Color:
				addComponent("Red");
				addComponent("Green");
				addComponent("Blue");
				break;
			}
		};

		String name;
		DataType type;

		void addComponent(String name) {
			DataComponent *c = new DataComponent(name);
			components.add(c);
		}

		DataComponent * getComponent(String componentName)
		{
			for (int i = components.size(); --i >= 0;)
			{
				DataComponent * c = components.getUnchecked(i);
				if (c->name == componentName) return c;
			}

			return nullptr;
		}

		OwnedArray<DataComponent> components;
		bool isComplex() { return components.size() > 1; }
	};

	
	virtual const String getName() const { return "[Data Processor]"; }

	OwnedArray<Data> inputDatas;
	OwnedArray<Data> outputDatas;

	Data * addInputData(String name,DataType type)
	{
		Data *d = new Data(name,type);
		inputDatas.add(d);
		return d;
	}

	Data * addOutputData (String name, DataType type)
	{
		Data * d = new Data(name, type);
		outputDatas.add(d);
		return d;
	}

	virtual void processData(Data * incomingData, String targetInputDataName,String targetDataComponentName) = 0;

	int getTotalNumInputData() const { return inputDatas.size(); }
	int getTotalNumOutputData() const { return outputDatas.size(); }


	DataType getInputDataType(String dataName, String componentName);
	DataType getOutputDataType(String dataName, String componentName);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataProcessor)
};

#endif  // DATAPROCESSOR_H_INCLUDED
