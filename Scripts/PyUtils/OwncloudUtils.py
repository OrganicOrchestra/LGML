import os
import json
from .shUtils import *
import urllib.request
def getCredential():
	if ('OWNCLOUDPASS' in os.environ) and ('OWNCLOUDUSER' in os.environ):
		credentials = os.environ['OWNCLOUDUSER']+':'+os.environ['OWNCLOUDPASS']

	if not credentials:
		raise NameError(" no credential for uploading to owncloud")
	return credentials

def makeDirIfNotExistent(destPath,forceCreation=False):
	destPath = urllib.request.pathname2url(destPath)
	
	res = sh("curl -X MKCOL \"https://storage.organic-orchestra.com/owncloud/remote.php/webdav/"+destPath+"\"  -u "+getCredential()+" -k",printIt=False)
	if res=="": print('made directory :' ,destPath)
	if res!="" and forceCreation:
		splittedPath = destPath.split('/')
		for i in range(1,len(splittedPath)):
			tryPath = '/'.join(splittedPath[:i])
			print('trying ',tryPath)
			r = makeDirIfNotExistent(tryPath,False)

		if r!='':
			print('failed to create dir')
	
	return res

def sendToOwnCloud(originPath,destPath):
	destPath = urllib.request.pathname2url(destPath)
	makeDirIfNotExistent(os.path.dirname(destPath))
	print('sending to owncloud:')
	print(originPath,' >> ', destPath)
	sh("curl -X PUT \"https://storage.organic-orchestra.com/owncloud/remote.php/webdav/"+destPath+"\" --data-binary @\""+originPath+"\" -u "+getCredential()+" -k",printIt=False)


if __name__=='__main__':
	lastLGMLPath = urllib.request.pathname2url("/Tools/LGML/App-Dev/OSX/lastLGML.dmg")
	currentLGML = urllib.request.pathname2url("/Tools/LGML/App-Dev/OSX/LGML_v1.1.6beta.dmg")

	sh("curl -X COPY \"https://storage.organic-orchestra.com/owncloud/remote.php/webdav/"+currentLGML+"\" \"https://storage.organic-orchestra.com/owncloud/remote.php/webdav/"+lastLGMLPath+"\" -u "+getCredential()+" -k",printIt=False)

