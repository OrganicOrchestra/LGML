/*
  ==============================================================================

    LGMLLoggerUI.h
    Created: 6 May 2016 2:13:35pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef LGMLLOGGERUI_H_INCLUDED
#define LGMLLOGGERUI_H_INCLUDED

#include "LGMLLogger.h"
#include "ShapeShifterContent.h"
#include "DebugHelpers.h"
#include "Style.h"

class LGMLLoggerUI : public ShapeShifterContent,public LGMLLogger::Listener{
public:
    LGMLLoggerUI(LGMLLogger * l):ShapeShifterContent("LGMLLogger"),
    logList(this),
    maxNumElement(999){
        l->addLogListener(this);
        TableHeaderComponent * thc = new TableHeaderComponent();
        thc->addColumn("source", 1, 100);
        thc->addColumn("content", 2, 300);


        logListComponent = new TableListBox("LGMLLogger",&logList);
        logListComponent->setColour(TableListBox::backgroundColourId, BG_COLOR);
        logListComponent->setHeader (thc);
        addAndMakeVisible(logListComponent);
        LOG( "LGMLv" + String(ProjectInfo::versionString); + "\nby OrganicOrchestra");
        LOG("please provide logFile for any bug report :\nlogFile in "+l->fileLog->getLogFile().getFullPathName());

    }

    ~LGMLLoggerUI(){

//        logListComponent.setModel(nullptr);
    }
    class LogList : public TableListBoxModel{
    public:
        LogList(LGMLLoggerUI * o):owner(o){}

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

    private:


        LGMLLoggerUI * owner;
    };

    void resized()override;
    LogList logList;
    ScopedPointer<TableListBox> logListComponent;
    int maxNumElement;



    OwnedArray<LogElement> logElements;
    void newMessage(const String & ) override;
private:
    int getTotalLogRow();
    const String & getSourceForRow(int r);
    const String & getContentForRow(int r);
    friend class LogList;


};



#endif  // LGMLLOGGERUI_H_INCLUDED
