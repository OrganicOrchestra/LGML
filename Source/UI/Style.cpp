/*
 ==============================================================================

 Copyright © Organic Orchestra, 2017

 This file is part of LGML. LGML is a software to manipulate sound in real-time

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation (version 3 of the License).

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

 ==============================================================================
 */

#if !ENGINE_HEADLESS

#include "Style.h"


/////////////////////
/// LGMLUIUtils
/////////////////////

template<class F,class... Args>
struct itercomp{
    static void doit(const F & func,Component *c , int idx,Args... args){
        if(!c ) return;
        func(c,idx,args... );
        for (auto & ch:c->getChildren()){
            doit(func,ch,idx+1,args...);
            jassert(ch && (ch->getParentComponent()==c));
        }
    }
};

static void _printComp(Component * c,int idx){
    if(!c->getName().isEmpty()){
        String msg;
        for(int i = 0; i  < idx ; i++){
            msg+="-";
        }

        msg+=c->getName();
        if(c->isVisible()){
            msg+="::";
            msg+=c->getScreenBounds().toString();
        }
        else{
            msg+="////////invisible";
        }
        if(c->getParentComponent() && !c->getScreenBounds().contains(c->getParentComponent()->getScreenBounds())){

            msg+= "/// excluded";
        }
        DBG(msg);
    }
}
static void _drawBounds(Component* c ,int idx,Graphics & g,Component * relativeTo){

    if(c->isVisible()){
        Rectangle<int >r = c->getLocalBounds();
        Component* p = c;
        while(p && p!=relativeTo){
            r+= p->getPosition();
            p = p->getParentComponent();
        }
        static float strk = 0.5f;
        g.drawRect(r.toFloat().reduced(idx*strk),strk*0.5f);
    }

}

static void _repaint(Component* c ,int /*idx*/){
    c->repaint();
}

Identifier bgIdentifier("bgDepth");
void LGMLUIUtils::markHasNewBackground(Component * c,int depth){
    c->getProperties().set(bgIdentifier,depth);
    c->setColour(LGMLColors::elementBackground,getCurrentBackgroundColor(c));

}

Colour LGMLUIUtils::getCurrentBackgroundColor(const Component *c){
    auto insp = c;
    int depth = 0;
    while(insp){
        int d = insp->getProperties().getWithDefault(bgIdentifier, -1);
        if(d>=0){
            depth =d;
            break;
        }
        insp = insp->getParentComponent();
    }
    return  getBackgroundColorForDepth(c->getLookAndFeel().findColour(LGMLColors::elementBackground),depth);

}


Colour LGMLUIUtils::getBackgroundColorForDepth( const Colour   baseColor,int depth){
    //    const auto amp = 0.15;
    //    while(depth>=5){
    //        depth-=5;
    //    };
    //    float brightness = amp*(1.0 - depth/5.0);
    //    if(depth%2==0){ brightness*=-1; }
    //    brightness+=amp+0.1;
    if(depth%2==0){
        return baseColor.withAlpha(1.f).brighter(.25);
    }
    //    return Colours::red.interpolatedWith(Colours::green, brightness).withAlpha(1.f);
    return baseColor.withAlpha(1.f);
    //    return baseColor.withBrightness(brightness).withAlpha(1.f);
}

void LGMLUIUtils::optionallySetBufferedToImage(Component * c, bool l){
    if(0){

    }
    else{
        c->setBufferedToImage(l);
    }
}

void LGMLUIUtils::fillBackground(const Component *c,Graphics & g){
    g.setColour (getCurrentBackgroundColor(c).withAlpha(1.f));
    g.fillRect (c->getLocalBounds());
    
}

void LGMLUIUtils::printComp(Component * c) {
    itercomp<decltype(_printComp)>::doit(_printComp,c,0);
}

void LGMLUIUtils::drawBounds(Component * c,Graphics & g) {
    g.setColour(juce::Colours::red);
    itercomp<decltype(_drawBounds),Graphics &,Component*>::doit(_drawBounds,c,0,g,c);
}
void LGMLUIUtils::forceRepaint(Component * c) {
    itercomp<decltype(_repaint)>::doit(_repaint,c,0);
}




