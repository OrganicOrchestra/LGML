# Automatically generated makefile, created by the Projucer
# Don't edit this file! Your changes will be overwritten when you re-save the Projucer project!

# (this disables dependency generation if multiple architectures are set)
DEPFLAGS := $(if $(word 2, $(TARGET_ARCH)), , -MMD)

ifndef CONFIG
  CONFIG=Debug
endif

ifeq ($(CONFIG),Debug)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Debug
  OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=armv6
  endif

  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "DEBUG=1" -D "_DEBUG=1" -D "JUCER_LINUX_MAKE_6D53C8B4=1" -D "JUCE_APP_VERSION=0.2.44" -D "JUCE_APP_VERSION_HEX=0x22c" -I /usr/include -I /usr/include/freetype2 -I ~/SDKs/VST3\ SDK -I ../../JuceLibraryCode -I D:/Softwares/Juce\ 4.2.1/JUCE/modules
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -g -ggdb -O0
  CXXFLAGS += $(CFLAGS) -std=c++11
  LDFLAGS += $(TARGET_ARCH) -L$(BINDIR) -L$(LIBDIR) -L/usr/X11R6/lib/ -lGL -lX11 -lXext -lXinerama -lasound -ldl -lfreetype -lpthread -lrt 

  TARGET := LGML
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
  CLEANCMD = rm -rf $(OUTDIR)/$(TARGET) $(OBJDIR)
endif

ifeq ($(CONFIG),Release)
  BINDIR := build
  LIBDIR := build
  OBJDIR := build/intermediate/Release
  OUTDIR := build

  ifeq ($(TARGET_ARCH),)
    TARGET_ARCH := -march=armv6
  endif

  CPPFLAGS := $(DEPFLAGS) -D "LINUX=1" -D "NDEBUG=1" -D "JUCER_LINUX_MAKE_6D53C8B4=1" -D "JUCE_APP_VERSION=0.2.44" -D "JUCE_APP_VERSION_HEX=0x22c" -I /usr/include -I /usr/include/freetype2 -I ~/SDKs/VST3\ SDK -I ../../JuceLibraryCode -I D:/Softwares/Juce\ 4.2.1/JUCE/modules
  CFLAGS += $(CPPFLAGS) $(TARGET_ARCH) -O3
  CXXFLAGS += $(CFLAGS) -std=c++11
  LDFLAGS += $(TARGET_ARCH) -L$(BINDIR) -L$(LIBDIR) -fvisibility=hidden -L/usr/X11R6/lib/ -lGL -lX11 -lXext -lXinerama -lasound -ldl -lfreetype -lpthread -lrt 

  TARGET := LGML
  BLDCMD = $(CXX) -o $(OUTDIR)/$(TARGET) $(OBJECTS) $(LDFLAGS) $(RESOURCES) $(TARGET_ARCH)
  CLEANCMD = rm -rf $(OUTDIR)/$(TARGET) $(OBJDIR)
endif

