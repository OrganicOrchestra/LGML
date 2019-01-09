/*
  ==============================================================================

    FileParameterUI.cpp
    Created: 2 Dec 2018 3:26:27pm
    Author:  Martin Hermant

  ==============================================================================
*/

#include "FileParameterUI.h"
#include "../../../Engine.h"
#include "StringParameterUI.h"

#include "../../../UI/Style.h" // for addElementButton
void createFile(FileParameter* fp);
void openFile(FileParameter* fp);

FileParameterUI::FileParameterUI (FileParameter* p): ParameterUI (p)
,options("options",DrawableButton::ButtonStyle::ImageFitted)
,hoveredByFile(false)
{
    options.addListener(this);
    options.setImages(AddElementButton::createDrawable());
    addAndMakeVisible(options);
    valueChanged(p->value);
    fnComponent  = new StringParameterUI(p);
    fnComponent->trimStart = true;
    addAndMakeVisible (fnComponent);
    p->addFileListener(this);
    addAndMakeVisible(errorLed);
    loadingEnded(p);


}

FileParameterUI::~FileParameterUI(){
    if(auto * p = getFileParameter())
        p->removeFileListener(this);
}



void FileParameterUI::resized()
{
    auto r =getLocalBounds();
    options.setBounds(r.removeFromLeft(r.getHeight()));
    if(auto fp = getFileParameter()){
        if(!fp->hasValidPath(true) || fp->getLoadingState()==LOADED_WITH_ERROR){
            errorLed.setBounds(r.removeFromLeft(10));
        }

    }
    fnComponent->setBounds (r);

}

const File  FileParameterUI::getCurrentFile() const{
    if(auto * p = getFileParameter()){
        return p->getFile();
    }
    return File();

}
void FileParameterUI::buttonClicked (Button*) {
    PopupMenu menu;
    auto * fp = getFileParameter();

    if(fp->hasValidPath(false)){
        menu.addItem(3, "reload");
        menu.addItem(4, "show");
        menu.addItem(5, "watch",true,getFileParameter()->isWatching);
        menu.addSeparator();
    }
    menu.addItem(1,"create");
    menu.addItem(2,"open");

    auto res = menu.show();
    switch(res){
        case 1:
            createFile(getFileParameter());
            break;
        case 2:
            openFile(getFileParameter());
            break;
        case 3:
            if(auto * fp =getFileParameter())
                fp->triggerFileChange();
            break;
        case 4:
            if (getCurrentFile().existsAsFile())
                getCurrentFile().startAsProcess();
            if(auto * fp =getFileParameter())
                fp->setIsWatching(true);
            break;
        case 5:
            if(auto * fp =getFileParameter())
                fp->setIsWatching(!getFileParameter()->isWatching);
            break;

    }

}



void FileParameterUI::watchStateChanged(FileParameter * fp) {
    options.setToggleState(fp->isWatching, dontSendNotification);
};


void FileParameterUI::loadingStarted(FileParameter * ){

};
void FileParameterUI::loadingEnded(FileParameter * fp){

    int size = 10;
    Path circle;
    circle.addEllipse (Rectangle<float> (0, 0, (float)size, (float)size));
    errorLed.setPath (circle);

    if(!fp->hasValidPath(false)){

    }
    if(fp->getLoadingState()==LOADED  || fp->stringValue().isEmpty()){
        errorLed.setVisible(false);


    }
    else{
        errorLed.setVisible(true);


    }
    errorLed.setFill (FillType (!fp->hasValidPath(true)? Colours::red:
                                  (fp->getLoadingState()==LOADED_WITH_ERROR ? Colours::orange :
                                   Colours::green)));

    resized();

};

// TODO add auto show

void show(FileParameter * fp){
    fp->getFile().startAsProcess();
}

ScopedPointer<FileChooser> getFcForFp(FileParameter * fp,String && name){
    return new FileChooser (name,//    const String& chooserBoxTitle,
                   getEngine()->getCurrentProjectFolder(),//    const File& currentFileOrDirectory,
                   fp->getAllowedExtensionsFilter(true),//    const String& fileFilters,
                   true,//    const bool useNativeBox,
                   false,//    const bool treatFilePackagesAsDirectories,
                nullptr);//    Component* parentComponentToUse
    

}

void openFile(FileParameter *fp){
    auto fc = getFcForFp(fp,juce::translate(String("open 123 file").replace("123", fp->getFullTypeName())));
    if(fc->browseForFileToOpen()){
        File file = fc->getResult();
        if(file.exists()){
            fp->setFile(file);
        }
    }
}

void createFile(FileParameter *fp){
    if(!getEngine()->getCurrentProjectFolder().exists()){
        auto fc = getFcForFp(fp,juce::translate(String("save 123 file").replace("123", fp->getFullTypeName())));
        auto res = fc->browseForFileToSave(true);
        if(res){
            File file = fc->getResult();
            Result r = file.create();
            if(!r){LOGE(r.getErrorMessage());}
            else{
                fp->setFile( file);
                show(fp);
            }
        }
        else{
            return;
        }
    }
    else{
        AlertWindow nameWindow (juce::translate("Create File"), juce::translate("name your file"), AlertWindow::AlertIconType::NoIcon);
        nameWindow.addTextEditor ("Name", "MyScript");

        nameWindow.addButton ("OK", 1, KeyPress (KeyPress::returnKey));
        nameWindow.addButton ("Cancel", 0, KeyPress (KeyPress::escapeKey));

        int result = nameWindow.runModalLoop();

        if (result)
        {
            String sN = nameWindow.getTextEditor("Name")->getText();
            if(sN.isNotEmpty()){
                if( sN.endsWith(".js")){
                    sN= sN.substring(0, -2);
                }
                File scriptFile = getEngine()->getCurrentProjectFolder().getChildFile("Scripts/");

                if(! scriptFile.exists()){
                    scriptFile.createDirectory();
                }
                scriptFile = scriptFile.getNonexistentChildFile(sN,".js",true);
                if(! scriptFile.exists()){
                    auto r = scriptFile.create();
                    if(!r){
                        LOGE(juce::translate("Can't create script : ") <<scriptFile.getFullPathName() << "("<< r.getErrorMessage()<<")");
                    }
                }
                else{
                    jassertfalse;
                }

                if(scriptFile.exists()){
                    fp->setFile(scriptFile);
                    show(fp);
                }
            }
            
        }
    }
}

bool FileParameterUI::isInterestedInFileDrag (const StringArray& files) {
    if(files.size()==1){
        File  f (files[0]);
        if(f.exists()){
            return f.hasFileExtension(getFileParameter()->getAllowedExtensionsFilter(false));
        }

    }
    return false;
};

void FileParameterUI::fileDragEnter (const StringArray& files, int x, int y){
    hoveredByFile = true;
    repaint();
};

void FileParameterUI::fileDragExit (const StringArray& files){
    hoveredByFile = false;
    repaint();
};
void FileParameterUI::filesDropped (const StringArray& files, int x, int y) {
    jassert(isInterestedInFileDrag(files));
    getFileParameter()->setFile(files[0]);
    hoveredByFile=false;
    repaint();
};

void FileParameterUI::paintOverChildren(Graphics & g) {
    if(hoveredByFile){
        auto r = getLocalBounds();
        g.setColour(findColour(TextButton::buttonOnColourId));
        g.drawRect(r);
    }
    
    
}

