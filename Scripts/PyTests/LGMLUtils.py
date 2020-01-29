import json
from pythonosc import osc_message_builder
from pythonosc import udp_client
from pythonosc import osc_server
from pythonosc import dispatcher
import mido
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
  LGMLMIDIOut = "Gestionnaire IAC Bus 1"
  closeWhenEnded = True
  def  __init__(self,path):
    self.path = path;
    self.udp= udp_client.SimpleUDPClient(self.LGMLIP, self.OSCOutPort)
    self.dispatcher = dispatcher.Dispatcher()
    self.dispatcher.map("/pong", self.pong)
    self.dispatcher.map("/*", self.anyMessage)
    self.dispatcher.map("/stats/*",self.stats)
    self.udpFB = osc_server.ThreadingOSCUDPServer(
      (self.LGMLIP, self.OSCInPort), self.dispatcher)
    self.udpFB_thread = Thread(target = self.udpFB.serve_forever)
    self.controllerAddress = ""
    self.lgml_thread = None
    self.stdout_filep = None 
    self.print_oscIn = True
    self.stats = {}
    self.startTime = time.time()*1000.0
    self.midiOut = mido.open_output(self.LGMLMIDIOut)

  def __enter__(self):
    self.udpFB_thread.start()
    self.connected = False
    return self


  def __exit__(self, exc_type, exc_value, traceback):
    if(self.closeWhenEnded):
      print('closing session ')
      self.close()
      
      print('session closed')
    else:
      print('ended with session')
    self.udpFB.shutdown()
    self.udpFB.server_close()
    self.udpFB_thread.join()

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
      return [item.split() for item in os.popen('ps -A -o pid  -o state -o command').read().splitlines() if process_name in item]
    info = getinfo('LGML.app')
    
    if info and len(info)>0:
      i = 0;
      while i<len(info):
        ii = info[i]
        if(ii[1] not in ['U','Z']):
          return (ii[0],'"'+' '.join(ii[2:])+'"')
        i+=1
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

  def pong(self,addr,name,*a):
    print("gotPong "+str(addr)+" : "+str(name))
    self.controllerAddress=name
    # if not self.connected:
    #   self.startTime = time.time()*1000.0
    self.connected = True

  def stats(self,addr,*a):
    if not self.connected:
      return
    statName = addr[7:]
    timeOfEvent = time.time()*1000.0- self.startTime 
    event = a[0]
    engineTime = a[1]
    
    print("got stats : %s , %s at %.2f (%.2f)"%(statName,event,timeOfEvent,engineTime))
    if not statName in self.stats:
      self.stats[statName] = []
    self.stats[statName]+=[(timeOfEvent,event,engineTime)]

  

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
  def send_midi_cc(self,cc,v):
    self.midiOut.send(mido.Message('control_change', control=cc,value=v))
  def set_controller_param(self,name,v):
    self.udp.send_message(self.controllerAddress+"/"+name,v)

def graph_stats(stats):
  import matplotlib.pyplot as plt
  for k,v in stats.items():
    localTime = [t[0] for t in v]
    engineTime = [t[2] for t in v]
    y = [t[1] for t in v]
    errorTime = [localTime[i] - engineTime[i] for i in range(len(v))]
    minError = errorTime[0]
    print(minError,errorTime)
    errorTime = [x - minError for x in errorTime]
    meanError = sum(errorTime)/len(errorTime)
    stall = [engineTime[i] -engineTime[i-1] - 300 for i in range(1,len(v))]
    meanStall = sum(stall)/len(stall)
    maxStall = max(stall)
    plt.plot(localTime,y,'o-',label=k+" (%.2f,%2.f,%.2f)"%(meanError,meanStall,maxStall))
    for t in range(len(localTime)):
      plt.plot((localTime[t],localTime[t]-errorTime[t]),(y[t],0),color='r')
    # plt.errorbar(localTime,y,fmt='o-',xerr=[[0 for _ in v],errorTime],label=k)
  plt.legend(loc='lower right')
  plt.show()

if __name__ == "__main__":
  sessionPath = "/Users/Tintamar/Documents/lgml.lgml"
  with LGMLSession(sessionPath) as s:
    s.closeWhenEnded = False
    ori = s.read()
    volParam = LGMLParam("/node/audiodevicein/volume1")
    s.run()
    rangeMax = 127*100
    startTime =  time.time();

    for i in range(rangeMax):
      pct = i*1.0/(rangeMax+1)
      #print(str(pct) + "\n")
      #s.send_osc(volParam.address,pct)
      s.send_midi_cc(12,i%127)
      # time.sleep(0.01)
    endTime = time.time();
    diffTime = endTime - startTime
    print("ended loop",rangeMax*1.0/diffTime,"Msg/s")
    time.sleep(3)
    # exit(1)
    # s.save()
    # newS = s.read()
    # time.sleep(2)
    # s.getTree()
    # time.sleep(4)
    # print("!!!!!!!!!!!!!!!!!compare versions")
    # assert(ori==newS)
    # print('ended')
  

  