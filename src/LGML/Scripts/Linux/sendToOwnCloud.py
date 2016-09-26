import urllib
import os
import json
localExportPath = os.path.abspath("../../Builds/LinuxMakefile/build/")
localAppPath = localExportPath+'/LGML'
localExportPath = localExportPath+"/LGML.tar.gz"




def sh(cmd):
	print ("exec : "+cmd);
	res =  os.popen(cmd).read()
	print res
	return res
def sendToOwnCloud(originPath,destPath):
	credPath = os.path.dirname(os.path.abspath(__file__));
	credPath = os.path.join(credPath,os.pardir,"owncloud.password")

	with open(credPath) as json_data:
		credentials = json.loads(json_data.read())

	sh("curl -X PUT \"http://163.172.42.66/owncloud/remote.php/webdav/"+destPath+"\" --data-binary @\""+originPath+"\" -u "+credentials["pass"])



sh('tar -zcvf '+localExportPath+' '+localAppPath)
ownCloudPath = "Tools/LGML/App-Dev/Linux/Ubuntu/x86-64/LGML.tar.gz"
sendToOwnCloud(localExportPath,urllib.pathname2url(ownCloudPath))


