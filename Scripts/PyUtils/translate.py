# coding=utf-8
import polib
import sys
import os
import glob
import re
import json

sourcePath = os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir,os.path.pardir,'Source'))
jucePath =  'lal'#os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir,os.path.pardir,os.path.pardir,'JUCE','modules'))

baseTranslationPath = os.path.expanduser('~/Documents/LGML/translations/')
# baseTranslationPath = os.path.expanduser('~/owncloud/DEVSPECTACLES/Tools/LGML/translations')

def getFileList():
  global sourcePath,jucePath
  files = []
  for p in [sourcePath,jucePath]:
    for l in glob.glob(p+'/**/*.cpp',recursive = True):
      files+=[l]
    for l in glob.glob(p+'/**/*.h',recursive = True):
      files+=[l]
  return files



def buildRegFunction(fname,valid_mask,strict = True):
  anys = r"[\s\n\r]*"
  s = r'(?<!//)'+anys+fname+anys+"\("+anys
  for m in  valid_mask:
    if m:
      s+=r'(?:String'+anys+'\(|)\"(.+?)\"'
    else:
      s+=r'.+'
    s+=anys+','+anys
  s = s[:-len(anys)-1]
  if(strict) :
    s+=anys+"\)"
  # print(s)
  return re.compile(s,re.MULTILINE)

def getRegExs(fl):
  regL = []
  res ={}
  regL+=[buildRegFunction("addNewParameter\<.*\>",[1,1],strict=False)]
  regL+=[buildRegFunction("DECLARE_OBJ_TYPE",[0,1])]
  regL+=[buildRegFunction("ShapeShifterContentComponent",[0,1])]
  regL+=[buildRegFunction("juce::translate",[1],strict=True)]
  regL+=[buildRegFunction("(?<!NEEDS_)TRANS",[1],strict=True)]
  regL+=[buildRegFunction("AppPropertiesUI::\w+",[1],strict=True)]
  regL+=[buildRegFunction("BoolPropUI",[1],strict=True)]
  regL+=[buildRegFunction("EnumPropUI",[1,0,0,1])]
  regL+=[buildRegFunction("BoolUnsavedPropUI",[1])]
  regL+=[buildRegFunction("ActionPropUI",[1])]
  regL+=[buildRegFunction("createActionProp",[1])]
  regL+=[buildRegFunction("createUnsavedPropUI",[1])]
  # regL+=[buildRegFunction("(?<!N)LOG",[1])]
  # regL+=[buildRegFunction("(?<!N)LOGW",[1])]
  # regL+=[buildRegFunction("(?<!N)LOGE",[1])]
  # regL+=[buildRegFunction("NLOG",[1,1])]
  # regL+=[buildRegFunction("NLOGW",[1,1])]
  # regL+=[buildRegFunction("NLOGE",[1,1])]
  for f in fl:
    # print('reading %s'%f)
    with open(f,'r',errors='replace',encoding='utf-8') as fp:
      # for l in fp.readlines():
      text = fp.read()
    end='.*\n'
    line=[]
    for m in re.finditer(end, text):
      line.append(m.end())

    for r in regL:
      literator =  (i for i in range(len(line)) if line[i]>m.start(1))
      it = re.finditer (r,text)
      lineNum = 0
      for m in it:
        if( line[lineNum]<m.start(1)):
          lineNum = next(literator) + 1
        numC = m.groups()
        for i in numC:
          res[i]=(f,lineNum)
          # print (os.path.basename(f),lineNum,i)
        # if m : print(m);
  return res



