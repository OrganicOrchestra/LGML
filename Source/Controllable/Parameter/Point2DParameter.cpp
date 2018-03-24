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


#include "Point2DParameter.h"


#include "ParameterFactory.h"

REGISTER_PARAM_TYPE_TEMPLATED (Point2DParameter, int,"Point2DInt")
REGISTER_PARAM_TYPE_TEMPLATED (Point2DParameter, floatParamType,"Point2DFloat")



template<typename T>
Point2DParameter<T>::Point2DParameter (const String& niceName, const String& description,
                                       T x, T y,
                                       var minPoint, var maxPoint,
                                       bool enabled) :
    MinMaxParameter ( niceName, description, Array<var> {x, y}, minPoint, maxPoint, enabled)
{
    if (!minimumValue.isUndefined() && ! minimumValue.isArray())
    {
        minimumValue = var::undefined();
        jassertfalse;
    }

    if (!maximumValue.isUndefined() && ! maximumValue.isArray())
    {
        maximumValue = var::undefined();
        jassertfalse;
    }
    xParam = new NumericParameter<T>("x","x param of point");
    xParam->setParentContainer(parentContainer);
    xParam->addParameterListener(this);
    yParam = new NumericParameter<T>("y","y param of point");
    yParam->setParentContainer(parentContainer);
    yParam->addParameterListener(this);
    setMinMax(minimumValue, maximumValue,this);
//    isHidenInEditor = true;
    setPoint (x, y);
}

template<typename T>
void Point2DParameter<T>::setPoint (const Point<T>& _value,ParameterBase::Listener * notifier)
{
    setPoint (_value.x, _value.y,notifier);
}

template<typename T>
void Point2DParameter<T>::setPoint (const T _x, const T _y,ParameterBase::Listener * notifier)
{
    var d;
    d.append (_x);
    d.append (_y);
    bool forceIt = notifier==this;
    setValueFrom (notifier,d,false,forceIt);
}

template<typename T>
void Point2DParameter<T>::setValueInternal (const var& _value)
{

    if (!_value.isArray() || _value.size() != 2)
    {
        jassertfalse;
        return;
    }

    if (!minimumValue.isUndefined())
    {
        _value.getArray()->set (0, jmax (static_cast<T> (minimumValue[0]), static_cast<T> (_value[0])));
        _value.getArray()->set (1, jmax (static_cast<T> (minimumValue[1]), static_cast<T> (_value[1])));
    }

    if (!maximumValue.isUndefined())
    {
        _value.getArray()->set (0, jmin (static_cast<T> (maximumValue[0]), static_cast<T> (_value[0])));
        _value.getArray()->set (1, jmin (static_cast<T> (maximumValue[1]), static_cast<T> (_value[1])));
    }
    xParam->setValueFrom(this,_value.getArray()->getReference(0),false,true);
    yParam->setValueFrom(this,_value.getArray()->getReference(1),false,true);
    ParameterBase::setValueInternal (_value);


}

template<typename T>
void Point2DParameter<T>::parameterValueChanged ( ParameterBase* p,ParameterBase::Listener * notifier) {
    if(notifier==this)return;
    if(p==xParam || p == yParam){
        setPoint(xParam->floatValue(),yParam->floatValue(),this);
    }

}
template<typename T>
Point<T> Point2DParameter<T>::getPoint() const
{
    return Point<T> (getX(), getY());
}

template<typename T>
bool Point2DParameter<T>::checkValueIsTheSame (const var& v1, const var& v2)
{
    if (! (v1.isArray() && v2.isArray())) return false;

    return v1[0] == v2[0] && v1[1] == v2[1];
}

template<typename T>
void Point2DParameter<T>::setMinMax (var min, var max,ParameterBase::Listener * notifier) {

    var minX = var::undefined();
    var minY = var::undefined();
    var maxX = var::undefined();
    var maxY = var::undefined();
    if(auto a = min.getArray()){
        if(a->size()==2){
        minX = (T)a->getUnchecked(0);
        minY = (T)a->getUnchecked(1);
        }
    }
    if(auto a = max.getArray()){
        if(a->size()==2){
            maxX =  (T)a->getUnchecked(0);
            maxY =  (T)a->getUnchecked(1);
        }
    }

    xParam->setMinMax(minX,maxX,notifier);
    yParam->setMinMax(minY,maxY,notifier);
    MinMaxParameter::setMinMax(min,max);

}

template class Point2DParameter<int>;
template class Point2DParameter<floatParamType>;
