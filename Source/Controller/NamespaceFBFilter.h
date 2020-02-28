/*
  ==============================================================================

    ControllerFBFilter.h
    Created: 2 Feb 2020 10:36:54pm
    Author:  Martin Hermant

  ==============================================================================
*/

#pragma once

#include <regex>
#include "../JuceHeaderCore.h"


class NamespaceFBFilter{

    struct NamespaceRule{
        struct FilterElement{
            virtual ~FilterElement(){}
            virtual bool check(const Identifier &i)const =0;
            String oriString = "";
        };
        struct IdentifierFilter:public FilterElement{
            IdentifierFilter(Identifier && i ):el(i){}
            bool check(const Identifier & i)const{return el==i;}
            Identifier el;
        };
        struct PureWildcard : public FilterElement{
            PureWildcard(){}
            bool check(const Identifier & )const {return true;}
        };
        struct DoubleWildcard : public FilterElement{
            DoubleWildcard(){}
            bool check(const Identifier & )const {return true;}
        };
        struct RegexFilter:public FilterElement{
            RegexFilter(std::regex & r ,String _ori):el(r),ori(_ori){}
            bool check(const Identifier & i)const{return std::regex_match (i.toString().toRawUTF8(),el);}
            std::regex el;
            String ori;
        };
        NamespaceRule(OwnedArray<FilterElement> & _filterEl,const String & _originalString,int _indent):originalString(_originalString),indent(_indent),_endsWithWildcard(false){
            elements.swapWith(_filterEl);
            jassert(elements.size());
            if(auto last = elements.getLast()){
                if(dynamic_cast<DoubleWildcard*>(last)){
                    _endsWithWildcard = true;
                }
            }
        }
        void addSubRule(NamespaceRule * r){
            if(endsWithDoubleWildCard()){
                LOGE("can't add sub rule to rule that ends with doubleWildCard");
                delete r;
            }
            else{
            subRules.add(r);
            r->parentRule = this;
            }
        }
        NamespaceRule * getThisOrParentWithIndent(int i){
            if(i==indent){return this;}
            auto p = parentRule;
            while(p){
                if(p->indent==i){break;}
                p = p->parentRule;
            }
            return p;
        }
        bool hasSameBase(NamespaceRule * other){
            if(other->elements.size()!=elements.size()){return false;}
            for(int i = 0 ; i <elements.size() ; i++ ){
                if(other->elements.getUnchecked(i)->oriString!=elements.getUnchecked(i)->oriString){
                    return false;
                }
            }
            return true;
        }
        static NamespaceRule * fromString(const String & s,int indent){

            OwnedArray<FilterElement> filterEl;
            auto sa = StringArray::fromTokens(s, juce::StringRef ("/"), juce::StringRef ("\""));
            sa.removeEmptyStrings();
            for(auto e:sa){
                FilterElement * addedEl = nullptr;
                if(e.containsAnyOf("*") || e.startsWith("[re]")){
                    if(e=="*"){
                        addedEl = new PureWildcard();
                    }
                    else if(e=="**"){
                        addedEl = new DoubleWildcard();
                    }
                    else{
                        try
                        {
                            auto validS = e.toLowerCase();
                            if(e.startsWith("[re]")){
                                validS = validS.substring(4);
                            }
                            else{
                                validS = validS.replace("*", ".*");
                            }
                            std::regex re(validS.toRawUTF8());
                            addedEl = new RegexFilter(re,validS);
                        }
                        catch (const std::regex_error& e)
                        {
                            LOGE(e.what());
                        }
                    }
                }
                else {
                    if(e.isNotEmpty()){
                        addedEl = new IdentifierFilter(Identifier(e.toLowerCase()));
                    }
                    else{
                        LOGE("empty filter element");

                    }
                }
                if(addedEl){
                    addedEl->oriString = e;
                    filterEl.add(addedEl);
                }
                else{
                    return nullptr;
                }
            }
            if(filterEl.size()>0){
                return new NamespaceRule(filterEl,s,indent);
            }

            return nullptr;
        }
        bool checkRule(const ControlAddressType & addr) const{
            // if address is smaller than elements it'll never be true
            if(addr.size()<elements.size()){
                return false;
            }
            const auto & addrArr = addr.getArray();
            int commonPart = elements.size();
            // discards if not in current namespace
            for(int i = 0 ; i < commonPart ; i++){
                auto rule = elements.getUnchecked(i);
                auto spart = addrArr.getUnchecked(i);
                if(!rule->check(spart)){
                    return false;
                }
            }
            // all rules where true;
            if(addr.size()==elements.size()){
                return true;
            }
            // if any subnamespace is validated, this rule is true
            // here addr.size IS > element.size()  if(addr.size()>elements.size()){
            auto subAddr = addr.subAddr(elements.size());
            for(auto r:subRules){
                if(r->checkRule(subAddr)){
                    return true;
                }
            }

            // no subrules are true,  namespace is correct, but address represent a child
            // this rule is true only if it ends with a double wildcard
            if(subRules.size()>0){
                return false;
            }
            return endsWithDoubleWildCard();
        }

