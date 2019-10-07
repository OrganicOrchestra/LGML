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

#pragma once


#include "../JuceHeaderUI.h"




class SwapComponents;

class StackedContainerBase : public Component{
public:
    explicit StackedContainerBase(int _minElemSize=20,bool isHorizontal=false,int _gap=2,int _padEnd=0):
    _isHorizontal(isHorizontal)
    ,gap(_gap)
    ,minElemSize(_minElemSize)
    ,padEnd(_padEnd){
        addMouseListener(this,true);
    }
    virtual ~StackedContainerBase(){}
    void clear();
    int getNumStacked() const;
    int getSize()const;
    int getFixSize()const;

    void updateSize();
    void mouseDown(const MouseEvent & e) override;
    void mouseDrag(const MouseEvent & e) override;
    void mouseUp(const MouseEvent & e) override;




    void resized() override;

    bool isHorizontal()const ;
    int getIdxForPos(Point<int> pos) ;


protected:
    virtual void swapElems(int iA,int iB);
    OwnedArray<Component> stackedUIs;
    bool _isHorizontal;
    int gap;
    int padEnd;
    int minElemSize;
    std::unique_ptr<Component> draggedUI;
private:
    void endDrag();
    void updateDrag(const MouseEvent & e);
    Component * getRelatedStackedComponent(Component *);
    friend class SwapComponents;
};


template<class UIT,class T>
class StackedContainerUI : public StackedContainerBase{
public:
    typedef std::function<T*(UIT*)> GetTFromUITTYPE;
    typedef std::function<void(int,int)> SwapElemsF;
    typedef std::function<int(const UIT*,const UIT*)> SortFType;
    typedef std::function<bool(UIT*)> FilterFType;
    StackedContainerUI(GetTFromUITTYPE f,SwapElemsF sw,int _minElemSize=20,bool isHorizontal=false,int _gap=2,int _padEnd=0):
    StackedContainerBase(_minElemSize,isHorizontal,_gap,_padEnd),
    toUIT(f),
    swapElemF(std::move(sw))
    {

    }
    virtual ~StackedContainerUI(){}
    FilterFType filterF;
    SortFType sortF;

    UIT * addFromT(T* obj){
        auto ui =new UIT(obj);
        addAndMakeVisible(ui);
        
        stackedUIs.add( ui);
        //ui->setPaintingIsUnclipped(false);
        filterAndSort();
        updateSize();
        resized();
        return ui;

    }
    void removeUI(UIT* ui){
        removeChildComponent (ui);
        stackedUIs.removeObject(ui);
        updateSize();
        resized();
    }

    void swapElems(int ia,int ib) override{
        StackedContainerBase::swapElems(ia,ib);
        swapElemF(ia,ib);
    }

    void setSortFunction(SortFType f){
        sortF = f;
        filterAndSort();
    }
    void setFilterFunction(FilterFType f){
        filterF = f;
        filterAndSort();
    }

    void removeFromT(T* obj){
        UIT * ui= getFromT(obj);
        if(ui){
            removeUI(ui);
        }
    }
    
    UIT * getFromT(T* obj){
        for (auto& o : stackedUIs){
            auto ot =dynamic_cast<UIT*>(o);
            if (toUIT(ot) == obj) return ot;
        }
        return nullptr;
    }

    void childBoundsChanged (Component* )override{updateSize();}

private:
    struct Sorter{
        Sorter(SortFType f):sortF(f){}
        int compareElements(const Component* a,const Component* b){
            return sortF(dynamic_cast<const UIT*>(a),dynamic_cast<const UIT*>(b));
        }
        SortFType sortF;
    };
    void filterAndSort(){

        if(sortF){
            auto ss =Sorter(sortF);
//            SortFunctionConverter<Sorter> sfc(ss);
            stackedUIs.sort(ss);
//            auto end = stackedUIs.end();
//            for (auto i = stackedUIs.begin(); i !=  end; ++i){
//                std::iter_swap(i, std::min_element(i, end,sortF));
//        }
            //std::sort((const UIT**)stackedUIs.begin(),(const UIT**)stackedUIs.end(),sortF);

        }
        if(filterF){
            for(auto o:stackedUIs){o->setVisible(filterF((UIT*)o));}
        }
        updateSize();
    }
    GetTFromUITTYPE toUIT;

    SwapElemsF swapElemF;
//    Array<Component*> visibleStackedUIs;




};

template<class UIT, class T>
class StackedContainerViewport : public Viewport{
public:
    explicit StackedContainerViewport(StackedContainerUI<UIT,T> * _stUI):
    stUI(_stUI)
    {

        setViewedComponent (stUI.get(), false);
        bool isH = stUI->isHorizontal();
        setScrollBarsShown (!isH,isH,!isH,isH);
        setScrollOnDragEnabled (false);
        setScrollBarThickness (10);
        setPaintingIsUnclipped(true);

    }
    virtual ~StackedContainerViewport(){}
    void resized() override{
        Viewport::resized();
        if(stUI->isHorizontal()){stUI->setSize(stUI->getWidth(),getHeight());}
        else                    {stUI->setSize(getWidth(),stUI->getHeight());}

    }


    UIT * addFromT(T* obj)      {return stUI->addFromT(obj);}
    UIT * getFromT(T* obj)      {return stUI->getFromT(obj);}
    int getNumStacked()         {return stUI->getNumStacked();}
    void removeUI(UIT* ui)      {stUI->removeUI(ui);}
    void removeFromT(T* obj)    {stUI->removeFromT(obj);}
    int getSize() const         {return stUI->getSize();}
    void clear()                {stUI->clear();}


    std::unique_ptr<StackedContainerUI<UIT, T> > stUI;

protected:


};
