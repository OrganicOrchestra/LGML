/*
 ==============================================================================
 
 Style.h
 Created: 4 Mar 2016 11:33:48am
 Author:  bkupe
 
 ==============================================================================
 */

#ifndef STYLE_H_INCLUDED
#define STYLE_H_INCLUDED


#define NORMAL_COLOR    Colour(0xff666666)
#define BG_COLOR        Colour(0xff222222)
#define PANEL_COLOR     Colour(0xff444444)
#define FRONT_COLOR     Colour(0xffCCCCCC)
#define HIGHLIGHT_COLOR Colour(0xffFF922F)
#define CONTOUR_COLOR   Colour(0xcc303030)

#define TEXT_COLOR      Colour(0xffCCCCCC)
#define TEXTNAME_COLOR  Colour(0xff999999)

#define AUDIO_COLOR     Colours::cadetblue
#define DATA_COLOR      Colours::pink

#define PARAMETER_FRONT_COLOR Colour(0xff99ff66)

#include "JuceHeader.h"

namespace LookAndFeelHelpers {
    
    
    // initialise the standard set of colours..
    static const uint32 textButtonColour      = 0xffbbbbff;
    static const uint32 textHighlightColour   = 0x401111ee;
    static const uint32 standardOutlineColour = 0xb2808080;
    
