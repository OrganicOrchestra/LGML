import os,sys
import json
import fnmatch

scriptDir = os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir))
sys.path.insert(1,scriptDir)

from PyUtils import *


import multiprocessing
njobs = min(4,multiprocessing.cpu_count())

execName = {"Ubuntu32":"LGML32","Ubuntu":"LGML"}
lgmlBasePath=os.path.abspath(os.path.join(scriptDir,os.path.pardir));
localMakePath = os.path.join(lgmlBasePath,"Builds/LinuxMakefile/")
localExportPath = localMakePath+'build/'
rootDir = os.path.abspath(os.path.join(lgmlBasePath,os.pardir))
version = ProJucerUtils.getVersionAsList()
lgmlname = "lgml_"+'.'.join(map(str,version[:3])) # semver (ignore beta)
distDir = scriptDir+'/linux/dist/'

verbose = False
print(localMakePath)


def buildApp(configuration):
  if configuration not in execName:
    raise NameError('unknown configuration : '+ configuration)

  makeCmd = 'make CONFIG='+configuration+' -j'+str(njobs)
  if (verbose):
    makeCmd+=" SHELL='sh +x' V=1"
  sh('cd '+localMakePath+' && '+makeCmd)
  localAppFile = localExportPath+execName[configuration]
  return localAppFile

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
  
  walk(rootDir,depFolders,res);
  # res = [x for x in res if x.split('.')[-1]in['cpp','h','c']]
  

  preSize = len(rootDir)+1
  filesL = '\n'.join([x[preSize:] for x in res])

  fileListPath = distDir+'filelist.txt'

  with open(fileListPath,'w') as fp:
    fp.writelines(filesL)
  return fileListPath

def package_source(fileListPath = None):
  fileListPath = fileListPath or generate_filelist()
  sh('COPYFILE_DISABLE=1 tar -zcvf "'+distDir+lgmlname+'.orig.tar.gz" --directory="'+rootDir+'" -T '+fileListPath)

def copy_source(fileListPath=None):
  fileListPath = fileListPath or generate_filelist()

  # Clean first
  import shutil
  destCopyDir = os.path.join(distDir,'lgml')
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
      sourceF = os.path.join(rootDir,f)
      destF =  os.path.join(destCopyDir,f)
      destD =os.path.abspath(os.path.join(destF, os.pardir))
      
      if not os.path.exists(destD):
        os.makedirs(destD)
      shutil.copy(sourceF,destF)


def exportApp(baseName,configuration,exportpath = None):
  if configuration not in execName:
    raise NameError('unknown configuration : '+ configuration)

  exportFile = exportpath or localExportPath
  exportFile= os.path.join(exportFile,execName[configuration]+'.tar.gz')
  sh('tar -zcvf "'+exportFile+'" --directory="'+localExportPath+'" '+execName[configuration])
  return exportFile


if __name__ == '__main__':
  package_source()


