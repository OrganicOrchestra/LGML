/*
  ==============================================================================

    StringUtil.h
    Created: 8 Mar 2016 2:43:25pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef STRINGUTIL_H_INCLUDED
#define STRINGUTIL_H_INCLUDED

#include "JuceHeader.h"

class StringUtil
{
public:
	static String toShortName(const String &niceName) {
		if (niceName.isEmpty()) return "";

		StringArray sa;
		sa.addTokens(niceName, false);
		int index = 0;
		for (auto &s : sa.strings)
		{
			if (s.isEmpty()) continue;
			String initial = s.substring(0, 1);
			s.swapWith(s.replaceSection(0, 1, index == 0 ? initial.toLowerCase() : initial.toUpperCase()));
			index++;
		}

		return sa.joinIntoString("");
	}
};


#endif  // STRINGUTIL_H_INCLUDED
