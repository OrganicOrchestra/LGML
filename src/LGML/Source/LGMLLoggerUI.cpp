/*
 ==============================================================================

 LGMLLoggerUI.cpp
 Created: 6 May 2016 2:13:35pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "LGMLLoggerUI.h"



void LGMLLoggerUI::newMessage(const String & s)
{
    LogElement * el = new LogElement(s);


	//@martin i did that so i could clear the log, but the scollbar still goes down after clearing
	if (totalLogRow >= maxNumElement)
	{
		logElements.clear();
		totalLogRow = 0;
		logListComponent->updateContent();

	}


	logElements.add(el);
	totalLogRow += el->getNumLines();


	//@martin problems here
	/*
	//bool overFlow = false;

	while (totalLogRow > maxNumElement) {
		LogElement * rmL = logElements[0];
        totalLogRow -= rmL->getNumLines();
		logElements.removeObject(logElements[0]);
		logListComponent->updateContent();
       // overFlow = true;
    }
	*/

    //coalesce messa
    triggerAsyncUpdate();

};
void LGMLLoggerUI::handleAsyncUpdate(){
    logListComponent->updateContent();
    logListComponent->scrollToEnsureRowIsOnscreen(totalLogRow-1);
}

LGMLLoggerUI::LGMLLoggerUI(const String &contentName, LGMLLogger * l) :
	logger(l),
	ShapeShifterContent(contentName),
	logList(this),
	maxNumElement(500),
	totalLogRow(0)
{
	logger->addLogListener(this);
	TableHeaderComponent * thc = new TableHeaderComponent();
	thc->addColumn("Time", 1, 60);
	thc->addColumn("Source", 2, 80);
	thc->addColumn("Content", 3, 400);


	logListComponent = new TableListBox("LGMLLogger", &logList);
	logListComponent->setRowHeight(13);
	logListComponent->setHeaderHeight(20);
	logListComponent->getViewport()->setScrollBarThickness(10);

	logListComponent->setColour(TableListBox::backgroundColourId, BG_COLOR);
	logListComponent->setHeader(thc);
	addAndMakeVisible(logListComponent);
	LOG("LGML v" + String(ProjectInfo::versionString) + "\nby OrganicOrchestra");
	LOG("please provide logFile for any bug report :\nlogFile in " + l->fileLog->getLogFile().getFullPathName());

	clearB.setButtonText("Clear");
	clearB.addListener(this);
	addAndMakeVisible(clearB);

}

LGMLLoggerUI::~LGMLLoggerUI() {

	//        logListComponent.setModel(nullptr);
	logger->removeLogListener(this);
}

void LGMLLoggerUI::resized(){

    Rectangle<int> area = getLocalBounds();
	clearB.setBounds(area.removeFromBottom(30).reduced(5));
    logListComponent->setBounds(area);

	int tw = getWidth();
	tw -= logListComponent->getHeader().getColumnWidth(1);
	tw -= logListComponent->getHeader().getColumnWidth(2);
	tw -= logListComponent->getViewport()->getScrollBarThickness();
	tw = jmax(tw, 100);
	logListComponent->getHeader().setColumnWidth(3, tw);
}



void LGMLLoggerUI::updateTotalLogRow(){
    totalLogRow=0;
    for(auto & l:logElements){
        totalLogRow+=l->getNumLines();
    }

}
const String & LGMLLoggerUI::getSourceForRow(int r){
    int count = 0;
    int idx = 0;
    while (count<=r ) {
        if(count==r){
            return logElements[idx]->source;
        }
        count+=logElements[idx]->getNumLines();
        idx++;
        if(idx >= logElements.size())return String::empty;

    }

    return String::empty;
}

const String &  LGMLLoggerUI::getContentForRow(int r){
    int count = 0;
    int idx = 0;

    while (idx < logElements.size()) {

        int nl = logElements[idx]->getNumLines();

        if(count+nl>r){
            return logElements[idx]->getLine(r-count);
        }

        count+=nl;
        idx++;
    }

    return String::empty;
};

String  LGMLLoggerUI::getTimeStringForRow(int r) {
	int count = 0;
	int idx = 0;
	while (count <= r) {
		if (count == r) {
			return String(logElements[idx]->time.toString(false, true, true, true));
		}
		count += logElements[idx]->getNumLines();
		idx++;
		if (idx >= logElements.size()) return String::empty;

	}

	return String::empty;
};



//////////////
// logList

LGMLLoggerUI::LogList::LogList(LGMLLoggerUI * o) :owner(o)
{
}

int LGMLLoggerUI::LogList::getNumRows() {

    return owner->totalLogRow;
};

void LGMLLoggerUI::LogList::paintRowBackground (Graphics& g,
                                                int rowNumber,
                                                int width, int height,
                                                bool)
{
    g.setColour(Colours::transparentBlack.withAlpha((rowNumber%2==0?0.1f:0.f)));
    g.fillRect(0, 0, width, height);
};

void LGMLLoggerUI::LogList::paintCell (Graphics& g,
                                       int rowNumber,
                                       int columnId,
                                       int width, int height,
                                       bool) {
    g.setFont(12);
    g.setColour(TEXT_COLOR);
    String text;

	switch (columnId)
	{
	case 1:
		text = owner->getTimeStringForRow(rowNumber);
		break;
	case 2:
		text = owner->getSourceForRow(rowNumber);
		break;
	case 3:
		text = owner->getContentForRow(rowNumber);
		break;
	}

    g.drawFittedText(text, 0, 0, width, height, Justification::left, 1);

};

void LGMLLoggerUI::buttonClicked (Button* b) {

    if(b==&clearB)
	{
        logElements.clear();
		totalLogRow = 0;
        logListComponent->updateContent();
		LOG("Cleared.");
    }
}