def buildLocalMT(strs,locale='fr'):
  mtfile = os.path.join(baseTranslationPath,'mt.'+locale+'.json')
  
  from Translators import ManualTranslator
  translator = ManualTranslator()
  anglicisms = getAnglicisms(translator,locale)
  i = 0
  if os.path.exists(mtfile):
    with open(mtfile ,'r') as fp:
      mt = json.load(fp)
  else:
    mt = {}

  res = {}
  
  checkUntranslatable(strs)

  def doBatchTranslate(txtL,mt,res,strs):
      if isinstance(txtL,str):
        txtL = txtL.split('\n')

      print('translating :%s '%(txtL))

      # flatten \n s
      itemsIdx =[]
      flattenedL = []
      for t in txtL:
        flat = t.split('\\n')
        flattenedL += flat
        itemsIdx+=[len(flat)]
      # print(itemsIdx,flattenedL)
      flattenedL = list(map(anglicisms.encode,flattenedL))
      trs = translator.translate(flattenedL,src='en',dest=locale)


      if(len(trs)==1 and len(txtL)==1):
        trs = trs[0]
        trsL=trs.split('\n')
      else:
        idx = 0
        idxOr = 0
        trsL = []
        while idxOr <len(txtL):
          toJoin = itemsIdx[idxOr]
          trsL+=['\\n'.join(trs[idx:idx+toJoin])]
          idx+=toJoin
          idxOr+=1
      trsL = list(map(unifyLanguage,trsL))
      trsL = list(map(anglicisms.decode,trsL))
      print('trsL',trsL)
      # print('textL',txtL)

      assert(len(trsL)==len(txtL))
      for i in range(len(txtL)):
        kk = txtL[i] 
        if(kk==''):
          continue 

        mt[kk]=trsL[i]
        res[kk] = {'trans':mt[kk],'meta':strs[kk]}
      with open(mtfile,'w',encoding='utf-8') as fp:
        json.dump(mt,fp,ensure_ascii=False,sort_keys=True)

  txt=""
  maxNumChars = translator.maxNumChar
  for k in strs:
    if(k in mt):
      res[k] = {'trans':mt[k],'meta':strs[k]}
      continue
    if(len(k)+len(txt)>=maxNumChars):
      if maxNumChars==0:
        txt = k
      if translator.allowList:
        txt = txt.split('\n')[:-1]
      doBatchTranslate(txt,mt,res,strs)
      txt=""
    else:
      txt+=k+'\n'
    
  if len(txt)>0:
    doBatchTranslate(txt,mt,res,strs)
  return res


def buildPO(mt,lang):
  from ProJucerUtils import getXmlVersion
  version = getXmlVersion()
  print ("version ",version)
  po = polib.POFile(encoding='utf-8')
  po.metadata = {
      'Project-Id-Version': version,
      'Report-Msgid-Bugs-To': 'lab@organic-orchestra.com',
      'POT-Creation-Date': '2007-10-18 14:00+0100',
      'PO-Revision-Date': '2007-10-18 14:00+0100',
      'Last-Translator': 'you <lab@organic-orchestra.com>',
      'Language-Team': 'Lab <lab@organic-orchestra.com>',
      'MIME-Version': '1.0',
      'Content-Type': 'text/plain; charset=utf-8',
      'Content-Transfer-Encoding': '8bit',
  }
  for k,v in mt.items():

    trs_s = v['trans'];
    relSrcFile = v['meta'][0][len(sourcePath)+1:]
    lineNum = v['meta'][1]
    # print (k,relSrcFile,lineNum)
    entry = polib.POEntry(
        msgid=k,
        msgstr=trs_s,
        
    )
    entry.occurrences=[(relSrcFile,lineNum)]
    po.append(entry)
  po.save(os.path.join(baseTranslationPath,'%s.po'%(lang['name'])))


def toJUCEfmt(mt,lang):
  outputF = os.path.join(baseTranslationPath,'%s.txt'%lang['name'])
  
  with open(outputF,'w',encoding='utf-8') as fp:
    fp.write("""language: %s
countries: %s

"""%(lang['name'],lang['code']))
    for k,v in mt.items():
      fp.write('"%s" = "%s"\n'%(k,v['trans']))





