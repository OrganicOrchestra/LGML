# coding=utf-8
import polib
import sys
import os
import glob
import re
import json

sourcePath = os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir,os.path.pardir,'Source'))
print (sourcePath)


def getFileList():
  global sourcePath
  files = []
  for l in glob.glob(sourcePath+'/**/*.cpp',recursive = True):
    files+=[l]
  for l in glob.glob(sourcePath+'/**/*.h',recursive = True):
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


def getTranslatedElements(fl):

  methodName = "juce::translate"
  # r = re.compile(methodName+"\ *\(([A-Za-z0-9_\ ]+)\)")
  res ={}
  for f in fl:
    # print('reading %s'%f)
    with open(f,'r',errors='replace',encoding='utf-8') as fp:
      
      i = -1
      for l in fp.readlines():
        i+=1
        found_idx = 0
        found_idx = l.find(methodName,found_idx)
        while found_idx>0:
          sub = getEnclosed(l,found_idx)
          if(sub is not None):
            innerT = l[sub[0]+1:sub[1]]
            res[innerT]=(f,i)
            found_idx=sub[1]+1
          else:
            raise NameError('weird')
            break;
          
          found_idx = l.find(methodName,found_idx+1)
          


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



def buildLocalMT(strs):
  mtfile = "/tmp/mt"
  from googletrans import Translator
  translator = Translator()
  
  i = 0
  if os.path.exists(mtfile):
    with open(mtfile ,'r') as fp:
      mt = json.load(fp)
  else:
    mt = {}
  for k in strs:
    if not k in mt:
      print('translating :%s'%k)
      kl,esc = splitEscaped(k)
      i+=1

      trs = translator.translate(kl,src='en',dest='fr')
      trs_l=[]
      for t in trs:
        trs_l += [t.text];#).decode('utf8')
      trs_s = mergeEscaped(trs_l,esc)
      mt[k]=trs_s
      if(i%10 ==0):
        with open(mtfile,'w') as fp:
          json.dump(mt,fp)
  if i>0:
    with open(mtfile,'w') as fp:
      json.dump(mt,fp)
  return mt


def buildPO(strs):
  mt = buildLocalMT(strs)
  po = polib.POFile(encoding='utf-8')
  po.metadata = {
      'Project-Id-Version': '1.2.7',
      'Report-Msgid-Bugs-To': 'lab@organic-orchestra.com',
      'POT-Creation-Date': '2007-10-18 14:00+0100',
      'PO-Revision-Date': '2007-10-18 14:00+0100',
      'Last-Translator': 'you <lab@organic-orchestra.com>',
      'Language-Team': 'French <lab@organic-orchestra.com>',
      'MIME-Version': '1.0',
      'Content-Type': 'text/plain; charset=utf-8',
      'Content-Transfer-Encoding': '8bit',
  }
  for k,v in mt.items():

    trs_s = mt[k];
    print (k)
    entry = polib.POEntry(
        msgid=k,
        msgstr=trs_s,
        occurrences=[(v[0],v[1])]
    )
    po.append(entry)
  po.save(os.path.expanduser('~/Documents/LGML/translations/tst.po'))


def toJUCEfmt(mt):
  outputF = os.path.expanduser('~/Documents/LGML/translations/french.txt')
  
  with open(outputF,'w',encoding='utf-8') as fp:
    fp.write("""language: French
countries: fr be mc ch lu

""")
    for k,v in mt.items():
      fp.write('"%s" = "%s"\n'%(k,v))


if __name__ == "__main__":
  fl = getFileList()

  tel = getTranslatedElements(fl)
  valid_s = {}
  invalid_s = {}
  for s,v in tel.items():
    if s[0]=='"' and s[-1]=='"':
      valid_s[s.strip('"')]=v
    else:
      invalid_s[s]=[v]
  print(valid_s)
  print(invalid_s)
  mt = buildLocalMT(valid_s)
  toJUCEfmt(mt)

  strs = getAllStrings(fl)
  #toJUCEfmt(buildLocalMT(strs))
  # buildPO(strs)


  for k,v in strs.items():
    print(k,os.path.basename(v[0]),v[1])
  for k,v in tel.items():
    print (os.path.basename(v[0]),v[1],k)