        bool checkHasRuleForContainer(const ControlAddressType & addr)const{
            // if address is smaller than elements it can be true if common part is valid
            if(addr.size()<=elements.size()){
                const auto & addrArr = addr.getArray();
                int commonPart = addr.size();
                // discards if not in current namespace
                for(int i = 0 ; i < commonPart ; i++){
                    auto rule = elements.getUnchecked(i);
                    auto spart = addrArr.getUnchecked(i);
                    if(!rule->check(spart)){
                        return false;
                    }
                }
                // we still have rules for potential childs 
                return true;
            }
            else{ //if(addr.size()>elements.size()){
            // if any subnamespace is validated, this rule is true
                auto subAddr = addr.subAddr(elements.size());
                for(auto r:subRules){
                    if(r->checkHasRuleForContainer(subAddr)){
                        return true;
                    }
                }
                // this rule is false as no sub rules includes addr
                if(subRules.size()>0){
                    return false;
                }
                return endsWithDoubleWildCard();
            }


        }
        bool endsWithDoubleWildCard() const{
            return _endsWithWildcard;// dynamic_cast<DoubleWildcard*>(elements.getLast())!=nullptr;
        }
        const String toString(const String & prefix =""){
            String  t = prefix+originalString + "("+String(indent) + ")";
            if(subRules.size())t+=":\n";
            auto np = prefix+" ";
            for(auto r:subRules){
                t+=r->toString(np)+"\n";
            }
            return t;
        }
        NamespaceRule * parentRule=nullptr;

        String originalString;
        OwnedArray<FilterElement> elements;
        OwnedArray<NamespaceRule> subRules;
        bool _endsWithWildcard;

        int indent = 0;
    };
public:
    static const String & getSyntaxHelperInfo(){
        static String h = juce::translate("a filter is a multiline text allowing to SELECT valid parameter addresses (thus ignoring non specified). \n one can specify either :\n  * full parameter name : /node/looper/enabled \n  * all parameters in container : /node/looper/** \n  * parameter having identical paths across containers with similar names : /node/looper*/tracks/0/volume \n  * go hardcore and integrate regexes with [re] prefixes : /node/[re]looper[0-9]/tracks/**");
        return h;
    }
    Result processFile(const File & f){
        rules.clear();
        if(!f.existsAsFile()){return Result::ok();}
        FileInputStream input (f);
        if (input.openedOk())
        {
            int lNum = -1;

            NamespaceRule * enclosingRule = nullptr;
            NamespaceRule * lastRule = nullptr;
            int lastIndent = 0;
            while(!input.isExhausted()){
                lNum++;
                auto l = input.readNextLine();
                if(!l.trim().isEmpty()){
                    if(l.trimStart().startsWith("#")){continue;}
                    int indent = l.length() - l.trimStart().length();
                    if(indent==0){ // if not starting with whitespace or tabs
                        enclosingRule = nullptr;
                    }
                    else if(indent>lastIndent){
                        jassert(lastRule);
                        enclosingRule = lastRule;
                    }
                    else if(indent<lastIndent && enclosingRule ){
                        auto sibling = enclosingRule->getThisOrParentWithIndent(indent);
                        if(sibling){enclosingRule = sibling->parentRule;}
                        else{enclosingRule = nullptr;}
                    }
                    lastIndent = indent;
                    if(auto ru = NamespaceRule::fromString(l.trim(),indent)){
                        if(enclosingRule){
                            enclosingRule->addSubRule(ru);
                        }
                        else{
                            jassert(indent==0);
                            rules.add(ru);
                        }
                        lastRule = ru;
                    }
                    else{
                        lastRule = nullptr;
                        // discard all on error
                        rules.clear();
                        return Result::fail("osc feedback can't process line : "+String(lNum));
                    }
                }
            }
            DBG("rules : ");
            for(auto r:rules){
                DBG("-"+r->toString());
            }


            for(int i = rules.size()-1 ; i>0 ; i-- ){
                auto r = rules.getUnchecked(i);
                for(int j = i-1 ; j>=0;j--){
                    auto rr = rules.getUnchecked(j);
                    if(rr->hasSameBase(r)){
                        DBG(String("merging rule @@1 to @@2").replace("@@1", String(i)).replace("@@2", String(j)));
                        for(int k = r->subRules.size()-1 ; k >=0 ; k--){
                            rr->addSubRule(r->subRules.removeAndReturn(k));
                        }
                        rules.remove(i);
                        break;
                    }
                }

            }
            DBG("rules after merge : ");
            for(auto r:rules){
                DBG("-"+r->toString());
            }
        }
        else{
            return Result::fail("can't open osc filter file : "+ f.getFullPathName());
        }
        if(rules.size()==0){
            return Result::fail("no rules present in file "+ f.getFullPathName());
        }
        return Result::ok();
    }

    bool checkAddr(const ControlAddressType & s){
        if(rules.size()==0){
            return true;
        }
        for(auto r:rules){
            if(r->checkRule(s)){
                return true;
            }
        }
        return false;
    }
    bool includesAddr(const ControlAddressType & s){
        if(rules.size()==0){
            return true;
        }
        for(auto r:rules){
            if(r->checkHasRuleForContainer(s)){
                return true;
            }
        }
        return false;

    }

    OwnedArray<NamespaceRule> rules;
};


