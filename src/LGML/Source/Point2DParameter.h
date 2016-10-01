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

class Point2DParameter : public Parameter
{
public:
	Point2DParameter(const String &niceName, const String &description, bool enabled = true);
	~Point2DParameter() {}

	float x, y;
	void setPoint(Point<float> value);
	void setPoint(float x, float y);
	void setValueInternal(var & _value) override;
	
	Point<float> getPoint();
	
	bool checkValueIsTheSame(var newValue, var oldValue) override;

	ControllableUI * createDefaultUI(Controllable * /*targetControllable*/ = nullptr) override { return nullptr; }
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Point2DParameter)
};


#endif  // POINT2DPARAMETER_H_INCLUDED
