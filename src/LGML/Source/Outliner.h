/*
  ==============================================================================

    Outliner.h
    Created: 7 Oct 2016 10:31:23am
    Author:  bkupe

  ==============================================================================
*/

#ifndef OUTLINER_H_INCLUDED
#define OUTLINER_H_INCLUDED


#include "ShapeShifterContent.h"
#include "ControllableContainer.h"
#include "InspectableComponent.h"
#include "ControllableEditor.h"


class OutlinerItem;
class ParameterUI;
class OutlinerItemComponent : public  InspectableComponent, public SettableTooltipClient
{
public:
	OutlinerItemComponent(OutlinerItem * item);
	OutlinerItem * item;
	
	Label label;
  ScopedPointer<ParameterUI> paramUI;
	void paint(Graphics &g) override;
	void mouseDown(const MouseEvent &e) override;
  void resized()override;
	InspectorEditor * createEditor() override;
};

class OutlinerItem : public TreeViewItem
{
public:
	OutlinerItem(ControllableContainer * container);
	OutlinerItem(Controllable * controllable);

	bool isContainer;

	ControllableContainer * container;
	Controllable * controllable;


  String getUniqueName() const override;

  bool mightContainSubItems() override;

	Component * createItemComponent() override;
};

class Outliner : public ShapeShifterContentComponent,
				 private ControllableContainerListener,AsyncUpdater,TextEditorListener
{
public:

	Outliner(const String &contentName);
	~Outliner();

	TreeView treeView;
	ScopedPointer<OutlinerItem> rootItem;
  TextEditor filterTextEditor;
  String nameFilter;

	bool showHiddenContainers; //include or exclude in treeview the "skipInAddress" containers (may be later exposed to user as an option)

	void resized() override;
	void paint(Graphics &g) override;

  void textEditorTextChanged (TextEditor&)override;

	void rebuildTree();
	void buildTree(OutlinerItem * parentItem, ControllableContainer * parentContainer,bool shouldFilter=true);

	void childStructureChanged(ControllableContainer *,ControllableContainer*) override;
  void handleAsyncUpdate()override;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Outliner)

private:
  void saveCurrentOpenChilds();
  void restoreCurrentOpenChilds();
//  Array<String> uniqueOpenNames;
  ScopedPointer<XmlElement> xmlState;
  
};

#endif  // OUTLINER_H_INCLUDED
