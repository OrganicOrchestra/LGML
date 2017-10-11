/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in realtime

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#ifndef STYLE_H_INCLUDED
#define STYLE_H_INCLUDED
#pragma once

#include "../JuceHeaderUI.h"


#define USE_GL 1


// debug util
struct LGMLUIUtils{


    static void printComp(Component * c);
    static void drawBounds(Component * c,Graphics & g);
    static void forceRepaint(Component * c) ;

};

namespace LGMLColors
{
enum ColorsIds
{
    audioColor = 0x200001,
    dataColor,
    elementBackground
};

}


class AddElementButton : public Button
{
public:
    AddElementButton();
    ~AddElementButton();

    void paintButton (Graphics& g,
                      bool isMouseOverButton,
                      bool isButtonDown)override;
    // utility function to share common positioning
    void setFromParentBounds (const Rectangle<int>& area);


};


class RemoveElementButton : public Button
{
public:
    RemoveElementButton();
    ~RemoveElementButton();

    void paintButton (Graphics& g,
                      bool isMouseOverButton,
                      bool isButtonDown)override;

    
    
};
#endif  // STYLE_H_INCLUDED
