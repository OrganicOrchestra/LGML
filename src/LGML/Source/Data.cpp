/*
  ==============================================================================

    Data.cpp
    Created: 25 Apr 2016 10:03:04am
    Author:  bkupe

  ==============================================================================
*/

#include "Data.h"

Data::Data(NodeBase * _node, String _name, DataType _type, IOType _ioType) :
	node(_node), name(_name), type(_type), numConnections(0), ioType(_ioType)
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
}

Data::~Data() {
    elements.clear();
}

void Data::addElement(const String & _name) {
    DataElement *e = new DataElement(_name);
    elements.add(e);
}

Data::DataElement * Data::getElement(const String & elementName)
{
    for (int i = elements.size(); --i >= 0;)
    {
        DataElement * e = elements.getUnchecked(i);
        if (e->name == elementName) return e;
    }

    return nullptr;
}

void Data::updateFromSourceData(Data * sourceData)
{
    bool hasChanged = false;
    int numElements = elements.size();
    for (int i = 0; i < numElements; i++)
    {

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

void Data::update(const float & value1, const float & value2, const float & value3)
{

    bool hasChanged = false;
    int numElements = elements.size();


    // const Array<float> values = { value1, value2, value3 };
    // above not compiling with c++ 98 / libstdc / Xcode
    Array<float> values;
    values.resize(3); values.set(0, value1); values.set(1, value2); values.set(2, value3);

    for (int i = 0; i < numElements; i++)
    {
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

bool Data::isTypeCompatible(const DataType & targetType) {

    return targetType == DataType::Unknown || type == targetType || getNumElementsForType(type) == getNumElementsForType(targetType);
}

int Data::getNumElementsForType(const DataType & _type)
{
    switch (_type)
    {
    case Number:
    case Boolean:
        return 1;
        

    case Position:
    case Orientation:
    case Color:
        return 3;


    default:
        DBG("Type not exist for data");
        return 0;

    }
}

String Data::getTypeString()
{
    const static String names[] = { "Unknown", "Float", "Number", "Boolean", "Position", "Orientation", "Color" };
    return names[(int)type];
}

Data::DataElement::DataElement(String _name) : name(_name), type(Float) {}


bool Data::DataElement::isTypeCompatible(const DataType & targetType) {
    return targetType == Float || targetType == Unknown;
}
