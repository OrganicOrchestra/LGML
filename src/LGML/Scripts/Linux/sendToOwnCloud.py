from __future__ import print_function

import urllib
import os
import json
import subprocess


execName = "LGML"
localMakePath = os.path.abspath("../../Builds/LinuxMakefile/")+'/'
localExportPath = localMakePath+'build/'
localAppFile = localExportPath+execName




config = 'Ubuntu'

def sh(cmd):
	print ("exec : "+cmd);
	res =  subprocess.Popen(cmd, shell=True,stdout=subprocess.PIPE, universal_newlines=True)
	stdout_lines = iter(res.stdout.readline, "")
	for stdout_line in stdout_lines:
		yield stdout_line

	res.stdout.close()
	return_code = res.wait()
	if return_code != 0:
		raise subprocess.CalledProcessError(return_code, cmd)

def getVersion():
	for l in sh("exec "+localAppFile+" -v"):
		return l[:-1]


def sendToOwnCloud(originPath,destPath):
	credPath = os.path.dirname(os.path.abspath(__file__));
	credPath = os.path.join(credPath,os.pardir,"owncloud.password")

	with open(credPath) as json_data:
		credentials = json.loads(json_data.read())

	for l in sh("curl -X PUT \"https://163.172.42.66/owncloud/remote.php/webdav/"+destPath+"\" --data-binary @\""+originPath+"\" -u "+credentials["pass"]+" -k"):
		print (l,end="")



for l in sh('cd '+localMakePath+' && make CONFIG='+config+' -j2'):
	print(l,end="");

version = getVersion();
localExportFile = localAppFile+".tar.gz"
print (localExportFile)

for l in sh('tar -zcvf '+localExportFile+' --directory="'+localExportPath+'" '+execName):
	print (l,end="")
ownCloudPath = "Tools/LGML/App-Dev/Linux/Ubuntu16.04/LGML"+"_"+version+".tar.gz"
sendToOwnCloud(localExportFile,urllib.pathname2url(ownCloudPath))


