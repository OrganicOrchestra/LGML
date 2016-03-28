/*
  ==============================================================================

    PluginWindow.h
    Created: 24 Mar 2016 6:58:45pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef PluginWindow_H_INCLUDED
#define PluginWindow_H_INCLUDED



#include "JuceHeader.h"

class VSTNode;

/** A desktop window containing a plugin's UI. */
class PluginWindow  : public DocumentWindow
{
public:
    enum WindowFormatType
    {
        Normal = 0,
        Generic,
        Programs,
        Parameters,
        NumTypes
    };

    PluginWindow (Component* pluginEditor, VSTNode*, WindowFormatType);
    ~PluginWindow();

    static PluginWindow* getWindowFor (VSTNode*, WindowFormatType w=WindowFormatType::Normal );

    static void closeCurrentlyOpenWindowsFor (VSTNode*);
    static void closeAllCurrentlyOpenWindows();

    void moved() override;
    void closeButtonPressed() override;

private:
    VSTNode* owner;
    WindowFormatType type;

    float getDesktopScaleFactor() const override     { return 1.0f; }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWindow)
};

inline String toString (PluginWindow::WindowFormatType type)
{
    switch (type)
    {
        case PluginWindow::Normal:     return "Normal";
        case PluginWindow::Generic:    return "Generic";
        case PluginWindow::Programs:   return "Programs";
        case PluginWindow::Parameters: return "Parameters";
        default:                       return String();
    }
}


#endif  // PluginWindow_H_INCLUDED
