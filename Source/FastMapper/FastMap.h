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


#ifndef FASTMAP_H_INCLUDED
#define FASTMAP_H_INCLUDED

#include "../Controllable/Parameter/ParameterContainer.h"
#include "../Controllable/Parameter/ParameterProxy.h"

class FastMap;


class FastMap :
public ParameterContainer,
public ParameterProxy::ParameterProxyListener
{
public:
  FastMap();
  virtual ~FastMap();

  BoolParameter * enabledParam;
  BoolParameter * fullSync;

  RangeParameter * inputRange;
  RangeParameter * outputRange;

  BoolParameter * invertParam;

  ParameterProxy * referenceIn;
  ParameterProxy *  referenceOut;


private:
  void onContainerParameterChanged(Parameter *)override;

  bool isInRange; //memory for triggering
  bool fastMapIsProcessing;

  void process(bool toReferenceOut=true);
  // inherited from proxy listener
  void linkedParamValueChanged(ParameterProxy *) override;
  void linkedParamChanged(ParameterProxy *) override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FastMap);
};


#endif  // FASTMAP_H_INCLUDED
