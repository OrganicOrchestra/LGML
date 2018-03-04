import os,sys
import json
import fnmatch
import platform
scriptDir = os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir))
sys.path.insert(1,scriptDir)

from PyUtils import *

from PyUtils.builderBase import BuilderBase

class LinuxBuilder (BuilderBase):
  lgmlBasePath=os.path.abspath(os.path.join(scriptDir,os.path.pardir));
  rootDir = os.path.abspath(os.path.join(lgmlBasePath,os.pardir))
  version = ProJucerUtils.getVersionAsList()
  lgml_dist_name = "lgml_"+'.'.join(map(str,version[:3])) # semver (ignore beta)
  
  localMakePath = os.path.join(lgmlBasePath,"Builds/LinuxMakefile/")
  localExportPath = localMakePath+'build/'
  distDir = scriptDir+'/linux/dist/'

  default_cfg = {
  "arch":platform.machine(),
  "ARCH_FLAGS":""
  }
  def apply_if_defined(self,n,localName=None):
    localName = localName or n
    if n in os.environ:
      self.default_cfg[localName]=os.environ[n]

  

  def __init__(self,cfg):
    BuilderBase.__init__(self,cfg)
    self.apply_if_defined("ARCH_FLAGS")
    self.apply_if_defined("CROSS_ARCH","arch")
    self.applyCfg(self.default_cfg)
    # hack to pass param to builder and linker
    os.environ["TARGET_ARCH"] = self.getFullArchFlags()

  def getPreprocessor(self):
    gcc = "g++"
    if "CXX" in os.environ:
      gcc = os.environ["CXX"]
    return sh(gcc+' -E -v -dM '+self.getFullArchFlags()+" - </dev/null 2>&1",printIt=True)


  def getFullArchFlags(self):
    return self.cfg["ARCH_FLAGS"]

  def buildApp(self):
    self.patchMakeFileIfNeeded();
    makeCmd = self.makeCmd()
    sh('cd '+self.localMakePath+' && '+makeCmd)
    localAppFile = self.getBinaryPath()
    return localAppFile

  def cleanApp(self):
    makeCmd = self.makeCmd()
    makeCmd += " clean"
    sh('cd '+self.localMakePath+' && '+makeCmd)

  def packageApp(self,exportpath = None):
    self.getReadmePath()
    exportFile = exportpath or self.localExportPath
    exportFile= os.path.join(exportFile,self.getUID()+'.tar.gz')
    sh('tar -zcvf "'+exportFile+'" --directory="'+self.localExportPath+'" '+self.cfg["appName"])
    return exportFile

  def getBinaryPath(self):
    return self.localExportPath+self.cfg["appName"]

  def getPlatformName(self):
    return "linux"

  def makeCmd(self):
    makeCmd = 'make CONFIG='+self.cfg["build_cfg_name"]+' -j'+str(self.cfg["njobs"])
    if (self.verbose=="verbose"):
      makeCmd+=" SHELL='sh +x' V=1"
    return makeCmd


  def patchMakeFileIfNeeded(self):

    def patchMakeFile(ori,rpl):
      makeFile = BuilderBase.rootPath+"/Builds/LinuxMakefile/Makefile"
      with open(makeFile,'r') as fp:
        lines = fp.readlines()
      lines = map(lambda l:l.replace(ori,rpl),lines);
      with open(makeFile,'w') as fp:
        fp.writelines(lines)

    gccv = sh("gcc -dumpversion")
    gccvnum  =list( map(lambda x:int(x),gccv.split('.')))
    print ("gcc version : %i,%i,%i"%(gccvnum[0],gccvnum[1],gccvnum[2]))
    if (gccvnum[0]<5 and gccvnum[1]<9 ):
      print ("patching makefile : to use c++11 (experiemental)")
      patchMakeFile("c++14","c++11")



################
# source deb package helpers
##################





