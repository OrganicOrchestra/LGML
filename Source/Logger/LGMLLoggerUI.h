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

#ifndef LGMLLOGGERUI_H_INCLUDED
#define LGMLLOGGERUI_H_INCLUDED

#include "LGMLLogger.h"
#include "../UI/ShapeShifter/ShapeShifterContent.h"
#include "../Utils/DebugHelpers.h"

class LGMLLoggerUI : public ShapeShifterContentComponent,
    public LGMLLogger::Listener,
    public TextButton::Listener,
    public AsyncUpdater
{
public:
    LGMLLoggerUI (const String& contentName, LGMLLogger* l);
    ~LGMLLoggerUI();

    class LogList : public juce::TableListBoxModel
    {
    public:
        LogList (LGMLLoggerUI* o);

        int getNumRows() override;

        void paintRowBackground (Graphics&,
                                 int rowNumber,
                                 int width, int height,
                                 bool rowIsSelected) override;

        void paintCell (Graphics&,
                        int rowNumber,
                        int columnId,
                        int width, int height,
                        bool rowIsSelected) override;
        String getCellTooltip (int /*rowNumber*/, int /*columnId*/)    override;

    private:


        LGMLLoggerUI* owner;
    };

    LGMLLogger* logger;

    void resized()override;
    LogList logList;
    TextButton clearB;
    ScopedPointer<TableListBox> logListComponent;
    int maxNumElement;

    void buttonClicked (Button*) override;


    OwnedArray<LogElement> logElements;
    void newMessage (const String& ) override;
private:
    int totalLogRow;
    void updateTotalLogRow();
    const LogElement * getElementForRow(const int r) const;
    const String& getSourceForRow  (const int r) const;
    const bool isPrimaryRow  (const int r) const;
    const String& getContentForRow (const int r) const;
    const Colour& getSeverityColourForRow (const int r)const;
    const String getTimeStringForRow (const int r) const;
    friend class LogList;

    void handleAsyncUpdate()override;

};



#endif  // LGMLLOGGERUI_H_INCLUDED
