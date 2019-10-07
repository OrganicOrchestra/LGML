/*
 ==============================================================================

 FileParameterUI.h
 Created: 2 Dec 2018 3:26:27pm
 Author:  Martin Hermant

 ==============================================================================
 */

#pragma once
#include "../FileParameter.h"
#include "ParameterUI.h"

class StringParameterUI;
class FileParameterUI : public ParameterUI,public FileDragAndDropTarget,private Button::Listener,private FileParameter::Listener{
public:
    FileParameterUI (FileParameter* p);
    ~FileParameterUI();


    void resized()override;

    FileParameter * getFileParameter() const{return dynamic_cast<FileParameter*>(parameter.get());}
    const File  getCurrentFile() const;


private:



    bool isInterestedInFileDrag (const StringArray& files) override;

    void fileDragEnter (const StringArray& files, int x, int y)override;
    void fileDragExit (const StringArray& files)override;
    void filesDropped (const StringArray& files, int x, int y) override;
    bool hoveredByFile = false;

    void paintOverChildren(Graphics & g) override;
    void buttonClicked (Button*) override;
    std::unique_ptr<StringParameterUI> fnComponent;
    DrawableButton options;
    DrawablePath  errorLed;

    /// FileParameter Listener
    void watchStateChanged(FileParameter *) override;
    void loadingStarted(FileParameter * )override;
    void loadingEnded(FileParameter * )override;
    
    
    
};
