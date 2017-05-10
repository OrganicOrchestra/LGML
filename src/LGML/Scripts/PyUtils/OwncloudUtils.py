import os
import json
from shUtils import *
import urllib
def getCredential():
	if ('OWNCLOUDPASS' in os.environ) and ('OWNCLOUDUSER' in os.environ):
		credentials = os.environ['OWNCLOUDUSER']+':'+os.environ['OWNCLOUDPASS']
	
	else:
		credPath = os.path.abspath(os.path.join(__file__,os.pardir,os.pardir));
		credPath = os.path.join(credPath,"owncloud.password")

		with open(credPath) as json_data:
			credentials = json.loads(json_data.read())["pass"]
		
	if not credentials:
		print 'no credentials found'
	return credentials

def makeDirIfNotExistent(destPath,forceCreation=False):
	destPath = urllib.pathname2url(destPath)
	
	res = sh("curl -X MKCOL \"https://163.172.42.66/owncloud/remote.php/webdav/"+destPath+"\"  -u "+getCredential()+" -k",printIt=False)
	if res=="": print 'made directory :' ,destPath
	if res!="" and forceCreation:
		splittedPath = destPath.split('/')
		for i in range(1,len(splittedPath)):
			tryPath = '/'.join(splittedPath[:i])
			print 'trying ',tryPath
			r = makeDirIfNotExistent(tryPath,False)

		if r!='':
			print 'failed to create dir'
	
	return res

def sendToOwnCloud(originPath,destPath):
	destPath = urllib.pathname2url(destPath)
	makeDirIfNotExistent(os.path.dirname(destPath))
	print 'sending to owncloud:'
	print originPath,' >> ', destPath
	sh("curl -X PUT \"https://163.172.42.66/owncloud/remote.php/webdav/"+destPath+"\" --data-binary @\""+originPath+"\" -u "+getCredential()+" -k",printIt=False)


if __name__=='__main__':
	lastLGMLPath = urllib.pathname2url("/Tools/LGML/App-Dev/OSX/lastLGML.dmg")
	currentLGML = urllib.pathname2url("/Tools/LGML/App-Dev/OSX/LGML_v1.1.6beta.dmg")

	sh("curl -X COPY \"https://163.172.42.66/owncloud/remote.php/webdav/"+currentLGML+"\" \"https://163.172.42.66/owncloud/remote.php/webdav/"+lastLGMLPath+"\" -u "+getCredential()+" -k",printIt=False)

