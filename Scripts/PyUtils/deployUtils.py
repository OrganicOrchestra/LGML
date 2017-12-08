import os, glob, json

import gitUtils
from shutil import copy2

distPath = os.path.expanduser("~/owncloud/DEVSPECTACLES/Tools/LGML/App-Dev/dist/")
desiredVersion = "1.2.6"
lastVPath = os.path.join(distPath,"bleedingEdge",desiredVersion)
publicFolder = '/Volumes/sshfs/owncloud/tools/LGML/'

allCfgs={}
for c in glob.glob(lastVPath+"/*.cfg"):
  with open(c,'r') as fp:
    allCfgs [os.path.basename(c)]=json.load(fp)

currentSha = gitUtils.getGitSha()

def checkIntegrity():
  global currentSha
  sha = currentSha
  shas = { k:v["git_sha"] for k,v in allCfgs.items()}
  print (shas)
  for k,v in shas.items():
    if v!= currentSha:
      print ("warning sha is not current : %s vs %s"%(v,currentSha))
      break;

  for k,v in shas.items():
    currentSha = sha
    if sha!= v:
      raise NameError("mismatching sha for %s: %s vs %s"%(k,sha,v))


  bins = {k:os.path.join(lastVPath,k[:-4]) for k in allCfgs.keys()}
  for b in bins.values():
    if not os.path.exists(b):
      raise NameError("not found bin for cfg : %s"%b)
  
  return bins

bins = checkIntegrity()
with open(os.path.join(lastVPath,"CHANGELOG.md"),'r') as fp:
  notes = ''.join(fp.readlines())

def createJSON():
  v = {"git_sha":currentSha,
      "notes":notes,
      "version":desiredVersion,
      "download_page" : "http://organic-orchestra.com/forum/d/6-lgml-telechargements"}
  vf = os.path.join(publicFolder,"version.tmp.json")
  with open(vf,'w') as fp:
    json.dump(v,fp,indent=4)


def printReleaseMessage():
  baseLink = "https://storage.organic-orchestra.com/tools/LGML/"+desiredVersion+"/"
  msg = "New LGML is Out : v"+str(desiredVersion)+"\n"
  msg+=notes
  msg+='\n\n'
  builds = {}
  for c,v in allCfgs.items():
    typ = v["build_os"]
    if 'Rpi' in c:
      typ = "Raspberry"
    if(not typ in builds):
      builds[typ] = []
    builds[typ]+=[c]
  for k,v in builds.items():
    msg+=k+" : \n"
    for bn in v:
      msg+=baseLink+bn[:-4]+'\n'
    msg+='\n'
  print (msg)


def sendBins():
  global bins
  publicFolder = os.path.join(publicFolder,desiredVersion)
  if not os.path.exists(publicFolder):
    os.makedirs(publicFolder)
  for b in bins.values():
    copy2(b,os.path.join(publicFolder,os.path.basename(b)))


if __name__ == '__main__':
  # sendBins()
  # createJSON()
  printReleaseMessage()
  