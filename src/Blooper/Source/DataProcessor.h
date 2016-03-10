/*
  ==============================================================================

    DataProcessor.h
    Created: 3 Mar 2016 1:53:08pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef DATAPROCESSOR_H_INCLUDED
#define DATAPROCESSOR_H_INCLUDED
/*

This class is a base class for all data processors :
	A data Processor has to override receiveData and sendData
	Data class is an Array of DataElement(typed data) 


*/

#include "JuceHeader.h"

class DataProcessor
{

public :
	enum DataType
	{
		Unknown, Float, Number,Boolean,Position,Orientation,Color
	};

	class DataElement
	{
	public:
		DataElement(String _name) : name(_name),type(Float) {}

		String name;
		DataType type;
		float value;

		bool isTypeCompatible(const DataType &targetType) {
			return targetType == Float || targetType == Unknown;
		}

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

			default:
				DBG("Type not exist for data");
				jassert(false);
				break;
			}
		};

		void addElement(const String &_name) {
			DataElement *e = new DataElement(_name);
			elements.add(e);
		}

		DataElement * getElement(const String &elementName)
		{
			for (int i = elements.size(); --i >= 0;)
			{
				DataElement * e = elements.getUnchecked(i);
				if (e->name == elementName) return e;
			}

			return nullptr;
		}

		
		bool isComplex() { return elements.size() > 1; }

		bool isTypeCompatible(const DataType &targetType) {

			return targetType == DataType::Unknown || type == targetType || getNumElementsForType(type) == getNumElementsForType(targetType);
		};

		int getNumElementsForType(const DataType &_type)
		{
			switch (_type)
			{
			case Number:
			case Boolean:
				return 1;
				break;

			case Position:
			case Orientation:
			case Color:
				return 3;
				break;

			default:
				DBG("Type not exist for data");
				return 0;
				break;
			}
		}

		String getTypeString()
		{
			Array<String> names = { "Unknown", "Float", "Number", "Boolean", "Position", "Orientation", "Color" };
			return names[(int)type];
		}

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Data)
	};

public:
	DataProcessor();
	virtual ~DataProcessor();

	virtual const String getName() const { return "[Data Processor]"; }

	OwnedArray<Data> inputDatas;
	OwnedArray<Data> outputDatas;

	Data * addInputData(const String &name,DataType type)
	{
		Data *d = new Data(name,type);
		inputDatas.add(d);

		listeners.call(&DataProcessor::Listener::inputAdded, d);

		return d;
	}

	Data * addOutputData (const String &name, DataType type)
	{
		Data * d = new Data(name, type);
		outputDatas.add(d);

		listeners.call(&DataProcessor::Listener::outputAdded, d);

		return d;
	}


	virtual void receiveData(const Data * incomingData, const String &destDataName, const String &destElementName = "", const String &sourceElementName = "") = 0;
	virtual void sendData(const Data * outgoingData, const String &sourceElementName = "") = 0;

	int getTotalNumInputData() const { return inputDatas.size(); }
	int getTotalNumOutputData() const { return outputDatas.size(); }

	StringArray getInputDataInfos()
	{
		StringArray dataInfos;
		for (auto &d : inputDatas) dataInfos.add(d->name + " ("+d->getTypeString()+")");
		return dataInfos;
	}

	StringArray getOutputDataInfos()
	{
		StringArray dataInfos;
		for (auto &d : outputDatas) dataInfos.add(d->name + " (" + d->getTypeString() + ")");
		return dataInfos;
	}

	DataType getInputDataType(const String &dataName, const String &elementName);
	DataType getOutputDataType(const String &dataName, const String &elementName);

	//Listener
	class  Listener
	{
	public:
		/** Destructor. */
		virtual ~Listener() {}

		virtual void inputAdded(Data *) = 0;
		virtual void inputRemoved(Data *) = 0;

		virtual void outputAdded(Data *) = 0;
		virtual void ouputRemoved(Data *) = 0;
	};

	ListenerList<Listener> listeners;
	void addDataProcessorListener(Listener* newListener) { listeners.add(newListener); }
	void removeDataProcessorListener(Listener* listener) { listeners.remove(listener); }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataProcessor)
};

#endif  // DATAPROCESSOR_H_INCLUDED