////////////////////////
// AddElementButton
///////////////////////

AddElementButton::AddElementButton(): DrawableButton ("Add",DrawableButton::ButtonStyle::ImageFitted) {
    setImages(createDrawable(),createDrawable(true));
    setPaintingIsUnclipped(true);
    setOpaque(false);
    LGMLUIUtils::optionallySetBufferedToImage(this);

};
AddElementButton::~AddElementButton(){

};
void AddElementButton::parentHierarchyChanged(){
    repaint();// for background
}
void AddElementButton::paint(Graphics & ){
    //    LGMLUIUtils::fillBackground(this,g);
}

class ElemDrawables : public DeletedAtShutdown{
public:
    ElemDrawables(Colour _crossColour,Colour _circleColour,int _crossRotation=0):
    crossColour(_crossColour)
    ,circleColour(_circleColour)
    ,crossRotation(_crossRotation){
        normal = build(false,_crossColour, _circleColour,_crossRotation);
        hovered  = build(true,_crossColour, _circleColour,_crossRotation);

    }

    static std::unique_ptr<DrawableComposite> build(bool isHovered,Colour crossColour,Colour circleColour,int rotation){
        //    auto area = getLocalBounds();
        auto color =circleColour;
        float scale= 25;
        if(isHovered){
            color = color.brighter();
        }
        auto bgColor = Colours::transparentWhite;
        float thickness = .05f*scale;
        Path circlePath;
        circlePath.addEllipse(0, 0, 1*scale, 1*scale);

        Path crossPath;
        float padOut = thickness + .1f*scale;
        float width = thickness/2.0f;
        crossPath.addRoundedRectangle(padOut, .5f*scale-width/2.0f, 1.0f*scale-2.0f*padOut, width, width/4.0f);
        crossPath.addRoundedRectangle( .5f*scale-width/2.0f,padOut, width,1.0f*scale-2.0f*padOut, width/4.0f);
        auto dp = std::make_unique< DrawableComposite>();
        dp->setBoundingBox({0,0,1*scale,1*scale});

        auto * cd = new DrawablePath();
        cd->setPath(circlePath);
        cd->setFill(FillType(color));
        cd->setStrokeFill(FillType(color));
        cd->setStrokeThickness(thickness);

        color = crossColour;
        auto * crd = new DrawablePath();
        crd->setFill(FillType(bgColor));
        crd->setStrokeFill(FillType(color));
        crd->setStrokeThickness(thickness);
        crd->setPath(crossPath);
        crd->setTransform(AffineTransform::rotation((float)(rotation/360.0*2.0*float_Pi),scale/2,scale/2));
        
        dp->addAndMakeVisible(cd);
        dp->addAndMakeVisible(crd);

        return dp;
    }
    std::unique_ptr<DrawableComposite> normal,hovered;
    Colour crossColour;
    Colour circleColour;
    int crossRotation;
};
Drawable * AddElementButton::createDrawable(bool isHovered){
    static ElemDrawables * el (nullptr);
    if(!el){el = new ElemDrawables(Colours::white,Colours::green,0);}

    return !isHovered?el->normal.get():el->hovered.get();
}


void AddElementButton::setFromParentBounds (const Rectangle<int>& area)
{
    auto fmb = area;
    const int fms = jmin (jmin (36, area.getWidth()), area.getHeight());
    setBounds (fmb.removeFromBottom (fms).removeFromRight (fms));
}


////////////////////////
// RemoveElementButton
///////////////////////


RemoveElementButton::RemoveElementButton():
DrawableButton ("Remove",DrawableButton::ButtonStyle::ImageFitted) {
    setImages(createDrawable(),createDrawable(true));
    setPaintingIsUnclipped(true);
    setOpaque(false);
    LGMLUIUtils::optionallySetBufferedToImage(this);

};


RemoveElementButton::~RemoveElementButton(){
};

