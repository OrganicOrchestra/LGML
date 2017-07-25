#include "JsEnvironmentUI.h"
#include "ParameterUIFactory.h"
#include "StringParameterUI.h"

JsEnvironmentUI::JsEnvironmentUI(JSEnvContainer * _cont) :cont(_cont) {
  env = cont->jsEnv;
  jassert(env);
	env->addListener(this);
  loadFileB = ParameterUIFactory::createDefaultUI(cont->loadT);
	addAndMakeVisible(loadFileB);


	reloadB = ParameterUIFactory::createDefaultUI(cont->reloadT);
	addAndMakeVisible(reloadB);


  openB = ParameterUIFactory::createDefaultUI(cont->showT);
	addAndMakeVisible(openB);

  watchT = ParameterUIFactory::createDefaultUI(cont->autoWatch);
	addAndMakeVisible(watchT);
  logEnvB = ParameterUIFactory::createDefaultUI(cont->logT);
	addAndMakeVisible(logEnvB);


  path = new StringParameterUI(cont->scriptPath);
  path->setNameLabelVisible(true);
  addAndMakeVisible(path);

    newJsFileLoaded(env->hasValidJsFile());
	addAndMakeVisible(validJsLed);
  

}
JsEnvironmentUI::~JsEnvironmentUI(){
  env->removeListener(this);
}

void JsEnvironmentUI::resized() {
	Rectangle<int> area = getLocalBounds().reduced(1);
	const int logEnvSize = 40;
  path->setBounds(area.removeFromTop(area.getHeight()/2).reduced(2));
	const int ledSize = area.getHeight()/2;
	const int step = (area.getWidth() - logEnvSize - ledSize) / 4;
	buildLed(ledSize);

  area.reduce(0, 2);
	validJsLed.setBounds(area.removeFromLeft(ledSize).reduced(0, (area.getHeight() - ledSize) / 2));
	loadFileB->setBounds(area.removeFromLeft(step).reduced(2,0));
	reloadB->setBounds(area.removeFromLeft(step).reduced(2,0));
	openB->setBounds(area.removeFromLeft(step).reduced(2,0));
	watchT->setBounds(area.removeFromLeft(step).reduced(2,0));
	logEnvB->setBounds(area.removeFromLeft(logEnvSize).reduced(2,0));

}

void JsEnvironmentUI::buildLed(int size) {
	Path circle;
	circle.addEllipse(Rectangle<float>(0, 0, (float)size, (float)size));
	validJsLed.setPath(circle);
}

void JsEnvironmentUI::newJsFileLoaded(bool) {
  validJsLed.setFill(FillType((env->hasValidJsFile() && env->isInSyncWithLGML()) ? Colours::green :
                              (env->hasValidJsFile() ? Colours::orange :
                               Colours::red)));
}


