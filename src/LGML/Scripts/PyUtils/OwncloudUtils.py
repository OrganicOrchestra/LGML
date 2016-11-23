import os
import json
from shUtils import *
def getCredential():
	if ('OWNCLOUDPASS' in os.environ) and ('OWNCLOUDUSER' in os.environ):
		credentials = os.environ['OWNCLOUDUSER']+':'+os.environ['OWNCLOUDPASS']
	
	else:
		credPath = os.path.abspath(os.path.join(__file__,os.pardir,os.pardir));
		credPath = os.path.join(credPath,"owncloud.password")

		with open(credPath) as json_data:
			credentials = json.loads(json_data.read())["pass"]
		

	return credentials

def sendToOwnCloud(originPath,destPath):
	sh("curl -X PUT \"https://163.172.42.66/owncloud/remote.php/webdav/"+destPath+"\" --data-binary @\""+originPath+"\" -u "+getCredential()+" -k",printIt=False)
