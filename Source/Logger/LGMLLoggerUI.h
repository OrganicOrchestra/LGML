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

#ifndef LGMLLOGGERUI_H_INCLUDED
#define LGMLLOGGERUI_H_INCLUDED

#include "LGMLLogger.h"
#include "../UI/Style.h"
#include "../UI/ShapeShifter/ShapeShifterContent.h"
#include "../Utils/DebugHelpers.h"

// no true performance gain proved atm
#define USE_CACHED_GLYPH 0

#define LOGGER_USE_LABEL 0

class LGMLLoggerUI : public ShapeShifterContentComponent,
    public LGMLLogger::Listener,
    public TextButton::Listener,
    public Timer
{
public:
    LGMLLoggerUI (const String& contentName, LGMLLogger* l);
    ~LGMLLoggerUI();

    

    class LogList : public juce::TableListBoxModel
    {
    public:
        LogList (LGMLLoggerUI* o);
        virtual ~LogList(){};
        int getNumRows() override;

        void paintRowBackground (Graphics&,
                                 int rowNumber,
                                 int width, int height,
                                 bool rowIsSelected) override;
#if LOGGER_USE_LABEL
        Component * refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected,
                                                        Component* existingComponentToUpdate) override;
#endif
        void paintCell (Graphics&,
                        int rowNumber,
                        int columnId,
                        int width, int height,
                        bool rowIsSelected) override;

        String getTextAt(int rowNumber,int columnId);

        String getCellTooltip (int /*rowNumber*/, int /*columnId*/)    override;

    private:
#if USE_CACHED_GLYPH
        HashMap<String,CachedGlyph > cachedG;
        void cleanUnusedGlyphs();
#endif


        int minRow,maxRow;
        LGMLLoggerUI* owner;
        friend class LGMLLoggerUI;
    };

    LGMLLogger* logger;

    void resized()override;
    LogList logList;
    TextButton clearB,copyB;
    ScopedPointer<TableListBox> logListComponent;
    int maxNumElement;

    void buttonClicked (Button*) override;


    OwnedArray<LogElement> logElements;
    void newMessage (const String& ) override;
private:


    bool keyPressed (const KeyPress&) override;
    
    MouseCursor  getMouseCursor() override;
    void mouseDown  (const MouseEvent&) override;
    void mouseDrag  (const MouseEvent&) override;

    

    Atomic<int> totalLogRow;
    void updateTotalLogRow();
    const LogElement * getElementForRow(const int r) const;
    const String& getSourceForRow  (const int r) const;
    const bool isPrimaryRow  (const int r) const;
    const String& getContentForRow (const int r) const;
    const Colour& getSeverityColourForRow (const int r)const;
    const String getTimeStringForRow (const int r) const;
    friend class LogList;

    int64 lastUpdateTime;
    void timerCallback()override;



};



#endif  // LGMLLOGGERUI_H_INCLUDED