void RemoveElementButton::paint(Graphics & g){
    LGMLUIUtils::fillBackground(this,g);

}
ElemDrawables * RemoveElementButton::getDrawables(){
    static ElemDrawables * el (nullptr);
    if(!el){el = new ElemDrawables(Colours::white,Colours::red,45);}
    return el;
}

Drawable * RemoveElementButton::createDrawable(bool isHovered){
    auto el = getDrawables();
    return !isHovered?el->normal.get():el->hovered.get();
}

void RemoveElementButton::parentHierarchyChanged(){
    repaint();// for background
              //    auto centre = getLocalBounds().toFloat().getCentre();
              //    getDrawables()->rotate(45,centre);
}

//void RemoveElementButton::paintButton (Graphics& g,
//                                       bool isMouseOverButton,
//                                       bool isButtonDown)
//{
//    g.addTransform(AffineTransform::rotation(float_Pi/4.0f,getWidth()/2.0f,getHeight()/2.0f));
//
//    auto area = getLocalBounds();
//    auto bgColor = findColour (TextButton::buttonColourId);
//
//    if((isButtonDown || isMouseOverButton) ){
//        bgColor = bgColor.brighter();
//    }
//    g.setColour (  bgColor);
//    const float stroke = 1;
//    g.drawEllipse (area.toFloat().reduced (stroke / 2), stroke);
//    g.setColour (Colours::red);
//    const float hw = stroke;//area.getHeight()/18.0;
//    const float offset = area.getWidth() / 4.0f ;
//
//    const float corner = hw;
//
//    g.fillRoundedRectangle (area.getX() + offset, area.getCentre().getY() - hw, area.getWidth() - 2 * offset, 2 * hw, corner);
//    g.fillRoundedRectangle ( area.getCentre().getX() - hw, area.getY() + offset, 2 * hw, area.getHeight() - 2 * offset,  corner);
//
//}


/////////////////////
// CachedGlyph
///////////////


class GlobalFonts: private DeletedAtShutdown{
public:
    ReferenceCountedArray<CachedGlyph::CachedFont> list;
};

auto usedFonts = new GlobalFonts();


CachedGlyph::CachedGlyph(const String &t,const Font & font)://,const String & fontName):
text(t),
useEllipsesIfTooBig(false),
justificationType(Justification::left)
{
    setFont(font);
}

void CachedGlyph::setSize(int w, int h){
    bounds.setSize(w, h);
    updateGlyph();
}

void CachedGlyph::setText(const String & t){
    text = t;
    updateGlyph();

}

void CachedGlyph::setFont(const Font &f){

    for(auto a : usedFonts->list){
        if(a->font==f){
            _font = a;
            return;
        }
    }
    CachedGlyph::CachedFont::Ptr nf ( new CachedGlyph::CachedFont(f));
    usedFonts->list.add(nf);
    _font = nf;
    


}

void CachedGlyph::updateGlyph(){
    if(isReady()){
        auto area = getLocalBounds();
        
        glyphArr.clear();
        glyphArr.addCurtailedLineOfText (*getCurFont(), text, 0.0f, 0.0f,
                                         (float)area.getWidth(), useEllipsesIfTooBig);

        glyphArr.justifyGlyphs (0, glyphArr.getNumGlyphs(),
                                area.getX(), area.getY(), area.getWidth(), area.getHeight(),
                                justificationType);
    }
}
bool CachedGlyph::isReady(){
    return  _font!=nullptr ;
}
Rectangle<int> & CachedGlyph::getLocalBounds(){
    return bounds;

}
Font * CachedGlyph::getCurFont(){
    return &_font->font;
}
void CachedGlyph::paint(Graphics & g){
    if(!isReady()){
        updateGlyph();
    }
    //    auto area = getLocalBounds();
    
    if (text.isNotEmpty())// && g.context.clipRegionIntersects (area.getSmallestIntegerContainer()))
    {
        glyphArr.draw (g);
    }
}

CachedGlyph::CachedFont::CachedFont(const Font &f):font(f){

}
CachedGlyph::CachedFont::~CachedFont(){

}

#endif
