import os
class BuilderBase:
  rootPath = os.path.abspath(os.path.join(__file__,os.pardir,os.pardir))
  default_cfg_all = {
  "appName": "LGML",
  "njobs" : 1,
  "lgml_root_path" : rootPath,
  "build_cfg_name" : "Debug",

  }

  def __init__(self,cfg=None):
    self.cfg = cfg or {};
    self.verbose = "normal" # or quiet or verbose
    self.applyCfg(self.default_cfg_all)


  def applyCfg(self, newCfg,overwrite = False):
    for k in newCfg:
      if overwrite or not k in self.cfg or self.cfg[k] is None:
        self.cfg[k] = newCfg[k]

  def getNameWithVersion(self):
    from PyUtils import ProJucerUtils
    name =  self.cfg["appName"]+ "_v"+str(ProJucerUtils.getVersion())
    return name
  

  def getBinaryPath(self):
    if "binary_path" in self.cfg:
      return self.cfg["binary_path"]
    else :
      raise NameError("no binary_path provided")

  # default package method zips application file
  def packageApp(self,export_path):
    from zipfile import ZipFile
    source =self.getBinaryPath();
    if not os.path.exists(export_path):
      raise NameError("can't package to "+export_path)
    zipname = self.getNameWithVersion()+"_"+self.cfg["build_os"]+"_"+self.cfg["arch"]+".zip"
    zipPath = os.path.join(export_path,zipname)
    with ZipFile(zipPath,'w') as z:
      z.write(source,arcname=os.path.basename(source))
      return zipPath






