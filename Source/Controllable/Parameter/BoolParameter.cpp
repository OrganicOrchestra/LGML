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

#include "BoolParameter.h"
#include "ParameterFactory.h"
REGISTER_PARAM_TYPE(BoolParameter)

BoolParameter::BoolParameter(const String &niceName, const String &description, bool initialValue, bool enabled) : ParameterBase(niceName, description, initialValue, enabled),
                                                                                                                   invertVisuals(false)
{
    setValue(initialValue);
}
