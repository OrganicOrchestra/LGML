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

  static String toShortName(const String &niceName, bool replaceSlashes = false) {
    if (niceName.isEmpty()) return "";


    String res = niceName;
    if(replaceSlashes) res = res.replaceCharacter('/','_');
    res=res.replaceCharacter('#','_');
    res=res.replaceCharacter('(','_');
    res=res.replaceCharacter(')','_');
    res=res.replaceCharacter('>','_');
    res=res.replaceCharacter('<','_');

    {
      auto statChar = res.getCharPointer();
      if(res.length()>2 && statChar.isUpperCase() && (statChar+1).isLowerCase()){
        statChar.write(statChar.toLowerCase());
      }
    }
    res=res.removeCharacters(" ");

    return res;
  }





};

#endif  // STRINGUTIL_H_INCLUDED
