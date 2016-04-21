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

//predeclaration
class DataProcessorDataListener;

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

		DataProcessor * processor;

		int numConnections;

        OwnedArray<DataElement> elements;

        Data(DataProcessor * processor,String _name, DataType _type) : processor(processor), name(_name), type(_type), numConnections(0)
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

		~Data() {
			elements.clear();
		}

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

		void updateFromSourceData(Data * sourceData)
		{
			bool hasChanged = false;
			int numElements = elements.size();
			for (int i = 0; i < numElements; i++)
			{
				DBG(sourceData->elements[i]->value << "< >" << elements[i]->value);

				if (sourceData->elements[i]->value != elements[i]->value)
				{
					elements[i]->value = sourceData->elements[i]->value;
					hasChanged = true;
				}
			}

			if (hasChanged)
			{
				listeners.call(&DataListener::dataChanged, this);
			}
		}

		void update(const float &value1, const float &value2 = 0, const float &value3 = 0)
		{
			
			bool hasChanged = false;
			int numElements = elements.size();
			const Array<float> values = { value1, value2, value3 };
			
			for (int i = 0; i < numElements; i++)
			{
				DBG(String(elements[i]->value) << " < > " << values[i]);
				if (elements[i]->value != values[i])
				{
					elements[i]->value = values[i];
					hasChanged = true;
				}
			}

			if (hasChanged)
			{
				listeners.call(&DataListener::dataChanged, this);
			}
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
            const static String names[] = { "Unknown", "Float", "Number", "Boolean", "Position", "Orientation", "Color" };
            return names[(int)type];
        }

		class  DataListener
		{
		public:
			/** Destructor. */
			virtual ~DataListener() {}

			virtual void dataChanged(Data *) = 0;
		};

		ListenerList<DataListener> listeners;
		void addDataListener(DataListener* newListener) { listeners.add(newListener); }
		void removeDataListener(DataListener* listener) { listeners.remove(listener); }

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
        Data *d = new Data(this,name,type);
        inputDatas.add(d);

        listeners.call(&DataProcessor::Listener::inputAdded, d);

        return d;
    }

    Data * addOutputData (const String &name, DataType type)
    {
        Data * d = new Data(this,name, type);
        outputDatas.add(d);

        listeners.call(&DataProcessor::Listener::outputAdded, d);

        return d;
    }

	void removeInputData(const String &name)
	{
		Data * d = getInputDataByName(name);
		if (d == nullptr) return;

		listeners.call(&DataProcessor::Listener::inputRemoved, d);
		inputDatas.removeObject(d, true);
	}

	void removeOutputData(const String &name)
	{
		Data * d = getOutputDataByName(name);
		if (d == nullptr) return;
		listeners.call(&DataProcessor::Listener::ouputRemoved, d);
		outputDatas.removeObject(d, true);
	}

	void inputDataChanged(Data *)
	{
		//to be overriden by child classes
	}
	
	virtual void updateOutputData(String &dataName, const float &value1, const float &value2 = 0, const float &value3 = 0)
	{
		Data * d = getOutputDataByName(dataName);
		if (d != nullptr) d->update(value1, value2, value3);
	}


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


	Data * getOutputDataByName(const String &dataName)
	{
		for (auto &d : outputDatas)
		{
			if (d->name == dataName) return d;
		}

		return nullptr;
	}

	Data * getInputDataByName(const String &dataName)
	{
		for (auto &d : inputDatas)
		{
			if (d->name == dataName) return d;
		}

		return nullptr;
	}

    //Listener
    class  Listener
    {
    public:
        /** Destructor. */
        virtual ~Listener() {}

		virtual void inputAdded(Data *) {}
		virtual void inputRemoved(Data *) {}

		virtual void outputAdded(Data *) {}
		virtual void ouputRemoved(Data *) {}

		virtual void inputDataChanged(Data *) {}
    };

    ListenerList<Listener> listeners;
    void addDataProcessorListener(Listener* newListener) { listeners.add(newListener); }
    void removeDataProcessorListener(Listener* listener) { listeners.remove(listener); }


private:
	void processInputDataChanged(Data * d)
	{
		inputDataChanged(d); //for safe override from child classes
		listeners.call(&Listener::inputDataChanged, d);
	}

public :
	//@Martin is this fucked up ? 
	//i can't make DataProcessor inherit from Data::DataListener since the Data class is defined inside the DataProcessor class.

	class ProcessorDataListener : public Data::DataListener, DataProcessor::Listener
	{
	public:
		ProcessorDataListener(DataProcessor * processor) :processor(processor)
		{
			processor->addDataProcessorListener(this);
		}

		DataProcessor * processor;

		// Inherited via DataListener
		virtual void dataChanged(Data * d) override
		{
			processor->processInputDataChanged(d);
		}

		// Inherited via Listener
		virtual void inputAdded(Data *d) override { d->addDataListener(this); }
		virtual void inputRemoved(Data *d) override { d->removeDataListener(this); }

	};


	ProcessorDataListener proxyListener;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataProcessor)
};


#endif  // DATAPROCESSOR_H_INCLUDED
