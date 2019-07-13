/*
  ==============================================================================

    CachedNameLabel.cpp
    Created: 12 Jul 2019 4:17:51pm
    Author:  Martin Hermant

  ==============================================================================
*/



class CachedNameLabel : public Label,public ParameterUI::Listener{
public:
    CachedNameLabel(ParameterUI * _pui):pui(_pui){
        LGMLUIUtils::optionallySetBufferedToImage(this);
        Label::setJustificationType(Justification::centred);

        setInterceptsMouseClicks(false, false);
        pui->paramUIListeners.add(this);
        updateText();
        setOpaque(false);
        setPaintingIsUnclipped(true);
        Label::setBorderSize({0,0,0,0});

    }
    ~CachedNameLabel(){
        pui->paramUIListeners.remove(this);

    }

    void displayedTextChanged(ParameterUI *pui ) final{
        updateText();
    }
    void resized()override{
        updateText();
        Label::resized();
    }
private:
    void updateText(){

        if(pui){
            auto cFont = getFont();
            int criticalSize = jmin(getLocalBounds().getWidth(),getLocalBounds().getHeight());
            int fontHeight = jmin(13,jmax(1,criticalSize));
            if(cFont.getHeight()!=fontHeight){
                setFont(cFont.withHeight(fontHeight));
            }
            Label::setText(pui->getDisplayedText(),juce::dontSendNotification );
        }
        else{
            jassertfalse;
            Label::setText("no controllable",juce::dontSendNotification );
        }
    }

    ParameterUI * pui;

};
