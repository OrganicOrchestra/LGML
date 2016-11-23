from __future__ import print_function

import urllib
import os
import json
import subprocess


import sys
pathToAdd = os.path.abspath(os.path.join(__file__,os.path.pardir,os.path.pardir))
sys.path.insert(1,pathToAdd)

execName = "LGML"
localMakePath = os.path.abspath("../../Builds/LinuxMakefile/")+'/'
localExportPath = localMakePath+'build/'
localAppFile = localExportPath+execName




config = 'Ubuntu'



def getVersion():
	for l in sh("exec "+localAppFile+" -v"):
		return l[:-1]





for l in sh('cd '+localMakePath+' && make CONFIG='+config+' -j2'):
	print(l,end="");

version = getVersion();
localExportFile = localAppFile+".tar.gz"
print (localExportFile)

for l in sh('tar -zcvf '+localExportFile+' --directory="'+localExportPath+'" '+execName):
	print (l,end="")
ownCloudPath = "Tools/LGML/App-Dev/Linux/Ubuntu16.04/LGML"+"_"+version+".tar.gz"
sendToOwnCloud(localExportFile,urllib.pathname2url(ownCloudPath))


