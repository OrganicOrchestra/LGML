/*
 ==============================================================================

 StackedContainerUI.h
 Created: 13 Mar 2018 1:24:39pm
 Author:  Martin Hermant

 ==============================================================================
 */

#pragma once

#include "../JuceHeaderUI.h"




class SwapComponents;
class StackedContainerBase : public Component{
public:
    StackedContainerBase(int _minElemSize=20,bool isHorizontal=false,int _gap=2):_isHorizontal(isHorizontal),gap(_gap),minElemSize(_minElemSize){
        addMouseListener(this,true);
    }
    virtual ~StackedContainerBase(){};
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
    int minElemSize;;
    ScopedPointer<Component> draggedUI;
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
    StackedContainerUI(GetTFromUITTYPE f,SwapElemsF sw,int _minElemSize=20,bool isHorizontal=false,int _gap=2):StackedContainerBase(_minElemSize,isHorizontal,_gap),
    toUIT(f),
    swapElemF(sw)

    {

    }


    UIT * addFromT(T* obj){
        auto ui =new UIT(obj);
        addAndMakeVisible(ui);
        stackedUIs.add( ui);
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



private:

    GetTFromUITTYPE toUIT;

    SwapElemsF swapElemF;




};

template<class UIT, class T>
class StackedContainerViewport : public Viewport{
public:
    StackedContainerViewport(StackedContainerUI<UIT,T> * _stUI):
    stUI(_stUI){

        setViewedComponent (stUI, false);
        bool isH = stUI->isHorizontal();
        setScrollBarsShown (!isH,isH,!isH,isH);
        setScrollOnDragEnabled (false);
        setScrollBarThickness (10);

    }

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


    ScopedPointer<StackedContainerUI<UIT, T> > stUI;

protected:



};
