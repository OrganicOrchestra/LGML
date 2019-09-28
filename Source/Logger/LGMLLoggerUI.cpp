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

#if !ENGINE_HEADLESS
#include "LGMLLoggerUI.h"
#include "../UI/Style.h"

#include "../Engine.h" //for version


int LGMLLoggerUI::maxDisplayedLogs = 5000;

const OwnedArray<LogElement,CriticalSection> & loggedElements(){return LGMLLogger::getInstance()->loggedElements;}
static String EmptyString;
void LGMLLoggerUI::newMessages (int from , int to)
{
    //bool overFlow = false;
    int delta = (to - startDisplayedIdx)-maxDisplayedLogs;
    if(delta>0){
        startDisplayedIdx+=delta;
    }
    totalLogRow = 0;
    for(int i = startDisplayedIdx ; i < to ; i++){
        auto el = loggedElements().getUnchecked(i);
//        if(!(el->getNumAppearances()>1))
            totalLogRow += el->getNumLines();
    }

        logListComponent->updateContent();
        logListComponent->scrollToEnsureRowIsOnscreen (totalLogRow.get() - 1);
#if USE_CACHED_GLYPH
        logList.cleanUnusedGlyphs();
#endif


        repaint();


//    }
}

LGMLLoggerUI::LGMLLoggerUI (const String& contentName, LGMLLogger* l) :
logger (l),
ShapeShifterContentComponent (contentName,
                              "See events occuring in LGML"),
CoalescedListener(100),
logList (this),
maxNumElement (100),
totalLogRow (0),
startDisplayedIdx(0)
{


    TableHeaderComponent* thc = new TableHeaderComponent();
    thc->addColumn (juce::translate("Time"), 1, 60);
    thc->addColumn (juce::translate("Source"), 2, 80);
    thc->addColumn (juce::translate("Content"), 3, 400);


    logListComponent = std::make_unique< TableListBox> ("LGMLLogger", &logList);
    logListComponent->setOpaque(true);
    logListComponent->setRowHeight (13);
    logListComponent->setHeaderHeight (20);
    logListComponent->getViewport()->setScrollBarThickness (10);

    logListComponent->setColour (TableListBox::backgroundColourId, findColour (ResizableWindow::backgroundColourId));
    logListComponent->setHeader (thc);
    logListComponent->setMouseCursor(MouseCursor::IBeamCursor);
    logListComponent->setMultipleSelectionEnabled(true);
    addAndMakeVisible (logListComponent.get());

    LOG (l->getWelcomeMessage());

    clearB.setButtonText (juce::translate("Clear"));
    clearB.addListener (this);
    addAndMakeVisible (clearB);

    copyB.setButtonText(juce::translate("Copy All to Clipboard"));
    copyB.addListener(this);
    addAndMakeVisible(copyB);

    setInterceptsMouseClicks(true,false);
    addMouseListener(this, true);



    logger->addLogCoalescedListener (this);

}


LGMLLoggerUI::~LGMLLoggerUI()
{
    //        logListComponent.setModel(nullptr);
    logger->removeLogListener (this);
}

void LGMLLoggerUI::resized()
{

    ShapeShifterContentComponent::resized();
    Rectangle<int> area = getLocalBounds().withTop(5);
    auto footer =area.removeFromBottom (30).reduced (5);
    clearB.setBounds (footer.removeFromLeft(footer.getWidth()/2).reduced(2));
    copyB.setBounds(footer.reduced(2));
    logListComponent->setBounds (area);
    bool firstVisible  = area.getWidth() > 400;
    logListComponent->getHeader().setColumnVisible (1, firstVisible);
    bool secondVisible = area.getWidth() > 300;
    logListComponent->getHeader().setColumnVisible (2, secondVisible);

    int tw = getWidth();

    if (firstVisible)tw -= logListComponent->getHeader().getColumnWidth (1);

    if (secondVisible)tw -= logListComponent->getHeader().getColumnWidth (2);

    tw -= logListComponent->getViewport()->getScrollBarThickness();
    tw = jmax (tw, 100);
    logListComponent->getHeader().setColumnWidth (3, tw);
}



