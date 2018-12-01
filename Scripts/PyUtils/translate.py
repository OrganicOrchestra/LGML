# coding=utf-8
import polib
import sys
import os
import glob
import re
import json

sourcePath = os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir,os.path.pardir,'Source'))
jucePath =  "/tmp/la/"#os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir,os.path.pardir,os.path.pardir,'JUCE','modules'))
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


# def getEnclosed(s,start,opc='(',clc=')',ignore_escaped=False):
#   starti = -1;
#   i=start-1
#   depth = 0
#   wasEscaped = False
#   for c in s[start:]:
#     i+=1
#     if wasEscaped:
#       wasEscaped=False
#       continue
#     if c==clc and depth>0:
#       depth-=1
#     elif c==opc:
#       depth+=1
#       if starti<0 :
#         starti = i;
#     elif ignore_escaped and c=='\\':
#       wasEscaped = True

#     if starti>=0 and depth==0:
#       return (starti,i)
#   return None



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
  print(s)
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

# def getAllStrings(fl):
#   valid_s = re.compile("([A-Za-z0-9\ ]+)")
#   digit_s = re.compile("([0-9\ ]+)")
#   res ={}
#   for f in fl:
#     if "test" in f.lower():
#       continue
#     # print('reading strings from %s'%f)
#     with open(f,'r',errors='replace',encoding='utf-8') as fp:

#       i = 0
#       for l in fp.readlines():
#         i+=1
#         l = l.strip('/ ')
#         if "DBG" in l:
#           continue
#         found_idx = 0
#         sub = getEnclosed(l,found_idx,'"','"',True)
#         while sub:
#           innerT = l[sub[0]+1:sub[1]]
#           if re.match(valid_s,innerT) and len(innerT)>1 and innerT[-2:] != '.h' and not re.match(digit_s,innerT) and not '_' in innerT and not 'http' in innerT:
#             res[innerT]=(f,i)
#           found_idx=l.find('"',sub[1]+1)
#           if found_idx<0:
#             break;
          
#           sub = getEnclosed(l,found_idx,'"','"',True)
          


#   return res

# def splitEscaped(s):
#   wasEscaped = False
#   cur = ""
#   ls = []
#   escaped = []
#   for c in s:
#     if wasEscaped:
#       escaped+=['\\'+c]
#       ls+=[cur]
#       cur=""
#       wasEscaped=False
#     elif c=='\\':
#       wasEscaped=True
#     else:
#       cur+=c
#   ls+=[cur]
#   return ls,escaped

# def mergeEscaped(ls,escaped):
#   res = ls[0]
#   for i in range(1,len(ls)):
#     res+=escaped[i-1]+ls[i]
#   return res


# class YTranslator():
#   baseAddr = "https://translate.yandex.net/api/v1.5/tr.json/translate"
#   def translate(self,text,src,dest):
#     import requests  
#     r = requests.get(self.baseAddr,params={'key':os.environ["YTRANS_TOKEN"],'text':text,'lang':'%s-%s'%(src,dest)})
#     print (r.json())
#     return r.json()['text']

# class GTranslator():
#   baseAddr = 'https://translate.google.com/'

#   def translate(self,text,src,dest):
#     import requests
#     r = requests.get(self.baseAddr,params={'um':1,'ie':'UTF-8','hl':dest,'client':'tw-ob','view':'home','op':'translate',
#                                       'sl':src,'tl':dest,'text':text},
#                                       headers={'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.95 Safari/537.36'}
# )
#     print (r.text)
#     print (r.json())
#     return r.json()['text']

class MTranslator():
  import uuid
  base_url = 'https://api.cognitive.microsofttranslator.com'
  path = '/translate?api-version=3.0'
  headers = {
    'Ocp-Apim-Subscription-Key': os.environ['AZURE_TRANS_TOKEN'],
    'Content-type': 'application/json',
    'X-ClientTraceId': str(uuid.uuid4())
  }
  maxNumChar = 2000
  allowList = True
  def translate(self,text,src,dest):
    import requests
    params = '&from'+src+'&to='+dest
    constructed_url = self.base_url + self.path + params
    jsonR = [{'Text' : ' \n '.join(k.split('\n'))} for k in text]
    request = requests.post(constructed_url, headers=self.headers, json=jsonR)
    return [k["translations"][0]['text'] for k in request.json()]


def buildLocalMT(strs,locale='fr'):
  mtfile = os.path.join(baseTranslationPath,'mt.'+locale+'.json')
  from googletrans import Translator
  translator = MTranslator()
  
  i = 0
  if os.path.exists(mtfile):
    with open(mtfile ,'r') as fp:
      mt = json.load(fp)
  else:
    mt = {}

  res = {}
  

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
      
      print('trsL',trsL)
      # print('textL',txtL)

      assert(len(trsL)==len(txtL))
      for i in range(len(txtL)):
        kk = txtL[i] 
        if(kk==''):
          continue 
        mt[kk]=trsL[i]
        res[kk] = {'trans':mt[kk],'meta':strs[kk]}
      with open(mtfile,'w') as fp:
        json.dump(mt,fp)

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
  po = polib.POFile(encoding='utf-8')
  po.metadata = {
      'Project-Id-Version': '1.2.10',
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





def makeRawFile(tel):
  with open(baseTranslationPath+ 'raw.txt','w') as fp:
    fp.write('\n'.join(list(tel.keys())))



if __name__ == "__main__":

  # with open('/tmp/tstReg.txt' ,'r') as fp:
  #   s = fp.read()
  # r=buildRegFunction("ShapeShifterContentComponent",[0,1])
  # print (s)
  # it = re.finditer(r,s)
  # for i in it:
  #   print (i.groups())
  # exit()

  fl = getFileList()
  tel = {}
  autoEl = getRegExs(fl)
  tel.update(autoEl)

  ds = getDefaultStrings()
  tel.update(ds)

  makeRawFile(tel)
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
