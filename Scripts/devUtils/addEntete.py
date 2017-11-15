import glob
import os

types = ('*.cpp','*.h','*.hpp')
sourcePath = "../../Source/"

encoding = 'utf8'

lgmlFiles = []
for t in types:
  lgmlFiles+= glob.glob(sourcePath+t);
  lgmlFiles+= glob.glob(sourcePath+"**/"+t);

ignoredFiles = ["GitSha.h"]

lgmlFiles = [x for i in ignoredFiles for x in lgmlFiles if i not in x]

def getEntete(f):

  enteteLines = (0,0)
  # print("parsing "+f)
  with open(f,'r',encoding=encoding) as fi:
    isEntete = False;
    idx = 0
    for line in fi:
        if(not isEntete and ('/*' in line)):
          isEntete = True
          enteteLines = (idx,idx)
        if(not isEntete and line.strip()!=('\n')):
          isEntete = False
          enteteLines = (idx,idx)
          if(idx>0) :  print("weird start of file : "+f)
          break
          

        if(isEntete and ('*/' in line)):
          enteteLines = (enteteLines[0],idx)
          isEntete = False
          break
        idx+=1
  return enteteLines


def printfile(f):
  with open(f,'r',encoding=encoding) as fi:
    for line in fi:
      print(line)

def addEntete(f,entete):
  e = getEntete(f)

  with open(f,'r',encoding=encoding) as fi:
    lines = fi.readlines()

  if(e[0]!=0):
    print("before entete : "+str(e)+f )
    print( lines[0:e[0]])
    exit(1)
    return
  if(e[0] == e[1] ) and (e[0]!=0):
    print("weird entete content : "+f)
    print( lines[0:e[0]])
    exit(1)
  if e!=(0,0):
    codestart = e[1]+1
    while not lines[codestart].strip() and codestart < len(lines):
      codestart+=1
    lines = lines[codestart:]

  lines=entete+['\n']+lines
  with open(f,'w',encoding=encoding) as fi:
    fi.writelines(lines)
  # print (lines)


fl = lgmlFiles
# fl = [lgmlFiles[0]]
with open('entete','r',encoding=encoding) as fi:
  desiredEntete =fi.readlines()

if(not desiredEntete):
  raise NameError('no entete found')

for f in fl:
  print ("adding entete on ", f)
  entete = addEntete(f,desiredEntete);
  if entete == (0,0):
    print("no entete in" + f)
    printfile(f)