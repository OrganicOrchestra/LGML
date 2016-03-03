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

	class DataElement
	{
	public:
		DataElement(String _name) : name(_name),type(Number) {}

		String name;
		DataType type;
		float value;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataElement)
	};

	class Data
	{
	public:
		String name;
		DataType type;

		OwnedArray<DataElement> elements;

		Data(String _name, DataType _type) : name(_name), type(_type)
		{
			switch (type)
			{
			case Number:
			case Boolean:
				addElement("value");
				break;

			case Position:
				addElement("x");
				addElement("y");
				addElement("z");
				break;

			case Orientation:
				addElement("Yaw");
				addElement("Pitch");
				addElement("Roll");
				break;

			case Color:
				addElement("Red");
				addElement("Green");
				addElement("Blue");
				break;
			}
		};

		void addElement(String name) {
			DataElement *e = new DataElement(name);
			elements.add(e);
		}

		DataElement * getElement(String elementName)
		{
			for (int i = elements.size(); --i >= 0;)
			{
				DataElement * e = elements.getUnchecked(i);
				if (e->name == elementName) return e;
			}

			return nullptr;
		}

		
		bool isComplex() { return elements.size() > 1; }

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Data)
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


	virtual void receiveData(const Data * incomingData, String destDataName, String destElementName = "", String sourceElementName = "") = 0;
	virtual void sendData(const Data * outgoingData, String sourceElementName = "") = 0;

	int getTotalNumInputData() const { return inputDatas.size(); }
	int getTotalNumOutputData() const { return outputDatas.size(); }


	DataType getInputDataType(String dataName, String elementName);
	DataType getOutputDataType(String dataName, String elementName);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataProcessor)
};

#endif  // DATAPROCESSOR_H_INCLUDED
