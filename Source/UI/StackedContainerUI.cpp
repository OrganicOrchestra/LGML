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

#include "StackedContainerUI.h"
class SwapComponents : public DrawableImage{
public:
    SwapComponents(Component * originComp,StackedContainerBase * rootOfDrag, const Point<int> &_moffset):root(rootOfDrag),moffset(_moffset){
        auto draggedImg = originComp->createComponentSnapshot(originComp->getLocalBounds());
        draggedImg.multiplyAllAlphas(0.7f);
        setImage(draggedImg);
        root->addAndMakeVisible(this);

        idxOfDragged = rootOfDrag->getIdxForPos(originComp->getPosition());
        idxOfDest = -1;
        Path p;
        p.lineTo(0, 0);
        if(rootOfDrag->isHorizontal()){
            p.lineTo(0,rootOfDrag->getFixSize());
        }
        else{
            p.lineTo(rootOfDrag->getFixSize(),0);
        }
        targetSwapLine.setPath(p);
        targetSwapLine.setStrokeThickness(5);
        targetSwapLine.setStrokeFill(juce::FillType(Colours::red));
        root->addAndMakeVisible(targetSwapLine);

    }

    ~SwapComponents(){
        if(root.get()){
            root->removeChildComponent(this);
            root->removeChildComponent(&targetSwapLine);
        }
    }
    void mouseDrag(const MouseEvent & e) override{
        jassert(root);
        if(root){
            auto pos = e.getEventRelativeTo(root);
            setTopLeftPosition(pos.x,pos.y);
        }
    }
    void mouseUp(const MouseEvent & ) override{
        if(root.get()){
            auto sUI = dynamic_cast<StackedContainerBase*>(root.get());
            if(sUI){

            }
            root->removeChildComponent(this);
        }
    }
    WeakReference<Component>  root;
    DrawablePath targetSwapLine;
    int idxOfDragged,idxOfDest;
    Point<int> moffset;
};




void StackedContainerBase::clear(){
    stackedUIs.clear();
    updateSize();
}
int StackedContainerBase::getNumStacked() const{
    return stackedUIs.size();
}
int StackedContainerBase::getSize()const{
    int s = gap;
    for(auto o:stackedUIs){
        s+=jmax(minElemSize,_isHorizontal?o->getWidth():o->getHeight()) + gap;
    }
    s+=gap;
    s+=padEnd;
    return s;
};

int StackedContainerBase::getFixSize()const{
    return _isHorizontal?getHeight():getWidth();
};

void StackedContainerBase::updateSize(){
    int fixSize = getFixSize();
    int ts = jmax(minElemSize,getSize());
    if(_isHorizontal)setSize(ts,fixSize);
    else setSize(fixSize,ts);
}

void StackedContainerBase::swapElems(int iA,int iB){
    stackedUIs.swap(iA,iB);
    resized();
}

void StackedContainerBase::resized() {
    updateSize();
    int step = gap;
    int fixSize = _isHorizontal?getHeight():getWidth();
    for(auto o:stackedUIs){
        int curS=_isHorizontal?o->getWidth():o->getHeight();
        curS = jmax(minElemSize,curS);
        if(_isHorizontal){o->setBounds(step,0,curS,fixSize);}
        else{o->setBounds(0,step,fixSize,curS);}
        step+=curS+gap;
    }

}
int StackedContainerBase::getIdxForPos(Point<int> pos) {

    int i=0;
    for(auto o:stackedUIs){
        int ns = _isHorizontal?o->getRight():o->getBottom();
        if(ns>=(_isHorizontal?pos.x:pos.y)){
            return i;
        }
        i++;
    }
    return stackedUIs.size()-1;
}

bool StackedContainerBase::isHorizontal()const {return _isHorizontal;}

Component * StackedContainerBase::getRelatedStackedComponent(Component * c){
    for(auto cc:stackedUIs){
        if((cc == c )|| (dynamic_cast<Label*>(c) && cc->isParentOf(c))){
            return cc;
        }
    }
    return nullptr;
}
void StackedContainerBase::mouseDown(const MouseEvent & e) {
    auto oc = getRelatedStackedComponent(e.originalComponent);
    if(draggedUI==nullptr && oc){
        draggedUI = std::make_unique<SwapComponents>(oc,this,e.getEventRelativeTo(oc).getPosition());
    }

}
void StackedContainerBase::mouseDrag(const MouseEvent & e) {
    updateDrag(e);

}

void StackedContainerBase::mouseUp(const MouseEvent & e) {
    if(draggedUI!=nullptr){
        endDrag();
    }

}


void StackedContainerBase::updateDrag(const MouseEvent & ee){
    if(draggedUI!=nullptr){
        auto dui =dynamic_cast<SwapComponents*>(draggedUI.get());
        auto e = ee.getEventRelativeTo(this);
        int newIdx = getIdxForPos(e.getPosition());
        if(newIdx>=0){
            dui->targetSwapLine.setVisible(newIdx!=dui->idxOfDragged);
            if(newIdx!= dui->idxOfDest){
                Rectangle<int> ib = getLocalBounds();
                if(newIdx<stackedUIs.size())
                    ib =stackedUIs[newIdx]->getBounds();

                dui->targetSwapLine.setTopLeftPosition(newIdx>=dui->idxOfDragged?ib.getBottomLeft():ib.getPosition());
                dui->idxOfDest = newIdx;
            }

        }
        else{
            dui->targetSwapLine.setVisible(false);
        }

        draggedUI->setTopLeftPosition(_isHorizontal?(e.x-dui->moffset.x):0,
                                      _isHorizontal?0:(e.y-dui->moffset.y));

    }
}
void StackedContainerBase::endDrag(){
    if(draggedUI){
        auto dui =dynamic_cast<SwapComponents*>(draggedUI.get());
        if(dui){
            int db = dui->idxOfDragged;
            int di = dui->idxOfDest;
            if(di>db){
                for(int i = db+1;i<=di ; i++){swapElems(i,i-1);}
            }
            else{
                for(int i = db;i>di ; i--){swapElems(i,i-1);}
            }
        }
        draggedUI = nullptr;
    }
}
