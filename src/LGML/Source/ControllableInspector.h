/*
  ==============================================================================

    ControllableInspector.h
    Created: 27 Mar 2016 11:58:52am
    Author:  Martin Hermant

  ==============================================================================
*/

#ifndef CONTROLLABLEINSPECTOR_H_INCLUDED
#define CONTROLLABLEINSPECTOR_H_INCLUDED



#include "ControllableContainerEditor.h"
#include "ControllableContainerSync.h"


// side bar component displaying editor of currently selected nodes
// TODO handle merging of multiple component of sameClass : almost there ....

class ControllableInspector:public SelectableComponentHandler::SelectableHandlerListener,
public Component,
public ControllableContainer::Listener,
public ControllableContainerSync::ContainerSyncListener

{
public:
    ControllableInspector(NodeManagerUI * _nmui);
    virtual ~ControllableInspector();

    void selectableChanged(SelectableComponent*  c,bool isSelected) override;
private:

    void addOrMergeControllableContainerEditor(ControllableContainer * c);
    void removeControllableContainerEditor(ControllableContainer * c);
    void generateFromCandidates();
   static void addControllableListenerToEditor(ControllableContainerEditor * source,ControllableContainer * listener);
    static bool hasSameControllableType(ControllableContainer * source,ControllableContainer * target);
    // create Controllables binded to UI that dispatch to multiple Controllables
    // each inspector UI element is binded to one Controllable in proxy,
    // if changed , it notifies All corresponding candidateControllable
    ScopedPointer<ControllableContainerSync> controllableContainerSync;


    ScopedPointer<ControllableContainerEditor> displayedEditor;
    Array<ControllableContainer *> candidateContainers;

    Array<Controllable*> removedControllables;
    Array<ControllableContainer*> removedContainers;


    void paint(Graphics &g) override;
    void resized()override;

    void controllableAdded(Controllable * c) override;
    void controllableRemoved(Controllable * c)override;
    void controllableContainerAdded(ControllableContainer * c)override ;
    void controllableContainerRemoved(ControllableContainer * c)override ;
    void controllableFeedbackUpdate(Controllable *) override;


    void sourceUpdated(ControllableContainer*) override;

    NodeManagerUI * nmui;



};

class ControllableInspectorViewPort:public Viewport{
public:
    ControllableInspectorViewPort(ControllableInspector * cI):Viewport("Inspector Viewport"),inspector(cI){
        setScrollBarsShown(true,true);
        setViewedComponent(cI,false);
    }


        void visibleAreaChanged (const Rectangle<int>&)override{
//            Point <int> mouse = getMouseXYRelative();
//            autoScroll(mouse.x, mouse.y, 100, 10);

        }
        void resized() override{
//            inspector->setSize(getWidth(),jmax(inspector->getHeight(),getHeight()));
//            if(getLocalBounds().contains(nmui->getLocalBounds())){
//                inspector->minBounds = getLocalBounds();
//            }
			inspector->setBounds(getLocalBounds());
        }


    void viewedComponentChanged(Component * )override{

    }
        ControllableInspector * inspector;


};


#endif  // CONTROLLABLEINSPECTOR_H_INCLUDED
