/*
  ==============================================================================

    Point2DParameter.h
    Created: 30 Sep 2016 9:37:36am
    Author:  bkupe

  ==============================================================================
*/

#ifndef POINT2DPARAMETER_H_INCLUDED
#define POINT2DPARAMETER_H_INCLUDED

#include "Parameter.h"

template<typename T>
class Point2DParameter : public Parameter
{
public:
  Point2DParameter(const String &niceName, const String &description, T x= T(0), T y = T(0),var minPoint=var::undefined(),var maxPoint=var::undefined(), bool enabled = true);
	~Point2DParameter() {}

  


	void setPoint(const Point<T> & value);
	void setPoint(const T x, const T y);
	void setValueInternal(var & _value) override;
	
	Point<T> getPoint();
	
	bool checkValueIsTheSame(var newValue, var oldValue) override;
  T getX(){return (T)value[0];}
  T getY(){return (T)value[1];}
private:

	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Point2DParameter)
};


template<typename T>
Point2DParameter<T>::Point2DParameter(const String & niceName, const String & description, T x, T y ,var minPoint,var maxPoint, bool enabled) :
Parameter(POINT2D, niceName, description,Array<var>{x,y},minPoint,maxPoint,enabled)
{
  if(!minimumValue.isUndefined() && ! minimumValue.isArray()){
    minimumValue = var::undefined();
    jassertfalse;
  }
  if(!maximumValue.isUndefined() && ! maximumValue.isArray()){
    maximumValue = var::undefined();
    jassertfalse;
  }
  hideInEditor = true;
  setPoint(x, y);
}

template<typename T>
void Point2DParameter<T>::setPoint(const Point<T> & _value)
{
  setPoint(_value.x, _value.y);
}

template<typename T>
void Point2DParameter<T>::setPoint(const T _x, const T _y)
{
  var d;
  d.append(_x);
  d.append(_y);
  setValue(d);
}

template<typename T>
void Point2DParameter<T>::setValueInternal(var & _value)
{

  if (!_value.isArray() || _value.size()!=2){
    jassertfalse;
    return;
  }
  if(!minimumValue.isUndefined()){
    _value.getArray()->set(0, jmax(static_cast<T>(minimumValue[0]),static_cast<T>(_value[0])));
    _value.getArray()->set(1, jmax(static_cast<T>(minimumValue[1]),static_cast<T>(_value[1])));
  }
  if(!maximumValue.isUndefined()){
    _value.getArray()->set(0, jmin(static_cast<T>(maximumValue[0]),static_cast<T>(_value[0])));
    _value.getArray()->set(1, jmin(static_cast<T>(maximumValue[1]),static_cast<T>(_value[1])));
  }
  Parameter::setValueInternal(_value);


}
template<typename T>
Point<T> Point2DParameter<T>::getPoint() {
  return Point<T>(getX(), getY());
}

template<typename T>
bool Point2DParameter<T>::checkValueIsTheSame(var newValue, var oldValue)
{
  if (!(newValue.isArray() && oldValue.isArray())) return false;
  return newValue[0] == oldValue[0] && newValue[1] == oldValue[1];
}



#endif  // POINT2DPARAMETER_H_INCLUDED
