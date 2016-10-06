/*
  ==============================================================================

    ProgressNotifier.h
    Created: 6 Oct 2016 2:36:31pm
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef PROGRESSNOTIFIER_H_INCLUDED
#define PROGRESSNOTIFIER_H_INCLUDED


class ProgressNotifier  {

public:
  ProgressNotifier(ProgressNotifier * _parentNotifier = nullptr):parentNotifier(_parentNotifier),task(0),progress(0){

  };
  virtual ~ProgressNotifier(){
    masterReference.clear();
  }
  ProgressNotifier * parentNotifier;


  int task;
  virtual int getTotalNumberOfTasks() = 0;

  virtual const String & getTaskNameForIdx(int task) {return String::empty;};
  float progress;
  class ProgressListener {
    public :

    ProgressListener(ProgressNotifier * notifier=nullptr):baseNotifier(notifier){
      if(notifier){
        notifier->addListener(this);
      }
    }
    virtual ~ProgressListener(){if(baseNotifier)baseNotifier->removeListener(this);}

    virtual void startedProgress(int task) {};
    virtual void endedProgress(int task) {};
    virtual void newProgress(int task,float p) {};


    // used for easy handling if only one notifier linked
    WeakReference<ProgressNotifier> baseNotifier;

  };



  struct StartCallbackMessage : public CallbackMessage{
  public:
    StartCallbackMessage(ListenerList<ProgressListener> * _n,int t):n(_n),task(t){}
    void messageCallback() override{n->call(&ProgressListener::startedProgress,task);}
    ListenerList<ProgressListener>* n;
    int task;
    };
  struct EndCallbackMessage : public CallbackMessage{
  public:
    EndCallbackMessage(ListenerList<ProgressListener> * _n,int t):n(_n),task(t){}
    void messageCallback() override{n->call(&ProgressListener::endedProgress,task);}
    ListenerList<ProgressListener>* n;
    int task;
  };

  struct ProgressCallbackMessage : public CallbackMessage{
  public:
    ProgressCallbackMessage(ListenerList<ProgressListener> * _n,int t,float p):n(_n),progress(p),task(t){}
    void messageCallback() override{n->call(&ProgressListener::newProgress,task,progress);}
    ListenerList<ProgressListener>* n;
    int task;
    float progress;
  };


  void startProgress(int task){(new StartCallbackMessage(&progressListeners,task))->post();fakeProgress = new FakeProgress(task,5000,this);}
 void endProgress(int task) {(new EndCallbackMessage(&progressListeners,task))->post();fakeProgress  = nullptr;}
  void setProgress(int task,float p) {(new ProgressCallbackMessage(&progressListeners,task,p))->post();}


  class FakeProgress : Timer{
    public :
    FakeProgress(int _task,float timeToComplete,ProgressNotifier * notif):notifier(notif),task(_task){
      const int period = 50;
      step = period*1.0/timeToComplete;
      curProgress = 0;
      startTimer(period);
    }
    void timerCallback(){
      curProgress+=step;
      notifier->setProgress( task,jmin(1.0f,curProgress));
    }
    ProgressNotifier * notifier;
    float step;
    float curProgress;
    int task;
  };

  ScopedPointer<FakeProgress> fakeProgress;

  ListenerList<ProgressListener> progressListeners;
  void addListener(ProgressListener* l){progressListeners.add(l);}
  void removeListener(ProgressListener* l){progressListeners.remove(l);}

  friend class WeakReference<ProgressNotifier>;
  WeakReference<ProgressNotifier>::Master masterReference;

  
};




#endif  // PROGRESSNOTIFIER_H_INCLUDED
