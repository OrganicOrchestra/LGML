/*
  ==============================================================================

    QAction.h
    Created: 11 Oct 2019 1:25:23pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once
#include "../JuceHeaderCore.h"
//#include "TimeManager.h"

struct QAction{
    QAction(double _beatToExec){
        setNewBeat(_beatToExec);
    }
    virtual ~QAction(){}
    typedef enum{
        DONE,
        TODO,
        REVOKED,
        IDLE
    } CheckResult;

    CheckResult check(double beat){
        if(revoked){revoked = false;return REVOKED;}
        if(done){return DONE;}
        if(beat>=beatToExec){
            done = true;
            return TODO;
        }
        return IDLE;
    }

    void setNewBeat(double _beatToExec){
        done = _beatToExec>beatToExec;
        beatToExec = _beatToExec;

    }
    void revoke(){
        revoked=true;
    }
//    virtual void revoke() = 0;
//    virtual void execute() = 0;
    double beatToExec;
    bool done = false;
    bool revoked = false;
};
//
//struct QFAction : public QAction{
//    typedef  std::function<void()> RevokeFType;
//    typedef  std::function<void()> ExecuteFType;
//    QFAction(double _beat,ExecuteFType ex,RevokeFType rev):QAction(_beat),exF(ex),revF(rev){}
//    void revoke() {revF();}
//    void execute() {exF();}
//    ExecuteFType exF;
//    RevokeFType revF;
//
//    };

//class QActionList{
//    typedef Array<QAction> QActionContener;
//    constexpr static int maxAddedActions = 100;
//
//    QActionList():actionFifo(maxAddedActions){
//        addedActions.resize(maxAddedActions);
//        sortedActions.ensure
//
//    }
//    void addAction(QAction && a){
//        int i1,l1,i2,l2;
//        actionFifo.prepareToWrite(1,i1,l1,i2,l2);
//        addedActions.setUnchecked(i1, a);
//        actionFifo.finishedWrite(1);
//    }
//
//    bool getNewActions(){
//        auto nR = actionFifo.getNumReady();
//        int i1,l1,i2,l2;
//        actionFifo.prepareToRead(nR,i1,l1,i2,l2);
//        for(int i = i1 ; i  < l1 ; i++){}
//        for(int i = i2 ; i  < l2 ; i++){}
//        actionFifo.finishedRead(nR);
//        return  nR>0;
//    }
//
//
//
//    QActionContener addedActions;
//    QActionContener sortedActions;
//    AbstractFifo actionFifo;
//    CriticalSection section;
//};
