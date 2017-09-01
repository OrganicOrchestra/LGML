import os,sys
import json

scriptDir = os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir))
sys.path.insert(1,scriptDir)

from PyUtils import *

import multiprocessing
njobs = min(4,multiprocessing.cpu_count())

execName = {"Ubuntu32":"LGML32","Ubuntu":"LGML"}
lgmlBasePath=os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir,os.path.pardir));
localMakePath = os.path.join(lgmlBasePath,"Builds/LinuxMakefile/")
localExportPath = localMakePath+'build/'


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

def package_sources():
  import glob
  matches = []
  depFolders ={
  'LGML':{
  'Tests':'**/*',
  'JuceLibraryCode':'**/*',
  'Builds/LinuxMakeFile':'Makefile',
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
  from PyUtils import ProJucerUtils
  depFolders['JUCE/modules'] = {}
  for t in ProJucerUtils.getModules():
    depFolders['JUCE/modules'][t]='**/*'

  def walk(path,d,res):
    if type(d)==dict:
      for k,v in d.items():
        walk(os.path.join(path,k),v,res);
    else:
      for l in glob.glob(os.path.join(path,d),recursive=True):
        if os.path.isfile(l) :
          res+=[l]

  res = []
  rootDir = os.path.abspath(os.path.join(lgmlBasePath,os.pardir))
  walk(rootDir,depFolders,res);
  # res = [x for x in res if x.split('.')[-1]in['cpp','h','c']]
  
  version = ProJucerUtils.getVersionAsList()
  
  lgmlname = "lgml_"+'.'.join(map(str,version[:3])) # semver (ignore beta)
  
  preSize = len(rootDir)+1
  filesL = '\n'.join([x[preSize:] for x in res])
  distDir = scriptDir+'/linux/dist/'
  fileListPath = distDir+'filelist.txt'

  with open(fileListPath,'w') as fp:
    fp.writelines(filesL)

  sh('tar -zcvf "'+distDir+lgmlname+'.orig.tar.gz" --directory="'+rootDir+'" -T '+fileListPath)


def exportApp(baseName,configuration,exportpath = None):
  if configuration not in execName:
    raise NameError('unknown configuration : '+ configuration)
    exportFile = exportpath or localExportPath
    exportFile= os.path.join(exportFile,execName[configuration]+'.tar.gz')
    sh('tar -zcvf "'+exportFile+'" --directory="'+localExportPath+'" '+execName[configuration])
    return exportFile


if __name__ == '__main__':
  package_sources()


