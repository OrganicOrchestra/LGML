/* Copyright © Organic Orchestra, 2017
 *
 * This file is part of LGML.  LGML is a software to manipulate sound in realtime
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation (version 3 of the License).
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 */
#if !ENGINE_HEADLESS

#include "ShapeShifterContainer.h"
#include "ShapeShifterManager.h"
#include "../Style.h"


ShapeShifterContainer::ShapeShifterContainer (Direction _direction) :
ShapeShifter (ShapeShifter::CONTAINER),
direction (_direction)
{
    setOpaque(true);
    setPaintingIsUnclipped(true);
    
}

ShapeShifterContainer::~ShapeShifterContainer()
{
    clear();
}

void ShapeShifterContainer::insertShifterAt (ShapeShifter* shifter, int index, bool resizeAfter)
{
    if (index == -1) index = shifters.size();

    shifters.insert (index, shifter);
    addAndMakeVisible (shifter);
    shifter->setParentContainer (this);

    if (shifters.size() > 1)
    {
        auto * gg = new GapGrabber (direction == HORIZONTAL ? GapGrabber::HORIZONTAL : GapGrabber::VERTICAL);
        grabbers.add (gg);
        addAndMakeVisible (gg);
        gg->addGrabberListener (this);
    }
    else{

        grabbers.clear();
    }

//    DBG ("Insert shifter at : " << shifter->getPreferredWidth());

    if (resizeAfter) {
        if(getMinWidth()> getWidth() || getMinHeight()>getHeight()){
            setSize(jmax<int> (getWidth(),getMinWidth()), jmax<int> (getHeight(),getMinHeight()));
        }
        else{
            resized();
        }
    }
    shifter->repaint();
}

void ShapeShifterContainer::removeShifter (ShapeShifter* shifter, bool deleteShifter, bool silent, bool resizeAfter)
{
    //DBG("Remove Shifter, deleteShifter ? " << String(deleteShifter) << ", silent " << String(silent));
    int shifterIndex = shifters.indexOf (shifter);
    shifters.removeAllInstancesOf (shifter);
    shifter->setParentContainer (nullptr);

    removeChildComponent (shifter);

    switch (shifter->shifterType)
    {
        case CONTAINER:
            ((ShapeShifterContainer*)shifter)->removeShapeShifterContainerListener (this);

            if (deleteShifter) delete shifter;

            break;

        case PANEL:
            ((ShapeShifterPanel*)shifter)->removeShapeShifterPanelListener (this);

            if (deleteShifter) ShapeShifterManager::getInstance()->removePanel (((ShapeShifterPanel*)shifter));

            break;
    }

    if (shifters.size() == 0)
    {
        //dispatch emptied container so parent container deletes it
        if (!silent) containerListeners.call (&ShapeShifterContainerListener::containerEmptied, this);
    }
    else
    {
        if (shifters.size() == 1 && !silent && parentShifterContainer != nullptr)
        {
            containerListeners.call (&ShapeShifterContainerListener::oneShifterRemaining, this, shifters[0]);
        }
        else
        {
            GapGrabber* gg = grabbers[ (jmin<int> (shifterIndex, grabbers.size() - 1))];
//            removeChildComponent (gg); will be called when component deletes
            grabbers.remove (grabbers.indexOf (gg), true);

            if (resizeAfter) resized();
        }
    }


}

ShapeShifterPanel* ShapeShifterContainer::insertPanelAt (ShapeShifterPanel* panel, int index, bool resizeAfter)
{
    insertShifterAt (panel, index, resizeAfter);
    panel->addShapeShifterPanelListener (this);
    return panel;
}

ShapeShifterPanel* ShapeShifterContainer::insertPanelRelative (ShapeShifterPanel* panel, ShapeShifterPanel* relativeTo, ShapeShifterPanel::AttachZone zone, bool resizeAfter)
{
    switch (zone)
    {
        case  ShapeShifterPanel::AttachZone::LEFT:
            if (direction == HORIZONTAL) insertPanelAt (panel, shifters.indexOf (relativeTo), resizeAfter);
            else movePanelsInContainer (panel, relativeTo, HORIZONTAL, false);

            break;

        case  ShapeShifterPanel::AttachZone::RIGHT:
            if (direction == HORIZONTAL) insertPanelAt (panel, shifters.indexOf (relativeTo) + 1, resizeAfter);
            else movePanelsInContainer (panel, relativeTo, HORIZONTAL, true);

            break;

        case  ShapeShifterPanel::AttachZone::TOP:
            if (direction == VERTICAL) insertPanelAt (panel, shifters.indexOf (relativeTo), resizeAfter);
            else movePanelsInContainer (panel, relativeTo, VERTICAL, false);

            break;

        case  ShapeShifterPanel::AttachZone::BOTTOM:
            if (direction == VERTICAL) insertPanelAt (panel, shifters.indexOf (relativeTo) + 1, resizeAfter);
            else movePanelsInContainer (panel, relativeTo, VERTICAL, true);

            break;

        case ShapeShifterPanel::AttachZone::NONE:
        case ShapeShifterPanel::AttachZone::CENTER:
            //@ben do we need to handle these?
            jassertfalse;
            break;


    }

    return panel;
}


