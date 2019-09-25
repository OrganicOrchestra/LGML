/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */


#pragma once
#if !ENGINE_HEADLESS

#include "../JuceHeaderUI.h"

#ifndef USE_GL
//    #if JUCE_ARM // remove GL for arm, seems to changes ui perfs
        #define USE_GL 0
//    #else
//        #define USE_GL 1
//    #endif
#endif

#if USE_GL
#include <juce_opengl/juce_opengl.h>
#endif


// debug util
struct LGMLUIUtils{


    static void printComp(Component * c);
    static void drawBounds(Component * c,Graphics & g);
    static void forceRepaint(Component * c) ;
    static Colour getBackgroundColorForDepth(const Colour   baseColor,int depth);
    static Colour getCurrentBackgroundColor(const Component *c);
    static void markHasNewBackground(Component * c,int depth);
    static void fillBackground(const Component *c,Graphics & g);
    static void optionallySetBufferedToImage(Component * c, bool l=true );
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


class AddElementButton : public DrawableButton
{
public:
    AddElementButton();
    ~AddElementButton();
    static Drawable * createDrawable(bool isHovered=false);
    void paintButton (Graphics& g,
                      bool isMouseOverButton,
                      bool isButtonDown)override;
    void paint(Graphics &g) override;
    // utility function to share common positioning
    void setFromParentBounds (const Rectangle<int>& area);
    void parentHierarchyChanged()override;

private:
    JUCE_LEAK_DETECTOR(AddElementButton)
};


class RemoveElementButton : public Button
{
public:
    RemoveElementButton();
    ~RemoveElementButton();

    void paintButton (Graphics& g,
                      bool isMouseOverButton,
                      bool isButtonDown)override;
    void paint(Graphics &g) override;

private:
    JUCE_LEAK_DETECTOR(RemoveElementButton)
};





class CachedGlyph {
public:

    explicit CachedGlyph(const String &t="",const Font & f = Font());
    void setSize(int w, int h);
    void setFont(const Font & );
    void setText(const String & t);
    void paint( Graphics & g) ;

    class CachedFont : public ReferenceCountedObject{
    public:
        explicit CachedFont(const Font &f);
        ~CachedFont();
        typedef ReferenceCountedObjectPtr<CachedFont> Ptr ;
        Font font;
    };
private:
    bool isReady();
    void updateGlyph();
    Font * getCurFont();
    Rectangle<int>& getLocalBounds();
    Rectangle<int> bounds;
    String text;
    bool useEllipsesIfTooBig;
    Justification justificationType;
    GlyphArrangement glyphArr;
    CachedFont::Ptr  _font;

};




#endif // WITHUI

