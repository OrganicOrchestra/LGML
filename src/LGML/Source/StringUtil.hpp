/*
 ==============================================================================

 StringUtil.hpp
 Created: 8 Mar 2016 2:43:25pm
 Author:  bkupe

 ==============================================================================
 */

#ifndef STRINGUTIL_H_INCLUDED
#define STRINGUTIL_H_INCLUDED
#pragma once

namespace StringUtil
{

  static String toShortName(const String &niceName) {
    if (niceName.isEmpty()) return "";
    //  based on OSC escaping
    // http://opensoundcontrol.org/spec-1_0
    return niceName.removeCharacters(" #*,?[]{}");
  }





};

#endif  // STRINGUTIL_H_INCLUDED
