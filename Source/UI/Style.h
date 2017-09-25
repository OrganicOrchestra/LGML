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


#ifndef STYLE_H_INCLUDED
#define STYLE_H_INCLUDED
#pragma once
// TODO : implement nicer colour handling (maybe skinnable)
// Warning don't use JUCE's internal color definitions as it will be empty at this stage of compilation
#include "../JuceHeaderUI.h"
const Colour NORMAL_COLOR    (0xff666666);
const Colour BG_COLOR        (0xff222222);
const Colour PANEL_COLOR     (0xff444444);
const Colour FRONT_COLOR     (0xffCCCCCC);
const Colour LIGHTCONTOUR_COLOR (0xffAAAAAA);
const Colour HIGHLIGHT_COLOR (0xffFF922F);
const Colour CONTOUR_COLOR   (0xcc303030);
const Colour TEXT_COLOR      (0xffCCCCCC);
const Colour TEXTNAME_COLOR  (0xff999999);
const Colour AUDIO_COLOR     (0xff5f9ea0);//(Colours::cadetblue);//
const Colour DATA_COLOR      (0xffffc0cb);//(Colours::pink);//
const Colour FEEDBACK_COLOR		(0xff18b5ef);
const Colour PARAMETER_FRONT_COLOR (0xff99ff66);


#endif  // STYLE_H_INCLUDED