def getDefaultStrings():
  resL= ["Node Manager", "Time Manager", "Inspector", "Logger", "Controllers", "Fast Mapper", "Outliner","Help"]
  res = {}
  for a in resL:
      res[a] = ("No File",0)
  return res

def unifyLanguage(line):
  # try to have a consistenet ortograph (french : 'nœud' is sometimes spelled 'noeud' )
  subs = [("œ","oe")]
  for s in subs:
    line = re.sub(s[0],s[1],line)
  return line

def getAnglicisms(translator,dest):
  #these are not translated but we try to spot them in full sentences
  class Anglicism(object):

    def __init__(self,anglicisms):
        from collections import OrderedDict
        self.encodeAngl = OrderedDict()
        cn = 65
        def getValidC(cn):
          while chr(cn) in 'AEIOUaeiou':
            cn+=1
          return cn,"#"+chr(cn)
        for i in range(len(anglicisms)):
          a = anglicisms[i]
          (cn,e) = getValidC(cn)
          self.encodeAngl[a] = e
          c = a[0]
          cn+=1
          (cn,e) = getValidC(cn)
          if c.lower()!=a[0]:
            self.encodeAngl[c.lower()+a[1:]] = e
          else:
            self.encodeAngl[a+'s'] = e


    def encode(self,s):
      for a,e in self.encodeAngl.items():
        # print('checking',trs[i],anglicisms[i],line)
        s = re.sub(a,e,s)
      return s
    def decode(self,s):
      for a,e in self.encodeAngl.items():
        # print('checking',trs[i],anglicisms[i],line)
        s = re.sub(e,a,s)
      return s
  return Anglicism(["node","logger","onset","solo","Link"]) #"x","y","X","Y",



def makeRawFile(tel):
  with open(baseTranslationPath+ 'raw.json','w') as fp:
    json.dump(tel,fp,sort_keys=True)

def getRawFile():
  with open(baseTranslationPath+ 'raw.json','r') as fp:
    return json.load(fp)



def checkUntranslatable(strs):
  #these are not translated if they fully match regEx (usually exact match)
  regs = []
  untranslated = []
  regs+=list(map(re.compile,["^"+x+"$" for x in ["Link","Link Peers","Link Latency","x","y","X","Y"]]))
  for k in strs:
    found = False
    for r in regs:
      if r.match(k):
        found = True
        break;
    if found:
      print('not translating ',k)
      untranslated +=[k]
  for k in untranslated:
    del strs[k]


def fromRawTranslation(locale):
  d = {}
  with open(baseTranslationPath+'raw.txt','r') as rawfp:
    with open(baseTranslationPath+'raw.'+locale+'.txt','r') as localefp:
      r = rawfp.readline()
      while( r):
        t = localefp.readline()
        r = r.strip()
        t = t.strip()
        print (r,t)
        assert(t!="")
        d[r] = t
        r = rawfp.readline()
  return d;


if __name__ == "__main__":
  
  skip_parse = True;
  force_rebuild = False;
  if not skip_parse:
    print ('parsing folders %s'%([sourcePath,jucePath]))
    fl = getFileList()
    tel = {}
    autoEl = getRegExs(fl)
    tel.update(autoEl)

    ds = getDefaultStrings()
    tel.update(ds)
    makeRawFile(tel)
  else:
    tel = getRawFile()

 

  # print (autoEl)
  langs  = [
  {'code':'fr','name':'french'},
  # {'code':'es','name':'spanish'},
  # {'code':'ru','name':'russian'},
  # {'code':'el','name':'greek'}
  ]
  # tel.update({k['name']:('None',0) for k in langs})
  for lang in langs:
    mt = buildLocalMT(tel,lang['code'])
    print('localMT built')
    toJUCEfmt(mt,lang)
  
    buildPO(mt,lang)

  exit()

  
  
  


  for k,v in strs.items():
    print(k,os.path.basename(v[0]),v[1])
  for k,v in tel.items():
    print (os.path.basename(v[0]),v[1],k)
