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

#include "ParameterFactory.h"
#include "ParameterContainer.h"

ParameterBase *ParameterFactory::createBaseFromVar(StringRef n, const var &v)
{
    if (v.isDouble())
    {
        return createFromTypeID(Identifier("t_NumericParameter_floatParamType"), n);
    }
    else if (v.isInt())
    {
        return createFromTypeID(Identifier("t_NumericParameter_int"), n);
    }
    else if (v.isBool())
    {
        return createFromTypeID(Identifier("t_BoolParameter"), n);
    }
    else if (v.isString())
    {
        return createFromTypeID(Identifier("t_StringParameter"), n);
    }
    else
    {

        jassertfalse;
        return nullptr;
    }
}

Array<Identifier> ParameterFactory::getCompatibleTypes(ParameterBase *p)
{
    static Array<Identifier> AllGroup{Trigger::_factoryType, BoolParameter::_factoryType, FloatParameter::_factoryType, IntParameter::_factoryType, EnumParameter::_factoryType, StringParameter::_factoryType, Point2DParameter<floatParamType>::_factoryType};
    Array<Identifier> res;
    for (auto &a : AllGroup)
    {
        if (a != p->getFactoryTypeId())
        {
            res.add(a);
        }
    }
    return res;
}