void LGMLLoggerUI::updateTotalLogRow()
{
    totalLogRow = 0;

    for (int i = startDisplayedIdx ; i< LGMLLogger::getInstance()->getNumLogs() ; i++)
    {
        totalLogRow += loggedElements().getUnchecked(i)->getNumLines();
    }

}
const String& LGMLLoggerUI::getSourceForRow (const int r) const
{
    if(auto el = getElementForRow(r)){ return el->source; }

    return EmptyString;
}
const bool LGMLLoggerUI::isPrimaryRow (const int r) const
{
    int count = 0;
    int idx = startDisplayedIdx;

    while (count <= r && idx < LGMLLogger::getInstance()->getNumLogs())
    {
        if (count== r)
        {
            return true;
        }
        count += loggedElements().getUnchecked(idx)->getNumLines();
        idx++;

    }

    return false;
}

const String&   LGMLLoggerUI::getContentForRow (const int r) const
{
    int count = 0;
    int idx = startDisplayedIdx;

    while (idx < LGMLLogger::getInstance()->getNumLogs())
    {

        int nl = loggedElements().getUnchecked(idx)->getNumLines();

        if (count + nl > r)
        {
            return loggedElements().getUnchecked(idx)->getLine (r - count);
        }

        count += nl;
        idx++;
    }

    return EmptyString;
};

const LogElement* LGMLLoggerUI::getElementForRow(const int r) const{
    int count = 0;
    int idx = startDisplayedIdx;

    while (idx < LGMLLogger::getInstance()->getNumLogs())
    {
        auto el = loggedElements().getUnchecked(idx);

        int nl = el->getNumLines();

        if (count + nl > r)
        {
            return el;
        }

        count += nl;
        idx++;
    }

    return nullptr;

}

const String  LGMLLoggerUI::getTimeStringForRow (const int r) const
{
    if(auto el = getElementForRow(r)){
        return String (el->time.toString (false, true, true, true));
    }

    return "";
};

const Colour& LGMLLoggerUI::getSeverityColourForRow (const int r) const
{

    if(auto el = getElementForRow(r))
    {
        LogElement::Severity s = el->severity;

        switch (s)
        {
            case LogElement::LOG_NONE:
                return Colours::darkgrey;

            case LogElement::LOG_DBG:
                return Colours::black;

            case LogElement::LOG_WARN:
                return Colours::orange;

            case LogElement::LOG_ERR:
                return Colours::red;

            default:
                return Colours::pink;

        }

    }

    return Colours::pink;
};


bool LGMLLoggerUI::keyPressed (const KeyPress& k)            {
    
    if(k == KeyPress('c',ModifierKeys::commandModifier,0)){
        String textToCopy;
        Array<Range<int>> ranges = logListComponent->getSelectedRows().getRanges();
        for(auto &r:ranges){
            for(int i = r.getStart() ; i < r.getEnd() ;i++){
                StringArray arr;
                for(int c = 1 ; c <= 3 ; c++){
                    if(logListComponent->getHeader().isColumnVisible(c)){
                        arr.add(logList.getTextAt(i, c));
                    }
                }
                textToCopy+=arr.joinIntoString("\t")+"\n";
            }
        }
        if(textToCopy.isNotEmpty()){
            SystemClipboard::copyTextToClipboard(textToCopy);
            return true;
        }
    }
    return false;
}

void LGMLLoggerUI::mouseDown  (const MouseEvent& me) {
    auto pos = me.getEventRelativeTo(logListComponent.get());
    auto rowUnderMouse =  logListComponent->getRowContainingPosition(pos.x,pos.y);
    logListComponent->selectRow(rowUnderMouse);
    grabKeyboardFocus();

};

void LGMLLoggerUI::mouseDrag  (const MouseEvent& me) {
    auto pos = me.getEventRelativeTo(logListComponent.get());
    auto rowUnderMouse =  logListComponent->getRowContainingPosition(pos.x,pos.y);
    auto rowStart =logListComponent->getRowContainingPosition(pos.getMouseDownX(),pos.getMouseDownY());
    logListComponent->selectRangeOfRows(rowStart, rowUnderMouse);

};

MouseCursor  LGMLLoggerUI::getMouseCursor(){
    return MouseCursor::IBeamCursor;
}


//////////////
// logList

LGMLLoggerUI::LogList::LogList (LGMLLoggerUI* o) : owner (o)
{
}

int LGMLLoggerUI::LogList::getNumRows()
{

    return owner->totalLogRow.get();
};

void LGMLLoggerUI::LogList::paintRowBackground (Graphics& g,
                                                int rowNumber,
                                                int width, int height,
                                                bool isSelected)
{
    auto c = owner->getSeverityColourForRow (rowNumber).withAlpha ((rowNumber % 2 == 0 ? 0.7f : 0.6f));
    if(isSelected){
        c = c.brighter();
    }
    g.setColour (c);
    g.fillRect (0, 0, width, height);
};


