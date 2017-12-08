import json
from pythonosc import osc_message_builder
from pythonosc import udp_client
from pythonosc import osc_server
from pythonosc import dispatcher
import time
from threading import Thread,Lock

try:
  from shUtils import *
except (ImportError,ValueError) as e:
  import sys, os
  sys.path.insert(0, os.path.abspath('..'))
  from PyUtils.shUtils import *


class LGMLParam:
  def __init__(self,address):
    self.address = address
    self.jsonAddr = [x for x in address.split('/')[1:]]
    

  def readInSession(self,session):
    d = session.read()

    def search(dic,addr):
      # print('searching for '+ str(addr) + json.dumps(dic,indent=4))
      for x in addr:
        for k,v in dic.items():
          if 'parameters' in v:
            params = v['parameters'] 
            if(len(addr)==1):
              return params[addr[0]]
            if 'name' in params:
              if '/' in v and x.lower() == params['name'].lower():
                  return search(v['/'],addr[1:])
              elif len(addr)==2 :
                return params[addr[1]]
    
    return search(d,self.jsonAddr)
    



class LGMLSession:
  OSCOutPort = 11000
  OSCInPort = 11001
  LGMLIP = "127.0.0.1"
  lgmlExecPath = '../../Builds/MacOSX/build/Debug/LGML.app/Contents/MacOS/LGML'
  def  __init__(self,path):
    self.path = path;
    self.udp= udp_client.SimpleUDPClient(self.LGMLIP, self.OSCOutPort)
    self.dispatcher = dispatcher.Dispatcher()
    self.dispatcher.map("/pong", self.pong)
    self.dispatcher.map("/*", self.anyMessage)
    self.udpFB = osc_server.ThreadingOSCUDPServer(
      (self.LGMLIP, self.OSCInPort), self.dispatcher)
    self.udpFB_thread = Thread(target = self.udpFB.serve_forever)
    self.controllerAddress = ""
    self.lgml_thread = None
    self.stdout_filep = None 
    self.print_oscIn = True

  def __enter__(self):
    self.udpFB_thread.start()
    self.connected = False
    return self


  def __exit__(self, exc_type, exc_value, traceback):
    print('closing session ')
    self.close()
    self.udpFB.shutdown()
    self.udpFB.server_close()
    self.udpFB_thread.join()
    print('session closed')

  def read(self):
    print('reading')
    with open(self.path,'r') as fp:
      return json.load(fp)

  def run(self,stdout_filep = None):
    print('running')
    self.connected = False
    lgmlPID = self.getPIDCMD()
    if lgmlPID:
      print(lgmlPID)
      self.lgmlExecPath = str(lgmlPID[1])
      print('using existing process '+self.lgmlExecPath)
    self.lgml_thread = start_thread(self.lgmlExecPath + " -f "+self.path+ " --remote ",stdout = stdout_filep);

    self.wait()

  def getPIDCMD(self):
    def getinfo(process_name):
      return [item.split() for item in os.popen('ps -Ax').read().splitlines() if process_name in item]
    info = getinfo('LGML.app')
    if info and len(info)>0:
      ii = info[0]
      return (ii[0],'"'+' '.join(ii[3:])+'"')
    return None

  def close(self):
    self.udp.send_message("/close",1)
    
    if(self.lgml_thread):
      print("waiting for close")
      try :
        self.lgml_thread.wait(timeout=10)
      except subprocess.TimeoutExpired:
        
        self.lgml_thread.send_signal(9)
        print('forse closing LGML')

      print("closed")
      self.lgml_thread = None

  def wait(self,timeout=0):
    print('waiting')
    self.connected = False
    start = time.time()*1000.0
    while  self.connected is False:
      self.ping()
      time.sleep(1)
      if timeout>0 and (time.time()*1000.0 - start > timeout):
        print('timed out')
        return False
    print("connected")
    return True

  def waitForMessage(self,addr,timeout=0):
    print('waiting for message at '+addr)
    start = time.time()*1000.0
    while (not self.lastMessage) or self.lastMessage[0]!=addr:
      time.sleep(0)
      if timeout>0 and (time.time()*1000.0 - start > timeout):
        print('timed out')
        return False
    return self.lastMessage

  def pong(self,p,name,*a):
    print("gotPong "+str(p)+" : "+str(name))
    self.controllerAddress=name
    self.connected = True

  

  def anyMessage(self,add,*val):
    if( self.print_oscIn):
      log = "got message : "+str(add);
      if val:
        log+=" : "+str(val)
      print(log)
    self.lastMessage = (add,*val)

  def ping(self):
    print("pinging")
    self.udp.send_message("/ping",1)

  def save(self,block=False):
    if  self.lgml_thread is None or self.lgml_thread.poll() is not None:
      if not self.wait(6000):
        raise NameError("process has stoped "+ str(self.lgml_thread))
    if block is True :
      self.lastMessage=None
    self.udp.send_message("/save",self.path)
    if block is True:
      self.waitForMessage("/save")
      print('saved' + str(self.lastMessage))

  def load(self):
      self.udp.send_message("/load",self.path)
  
  def getTree(self):
    self.udp.send_message(self.controllerAddress+"/sendall",1)

  def send_osc(self,a,m):
    self.udp.send_message(a,m)

  def set_blockfeedback(self,v):
    self.udp.send_message(self.controllerAddress+"/blockfeedback",v)

  def set_logoutgoingosc(self,v):
    self.udp.send_message(self.controllerAddress+"/logoutgoingosc",v)

if __name__ == "__main__":
  sessionPath = "/Users/Tintamar/Documents/lgml.lgml"
  with LGMLSession(sessionPath) as s:
    ori = s.read()
    volParam = LGMLParam("/node/audiodeviceinnode/volume1")
    s.run()
    s.save()
    newS = s.read()
    time.sleep(2)
    s.getTree()
    time.sleep(4)
    print("!!!!!!!!!!!!!!!!!compare versions")
    assert(ori==newS)
    print('ended')
  

  