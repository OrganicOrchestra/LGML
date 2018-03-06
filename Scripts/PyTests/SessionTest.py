import unittest
from LGMLUtils import *
import os
import glob
import time


class SessionTest(unittest.TestCase):
  logout = os.path.join(os.path.dirname(__file__),"tst.log")
  statsFile = os.path.join(os.path.dirname(__file__),"stats.log")
  def session_consistency(self,path):
    with LGMLSession(path) as s:
      ori = s.read()
      s.run()
      s.save()
      newS = s.read()
      self.assertEqual(ori,newS)
      print('ended')

  def osc_flood(self,session,log):
    session.set_controller_param("blockfeedback",False)
    session.set_controller_param("logoutgoingosc",True)
    session.set_controller_param("logincomingosc",True)
    session.set_controller_param("syncallparameters",True)
    i =1;
    volParam = LGMLParam("/node/audiodevicein/volume1")
    freq = 1
    sleep_time = 1/1000.0
    max_time = 2
    max_iter = 10000 if(sleep_time==0) else max_time/sleep_time
    
    session.print_oscIn = False;
    startTime = time.clock()
    while i < max_iter:
      msg = (volParam.address,(i*freq)%max_iter*1.0/max_iter)
      log.write(str(msg)+'\n')
      session.send_osc(*msg)
      i+=1;
      # if i%10==0:
      #   log.flush()
      time.sleep(sleep_time)
    msg = (volParam.address,0.0)
    log.write(str(msg)+'\n')
    timeTaken = time.clock()-startTime
    print('sent : %i msgs in %s : (%.2f msg/s)\n'%(i,timeTaken,i*1.0/timeTaken))
    session.send_osc(*msg)
    log.flush();
    session.save(block=True)

    savedVol = volParam.readInSession(session)
    print(savedVol)
    self.assertEqual(savedVol,0)


def create_test (path):
  def do_test(self):
    # self.session_consistency(path)
      with LGMLSession(path) as s:
        with open(self.logout,'w') as log:
          s.run(stdout_filep=log)
          print('start osc flood test')
          self.osc_flood(s,log)
          #time.sleep(3)
          print('end test')
        stats = s.stats
      graph_stats(stats)
      with open(self.statsFile,'w') as stat:
          json.dump(stats,stat);
  return do_test

scriptPath = os.path.dirname(os.path.abspath(__file__));
exampleDir = os.path.abspath(os.path.join(scriptPath,os.path.pardir,os.pardir,'Examples/'))
paths = glob.glob(os.path.join(exampleDir,"**/*.lgml"))

for k in paths:
  test_method = create_test (k)
  test_method.__name__ = 'test_file_%s' % k
  setattr (SessionTest, test_method.__name__, test_method)

if __name__ == '__main__':
  unittest.main()
  
