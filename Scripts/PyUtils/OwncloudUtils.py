import os,sys
import json

try:
	from .shUtils import *
except (ImportError,ValueError) as e:
	from shUtils import *


import requests 

#py 2-3 compat urllib
if sys.version_info < (3, 0):
	from future.standard_library import install_aliases
	install_aliases()
import urllib.request


baseURL = "https://storage.organic-orchestra.com/owncloud/remote.php/webdav/"

def getCredential():
	credentials=None
	if ('OWNCLOUDPASS' in os.environ) and ('OWNCLOUDUSER' in os.environ):
		credentials = (os.environ['OWNCLOUDUSER'],os.environ['OWNCLOUDPASS'])

	if not credentials:
		raise NameError(" no credential for uploading to owncloud")
	return credentials

def getSession():
	session = requests.session()
	session.stream = True
	session.auth = getCredential()
	return session;


def decomposePath(path):
	folders = []
	while 1:
		path, folder = os.path.split(path)
		if folder != "":
			folders.append(folder)
		else:
			if path != "":
				folders.append(path)
			break
	folders.reverse()
	return folders

def makeDirIfNotExistent(destPath,forceCreation=False,session=None):
	if not '%' in destPath:
		destPath = urllib.request.quote(destPath.strip())
	session = session or getSession()
	r = session.request("HEAD", baseURL+destPath, allow_redirects=True)
	# if not found
	if r.status_code==404:
		r = session.request("MKCOL", baseURL+destPath, allow_redirects=True)
		# do partially if not found
		if r.status_code not in (201, 301,405):
			if forceCreation:
				splittedPath = decomposePath(destPath)
				for i in range(1,len(splittedPath)):
					tryPath = '/'.join(splittedPath[:i])
					print("mkdir :::: " +tryPath)
					return makeDirIfNotExistent(tryPath,False,session)
			else:
				raise NameError("can't create dir : "+destPath)
		else:
			print('made directory :' ,destPath)
			return r
	else:
		print ('found owncloud directory : '+destPath)
		return r;


def sendToOwnCloud(originPath,destPath,session=None):
	if not '%' in destPath:
		destPath = urllib.request.quote(destPath.strip())
	session = getSession()
	makeDirIfNotExistent(os.path.dirname(destPath),True,session=session)
	print('sending to owncloud:')
	print(originPath,' >> ', destPath)
	with open(originPath,'rb') as fp:
		r = session.request("PUT",baseURL+destPath,data=fp,allow_redirects=True)
		if r.status_code not in (200, 201, 204):
			raise NameError("can't upload file, error : "+str(r.status_code) + '\n'+r.text)
	# else:
		# sh("curl -X PUT \"https://storage.organic-orchestra.com/owncloud/remote.php/webdav/"+destPath+"\" --data-binary @\""+originPath+"\" -u "+getCredential()+" -k",printIt=False)


if __name__=='__main__':
	import sys
	if len(sys.argv)>2:
		sendToOwnCloud(sys.argv[1],sys.argv[2])
	# lastLGMLPath = urllib.request.pathname2url("/Tools/LGML/App-Dev/OSX/lastLGML.dmg")
	# currentLGML = urllib.request.pathname2url("/Tools/LGML/App-Dev/OSX/LGML_v1.1.6beta.dmg")

	# sh("curl -X COPY \"https://storage.organic-orchestra.com/owncloud/remote.php/webdav/"+currentLGML+"\" \"https://storage.organic-orchestra.com/owncloud/remote.php/webdav/"+lastLGMLPath+"\" -u "+getCredential()+" -k",printIt=False)

