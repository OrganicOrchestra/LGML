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


#ifndef BOOLPARAMETER_H_INCLUDED
#define BOOLPARAMETER_H_INCLUDED

#include "Parameter.h"



class BoolParameter : public ParameterBase
{
public:

    explicit BoolParameter (const String& niceName, const String& description = "", bool initialValue = false, bool enabled = true);
    ~BoolParameter() {}



    bool invertVisuals; //moved here for coherence when ui is generated automatically
    DECLARE_OBJ_TYPE (BoolParameter,"Boolean Parameter");
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BoolParameter)
};



#endif  // BOOLPARAMETER_H_INCLUDED
