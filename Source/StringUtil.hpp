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


#ifndef STRINGUTIL_H_INCLUDED
#define STRINGUTIL_H_INCLUDED
#pragma once

namespace StringUtil
{

  forcedinline static String toShortName(const String &s) {
    if (s.isEmpty()) return "";
    //  based on OSC escaping
    // http://opensoundcontrol.org/spec-1_0
    return s.removeCharacters(" #*,?[]{}");
  }

  forcedinline static String toFullyEscaped(const String & s){
    if (s.isEmpty()) return "";
    return s.removeCharacters("#*,?[]{} :;%$/<>");
  }



};

#endif  // STRINGUTIL_H_INCLUDED
