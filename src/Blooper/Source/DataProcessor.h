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

			default:
				DBG("Type not exist");
				jassert(false);
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

public:
	DataProcessor();
	virtual ~DataProcessor();

	virtual const String getName() const { return "[Data Processor]"; }

	OwnedArray<Data> inputDatas;
	OwnedArray<Data> outputDatas;

	Data * addInputData(String name,DataType type)
	{
		Data *d = new Data(name,type);
		inputDatas.add(d);

		listeners.call(&DataProcessor::Listener::inputAdded, d);

		return d;
	}

	Data * addOutputData (String name, DataType type)
	{
		Data * d = new Data(name, type);
		outputDatas.add(d);

		listeners.call(&DataProcessor::Listener::outputAdded, d);

		return d;
	}


	virtual void receiveData(const Data * incomingData, String destDataName, String destElementName = "", String sourceElementName = "") = 0;
	virtual void sendData(const Data * outgoingData, String sourceElementName = "") = 0;

	int getTotalNumInputData() const { return inputDatas.size(); }
	int getTotalNumOutputData() const { return outputDatas.size(); }


	DataType getInputDataType(String dataName, String elementName);
	DataType getOutputDataType(String dataName, String elementName);

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

	void addListener(Listener* newListener);

	/** Removes a previously-registered button listener
	@see addListener
	*/
	void removeListener(Listener* listener);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataProcessor)
};

#endif  // DATAPROCESSOR_H_INCLUDED
