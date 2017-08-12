import urllib.request, urllib.parse, urllib.error
import os,sys
import json

pathToAdd = os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir))
sys.path.insert(1,pathToAdd)

from PyUtils import *

import multiprocessing
njobs = multiprocessing.cpu_count()

execName = "LGML"
localMakePath = os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir,os.path.pardir,"Builds/LinuxMakefile/"))+'/'
localExportPath = localMakePath+'build/'
localAppFile = localExportPath+execName
localExportFile = localAppFile+".tar.gz"
verbose = False
print(localMakePath)


def buildApp(configuration):
  makeCmd = 'make CONFIG='+configuration+' -j'+str(njobs)
  if (verbose):
    makeCmd+=" SHELL='sh +x' "
  sh('cd '+localMakePath+' && '+makeCmd)


def exportApp(baseName):
	sh('tar -zcvf '+localExportFile+' --directory="'+localExportPath+'" '+execName)
	return localExportFile