OBJECTS := \
  $(OBJDIR)/ControllableUI_c544de6.o \
  $(OBJDIR)/TriggerBlinkUI_693175a1.o \
  $(OBJDIR)/TriggerButtonUI_d1a8d47f.o \
  $(OBJDIR)/TriggerUI_2ce661ad.o \
  $(OBJDIR)/ControllableHelpers_912c061d.o \
  $(OBJDIR)/ParameterProxyUI_7009c7a2.o \
  $(OBJDIR)/FloatStepperUI_bb9cf532.o \
  $(OBJDIR)/BoolToggleUI_bb303d7b.o \
  $(OBJDIR)/FloatSliderUI_d00e5892.o \
  $(OBJDIR)/IntSliderUI_a48244a5.o \
  $(OBJDIR)/IntStepperUI_75a68b7f.o \
  $(OBJDIR)/ParameterUI_37f5cbde.o \
  $(OBJDIR)/StringParameterUI_aac76e8d.o \
  $(OBJDIR)/BoolParameter_60c297c0.o \
  $(OBJDIR)/FloatParameter_67c285b6.o \
  $(OBJDIR)/IntParameter_21cc1c03.o \
  $(OBJDIR)/Parameter_73564c8a.o \
  $(OBJDIR)/ParameterProxy_74fe694e.o \
  $(OBJDIR)/PresetManager_fc30642f.o \
  $(OBJDIR)/StringParameter_fe3ecef9.o \
  $(OBJDIR)/Controllable_d3fdb092.o \
  $(OBJDIR)/ControllableContainer_f3a7c159.o \
  $(OBJDIR)/ControllableIndexedContainer_a91d6542.o \
  $(OBJDIR)/Trigger_dbc00a19.o \
  $(OBJDIR)/SerialManager_3fa6f9fa.o \
  $(OBJDIR)/SerialPort_11c2339e.o \
  $(OBJDIR)/SerialUIHelper_464101df.o \
  $(OBJDIR)/MIDIListener_ec38fcde.o \
  $(OBJDIR)/MIDIManager_ba3551cd.o \
  $(OBJDIR)/MIDIUIHelper_1d7da46c.o \
  $(OBJDIR)/JsEnvironmentUI_eb77ea7f.o \
  $(OBJDIR)/JsLiveEditor_98e58f2b.o \
  $(OBJDIR)/JsContainerSync_318a26d4.o \
  $(OBJDIR)/JsEnvironment_3c619f6b.o \
  $(OBJDIR)/JsGlobalEnvironment_c35c5308.o \
  $(OBJDIR)/SerialController_3b6eab39.o \
  $(OBJDIR)/SerialControllerEditor_48dab746.o \
  $(OBJDIR)/SerialControllerUI_5b713acd.o \
  $(OBJDIR)/ControllerManagerUI_7e8032a6.o \
  $(OBJDIR)/ControllerUIHelpers_d1429d16.o \
  $(OBJDIR)/ControlVariable_ab8faeda.o \
  $(OBJDIR)/ControlVariableUI_475fda2e.o \
  $(OBJDIR)/Controller_2961cb25.o \
  $(OBJDIR)/ControllerEditor_59f93632.o \
  $(OBJDIR)/ControllerUI_991c0fb9.o \
  $(OBJDIR)/DMXController_ce875bec.o \
  $(OBJDIR)/MIDIController_360cde86.o \
  $(OBJDIR)/MIDIControllerEditor_1c067cd3.o \
  $(OBJDIR)/MIDIControllerUI_274fceda.o \
  $(OBJDIR)/JavascriptController_d1e074f2.o \
  $(OBJDIR)/JavascriptControllerEditor_8572923f.o \
  $(OBJDIR)/JavascriptControllerUI_1c977a46.o \
  $(OBJDIR)/OSCDirectController_935da9e5.o \
  $(OBJDIR)/OSCCustomController_ec14cb6d.o \
  $(OBJDIR)/OSCCustomControllerEditor_eb2ce07a.o \
  $(OBJDIR)/OSCCustomControllerUI_7b101e01.o \
  $(OBJDIR)/OSCControllerUI_5eeed4b0.o \
  $(OBJDIR)/OSCControllerEditor_be62a7a9.o \
  $(OBJDIR)/OSCController_303f61dc.o \
  $(OBJDIR)/ControlManager_886b319.o \
  $(OBJDIR)/FastMapperUI_9e2f9d1a.o \
  $(OBJDIR)/FastMapUI_bf555555.o \
  $(OBJDIR)/FastMap_2927c7c1.o \
  $(OBJDIR)/FastMapper_f30b3cc6.o \
  $(OBJDIR)/ScriptedCondition_af318e72.o \
  $(OBJDIR)/ScriptedConditionUI_ea0833c6.o \
  $(OBJDIR)/ScriptedConsequence_2ad9a41a.o \
  $(OBJDIR)/ScriptedConsequenceUI_1c017f6e.o \
  $(OBJDIR)/ControlVariableReferenceUI_e5de40ef.o \
  $(OBJDIR)/RuleEditor_646e2092.o \
  $(OBJDIR)/RuleConditionGroupUI_8bb2995d.o \
  $(OBJDIR)/RuleConditionUI_d4953b34.o \
  $(OBJDIR)/RuleConsequenceUI_976a635c.o \
  $(OBJDIR)/RuleManagerUI_5a16ac46.o \
  $(OBJDIR)/RuleUI_fc652a19.o \
  $(OBJDIR)/ControlVariableReference_8c4ad1db.o \
  $(OBJDIR)/Rule_beedfd85.o \
  $(OBJDIR)/RuleCondition_e2959960.o \
  $(OBJDIR)/RuleConditionGroup_96966dc9.o \
  $(OBJDIR)/RuleConsequence_1566ab88.o \
  $(OBJDIR)/RuleManager_698426f2.o \
  $(OBJDIR)/NodeBase_eef060fc.o \
  $(OBJDIR)/ConnectableNode_7ac32ac7.o \
  $(OBJDIR)/NodeContainer_ab155880.o \
  $(OBJDIR)/NodeContainerContentUI_a3d0d777.o \
  $(OBJDIR)/NodeContainerViewer_323a4972.o \
  $(OBJDIR)/NodeContainerUI_7c21b254.o \
  $(OBJDIR)/NodeBaseAudioCtlUI_23419795.o \
  $(OBJDIR)/NodeBaseContentUI_25719a7b.o \
  $(OBJDIR)/NodeBaseHeaderUI_69c3237d.o \
  $(OBJDIR)/NodeBaseUI_355c8bd0.o \
  $(OBJDIR)/ConnectableNodeContentUI_25eda510.o \
  $(OBJDIR)/ConnectableNodeAudioCtlUI_3246dfa0.o \
  $(OBJDIR)/ConnectableNodeHeaderUI_5100f248.o \
  $(OBJDIR)/ConnectableNodeUI_17a40edb.o \
  $(OBJDIR)/Data_fc0f4f33.o \
  $(OBJDIR)/DataProcessorGraph_756098af.o \
  $(OBJDIR)/NodeConnectionEditor_faed6a16.o \
  $(OBJDIR)/NodeConnectionEditorDataSlot_356891fe.o \
  $(OBJDIR)/NodeConnectionEditorLink_f98d58b0.o \
  $(OBJDIR)/ConnectorComponent_91ce38b9.o \
  $(OBJDIR)/NodeConnection_d2f3ba09.o \
  $(OBJDIR)/NodeConnectionUI_25edd59d.o \
  $(OBJDIR)/JsNode_314f14f4.o \
  $(OBJDIR)/JsNodeUI_5ade21c8.o \
  $(OBJDIR)/ContainerOutNode_8c5d19b8.o \
  $(OBJDIR)/ContainerInNode_4e507909.o \
  $(OBJDIR)/AudioDeviceInNode_cb757d94.o \
  $(OBJDIR)/AudioDeviceInNodeUI_50ce268.o \
  $(OBJDIR)/AudioDeviceOutNode_b3d8a68d.o \
  $(OBJDIR)/AudioDeviceOutNodeUI_6149b121.o \
  $(OBJDIR)/DataInNode_dd21ae1a.o \
  $(OBJDIR)/DataInNodeUI_5c6f096e.o \
  $(OBJDIR)/DummyNode_fd1bb32b.o \
  $(OBJDIR)/DummyNodeContentUI_375e4d2c.o \
  $(OBJDIR)/AudioMixerNode_60d9bf7e.o \
  $(OBJDIR)/AudioMixerNodeUI_d26851d2.o \
  $(OBJDIR)/LooperNode_9342affc.o \
  $(OBJDIR)/LooperNodeUI_e571ad0.o \
  $(OBJDIR)/LooperTrack_e7785edb.o \
  $(OBJDIR)/SpatNode_270022fb.o \
  $(OBJDIR)/PluginWindow_609dbc0c.o \
  $(OBJDIR)/VSTManager_134ffcdf.o \
  $(OBJDIR)/VSTNode_c6d00fda.o \
  $(OBJDIR)/VSTNodeUI_940bfb2e.o \
  $(OBJDIR)/NodeFactory_4513bb09.o \
  $(OBJDIR)/NodeManagerUI_ff28af00.o \
  $(OBJDIR)/NodeManager_42cb902c.o \
  $(OBJDIR)/LGMLLogger_4146caf3.o \
  $(OBJDIR)/LGMLLoggerUI_4bc05407.o \
  $(OBJDIR)/CustomEditor_3b5461c7.o \
  $(OBJDIR)/GenericControllableContainerEditor_117ebfb7.o \
  $(OBJDIR)/InspectableComponent_72aa9838.o \
  $(OBJDIR)/Inspector_d6237cd8.o \
  $(OBJDIR)/InspectorEditor_dded95a5.o \
  $(OBJDIR)/TimeManager_75588e81.o \
  $(OBJDIR)/TimeManagerUI_c26f6c15.o \
  $(OBJDIR)/TimeMasterCandidate_6cba9f15.o \
  $(OBJDIR)/ProjectManager_ec121fdd.o \
  $(OBJDIR)/SettingsManager_f0565d6b.o \
  $(OBJDIR)/ShapeShifterFactory_6c2ae3bd.o \
  $(OBJDIR)/ShapeShifterPanelHeader_8b4f9de4.o \
  $(OBJDIR)/ShapeShifterPanelTab_a4791be8.o \
  $(OBJDIR)/ShapeShifterContent_505997ac.o \
  $(OBJDIR)/ShapeShifter_bf2b9b57.o \
  $(OBJDIR)/GapGrabber_4d565216.o \
  $(OBJDIR)/ShapeShifterWindow_e5e759e7.o \
  $(OBJDIR)/ShapeShifterPanel_821dd737.o \
  $(OBJDIR)/ShapeShifterContainer_69052434.o \
  $(OBJDIR)/ShapeShifterManager_69e2b8e0.o \
  $(OBJDIR)/LookAndFeelOO_b6d1a33f.o \
  $(OBJDIR)/Engine_7f3228cb.o \
  $(OBJDIR)/EngineFileDocument_abfb722.o \
  $(OBJDIR)/MainComponent_a6ffb4a5.o \
  $(OBJDIR)/MainComponentCommands_b1179f4d.o \
  $(OBJDIR)/Main_90ebc5c2.o \
  $(OBJDIR)/list_ports_linux_698dd22b.o \
  $(OBJDIR)/list_ports_osx_96f9b04b.o \
  $(OBJDIR)/list_ports_win_a40efe83.o \
  $(OBJDIR)/unix_2bfcd685.o \
  $(OBJDIR)/win_d3034fd3.o \
  $(OBJDIR)/serial_ecdf186c.o \
  $(OBJDIR)/BinaryData_ce4232d4.o \
  $(OBJDIR)/juce_audio_basics_6b797ca1.o \
  $(OBJDIR)/juce_audio_devices_a742c38b.o \
  $(OBJDIR)/juce_audio_formats_5a29c68a.o \
  $(OBJDIR)/juce_audio_processors_dea3173d.o \
  $(OBJDIR)/juce_audio_utils_c7eb679f.o \
  $(OBJDIR)/juce_core_75b14332.o \
  $(OBJDIR)/juce_cryptography_6de2ebff.o \
  $(OBJDIR)/juce_data_structures_72d3da2c.o \
  $(OBJDIR)/juce_events_d2be882c.o \
  $(OBJDIR)/juce_graphics_9c18891e.o \
  $(OBJDIR)/juce_gui_basics_8a6da59c.o \
  $(OBJDIR)/juce_gui_extra_4a026f23.o \
  $(OBJDIR)/juce_opengl_cd70b4c2.o \
  $(OBJDIR)/juce_osc_be4cee16.o \

