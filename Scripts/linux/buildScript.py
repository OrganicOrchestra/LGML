import os,sys
import json

pathToAdd = os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir))
sys.path.insert(1,pathToAdd)

from PyUtils import *

import multiprocessing
njobs = min(4,multiprocessing.cpu_count())

execName = {"Ubuntu32":"LGML32","Ubuntu":"LGML"}
localMakePath = os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir,os.path.pardir,"Builds/LinuxMakefile/"))+'/'
localExportPath = localMakePath+'build/'


verbose = False
print(localMakePath)


def buildApp(configuration):
  if configuration not in execName:
    raise NameError('unknown configuration : '+ configuration)
  makeCmd = 'make CONFIG='+configuration+' -j'+str(njobs)
  if (verbose):
    makeCmd+=" SHELL='sh +x' "
  sh('cd '+localMakePath+' && '+makeCmd)
  localAppFile = localExportPath+execName[configuration]
  return localAppFile


def exportApp(baseName,configuration,exportpath = None):
  if configuration not in execName:
    raise NameError('unknown configuration : '+ configuration)
  localExportFile = exportpath or localAppFile
  localExportFile+='.tar.gz'
  sh('tar -zcvf '+localExportFile+' --directory="'+localExportPath+'" '+execName[configuration])
  return localExportFile



