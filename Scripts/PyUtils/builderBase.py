import os
class BuilderBase:
  rootPath = os.path.abspath(os.path.join(__file__,os.pardir,os.pardir))
  default_cfg_all = {
  "appName": "LGML",
  "njobs" : 1,
  "lgml_root_path" : rootPath,
  "build_cfg_name" : "Debug",

  }

  def __init__(self,cfg=None,actions = None):
    self.cfg = cfg or {};
    self.action = actions or ['build']
    self.verbose = "normal" # or quiet or verbose
    self.applyCfg(self.default_cfg_all)


  def applyCfg(self, default,overwrite = False):
    for k in default:
      if overwrite or not k in self.cfg:
        self.cfg[k] = default[k]

  def getNameWithVersion(self):
    from PyUtils import ProJucerUtils
    name =  self.cfg["appName"]+ "_v"+str(ProJucerUtils.getVersion())
    return name
  




