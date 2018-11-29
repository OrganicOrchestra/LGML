import os, glob, json


from shutil import copy2 , copyfileobj
import zipfile
import gzip



raiseErrorOnDifferentSha = True;
dry_run = False;
distPath = os.path.expanduser("~/owncloud/DEVSPECTACLES/Tools/LGML/App-Dev/dist/")
if not os.path.exists(distPath):
  distPath = "/var/www/owncloud/data/admin/files/OrganicRoot/Spectacles/Tools/LGML/App-Dev/dist/"
if not os.path.exists(distPath):
  raise NameError("dist path not found")

desiredVersion = "1.2.9"
lastVPath = os.path.join(distPath,"bleedingEdge",desiredVersion)
publicFolder = '/Volumes/sshfs/owncloud/tools/LGML/'
if not os.path.exists(publicFolder):
  publicFolder = os.path.expanduser("~/tools/LGML");
if not os.path.exists(publicFolder):
  raise NameError("public folder path not found") 
# publicFolder = '/tmp/LGML/dist'
changeLogPath  = os.path.join(lastVPath,"CHANGELOG.md")


allCfgs={}
for c in glob.glob(lastVPath+"/*.cfg"):
  if not "Debug" in c:
    with open(c,'r') as fp:
      allCfgs [os.path.basename(c)[:-4]]=json.load(fp)

currentSha = ""

def checkIntegrity(errorOnWrongSha=True):
  global currentSha
  
  if( not os.path.exists(changeLogPath)):
    raise NameError("can't find CHANGELOG at "+changeLogPath)

  shas = { k:v["git_sha"] for k,v in allCfgs.items()}
  
  
  for k,v in shas.items():
    if currentSha=="":
      currentSha = v;
    if currentSha!= v:
      print (shas)
      err = "mismatching sha for %s: %s vs %s"%(k,currentSha,v)
      if errorOnWrongSha:
        raise NameError(err)
      else:
        print (err)



  bins = {k:os.path.join(lastVPath,os.path.basename(allCfgs[k]["packaged_name"])) for k in allCfgs.keys()}
  for k,b in bins.items():
    if not os.path.exists(b):
      raise NameError("not found bin for cfg : %s"%b)
    allCfgs[k]["local_bin"] = b;
    allCfgs[k]["published_basename"] = "LGML_v"+desiredVersion+'_'+allCfgs[k]["build_version_uid"]


  zips = {k:os.path.join(lastVPath,k+".zip") for k in allCfgs.keys()}
  for k,z in zips.items():
    if not os.path.exists(z):
      raise NameError("not found zip for cfg : %s"%z)
    allCfgs[k]["local_zip"] = z;
    
    

  exported_zips = {}
  for k,v in allCfgs.items():
    # print(k)
    bid= v["build_version_uid"]
    cf = v["build_cfg_name"]
    if "Release" in cf:
      if bid in exported_zips.keys():
        raise NameError("duplicate build")
      exported_zips[bid] = cf

  return bins,zips

bins,zips = checkIntegrity(raiseErrorOnDifferentSha)
with open(changeLogPath,'r') as fp:
  notes = ''.join(fp.readlines())

def createJSON(destFolder):
  global allCfgs
  destFolder = destFolder
  v = {"git_sha":currentSha,
      "notes":notes,
      "version":desiredVersion,
      "download_page" : "http://organic-orchestra.com/forum/d/6-lgml-telechargements",
      "zip_link" : {v["build_version_uid"]:v["published_basename"]+'.zip' for v in allCfgs.values()}
      }
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
    builds[typ]+=[os.path.basename(v["packaged_name"])]
  for k,v in builds.items():
    msg+=k+" : \n"
    for bn in v:
      msg+=bn+'\n'
    msg+='\n'
  print (msg)


def deployBinsToOwncloud():
  global allCfgs
  vpublicFolder = os.path.join(publicFolder,desiredVersion)
  if not os.path.exists(vpublicFolder):
    os.makedirs(vpublicFolder)

  jsonF = createJSON(vpublicFolder);
  copy2(jsonF,publicFolder+'/');
  copy2(changeLogPath,vpublicFolder);
  

  for k,c in allCfgs.items():
    print("copying : %s"%k)
  #   #copy2(c["local_bin"],os.path.join(vpublicFolder,os.path.basename(c["local_bin"])))
    copy2(c["local_zip"],os.path.join(vpublicFolder,c["published_basename"]+'.zip'))

def deployBinsToGithub():
  global allCfgs,dry_run
  import github_release as gh
  repo_name = "OrganicOrchestra/LGML"
  #checkNotReleased
  rl = gh.get_releases(repo_name)
  for r in rl:
    assert(r['tag_name']!=desiredVersion)

  tempD = '/tmp/githubExports'
  if(not os.path.exists(tempD)):
    os.makedirs(tempD)
  
  # print(json.dumps(allCfgs,indent=4))
  allAssets = []
  for k,v in allCfgs.items():
      ext = os.path.splitext(v["local_bin"])[1]
      if v["local_bin"].endswith(".tar.gz"):
        ext = ".tar.gz";
      tmpRenamed = tempD+'/'+v["published_basename"]+ext
      copy2(v["local_bin"],tmpRenamed)
      allAssets+= [tmpRenamed]
  print (allAssets)
  gh.gh_release_create(repo_name,desiredVersion,publish=True,asset_pattern =allAssets, name=desiredVersion,body=notes,dry_run=dry_run)



  
  



if __name__ == '__main__':
  
  print(json.dumps(allCfgs,indent=1))
  printReleaseMessage()
  print ('doyouwantToProceed (y/N)')
  nn = input()
  if nn=='y':
    deployBinsToOwncloud()
    deployBinsToGithub()
  else:
    print('not deploying')
  