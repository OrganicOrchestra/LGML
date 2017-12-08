import unittest
from LGMLUtils import *
import os
import glob
import time


class SessionTest(unittest.TestCase):
  logout = os.path.join(os.path.dirname(__file__),"tst.log")
  def session_consistency(self,path):
    with LGMLSession(path) as s:
      ori = s.read()
      s.run()
      s.save()
      newS = s.read()
      self.assertEqual(ori,newS)
      print('ended')

  def osc_flood(self,session,log):
    session.set_blockfeedback(False)
    session.set_logoutgoingosc(True)
    i =1;
    volParam = LGMLParam("/node/audiodeviceinnode/volume1")
    freq = 1
    sleep_time = 0.#00001
    max_time = 2
    max_iter = 1000 if(sleep_time==0) else max_time/sleep_time
    max_iter = 100000
    session.print_oscIn = False;
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
          log.write("init")
          log.flush()
          s.run(stdout_filep=log)
          print('start osc flood test')
          self.osc_flood(s,log)
          print('end test')
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
  