.PHONY: clean

$(OUTDIR)/$(TARGET): $(OBJECTS) $(RESOURCES)
	@echo Linking LGML
	-@mkdir -p $(BINDIR)
	-@mkdir -p $(LIBDIR)
	-@mkdir -p $(OUTDIR)
	@$(BLDCMD)

clean:
	@echo Cleaning LGML
	@$(CLEANCMD)

strip:
	@echo Stripping LGML
	-@strip --strip-unneeded $(OUTDIR)/$(TARGET)

$(OBJDIR)/ControllableUI_c544de6.o: ../../Source/ControllableUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ControllableUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/TriggerBlinkUI_693175a1.o: ../../Source/TriggerBlinkUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling TriggerBlinkUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/TriggerButtonUI_d1a8d47f.o: ../../Source/TriggerButtonUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling TriggerButtonUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/TriggerUI_2ce661ad.o: ../../Source/TriggerUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling TriggerUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ControllableHelpers_912c061d.o: ../../Source/ControllableHelpers.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ControllableHelpers.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ParameterProxyUI_7009c7a2.o: ../../Source/ParameterProxyUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ParameterProxyUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/FloatStepperUI_bb9cf532.o: ../../Source/FloatStepperUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling FloatStepperUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/BoolToggleUI_bb303d7b.o: ../../Source/BoolToggleUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling BoolToggleUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/FloatSliderUI_d00e5892.o: ../../Source/FloatSliderUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling FloatSliderUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/IntSliderUI_a48244a5.o: ../../Source/IntSliderUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling IntSliderUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/IntStepperUI_75a68b7f.o: ../../Source/IntStepperUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling IntStepperUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ParameterUI_37f5cbde.o: ../../Source/ParameterUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ParameterUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/StringParameterUI_aac76e8d.o: ../../Source/StringParameterUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling StringParameterUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/BoolParameter_60c297c0.o: ../../Source/BoolParameter.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling BoolParameter.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/FloatParameter_67c285b6.o: ../../Source/FloatParameter.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling FloatParameter.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/IntParameter_21cc1c03.o: ../../Source/IntParameter.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling IntParameter.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Parameter_73564c8a.o: ../../Source/Parameter.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Parameter.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ParameterProxy_74fe694e.o: ../../Source/ParameterProxy.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ParameterProxy.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/PresetManager_fc30642f.o: ../../Source/PresetManager.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling PresetManager.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/StringParameter_fe3ecef9.o: ../../Source/StringParameter.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling StringParameter.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Controllable_d3fdb092.o: ../../Source/Controllable.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Controllable.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ControllableContainer_f3a7c159.o: ../../Source/ControllableContainer.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ControllableContainer.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ControllableIndexedContainer_a91d6542.o: ../../Source/ControllableIndexedContainer.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ControllableIndexedContainer.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Trigger_dbc00a19.o: ../../Source/Trigger.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Trigger.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SerialManager_3fa6f9fa.o: ../../Source/SerialManager.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SerialManager.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SerialPort_11c2339e.o: ../../Source/SerialPort.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SerialPort.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SerialUIHelper_464101df.o: ../../Source/SerialUIHelper.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SerialUIHelper.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/MIDIListener_ec38fcde.o: ../../Source/MIDIListener.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling MIDIListener.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/MIDIManager_ba3551cd.o: ../../Source/MIDIManager.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling MIDIManager.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/MIDIUIHelper_1d7da46c.o: ../../Source/MIDIUIHelper.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling MIDIUIHelper.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/JsEnvironmentUI_eb77ea7f.o: ../../Source/JsEnvironmentUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling JsEnvironmentUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/JsLiveEditor_98e58f2b.o: ../../Source/JsLiveEditor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling JsLiveEditor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/JsContainerSync_318a26d4.o: ../../Source/JsContainerSync.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling JsContainerSync.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/JsEnvironment_3c619f6b.o: ../../Source/JsEnvironment.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling JsEnvironment.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/JsGlobalEnvironment_c35c5308.o: ../../Source/JsGlobalEnvironment.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling JsGlobalEnvironment.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SerialController_3b6eab39.o: ../../Source/SerialController.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SerialController.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SerialControllerEditor_48dab746.o: ../../Source/SerialControllerEditor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SerialControllerEditor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SerialControllerUI_5b713acd.o: ../../Source/SerialControllerUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SerialControllerUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ControllerManagerUI_7e8032a6.o: ../../Source/ControllerManagerUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ControllerManagerUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ControllerUIHelpers_d1429d16.o: ../../Source/ControllerUIHelpers.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ControllerUIHelpers.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ControlVariable_ab8faeda.o: ../../Source/ControlVariable.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ControlVariable.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ControlVariableUI_475fda2e.o: ../../Source/ControlVariableUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ControlVariableUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Controller_2961cb25.o: ../../Source/Controller.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Controller.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ControllerEditor_59f93632.o: ../../Source/ControllerEditor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ControllerEditor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ControllerUI_991c0fb9.o: ../../Source/ControllerUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ControllerUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/DMXController_ce875bec.o: ../../Source/DMXController.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling DMXController.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/MIDIController_360cde86.o: ../../Source/MIDIController.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling MIDIController.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/MIDIControllerEditor_1c067cd3.o: ../../Source/MIDIControllerEditor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling MIDIControllerEditor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/MIDIControllerUI_274fceda.o: ../../Source/MIDIControllerUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling MIDIControllerUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/JavascriptController_d1e074f2.o: ../../Source/JavascriptController.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling JavascriptController.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/JavascriptControllerEditor_8572923f.o: ../../Source/JavascriptControllerEditor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling JavascriptControllerEditor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/JavascriptControllerUI_1c977a46.o: ../../Source/JavascriptControllerUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling JavascriptControllerUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/OSCDirectController_935da9e5.o: ../../Source/OSCDirectController.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling OSCDirectController.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/OSCCustomController_ec14cb6d.o: ../../Source/OSCCustomController.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling OSCCustomController.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/OSCCustomControllerEditor_eb2ce07a.o: ../../Source/OSCCustomControllerEditor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling OSCCustomControllerEditor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/OSCCustomControllerUI_7b101e01.o: ../../Source/OSCCustomControllerUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling OSCCustomControllerUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/OSCControllerUI_5eeed4b0.o: ../../Source/OSCControllerUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling OSCControllerUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/OSCControllerEditor_be62a7a9.o: ../../Source/OSCControllerEditor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling OSCControllerEditor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/OSCController_303f61dc.o: ../../Source/OSCController.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling OSCController.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ControlManager_886b319.o: ../../Source/ControlManager.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ControlManager.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/FastMapperUI_9e2f9d1a.o: ../../Source/FastMapperUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling FastMapperUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/FastMapUI_bf555555.o: ../../Source/FastMapUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling FastMapUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/FastMap_2927c7c1.o: ../../Source/FastMap.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling FastMap.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/FastMapper_f30b3cc6.o: ../../Source/FastMapper.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling FastMapper.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ScriptedCondition_af318e72.o: ../../Source/ScriptedCondition.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ScriptedCondition.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ScriptedConditionUI_ea0833c6.o: ../../Source/ScriptedConditionUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ScriptedConditionUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ScriptedConsequence_2ad9a41a.o: ../../Source/ScriptedConsequence.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ScriptedConsequence.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ScriptedConsequenceUI_1c017f6e.o: ../../Source/ScriptedConsequenceUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ScriptedConsequenceUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ControlVariableReferenceUI_e5de40ef.o: ../../Source/ControlVariableReferenceUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ControlVariableReferenceUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/RuleEditor_646e2092.o: ../../Source/RuleEditor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling RuleEditor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/RuleConditionGroupUI_8bb2995d.o: ../../Source/RuleConditionGroupUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling RuleConditionGroupUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/RuleConditionUI_d4953b34.o: ../../Source/RuleConditionUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling RuleConditionUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/RuleConsequenceUI_976a635c.o: ../../Source/RuleConsequenceUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling RuleConsequenceUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/RuleManagerUI_5a16ac46.o: ../../Source/RuleManagerUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling RuleManagerUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/RuleUI_fc652a19.o: ../../Source/RuleUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling RuleUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ControlVariableReference_8c4ad1db.o: ../../Source/ControlVariableReference.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ControlVariableReference.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Rule_beedfd85.o: ../../Source/Rule.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Rule.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/RuleCondition_e2959960.o: ../../Source/RuleCondition.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling RuleCondition.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/RuleConditionGroup_96966dc9.o: ../../Source/RuleConditionGroup.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling RuleConditionGroup.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/RuleConsequence_1566ab88.o: ../../Source/RuleConsequence.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling RuleConsequence.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/RuleManager_698426f2.o: ../../Source/RuleManager.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling RuleManager.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/NodeBase_eef060fc.o: ../../Source/NodeBase.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling NodeBase.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ConnectableNode_7ac32ac7.o: ../../Source/ConnectableNode.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ConnectableNode.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/NodeContainer_ab155880.o: ../../Source/NodeContainer.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling NodeContainer.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/NodeContainerContentUI_a3d0d777.o: ../../Source/NodeContainerContentUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling NodeContainerContentUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/NodeContainerViewer_323a4972.o: ../../Source/NodeContainerViewer.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling NodeContainerViewer.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/NodeContainerUI_7c21b254.o: ../../Source/NodeContainerUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling NodeContainerUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/NodeBaseAudioCtlUI_23419795.o: ../../Source/NodeBaseAudioCtlUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling NodeBaseAudioCtlUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/NodeBaseContentUI_25719a7b.o: ../../Source/NodeBaseContentUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling NodeBaseContentUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/NodeBaseHeaderUI_69c3237d.o: ../../Source/NodeBaseHeaderUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling NodeBaseHeaderUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/NodeBaseUI_355c8bd0.o: ../../Source/NodeBaseUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling NodeBaseUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ConnectableNodeContentUI_25eda510.o: ../../Source/ConnectableNodeContentUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ConnectableNodeContentUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ConnectableNodeAudioCtlUI_3246dfa0.o: ../../Source/ConnectableNodeAudioCtlUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ConnectableNodeAudioCtlUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ConnectableNodeHeaderUI_5100f248.o: ../../Source/ConnectableNodeHeaderUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ConnectableNodeHeaderUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ConnectableNodeUI_17a40edb.o: ../../Source/ConnectableNodeUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ConnectableNodeUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Data_fc0f4f33.o: ../../Source/Data.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Data.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/DataProcessorGraph_756098af.o: ../../Source/DataProcessorGraph.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling DataProcessorGraph.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/NodeConnectionEditor_faed6a16.o: ../../Source/NodeConnectionEditor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling NodeConnectionEditor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/NodeConnectionEditorDataSlot_356891fe.o: ../../Source/NodeConnectionEditorDataSlot.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling NodeConnectionEditorDataSlot.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/NodeConnectionEditorLink_f98d58b0.o: ../../Source/NodeConnectionEditorLink.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling NodeConnectionEditorLink.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ConnectorComponent_91ce38b9.o: ../../Source/ConnectorComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ConnectorComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/NodeConnection_d2f3ba09.o: ../../Source/NodeConnection.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling NodeConnection.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/NodeConnectionUI_25edd59d.o: ../../Source/NodeConnectionUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling NodeConnectionUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/JsNode_314f14f4.o: ../../Source/JsNode.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling JsNode.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/JsNodeUI_5ade21c8.o: ../../Source/JsNodeUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling JsNodeUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ContainerOutNode_8c5d19b8.o: ../../Source/ContainerOutNode.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ContainerOutNode.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ContainerInNode_4e507909.o: ../../Source/ContainerInNode.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ContainerInNode.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/AudioDeviceInNode_cb757d94.o: ../../Source/AudioDeviceInNode.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling AudioDeviceInNode.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/AudioDeviceInNodeUI_50ce268.o: ../../Source/AudioDeviceInNodeUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling AudioDeviceInNodeUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/AudioDeviceOutNode_b3d8a68d.o: ../../Source/AudioDeviceOutNode.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling AudioDeviceOutNode.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/AudioDeviceOutNodeUI_6149b121.o: ../../Source/AudioDeviceOutNodeUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling AudioDeviceOutNodeUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/DataInNode_dd21ae1a.o: ../../Source/DataInNode.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling DataInNode.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/DataInNodeUI_5c6f096e.o: ../../Source/DataInNodeUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling DataInNodeUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/DummyNode_fd1bb32b.o: ../../Source/DummyNode.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling DummyNode.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/DummyNodeContentUI_375e4d2c.o: ../../Source/DummyNodeContentUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling DummyNodeContentUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/AudioMixerNode_60d9bf7e.o: ../../Source/AudioMixerNode.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling AudioMixerNode.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/AudioMixerNodeUI_d26851d2.o: ../../Source/AudioMixerNodeUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling AudioMixerNodeUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/LooperNode_9342affc.o: ../../Source/LooperNode.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling LooperNode.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/LooperNodeUI_e571ad0.o: ../../Source/LooperNodeUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling LooperNodeUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/LooperTrack_e7785edb.o: ../../Source/LooperTrack.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling LooperTrack.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SpatNode_270022fb.o: ../../Source/SpatNode.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SpatNode.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/PluginWindow_609dbc0c.o: ../../Source/PluginWindow.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling PluginWindow.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/VSTManager_134ffcdf.o: ../../Source/VSTManager.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling VSTManager.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/VSTNode_c6d00fda.o: ../../Source/VSTNode.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling VSTNode.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/VSTNodeUI_940bfb2e.o: ../../Source/VSTNodeUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling VSTNodeUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/NodeFactory_4513bb09.o: ../../Source/NodeFactory.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling NodeFactory.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/NodeManagerUI_ff28af00.o: ../../Source/NodeManagerUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling NodeManagerUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/NodeManager_42cb902c.o: ../../Source/NodeManager.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling NodeManager.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/LGMLLogger_4146caf3.o: ../../Source/LGMLLogger.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling LGMLLogger.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/LGMLLoggerUI_4bc05407.o: ../../Source/LGMLLoggerUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling LGMLLoggerUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/CustomEditor_3b5461c7.o: ../../Source/CustomEditor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling CustomEditor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/GenericControllableContainerEditor_117ebfb7.o: ../../Source/GenericControllableContainerEditor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling GenericControllableContainerEditor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/InspectableComponent_72aa9838.o: ../../Source/InspectableComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling InspectableComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Inspector_d6237cd8.o: ../../Source/Inspector.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Inspector.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/InspectorEditor_dded95a5.o: ../../Source/InspectorEditor.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling InspectorEditor.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/TimeManager_75588e81.o: ../../Source/TimeManager.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling TimeManager.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/TimeManagerUI_c26f6c15.o: ../../Source/TimeManagerUI.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling TimeManagerUI.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/TimeMasterCandidate_6cba9f15.o: ../../Source/TimeMasterCandidate.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling TimeMasterCandidate.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ProjectManager_ec121fdd.o: ../../Source/ProjectManager.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ProjectManager.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/SettingsManager_f0565d6b.o: ../../Source/SettingsManager.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling SettingsManager.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ShapeShifterFactory_6c2ae3bd.o: ../../Source/ShapeShifterFactory.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ShapeShifterFactory.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ShapeShifterPanelHeader_8b4f9de4.o: ../../Source/ShapeShifterPanelHeader.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ShapeShifterPanelHeader.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ShapeShifterPanelTab_a4791be8.o: ../../Source/ShapeShifterPanelTab.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ShapeShifterPanelTab.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ShapeShifterContent_505997ac.o: ../../Source/ShapeShifterContent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ShapeShifterContent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ShapeShifter_bf2b9b57.o: ../../Source/ShapeShifter.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ShapeShifter.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/GapGrabber_4d565216.o: ../../Source/GapGrabber.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling GapGrabber.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ShapeShifterWindow_e5e759e7.o: ../../Source/ShapeShifterWindow.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ShapeShifterWindow.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ShapeShifterPanel_821dd737.o: ../../Source/ShapeShifterPanel.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ShapeShifterPanel.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ShapeShifterContainer_69052434.o: ../../Source/ShapeShifterContainer.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ShapeShifterContainer.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/ShapeShifterManager_69e2b8e0.o: ../../Source/ShapeShifterManager.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling ShapeShifterManager.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/LookAndFeelOO_b6d1a33f.o: ../../Source/LookAndFeelOO.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling LookAndFeelOO.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Engine_7f3228cb.o: ../../Source/Engine.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Engine.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/EngineFileDocument_abfb722.o: ../../Source/EngineFileDocument.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling EngineFileDocument.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/MainComponent_a6ffb4a5.o: ../../Source/MainComponent.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling MainComponent.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/MainComponentCommands_b1179f4d.o: ../../Source/MainComponentCommands.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling MainComponentCommands.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/Main_90ebc5c2.o: ../../Source/Main.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling Main.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/list_ports_linux_698dd22b.o: ../../Source/libs/serial/impl/list_ports/list_ports_linux.cc
	-@mkdir -p $(OBJDIR)
	@echo "Compiling list_ports_linux.cc"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/list_ports_osx_96f9b04b.o: ../../Source/libs/serial/impl/list_ports/list_ports_osx.cc
	-@mkdir -p $(OBJDIR)
	@echo "Compiling list_ports_osx.cc"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/list_ports_win_a40efe83.o: ../../Source/libs/serial/impl/list_ports/list_ports_win.cc
	-@mkdir -p $(OBJDIR)
	@echo "Compiling list_ports_win.cc"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/unix_2bfcd685.o: ../../Source/libs/serial/impl/unix.cc
	-@mkdir -p $(OBJDIR)
	@echo "Compiling unix.cc"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/win_d3034fd3.o: ../../Source/libs/serial/impl/win.cc
	-@mkdir -p $(OBJDIR)
	@echo "Compiling win.cc"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/serial_ecdf186c.o: ../../Source/libs/serial/serial.cc
	-@mkdir -p $(OBJDIR)
	@echo "Compiling serial.cc"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/BinaryData_ce4232d4.o: ../../JuceLibraryCode/BinaryData.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling BinaryData.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_basics_6b797ca1.o: ../../JuceLibraryCode/juce_audio_basics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_basics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_devices_a742c38b.o: ../../JuceLibraryCode/juce_audio_devices.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_devices.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_formats_5a29c68a.o: ../../JuceLibraryCode/juce_audio_formats.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_formats.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_processors_dea3173d.o: ../../JuceLibraryCode/juce_audio_processors.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_processors.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_audio_utils_c7eb679f.o: ../../JuceLibraryCode/juce_audio_utils.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_audio_utils.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_core_75b14332.o: ../../JuceLibraryCode/juce_core.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_core.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_cryptography_6de2ebff.o: ../../JuceLibraryCode/juce_cryptography.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_cryptography.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_data_structures_72d3da2c.o: ../../JuceLibraryCode/juce_data_structures.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_data_structures.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_events_d2be882c.o: ../../JuceLibraryCode/juce_events.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_events.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_graphics_9c18891e.o: ../../JuceLibraryCode/juce_graphics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_graphics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_gui_basics_8a6da59c.o: ../../JuceLibraryCode/juce_gui_basics.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_gui_basics.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_gui_extra_4a026f23.o: ../../JuceLibraryCode/juce_gui_extra.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_gui_extra.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_opengl_cd70b4c2.o: ../../JuceLibraryCode/juce_opengl.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_opengl.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

$(OBJDIR)/juce_osc_be4cee16.o: ../../JuceLibraryCode/juce_osc.cpp
	-@mkdir -p $(OBJDIR)
	@echo "Compiling juce_osc.cpp"
	@$(CXX) $(CXXFLAGS) -o "$@" -c "$<"

-include $(OBJECTS:%.o=%.d)