    static const uint32 standardColours[] =
    {
        TextButton::buttonColourId,                 textButtonColour,
        TextButton::buttonOnColourId,               0xff4444ff,
        TextButton::textColourOnId,                 0xff000000,
        TextButton::textColourOffId,                0xff000000,
        
        ToggleButton::textColourId,                 0xff000000,
        
        TextEditor::backgroundColourId,             0xffffffff,
        TextEditor::textColourId,                   0xff000000,
        TextEditor::highlightColourId,              textHighlightColour,
        TextEditor::highlightedTextColourId,        0xff000000,
        TextEditor::outlineColourId,                0x00000000,
        TextEditor::focusedOutlineColourId,         textButtonColour,
        TextEditor::shadowColourId,                 0x38000000,
        
        CaretComponent::caretColourId,              0xff000000,
        
        Label::backgroundColourId,                  0x00000000,
        Label::textColourId,                        0xff000000,
        Label::outlineColourId,                     0x00000000,
        
        ScrollBar::backgroundColourId,              Colours::whitesmoke.getARGB(),
        ScrollBar::thumbColourId,                   0xffffffff,
        
        TreeView::linesColourId,                    0x4c000000,
        TreeView::backgroundColourId,               0x00000000,
        TreeView::dragAndDropIndicatorColourId,     0x80ff0000,
        TreeView::selectedItemBackgroundColourId,   0x00000000,
        
        PopupMenu::backgroundColourId,              0xffffffff,
        PopupMenu::textColourId,                    0xff000000,
        PopupMenu::headerTextColourId,              0xff000000,
        PopupMenu::highlightedTextColourId,         0xffffffff,
        PopupMenu::highlightedBackgroundColourId,   0x991111aa,
        
        ComboBox::buttonColourId,                   0xffbbbbff,
        ComboBox::outlineColourId,                  standardOutlineColour,
        ComboBox::textColourId,                     0xff000000,
        ComboBox::backgroundColourId,               0xffffffff,
        ComboBox::arrowColourId,                    0x99000000,
        
        PropertyComponent::backgroundColourId,      0x66ffffff,
        PropertyComponent::labelTextColourId,       0xff000000,
        
        TextPropertyComponent::backgroundColourId,  0xffffffff,
        TextPropertyComponent::textColourId,        0xff000000,
        TextPropertyComponent::outlineColourId,     standardOutlineColour,
        
        BooleanPropertyComponent::backgroundColourId, 0xffffffff,
        BooleanPropertyComponent::outlineColourId,  standardOutlineColour,
        
        ListBox::backgroundColourId,                0xffffffff,
        ListBox::outlineColourId,                   standardOutlineColour,
        ListBox::textColourId,                      0xff000000,
        
        Slider::backgroundColourId,                 0x00000000,
        Slider::thumbColourId,                      textButtonColour,
        Slider::trackColourId,                      0x7fffffff,
        Slider::rotarySliderFillColourId,           0x7f0000ff,
        Slider::rotarySliderOutlineColourId,        0x66000000,
        Slider::textBoxTextColourId,                0xff000000,
        Slider::textBoxBackgroundColourId,          0xffffffff,
        Slider::textBoxHighlightColourId,           textHighlightColour,
        Slider::textBoxOutlineColourId,             standardOutlineColour,
        
        ResizableWindow::backgroundColourId,        0xff777777,
        //DocumentWindow::textColourId,               0xff000000, // (this is deliberately not set)
        
        AlertWindow::backgroundColourId,            0xffededed,
        AlertWindow::textColourId,                  0xff000000,
        AlertWindow::outlineColourId,               0xff666666,
        
        ProgressBar::backgroundColourId,            0xffeeeeee,
        ProgressBar::foregroundColourId,            0xffaaaaee,
        
        TooltipWindow::backgroundColourId,          0xffeeeebb,
        TooltipWindow::textColourId,                0xff000000,
        TooltipWindow::outlineColourId,             0x4c000000,
        
        TabbedComponent::backgroundColourId,        0x00000000,
        TabbedComponent::outlineColourId,           0xff777777,
        TabbedButtonBar::tabOutlineColourId,        0x80000000,
        TabbedButtonBar::frontOutlineColourId,      0x90000000,
        
        Toolbar::backgroundColourId,                0xfff6f8f9,
        Toolbar::separatorColourId,                 0x4c000000,
        Toolbar::buttonMouseOverBackgroundColourId, 0x4c0000ff,
        Toolbar::buttonMouseDownBackgroundColourId, 0x800000ff,
        Toolbar::labelTextColourId,                 0xff000000,
        Toolbar::editingModeOutlineColourId,        0xffff0000,
        
        DrawableButton::textColourId,               0xff000000,
        DrawableButton::textColourOnId,             0xff000000,
        DrawableButton::backgroundColourId,         0x00000000,
        DrawableButton::backgroundOnColourId,       0xaabbbbff,
        
        HyperlinkButton::textColourId,              0xcc1111ee,
        
        GroupComponent::outlineColourId,            0x66000000,
        GroupComponent::textColourId,               0xff000000,
        
        BubbleComponent::backgroundColourId,        0xeeeeeebb,
        BubbleComponent::outlineColourId,           0x77000000,
        
        DirectoryContentsDisplayComponent::highlightColourId,   textHighlightColour,
        DirectoryContentsDisplayComponent::textColourId,        0xff000000,
        
        0x1000440, /*LassoComponent::lassoFillColourId*/        0x66dddddd,
        0x1000441, /*LassoComponent::lassoOutlineColourId*/     0x99111111,
        
        0x1005000, /*MidiKeyboardComponent::whiteNoteColourId*/               0xffffffff,
        0x1005001, /*MidiKeyboardComponent::blackNoteColourId*/               0xff000000,
        0x1005002, /*MidiKeyboardComponent::keySeparatorLineColourId*/        0x66000000,
        0x1005003, /*MidiKeyboardComponent::mouseOverKeyOverlayColourId*/     0x80ffff00,
        0x1005004, /*MidiKeyboardComponent::keyDownOverlayColourId*/          0xffb6b600,
        0x1005005, /*MidiKeyboardComponent::textLabelColourId*/               0xff000000,
        0x1005006, /*MidiKeyboardComponent::upDownButtonBackgroundColourId*/  0xffd3d3d3,
        0x1005007, /*MidiKeyboardComponent::upDownButtonArrowColourId*/       0xff000000,
        0x1005008, /*MidiKeyboardComponent::shadowColourId*/                  0x4c000000,
        
        0x1004500, /*CodeEditorComponent::backgroundColourId*/                0xffffffff,
        0x1004502, /*CodeEditorComponent::highlightColourId*/                 textHighlightColour,
        0x1004503, /*CodeEditorComponent::defaultTextColourId*/               0xff000000,
        0x1004504, /*CodeEditorComponent::lineNumberBackgroundId*/            0x44999999,
        0x1004505, /*CodeEditorComponent::lineNumberTextId*/                  0x44000000,
        
        0x1007000, /*ColourSelector::backgroundColourId*/                     0xffe5e5e5,
        0x1007001, /*ColourSelector::labelTextColourId*/                      0xff000000,
        
        0x100ad00, /*KeyMappingEditorComponent::backgroundColourId*/          0x00000000,
        0x100ad01, /*KeyMappingEditorComponent::textColourId*/                0xff000000,
        
        FileSearchPathListComponent::backgroundColourId,        0xffffffff,
        
        FileChooserDialogBox::titleTextColourId,                0xff000000,
    };
    
    static Colour createBaseColour (Colour buttonColour,
                                    bool hasKeyboardFocus,
                                    bool isMouseOverButton,
                                    bool isButtonDown) noexcept
    {
        const float sat = hasKeyboardFocus ? 1.3f : 0.9f;
        const Colour baseColour (buttonColour.withMultipliedSaturation (sat));
        
        if (isButtonDown)      return baseColour.contrasting (0.2f);
            if (isMouseOverButton) return baseColour.contrasting (0.1f);
                
                return baseColour;
    }
    
    static TextLayout layoutTooltipText (const String& text, Colour colour) noexcept
    {
        const float tooltipFontSize = 13.0f;
        const int maxToolTipWidth = 400;
        
        AttributedString s;
        s.setJustification (Justification::centred);
        s.append (text, Font (tooltipFontSize, Font::bold), colour);
        
        TextLayout tl;
        tl.createLayoutWithBalancedLineLengths (s, (float) maxToolTipWidth);
        return tl;
    }
    
    
}
#endif  // STYLE_H_INCLUDED
