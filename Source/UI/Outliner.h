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


#ifndef OUTLINER_H_INCLUDED
#define OUTLINER_H_INCLUDED


#include "ShapeShifter/ShapeShifterContent.h"
#include "../Controllable/Parameter/ParameterContainer.h"
#include "Inspector/InspectableComponent.h"
#include "../Controllable/UI/ControllableEditor.h"


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
	OutlinerItem(ParameterContainer * container);
	OutlinerItem(Parameter * controllable);

	bool isContainer;

	ParameterContainer * container;
	Parameter * parameter;


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
	void buildTree(OutlinerItem * parentItem, ParameterContainer * parentContainer,bool shouldFilter=true);

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
