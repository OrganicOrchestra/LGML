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

class LGMLLoggerUI : public ShapeShifterContent,
	public LGMLLogger::Listener,
	public TextButton::Listener{
public:
	LGMLLoggerUI(LGMLLogger * l);

    ~LGMLLoggerUI(){

//        logListComponent.setModel(nullptr);
    }
    class LogList : public juce::TableListBoxModel
	{
    public:
		LogList(LGMLLoggerUI * o);

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
    TextButton clearB;
    ScopedPointer<TableListBox> logListComponent;
    int maxNumElement;

    void buttonClicked (Button*) override;


    OwnedArray<LogElement> logElements;
    void newMessage(const String & ) override;
private:
    int getTotalLogRow();
    const String & getSourceForRow(int r);
    const String & getContentForRow(int r);
	String getTimeStringForRow(int r);
    friend class LogList;


};



#endif  // LGMLLOGGERUI_H_INCLUDED
