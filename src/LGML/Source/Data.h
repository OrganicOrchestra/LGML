/*
  ==============================================================================

    Data.h
    Created: 25 Apr 2016 10:03:04am
    Author:  bkupe

  ==============================================================================
*/

#ifndef DATA_H_INCLUDED
#define DATA_H_INCLUDED

#include "JuceHeader.h"//keep
class NodeBase;

class Data
{
public:
    enum DataType
    {
        Unknown, Float, Number, Boolean, Position, Orientation, Color
    };

    class DataElement
    {
    public:
        DataElement(String _name);

        String name;
        DataType type;
        float value;

        bool isTypeCompatible(const DataType &targetType);

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DataElement)
    };



    String name;
    DataType type;

    NodeBase * node;

    int numConnections;

    OwnedArray<DataElement> elements;

    Data(NodeBase * node, String _name, DataType _type);
    ~Data();

    void addElement(const String &_name);

    DataElement * getElement(const String &elementName);

    void updateFromSourceData(Data * sourceData);

    void update(const float &value1, const float &value2 = 0, const float &value3 = 0);

    bool isComplex() { return elements.size() > 1; }

    bool isTypeCompatible(const DataType &targetType);;

    int getNumElementsForType(const DataType &_type);

    String getTypeString();

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


#endif  // DATA_H_INCLUDED
