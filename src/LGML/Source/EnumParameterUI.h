/*
  ==============================================================================

    EnumParameterUI.h
    Created: 29 Sep 2016 5:35:12pm
    Author:  bkupe

  ==============================================================================
*/

#ifndef ENUMPARAMETERUI_H_INCLUDED
#define ENUMPARAMETERUI_H_INCLUDED


#include "ParameterUI.h"
#include "EnumParameter.h"



class EnumParameterUI : public ParameterUI, public EnumParameter::Listener, public ComboBox::Listener
{
public:
    EnumParameterUI(Parameter * parameter = nullptr);
    virtual ~EnumParameterUI();

	EnumParameter * ep;
  

	ComboBox cb;

	void updateComboBox();
	String getCBSelectedKey();

	void resized() override;

  void setCanModifyModel(bool isModifiable,bool isFile);
  bool canModifyModel,isFileBased;
  enum Actions{
    addElementId = -1,
    removeElementId = -2,
    NoneId = -3
  };

	
	void enumOptionAdded(EnumParameter *, const Identifier &key) override;
	void enumOptionRemoved(EnumParameter *, const Identifier &key) override;
  void enumOptionSelectionChanged(EnumParameter *,bool isSelected,bool isValid, const Identifier &name) override;
	// Inherited via Listener
	virtual void comboBoxChanged(ComboBox *) override;
	
private:
	HashMap<int, String> idKeyMap;
	HashMap<String,int> keyIdMap;

  void selectString(const String & );

protected:
    void valueChanged(const var &) override ;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EnumParameterUI)

		
};

#endif  // ENUMPARAMETERUI_H_INCLUDED
