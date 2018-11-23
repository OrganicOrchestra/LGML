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


#include "ParameterList.h"


#include "ParameterFactory.h"

REGISTER_PARAM_TYPE_TEMPLATED (Point2DParameter, int,"Point2DInt")
REGISTER_PARAM_TYPE_TEMPLATED (Point2DParameter, floatParamType,"Point2DFloat")



template<typename T>
ParameterList<T>::ParameterList (const String& niceName, const String& description,
                                 var startPoint,
                                 var minPoint, var maxPoint,
                                 bool enabled) :
MinMaxParameter ( niceName, description, startPoint, minPoint, maxPoint, enabled)
{
    if(!startPoint.getArray()){
        jassertfalse;
    }
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

    updateParamsFromValue();

    setMinMax(minimumValue, maximumValue,this);
    jassert(value.size()==size());
    //    isHidenInEditor = true;

}

template <class T>
void ParameterList<T>::updateParamsFromValue() {
    auto a  = value.getArray();
    int targetSize = a?a->size():0;

    // avoid shrinking for now
    jassert(paramsList.size()<=targetSize);
    for(int i = paramsList.size();  i <targetSize;i++){
        auto np = new NumericParameter<T>(String(i),"param "+String(i),a->getUnchecked(i));
        paramsList.add(np);
        np->setParentContainer(parentContainer);
        np->addParameterListener(this);
    }
    for(int i = paramsList.size();  i >targetSize;i--){

        auto np = paramsList.getLast();
        np->removeParameterListener(this);
        paramsList.removeLast();
    }
}


template<typename T>
void ParameterList<T>::parameterValueChanged ( ParameterBase* p,ParameterBase::Listener * notifier) {
    if(notifier==this)return;
    if(!value.isArray()){jassertfalse; return;}
    if(paramsList.contains((NumericParameter<T>*)p)){
        int idx = paramsList.indexOf((NumericParameter<T>*)p);
        var nv = value.clone();
        nv.getArray()->set(idx, (T)p->value.clone());
        setValueFrom(notifier,nv);
    }

}

template<typename T>
void ParameterList<T>::setMinMax (var min, var max,ParameterBase::Listener * notifier) {

    var minX = var::undefined();

    var maxX = var::undefined();
    if(min.isDouble() || min.isInt64() || min.isInt()){
        minX = min;
    }
    if(max.isDouble() || max.isInt64() || max.isInt()){
        maxX = max;
    }
    for(int i = 0; i < size() ; i++){
        if(auto a = min.getArray()){
            if(i<a->size()){
                minX = (T)a->getUnchecked(i);
            }
        }

        if(auto a = max.getArray()){
            if(i<a->size()){
                maxX =  (T)a->getUnchecked(i);
            }
        }
        paramsList.getUnchecked(i)->setMinMax(minX,maxX,notifier);
    }
    MinMaxParameter::setMinMax(min,max);

}

template<typename T>
bool ParameterList<T>::checkValueIsTheSame (const var& v1, const var& v2)
{
    if (! (v1.isArray() && v2.isArray())) return false;
    if(v1.getArray()->size() != v2.getArray()->size()) return false;
    for(int i = 0 ; i < v1.getArray()->size() ;i++){
        if(v1[i]!=v2[i])return false;
    }
    return true;
}


template<typename T>
var ParameterList<T>::getMinForIdx(int i){
    if(auto a = minimumValue.getArray()){
        if(i<a->size())
            return a->getUnchecked(i);
        return var::undefined();
    }
    if(minimumValue.isInt() || minimumValue.isDouble() || minimumValue.isInt64()){
        return (T)minimumValue;
    }
    return var::undefined();

}
template<typename T>
var ParameterList<T>::getMaxForIdx(int i){
    if(auto a = maximumValue.getArray()){
        if(i<a->size())
            return a->getUnchecked(i);
        return var::undefined();
    }
    if(maximumValue.isInt() || maximumValue.isDouble() || maximumValue.isInt64()){
        return (T)maximumValue;
    }
    return var::undefined();
}

template<typename T>
void ParameterList<T>::setValueInternal (const var& _value)
{

    if (!_value.isArray())
    {
        jassertfalse;
        return;
    }
    if( _value.size() != size()){
        jassertfalse;
        return ;
    }

    for(int i = 0 ;i < size() ; i++){

        var min = getMinForIdx(i);
        var max = getMaxForIdx(i);
        if (!min.isUndefined())
        {
            _value.getArray()->set (i, jmax (static_cast<T> (min), static_cast<T> (_value[i])));
        }

        if (!max.isUndefined())
        {
            _value.getArray()->set (i, jmin (static_cast<T> (max), static_cast<T> (_value[i])));

        }
        paramsList.getUnchecked(i)->setValueFrom(this,_value.getArray()->getReference(i),false,true);
    }
    ParameterBase::setValueInternal (_value);

    jassert(value.size()==size());
}


/////////////
// Point2D

template<typename T>
Point2DParameter<T>::Point2DParameter (const String& niceName, const String& description,
                                       T x, T y,
                                       var minPoint, var maxPoint,
                                       bool enabled) :
ParameterList<T>( niceName, description, Array<var> {x, y}, minPoint, maxPoint, enabled)
{

    ParameterList<T>::paramsList.getUnchecked(0)->setNiceName("x");
    ParameterList<T>::paramsList.getUnchecked(1)->setNiceName("y");


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
    ParameterList<T>::setValueFrom (notifier,d,false,forceIt);
}

template<typename T>
T Point2DParameter<T>::getX() const {return (T)ParameterList<T>::value[0];}

template<typename T>
T Point2DParameter<T>::getY() const {return (T)ParameterList<T>::value[1];}

template<typename T>
NumericParameter<T>* Point2DParameter<T>::getXParam()  {return ParameterList<T>::paramsList[0];}

template<typename T>
NumericParameter<T>* Point2DParameter<T>::getYParam()  {return ParameterList<T>::paramsList[1];}



template<typename T>
Point<T> Point2DParameter<T>::getPoint() const
{
    return Point<T> (getX(), getY());
}




template class Point2DParameter<int>;
template class Point2DParameter<floatParamType>;
