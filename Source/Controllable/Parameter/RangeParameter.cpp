/* Copyright © Organic Orchestra, 2017
*
* This file is part of LGML.  LGML is a software to manipulate sound in realtime
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation (version 3 of the License).
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
*/


#include "RangeParameter.h"


#include "ParameterFactory.h"
REGISTER_PARAM_TYPE (RangeParameter);

RangeParameter::RangeParameter (const String& niceName, const String& description, float _min, float _max, float init_min, float init_max, bool enabled) :
    MinMaxParameter ( niceName, description, 0, 0, 1, enabled)
{
    //  isHidenInEditor = true;
    value .append (0);
    value .append (1);
    defaultValue = var();
    defaultValue.append (init_min);
    defaultValue.append (init_max);
    setMinMax (_min, _max);
    setValue (init_min, init_max);

}

//void RangeParameter::setValue(Point<double> _value)
//{
//  setValue(_value.x, _value.y);
//}

void RangeParameter::setValue (double _x, double _y)
{
    var d;
    d.append (_x);
    d.append (_y);
    ParameterBase::setValue (d);
}

void RangeParameter::setValueInternal (const var& _value)
{

    if (!_value.isArray())
    {
        jassertfalse;
        return;
    }
    if(minimumValue.isUndefined() && maximumValue.isUndefined()){
        value[0] = _value[0];
        value[1] = _value[1];
    }
    else if(minimumValue.isUndefined()){
        value[0] = jmin(maximumValue,_value[0]);
        value[1] = jmin(maximumValue,_value[1]);
    }
    else if (maximumValue.isUndefined()){
        value[0] = jmax(minimumValue,_value[0]);
        value[1] = jmax(minimumValue,_value[1]);
    }
    else{
    value[0] = jlimit<float> (minimumValue, maximumValue, _value[0]);
    value[1] = jlimit<float> (minimumValue, maximumValue, _value[1]);
    }
}

//Point<double> RangeParameter::getRangeMinMax() {
//  return Point<double>(getRangeMin(), getRangeMax());
//}
float RangeParameter::getRangeMin()
{
    return value[0];
}
float RangeParameter::getRangeMax()
{
    return value[1];
}
bool RangeParameter::checkValueIsTheSame (const var& v1, const var& v2)
{
    if (! (v1.isArray() && v2.isArray())) return false;
    if (v1.getArray()==v2.getArray())
        return true;
    return (v1[0] == v2[0]) && (v1[1] == v2[1]);
}
bool RangeParameter::hasFiniteRange(){
    return ! (minimumValue.isUndefined() || maximumValue.isUndefined());
}

float RangeParameter::getNormalizedValue(float v){
    if(!hasFiniteRange() ){
        jassertfalse;
        return v;
    }
    else{
        return jmap<float>(v,(float)minimumValue,(float)maximumValue,0,1);
    }

}

float RangeParameter::getNormalizedRangeMin(){
    return getNormalizedValue(getRangeMin());
}
float RangeParameter::getNormalizedRangeMax(){
    return getNormalizedValue(getRangeMax());

}

void RangeParameter::setNormalizedRangeMinMax(float f,float fm){
    if(hasFiniteRange())
        setValue( jmap<float>(f,0.f,1.f,(float)minimumValue,(float)maximumValue),
                  jmap<float>(fm,0.f,1.f,(float)minimumValue,(float)maximumValue));
    else
        jassertfalse;
}