def generate_filelist():
  import glob
  matches = []
  depFolders ={
  'LGML':{
  'Tests':'**/*',
  'JuceLibraryCode':'**/*',
  'Builds/LinuxMakefile':'Makefile',
  'Source':'**/*',
  'third_party':{
    'link':{
      'include':'**/*',
      'modules/asio-standalone/asio/include':'**/*',
      },
    'serial':{
      'include':'**/*',
      'src':'**/*',
    },
    # 'VST3_SDK':{'base/source/':'**/*','pluginterfaces/':'**/*','public.sdk/source/':'**/*',}
      
  },  
  },

  }
  
  depFolders['JUCE/modules'] = {}
  for t in ProJucerUtils.getModules():
    depFolders['JUCE/modules'][t]='**/*'

  def walk(path,d,res):

    if type(d)==dict:
      for k,v in d.items():
        walk(os.path.join(path,k),v,res);
    else:
      if len(d)>2 and d[0:3]=='**/':
        for root, dirnames, filenames in os.walk(path):
          for filename in fnmatch.filter(filenames, d[3:]):
            fp = os.path.join(root,filename)
            if os.path.isfile(fp) and not os.path.basename(fp).startswith('.'):
              res+=[fp]
      else:
        for filename in fnmatch.filter(os.listdir(path),d):
          fp = os.path.join(path,filename)
          if os.path.isfile(fp)  and not os.path.basename(fp).startswith('.'):
            res+=[fp]

  res = []
  
  walk(LinuxBuilder.rootDir,depFolders,res);
  # res = [x for x in res if x.split('.')[-1]in['cpp','h','c']]
  

  preSize = len(LinuxBuilder.rootDir)+1
  filesL = '\n'.join([x[preSize:] for x in res])

  fileListPath = os.path.join(LinuxBuilder.distDir,'filelist.txt')

  with open(fileListPath,'w') as fp:
    fp.writelines(filesL)
  return fileListPath



def package_source_dist(fileListPath = None):
  fileListPath = fileListPath or generate_filelist()
  destFile= LinuxBuilder.distDir+LinuxBuilder.lgml_dist_name+'.orig.tar.gz'
  sh('COPYFILE_DISABLE=1 tar -zcvf "'+destFile+'" --directory="'+LinuxBuilder.rootDir+'" -T '+fileListPath)
  return destFile

def get_upstream_source_package():
  targetFile =  LinuxBuilder.distDir+LinuxBuilder.lgml_dist_name+".orig.tar.gz"
  if(os.path.exists(targetFile)):
    print("using cached sourcePackage : "+ targetFile)
    return
  #py 2-3 compat urllib
  if sys.version_info < (3, 0):
    from future.standard_library import install_aliases
    install_aliases()
  import urllib.request 
  print('downloading upstream package for '+LinuxBuilder.lgml_dist_name)
  url = "https://launchpad.net/~tintamarunix/+archive/ubuntu/lgml/+files/"+LinuxBuilder.lgml_dist_name+".orig.tar.gz"
  
  displayed = 0
  downloaded=0
  def show_progress(count, block_size, total_size):
    global displayed
    global downloaded
    if(count == 0): displayed= 0; downloaded=0;
    downloaded += block_size
    step = 50000.0
    isLast = downloaded>=total_size
    shouldDisplay = int(downloaded/step)!=int(displayed/step)
    shouldDisplay |= isLast
    if(shouldDisplay):
      displayed = downloaded
      info = "-> %.1f kB / %.1f kB"%(displayed/1000.0 ,total_size/1000.0)
      if(total_size>0):
        info += " %.2f"%(displayed*100.0/total_size) + "%"
      endChar = '\r'
      if(isLast) :endChar = '\n'
      print(info, sep=' ', end=endChar, flush=True)
    if isLast:
        print("ended")
        downloaded = 0


  urllib.request.urlretrieve(url,targetFile,show_progress)

def copy_source_dist(fileListPath=None):
  fileListPath = fileListPath or generate_filelist()

  # Clean first
  import shutil
  destCopyDir = os.path.join(LinuxBuilder.distDir,'lgml')
  for dirs in os.listdir(destCopyDir):
    
    if(dirs != 'debian'):
      cF = os.path.join(destCopyDir,dirs)
      if os.path.isdir(cF):
        shutil.rmtree(os.path.join(destCopyDir,dirs))
      elif os.path.isfile(cF):
        os.remove(cF)


  with open(fileListPath,'r') as fp:
    for f in fp.readlines():
      f = f.strip()
      sourceF = os.path.join(LinuxBuilder.rootDir,f)
      destF =  os.path.join(destCopyDir,f)
      destD =os.path.abspath(os.path.join(destF, os.pardir))
      
      if not os.path.exists(destD):
        os.makedirs(destD)
      shutil.copy(sourceF,destF)




if __name__ == '__main__':
  
  
  # get_upstream_source_package();
  # copy_source_dist()
  
  builder = LinuxBuilder({"arch":"native","ARCH_FLAGS":"-ffast-math"});
  builder.patchMakeFileIfNeeded();
  exit();
  # builder.buildApp()

  print( builder.getPreprocessor())
  # package_source_dist()


