import os, glob, json

import gitUtils
from shutil import copy2 , copyfileobj
import zipfile
import gzip
import tempfile 

distPath = os.path.expanduser("~/owncloud/DEVSPECTACLES/Tools/LGML/App-Dev/dist/")
desiredVersion = "1.2.7"
lastVPath = os.path.join(distPath,"bleedingEdge",desiredVersion)
# publicFolder = '/Volumes/sshfs/owncloud/tools/LGML/'
publicFolder = '/tmp/LGML/dist'
changeLogPath  = os.path.join(lastVPath,"CHANGELOG.md")

readmePath = os.path.dirname(os.path.abspath(__file__));
readmePath = os.path.join(readmePath,"../../README.md");


allCfgs={}
for c in glob.glob(lastVPath+"/*.cfg"):
  with open(c,'r') as fp:
    allCfgs [os.path.basename(c)]=json.load(fp)

currentSha = gitUtils.getGitSha()

def checkIntegrity():
  global currentSha
  sha = currentSha
  if( not os.path.exists(readmePath)):
    raise NameError("can't find README")
  if( not os.path.exists(changeLogPath)):
    raise NameError("can't find CHANGELOG")

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

  zips = {k:os.path.join(lastVPath,os.path.basename(allCfgs["zipped_file"])) for k in allCfgs.keys()}
  for z in zips.values():
    if not os.path.exists(z):
      raise NameError("not found zip for cfg : %s"%z)

  return bins,zips

bins,zips = checkIntegrity()
with open(changeLogPath,'r') as fp:
  notes = ''.join(fp.readlines())

def createJSON(destFolder,bins,zips):
  destFolder = destFolder
  v = {"git_sha":currentSha,
      "notes":notes,
      "version":desiredVersion,
      "download_page" : "http://organic-orchestra.com/forum/d/6-lgml-telechargements",
      "zip_link" : zips}
  vf = os.path.join(destFolder,"version.json")
  with open(vf,'w') as fp:
    json.dump(v,fp,indent=4)
  return vf


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


def deployBins():

  global bins,zips
  vpublicFolder = os.path.join(publicFolder,desiredVersion)
  if not os.path.exists(vpublicFolder):
    os.makedirs(vpublicFolder)
  jsonF = createJSON(vpublicFolder,bins,zips);
  copy2(changeLogPath,vpublicFolder)

  for b in bins.values():
    copy2(b,os.path.join(vpublicFolder,os.path.basename(b)))




if __name__ == '__main__':
  
  
  printReleaseMessage()
  deployBins()
  