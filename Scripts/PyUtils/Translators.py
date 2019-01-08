
# class YTranslator():
#   baseAddr = "https://translate.yandex.net/api/v1.5/tr.json/translate"
#   def translate(self,text,src,dest):
#     import requests  
#     r = requests.get(self.baseAddr,params={'key':os.environ["YTRANS_TOKEN"],'text':text,'lang':'%s-%s'%(src,dest)})
#     print (r.json())
#     return r.json()['text']

# class GTranslator():
#   baseAddr = 'https://translate.google.com/'

#   def translate(self,text,src,dest):
#     import requests
#     r = requests.get(self.baseAddr,params={'um':1,'ie':'UTF-8','hl':dest,'client':'tw-ob','view':'home','op':'translate',
#                                       'sl':src,'tl':dest,'text':text},
#                                       headers={'User-Agent': 'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/39.0.2171.95 Safari/537.36'}
# )
#     print (r.text)
#     print (r.json())
#     return r.json()['text']

# class MTranslator():
#   import uuid
#   base_url = 'https://api.cognitive.microsofttranslator.com'
#   path = '/translate?api-version=3.0'
#   headers = {
#     'Ocp-Apim-Subscription-Key': os.environ['AZURE_TRANS_TOKEN'],
#     'Content-type': 'application/json',
#     'X-ClientTraceId': str(uuid.uuid4())
#   }
#   maxNumChar = 2000
#   allowList = True
#   def translate(self,text,src,dest):
#     import requests
#     params = '&from'+src+'&to='+dest
#     constructed_url = self.base_url + self.path + params
#     jsonR = [{'Text' : ' \n '.join(k.split('\n'))} for k in text]
#     request = requests.post(constructed_url, headers=self.headers, json=jsonR)
#     return [k["translations"][0]['text'] for k in request.json()]

import pyperclip
class ManualTranslator():
    
    maxNumChar = 4800
    allowList = True
    def translate(self,text,src,dest):
      if(not text[-1]):
          text = text[:-1]
      strL = '\n'.join(text)
      print('copying to pasteboard (%i chars)'%(len(strL)))
      pyperclip.copy(strL)
      print('##################')
      input('enter when translation from google is in pasteboard')
      gTranslated = pyperclip.paste()
      if(gTranslated):
        gTranslated = gTranslated.split('\n')
        if(not gTranslated[-1]):
          gTranslated = gTranslated[:-1]
        if(len(gTranslated)!=len(text)):
          raise Error('parsing error num lines mismatch')
        print(gTranslated)
      else:
        print('nothing pasted exiting')
        exit(0)
      return gTranslated
