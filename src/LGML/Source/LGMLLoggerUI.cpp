/*
 ==============================================================================

 LGMLLoggerUI.cpp
 Created: 6 May 2016 2:13:35pm
 Author:  Martin Hermant

 ==============================================================================
 */

#include "LGMLLoggerUI.h"



void LGMLLoggerUI::newMessage(const String & s) {
    int oldNumRow = getTotalLogRow();
    LogElement * el = new LogElement(s);
    logElements.add(el);
    int newNumRow = oldNumRow + el->getNumLines() ;
    for(int i = oldNumRow ; i < newNumRow;  i++){
        logListComponent.refreshComponentForRow(i, false, nullptr);
    }

    bool overFlow = false;
    while(newNumRow>maxNumElement){
        logElements.remove(0);
        newNumRow = getTotalLogRow();
        overFlow = true;
    }
//    if(overFlow)
        logListComponent.updateContent();


};

void LGMLLoggerUI::resized(){
    Rectangle<int> area = getLocalBounds();
    logListComponent.setBounds(area);
}



int LGMLLoggerUI::getTotalLogRow(){
    int res=0;
    for(auto & l:logElements){
        res+=l->getNumLines();
    }
    return res;
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
    while (true ) {
        int nl = logElements[idx]->getNumLines();
        if(count+nl>=r){
            return logElements[idx]->getLine(r-count);
        }
        count+=nl;
        idx++;
        if(idx >= logElements.size())return String::empty;

    }



    return String::empty;
};


//////////////
// logList

int LGMLLoggerUI::LogList::getNumRows() {

    return owner->getTotalLogRow();
};

void LGMLLoggerUI::LogList::paintRowBackground (Graphics& g,
                                                int rowNumber,
                                                int width, int height,
                                                bool rowIsSelected) {
    g.setColour(Colours::grey);
    g.drawRect(0, 0, width, height);

};

void LGMLLoggerUI::LogList::paintCell (Graphics& g,
                                       int rowNumber,
                                       int columnId,
                                       int width, int height,
                                       bool rowIsSelected) {

    g.setColour(Colours::black);
    String text;
    if(columnId==1)
        text = owner->getSourceForRow(rowNumber);
    else if(columnId==2)
        text = owner->getContentForRow(rowNumber);
    
    g.drawFittedText(text, 0, 0, width, height, Justification::left, 1);
    
};