ShapeShifterContainer* ShapeShifterContainer::insertContainerAt (ShapeShifterContainer* container, int index, bool resizeAfter)
{
    container->addShapeShifterContainerListener (this);
    insertShifterAt (container, index, resizeAfter);
    return container;
}


void ShapeShifterContainer::movePanelsInContainer (ShapeShifterPanel* newPanel, ShapeShifterPanel* containedPanel, Direction _newDir, bool secondBeforeFirst)
{
    int targetIndex = shifters.indexOf (containedPanel);
    removeShifter (containedPanel, false, true, false);

    auto * newContainer = new ShapeShifterContainer (_newDir);
    newContainer->insertPanelAt (containedPanel, 0, true);
    newContainer->insertPanelAt (newPanel, secondBeforeFirst ? -1 : 0, false);

//    newContainer->setPreferredWidth (containedPanel->getPreferredWidth());
//    newContainer->setPreferredHeight (containedPanel->getPreferredHeight());
    //    newPanel->repaint();
    //    newContainer->setBounds(getLocalBounds());


    DBG ("MovePanels " << containedPanel->contents[0]->contentName << "/" << containedPanel->getPreferredWidth() << " / " << newContainer->getPreferredWidth());

    insertContainerAt (newContainer, targetIndex);
    DBG ("After insertContainerAt : " << containedPanel->contents[0]->contentName << "/" << containedPanel->getPreferredWidth() << " / " << newContainer->getPreferredWidth());

}

bool ShapeShifterContainer::isFlexible()
{
    for (auto& p : shifters)
    {
        if (p->isFlexible()) return true;
    }

    return false;
}

class ShapeShifterResolver{
public:
    typedef ShapeShifterContainer::Direction Direction ;
    ShapeShifterResolver(Array<ShapeShifter*>  _s,int _totalSpace,Direction _direction):
    shifters(std::move(_s)),
    direction(_direction),
    totalSpace(_totalSpace){
    }

    void resolveAll(){
        if(totalSpace < (shifters.size())*minElemSize){
            jassertfalse;
        }

        if(shifters.size()==0){
            jassertfalse;
        }
        if(!resolveFlexible()){
            if(!resolveOverflows()){
                if(!resolveForced()){
                    jassertfalse;
                }
            }
        }
        jassert(getOverflow()==0);

    }

    bool resolveFlexible(){


        auto spaceDiff = getOverflow();
        if(spaceDiff!=0){
            int numFlexibleShifters = 0;
            for (auto p : shifters){if (p->isFlexible() ) numFlexibleShifters++;}
            bool cannotResolve = false;
            if(numFlexibleShifters>0){
                int spaceDiffPerShifter = spaceDiff / numFlexibleShifters;
                bool padRemain = (spaceDiff % numFlexibleShifters )!= 0;
                ShapeShifter * lastFlexible(nullptr);
                for (auto& p : shifters)
                {
                    if (p->isFlexible() )
                    {
                        auto nL = getLength(p)-spaceDiffPerShifter;
                        if(minElemSize>nL){
                            cannotResolve= true;
                            nL =jmax<int>(minElemSize,nL);
                        }
                        setLength(p,nL);
                        lastFlexible = p;
                    }
                }
                if(padRemain){
                    jassert(lastFlexible);
                    setLength(lastFlexible, getLength(lastFlexible)+1);
                }

                if(!cannotResolve){jassert(getOverflow()>=0);}
                spaceDiff = getOverflow();
                return spaceDiff==0;
            }

        }
        return false;
    }

    bool resolveOverflows(){
        auto spaceDiff = getOverflow();
        if(spaceDiff>0){
            int idx = 0;
            int curEdge = 0;
            float averageSize = totalSpace*1.0f/shifters.size();
            for (auto p : shifters)
            {
                auto cL = getLength(p);
                bool isLast = idx == shifters.size()-1;
                const auto maxEdge = totalSpace - (shifters.size()-idx-1)*minElemSize;
                jassert(maxEdge>0);
                if ( (curEdge+cL >= maxEdge ) || isLast)
                {
                    int bestEdge = static_cast<int>(averageSize * (idx + 1));
                    if(isLast){
                        cL = totalSpace - curEdge;
                    }
                    else if( bestEdge-curEdge >= minElemSize){
                        cL = bestEdge-curEdge;
                    }
                    else{
                        cL = maxEdge-curEdge;
                    }
                    jassert(cL>=minElemSize);
                    setLength(p, cL);
                }
                curEdge+=cL;
                idx++;
            }
            jassert(curEdge==totalSpace);
            spaceDiff = getOverflow();
        }

        return spaceDiff == 0;


    }

