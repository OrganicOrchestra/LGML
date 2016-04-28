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


#include "Data.h"



class DataProcessor : public Data::DataListener
{


public:
    typedef Data::DataType DataType;
    typedef Data::DataElement DataElement;


    DataProcessor();
    virtual ~DataProcessor();

    virtual const String getName() const { return "[Data Processor]"; }

    OwnedArray<Data> inputDatas;
    OwnedArray<Data> outputDatas;

    Data * addInputData(const String &name, DataType type);
    Data * addOutputData(const String &name, DataType type);

    void removeInputData(const String &name);
    void removeOutputData(const String &name);

    void inputDataChanged(Data *)
    {
        //to be overriden by child classes
    }

    virtual void updateOutputData(String &dataName, const float &value1, const float &value2 = 0, const float &value3 = 0);


    int getTotalNumInputData() const { return inputDatas.size(); }
    int getTotalNumOutputData() const { return outputDatas.size(); }

    StringArray getInputDataInfos();

    StringArray getOutputDataInfos();

    Data::DataType getInputDataType(const String &dataName, const String &elementName);
    Data::DataType getOutputDataType(const String &dataName, const String &elementName);


    Data * getOutputDataByName(const String &dataName);
    Data * getInputDataByName(const String &dataName);

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
    virtual void dataChanged(Data *) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataProcessor)


};


#endif  // DATAPROCESSOR_H_INCLUDED
