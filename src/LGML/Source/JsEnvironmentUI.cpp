#include "JsEnvironmentUI.h"

JsEnvironmentUI::JsEnvironmentUI(JSEnvContainer * _cont) :cont(_cont) {
  env = cont->jsEnv;
  jassert(env);
	env->addListener(this);
  loadFileB = cont->loadT->createBlinkUI();
	addAndMakeVisible(loadFileB);


	reloadB = cont->reloadT->createBlinkUI();
	addAndMakeVisible(reloadB);


  openB = cont->showT->createBlinkUI();
	addAndMakeVisible(openB);

  watchT = cont->autoWatch->createToggle();
	addAndMakeVisible(watchT);
  logEnvB = cont->logT->createBlinkUI();
	addAndMakeVisible(logEnvB);


  path = cont->scriptPath->createStringParameterUI();
  path->setNameLabelVisible(true);
  addAndMakeVisible(path);

    newJsFileLoaded(env->hasValidJsFile());
	addAndMakeVisible(validJsLed);
  

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


