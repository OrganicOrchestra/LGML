file = '/Users/Tintamar/owncloud/Projets/Chrones/Sessions/lgml/chronesWIPMartin.lgml'
outfile = '/tmp/tst.lgml'


import json



def do():
  with open(file,'r') as inF:
    jdata = json.load(inF)

  res = transform(jdata) 


  with open(outfile,'w') as oF:
    json.dump(res,oF,indent=2)


def transformNodes(d,dout):
  for k,v in d.items():

    if k=='parameters':
      dout[k] = {}
      for e in v:
        print(e)
        dout[k][e['controlAddress'][1:]] = e['value']
    elif k=='nodes':
      dout[k] = {}
      for n in v:
        td = {}
        dout[k][n['parameters'][0]["value"]] = td

        transformNodes(n,td)
    else:
      dout[k] = v




def transform(d):
  res = {'nodeManager':{'mainContainer':{}}}
  nodes = d['nodeManager']['mainContainer']

  newDic = res['nodeManager']['mainContainer']

  transformNodes(nodes,newDic)
  print (res)
  return res

do()