    bool resolveForced(){
        auto spaceDiff = getOverflow();
        jassert(spaceDiff!=0);
        ShapeShifter * p(nullptr);
        if(spaceDiff>0){
             p = getBigger();
        }
        else{
            p = getSmaller();
        }
        if(p){
            setLength(p,getLength(p) - spaceDiff);
            jassert(getLength(p)>=minElemSize);
            spaceDiff = getOverflow();
        }
        else{
            jassertfalse;

        }
        return spaceDiff == 0;

    }

private:
    bool isHorizontal() const{
        return direction==ShapeShifterContainer::HORIZONTAL;
    }
    ShapeShifter * getBigger(){
        ShapeShifter * res = nullptr;
        int maxCl = 0;
        for (auto p : shifters)
        { auto cL = getLength(p);
            if(cL>maxCl){
                res = p;
                maxCl = cL;
            }
        }
        return res;
    }

    ShapeShifter * getSmaller(){
        ShapeShifter * res = nullptr;
        int minCl = 0;
        for (auto p : shifters)
        { auto cL = getLength(p);
            if(cL<minCl || minCl==0){
                res = p;
                minCl = cL;
            }
        }
        return res;
    }
    int getLength(ShapeShifter *p) const{
        return isHorizontal() ? p->getPreferredWidth() : p->getPreferredHeight();
    }
    int getOverflow()const {

        return getUsedSpace() - totalSpace;
    }
    void setLength(ShapeShifter *p,int l){
        jassert(l>=minElemSize);
        if (isHorizontal()) p->setPreferredWidth (l);
        else p->setPreferredHeight (l);
    }

    int getUsedSpace() const{
        int res = 0;
        for(auto l:shifters){
            res+=getLength(l);
        }
        return  res;
    }
    int totalSpace;
    Array<ShapeShifter*>  shifters;
    Direction direction;
    const int minElemSize = ShapeShifter::minSize;
};

void ShapeShifterContainer::resized()
{
    if (shifters.size() == 0) return;

    if (parentShifterContainer == nullptr && shifters.size() == 1) //Main container, only one item
    {
        shifters[0]->setBounds (getLocalBounds());
        return;
    }

    int totalSpace = (direction == HORIZONTAL) ? getWidth() : getHeight();
    if (totalSpace==0)return;
    int numShifters = shifters.size();
    int totalSpaceWithoutGap = totalSpace - gap * (numShifters - 1);

    ShapeShifterResolver sr(shifters,totalSpaceWithoutGap,direction);

    sr.resolveAll();


    Rectangle<int> r = getLocalBounds();
    int index = 0;
    int lastNonMinimizedShifter = shifters.size()-1;
    constexpr int miniSize = ShapeShifterPanel::headerHeight;
    for (; lastNonMinimizedShifter>0 ; lastNonMinimizedShifter--)
    {
        if(!shifters[lastNonMinimizedShifter]->isMini)break;
    }
    int trailingMinimizedShifters = (shifters.size()-1-lastNonMinimizedShifter);

    for (auto& p : shifters)
    {
        bool isLastNonMinimizedShifter = index == lastNonMinimizedShifter;
        bool isLastShifter  =index ==shifters.size()-1;
        if (!isLastNonMinimizedShifter)
        {
            if (direction == HORIZONTAL) p->setBounds (r.removeFromLeft (p->isMini?miniSize:p->getPreferredWidth()));
            else p->setBounds (r.removeFromTop (p->isMini?miniSize:p->getPreferredHeight()));
        }
        else{
            auto nonMinimizedR = r;
            if (direction == HORIZONTAL)nonMinimizedR.removeFromRight(trailingMinimizedShifters*miniSize);
            else nonMinimizedR.removeFromBottom(trailingMinimizedShifters*miniSize);
            jassert(isMini || (nonMinimizedR.getWidth()>=p->getMinWidth() && nonMinimizedR.getHeight()>=p->getMinHeight()));
            p->setBounds(nonMinimizedR);
            if (direction == HORIZONTAL) r.removeFromLeft(nonMinimizedR.getWidth());
            else r.removeFromTop(nonMinimizedR.getHeight());
        }
        
        if(!isLastShifter){
        Rectangle<int> gr = (direction == HORIZONTAL) ? r.removeFromLeft (gap) : r.removeFromTop (gap);
        grabbers[index]->setBounds (gr);
        }

        index++;
    }

}

void ShapeShifterContainer::clear()
{
    while (shifters.size() > 0)
    {
        removeShifter (shifters[0], true, true);
    }
}

