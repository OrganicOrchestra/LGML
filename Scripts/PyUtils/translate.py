# coding=utf-8
import polib
import sys
import os
import glob
import re
import json

sourcePath = os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir,os.path.pardir,'Source'))
jucePath = os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir,os.path.pardir,os.path.pardir,'JUCE','modules'))
print (sourcePath,jucePath)
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


def getEnclosed(s,start,opc='(',clc=')',ignore_escaped=False):
  starti = -1;
  i=start-1
  depth = 0
  wasEscaped = False
  for c in s[start:]:
    i+=1
    if wasEscaped:
      wasEscaped=False
      continue
    if c==clc and depth>0:
      depth-=1
    elif c==opc:
      depth+=1
      if starti<0 :
        starti = i;
    elif ignore_escaped and c=='\\':
      wasEscaped = True

    if starti>=0 and depth==0:
      return (starti,i)
  return None



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
    print('reading %s'%f)
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

def getAllStrings(fl):
  valid_s = re.compile("([A-Za-z0-9\ ]+)")
  digit_s = re.compile("([0-9\ ]+)")
  res ={}
  for f in fl:
    if "test" in f.lower():
      continue
    # print('reading strings from %s'%f)
    with open(f,'r',errors='replace',encoding='utf-8') as fp:

      i = 0
      for l in fp.readlines():
        i+=1
        l = l.strip('/ ')
        if "DBG" in l:
          continue
        found_idx = 0
        sub = getEnclosed(l,found_idx,'"','"',True)
        while sub:
          innerT = l[sub[0]+1:sub[1]]
          if re.match(valid_s,innerT) and len(innerT)>1 and innerT[-2:] != '.h' and not re.match(digit_s,innerT) and not '_' in innerT and not 'http' in innerT:
            res[innerT]=(f,i)
          found_idx=l.find('"',sub[1]+1)
          if found_idx<0:
            break;
          
          sub = getEnclosed(l,found_idx,'"','"',True)
          


  return res

def splitEscaped(s):
  wasEscaped = False
  cur = ""
  ls = []
  escaped = []
  for c in s:
    if wasEscaped:
      escaped+=['\\'+c]
      ls+=[cur]
      cur=""
      wasEscaped=False
    elif c=='\\':
      wasEscaped=True
    else:
      cur+=c
  ls+=[cur]
  return ls,escaped

def mergeEscaped(ls,escaped):
  res = ls[0]
  for i in range(1,len(ls)):
    res+=escaped[i-1]+ls[i]
  return res



def buildLocalMT(strs,locale='fr'):
  mtfile = os.path.join(baseTranslationPath,'mt.'+locale+'.json')
  from googletrans import Translator
  translator = Translator()
  
  i = 0
  if os.path.exists(mtfile):
    with open(mtfile ,'r') as fp:
      mt = json.load(fp)
  else:
    mt = {}

  res = {}
  for k in strs:
    if not k in mt:
      print('translating :%s (%s,%i)'%(k,os.path.basename(strs[k][0]),strs[k][1]))
      kl,esc = splitEscaped(k)
      i+=1

      trs = translator.translate(kl,src='en',dest=locale)
      trs_l=[]
      for t in trs:
        trs_l += [t.text];#).decode('utf8')
      trs_s = mergeEscaped(trs_l,esc)
      mt[k]=trs_s
      if(i%10 ==0):
        with open(mtfile,'w') as fp:
          json.dump(mt,fp)

    res[k] = {'trans':mt[k],'meta':strs[k]}
  if i>0:
    with open(mtfile,'w') as fp:
      json.dump(mt,fp)
  return res


def buildPO(mt,lang):
  po = polib.POFile(encoding='utf-8')
  po.metadata = {
      'Project-Id-Version': '1.2.7',
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
    print (k,relSrcFile,lineNum)
    entry = polib.POEntry(
        msgid=k,
        msgstr=trs_s,
        occurrences=[(relSrcFile,lineNum)]
    )
    po.append(entry)
  po.save(os.path.join(baseTranslationPath,'%s.po'%lang['name']))


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





def buildRegFunction(fname,valid_mask,strict = True):
  anys = r"[\s\n\r]*"
  s = r'(?<!//)'+anys+fname+anys+"\("+anys
  for m in  valid_mask:
    if m:
      s+=r'(?:String'+anys+'\(|)"(.+?)"'
    else:
      s+=r'.+'
    s+=anys+','+anys
  s = s[:-len(anys)-1]
  if(strict) :
    s+=anys+"\)"
  print(s)
  return re.compile(s,re.MULTILINE)


if __name__ == "__main__":

  # s = """NLOGE("Engine", String("File : 123 not found.").replace("123", fileArg));"""
  # r = buildRegFunction("(?<!N)LOGE",[1,1],strict=False)
  # print (re.findall(r,s))
  # exit()

  fl = getFileList()
  tel = {}
  autoEl = getRegExs(fl)
  tel.update(autoEl)

  ds = getDefaultStrings()
  tel.update(ds)
  # print (autoEl)
  langs  = [
  {'code':'fr','name':'french'},
  {'code':'es','name':'spanish'},
  {'code':'ru','name':'russian'},
  {'code':'el','name':'greek'}
  ]
  # tel.update({k['name']:('None',0) for k in langs})
  for lang in langs:
    mt = buildLocalMT(tel,lang['code'])
    toJUCEfmt(mt,lang)
  
    buildPO(mt,lang)

  exit()

  
  
  


  for k,v in strs.items():
    print(k,os.path.basename(v[0]),v[1])
  for k,v in tel.items():
    print (os.path.basename(v[0]),v[1],k)