// use as function to prevent juce leak detection
const Font  getLogFont(){
    static Font  f(12);
    return f;
}

String LGMLLoggerUI::LogList::getTextAt(int rowNumber,int columnId){
    String text;


    switch (columnId)
    {
        case 1:
            text = owner->isPrimaryRow(rowNumber)?owner->getTimeStringForRow (rowNumber):"";
            break;

        case 2:
            text = owner->isPrimaryRow(rowNumber)?owner->getSourceForRow (rowNumber):"";
            break;

        case 3:
            text = owner->getContentForRow (rowNumber);
            break;
    }
    if(columnId==2 ){
        auto el = owner->getElementForRow(rowNumber);
        if(el->getNumAppearances()>1){
            text = String("(") + String(el->getNumAppearances()) + ") " + text;
        }
    }
    return text;

}
#if LOGGER_USE_LABEL
Component * LGMLLoggerUI::LogList::refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected,
                                     Component* existingComponentToUpdate){
    Colour color = owner->findColour (Label::textColourId);
    String text = getTextAt(rowNumber, columnId);
    Label * lp=nullptr;

    if(existingComponentToUpdate){
        lp = dynamic_cast<Label*>(existingComponentToUpdate);

    }
    else{
        lp = new Label();
        lp->setFont(getLogFont());
        lp->setEditable(true);
//        lp->showEditor();

    }
    jassert(lp);
    if(lp){
        lp->setText(text,dontSendNotification);
    }
    return lp;
}
#endif
void LGMLLoggerUI::LogList::paintCell (Graphics& g,
                                       int rowNumber,
                                       int columnId,
                                       int width, int height,
                                       bool )
{

    Colour c =owner->findColour (Label::textColourId);
    g.setColour (c);
#if !LOGGER_USE_LABEL
    String text=getTextAt(rowNumber,columnId);

#if USE_CACHED_GLYPH
    if(cachedG.contains(text)){
        auto & cg  = cachedG.getReference(text);
        cg.paint(g);
        return;
    }


    auto & cg  = cachedG.getReference(text);
    cg.setFont(getLogFont());
    cg.setText(text);
    cg.setSize(width, height);
    cg.paint(g);
#else
    g.setFont (getLogFont());

    g.drawText(text, 0, 0, width,height, Justification::left,true);
#endif

#endif
};

String LGMLLoggerUI::LogList::getCellTooltip (int rowNumber, int /*columnId*/)
{
    auto el = owner->getElementForRow(rowNumber);

    String sR = el->source;
    return
    (sR.isNotEmpty()?
     sR +" ("+el->time.toString(false, true, true, true)+")"+ "\n":"")
    +(el->getNumLines()< 10?el->content:owner->getSourceForRow(rowNumber));


};

#if USE_CACHED_GLYPH
void LGMLLoggerUI::LogList::cleanUnusedGlyphs(){
    int nminRow = owner->logListComponent->getRowContainingPosition (1,1);
    int nmaxRow = owner->logListComponent->getRowContainingPosition (1, owner->logListComponent->getHeight());
    if(nminRow==-1)return;
    if(nmaxRow==-1)nmaxRow = owner->totalLogRow.get();

    int min=0,max=0;
    if(nminRow>minRow){
        min = minRow;max=nminRow;
    }
    if(nmaxRow<maxRow){
        min = nmaxRow;max = maxRow;
    }

    for(int i = min; i < max ; i++){
        cachedG.remove(owner->getContentForRow (i));
        if(owner->isPrimaryRow(i)){
            cachedG.remove(owner->getSourceForRow (i));
            cachedG.remove(owner->getTimeStringForRow(i));
        }
    }

    minRow = nminRow;
    maxRow = nmaxRow;


}
#endif

void LGMLLoggerUI::buttonClicked (Button* b)
{

    if (b == &clearB)
    {
//        startDisplayedIdx = LGMLLogger::getInstance()->getNumLogs();
        LGMLLogger::getInstance()->clearLog();
        totalLogRow = 0;
        logListComponent->updateContent();
        LOG (juce::translate("Cleared."));
    }
    
    else if (b == &copyB){
        String s;
        for(int i = startDisplayedIdx ; i< LGMLLogger::getInstance()->getNumLogs() ; i++){
            const LogElement  * el = loggedElements().getUnchecked(i);
            s+=el->toNiceString();
        }
        SystemClipboard::copyTextToClipboard (s);
    }
}
#endif