var ShapeShifterContainer::getCurrentLayout()
{
    var layout = ShapeShifter::getCurrentLayout();
    layout.getDynamicObject()->setProperty ("direction", (int)direction);

    var sData;

    for (auto& s : shifters)
    {
        sData.append (s->getCurrentLayout());
    }

    layout.getDynamicObject()->setProperty ("shifters", sData);
    return layout;
}

void ShapeShifterContainer::loadLayoutInternal (var layout)
{

    Array<var>* sArray = layout.getDynamicObject()->getProperty ("shifters").getArray();

    if (sArray != nullptr)
    {
        for (auto& sData : *sArray)
        {
            auto t = (ShapeShifter::Type) (int) (sData.getDynamicObject()->getProperty ("type"));

            if (t == PANEL)
            {
                ShapeShifterPanel* c = ShapeShifterManager::getInstance()->createPanel (nullptr);
                c->loadLayout (sData);
                insertPanelAt (c, -1, false);
            }
            else if (t == CONTAINER)
            {
                Direction dir = (Direction) (int)sData.getDynamicObject()->getProperty ("direction");
                auto * sc = new ShapeShifterContainer (dir);
                insertContainerAt (sc, -1, false);
                sc->loadLayout (sData);
            }
        }
    }

    //      resized();
}

void ShapeShifterContainer::grabberGrabUpdate (GapGrabber* gg, int dist)
{
    ShapeShifter* firstShifter = shifters[grabbers.indexOf (gg)];
    ShapeShifter* secondShifter = shifters[grabbers.indexOf (gg) + 1];

    switch (direction)
    {
        case HORIZONTAL:
            firstShifter->setPreferredWidth (firstShifter->getPreferredWidth() + dist);
            secondShifter->setPreferredWidth (secondShifter->getPreferredWidth() - dist);
            break;

        case VERTICAL:
            firstShifter->setPreferredHeight (firstShifter->getPreferredHeight() + dist);
            secondShifter->setPreferredHeight (secondShifter->getPreferredHeight() - dist);
            break;

        case NONE:
            jassertfalse;
            break;
    }



    resized();
}


void ShapeShifterContainer::panelDetach (ShapeShifterPanel* panel)
{
    Rectangle<int> panelBounds = panel->getScreenBounds();
    removeShifter (panel, false,false,false);
    ShapeShifterManager::getInstance()->showPanelWindow (panel, panelBounds);
}

void ShapeShifterContainer::panelEmptied (ShapeShifterPanel* panel)
{
    removeShifter (panel, true, false);
}

void ShapeShifterContainer::panelDestroyed (ShapeShifterPanel* panel)
{
    removeShifter (panel, false, true);
}

void ShapeShifterContainer::containerEmptied (ShapeShifterContainer* container)
{
    removeShifter (container, true);
}

void ShapeShifterContainer::oneShifterRemaining (ShapeShifterContainer* container, ShapeShifter* lastShifter)
{
    int containerIndex = shifters.indexOf (container);

    //DBG("Remove shifter from child container");
    container->removeShifter ((ShapeShifterContainer*)lastShifter, false, true, false);

    ShapeShifter* ss = nullptr;

    //DBG("Insert last shifter in parent container");
    if (lastShifter->shifterType == PANEL)  ss = insertPanelAt ((ShapeShifterPanel*)lastShifter, containerIndex, false);
    else ss = insertContainerAt ((ShapeShifterContainer*)lastShifter, containerIndex, false);
    
    ss->setPreferredWidth (container->getPreferredWidth());
    ss->setPreferredHeight (container->getPreferredHeight());
    
    //DBG("Remove useless container");
    removeShifter (container, true, true);
    
}

void ShapeShifterContainer::paintOverChildren(Graphics & ) {
    //    LGMLUIUtils::drawBounds(this,g);
    
}
void ShapeShifterContainer::paint(Graphics & g){

    LGMLUIUtils::fillBackground(this,g);

}

int ShapeShifterContainer::getPreferredWidth() {
    if(direction==HORIZONTAL)
        return jmax<int> (ShapeShifter::getPreferredWidth(),getMinWidth());

    return ShapeShifter::getPreferredWidth();
}
int ShapeShifterContainer::getPreferredHeight(){
    if(direction==VERTICAL)
        return jmax<int> (ShapeShifter::getPreferredHeight(),getMinHeight());

    return ShapeShifter::getPreferredHeight();
}

int ShapeShifterContainer::getMinWidth(){
    if(direction==HORIZONTAL)
        return shifters.size()*minSize + (shifters.size() - 1)*gap;

    return minSize;

}
int ShapeShifterContainer::getMinHeight(){
    if(direction==VERTICAL)
        return shifters.size()*minSize+ (shifters.size() - 1)*gap;

    return minSize;

}



#endif
