cmake_minimum_required(VERSION 3.22)

project(KryptoSyn VERSION 0.0.1 LANGUAGES C CXX)

# activation of the C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# optimizations and deactivations
add_compile_definitions(JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
add_compile_definitions(JUCE_VST3_CAN_REPLACE_VST2=0)

# integrate JUCE
add_subdirectory(JUCE)

# Ffley's GUI magic module (slightly modified vendor version)
juce_add_module(modules/foleys_gui_magic)

# create plugin target
juce_add_plugin(Kryptosync
		COMPANY_NAME "WIVITEC"
		BUNDLE_ID "com.wivitec.Kryptosync"
		VERSION "1.0.0"
		PLUGIN_MANUFACTURER_CODE "Wivi"
		PLUGIN_CODE "Wvs1"
		VST3_CATEGORIES "Synth" "Generator"
		EDITOR_WANTS_KEYBOARD_FOCUS TRUE
		IS_SYNTH TRUE
		NEEDS_MIDI_INPUT TRUE
		NEEDS_MIDI_OUTPUT FALSE
		COPY_PLUGIN_AFTER_BUILD TRUE
		FORMATS VST3 Standalone
		PRODUCT_NAME "Kryptosync"
		VST3_AUTO_MANIFEST FALSE
)

# link all source files
target_sources(Kryptosync PRIVATE
		Source/PluginProcessor.cpp
		Source/PluginProcessor.h
		Source/SynthVoice.cpp
		Source/SynthVoice.h
		Source/SynthSound.h
		Source/SamplerVoice.h
		Source/SamplerVoice.cpp
		Source/UI/CustomLNF.cpp
		Source/UI/CustomLNF.h
		Source/UI/CustomLNFButton.h
		Source/UI/ADSRPlot.h
		Source/UI/CustomLNFTextButton.h
		Source/UI/AbsynthTabLookAndFeel.h
		Source/Data/AdsrData.cpp
		Source/Data/AdsrData.h
		Source/Data/FilterData.cpp
		Source/Data/FilterData.h
		Source/Data/FilterBase.h
		Source/Data/Lfos.cpp
		Source/Data/Lfos.h
		Source/Data/MasterEQ.cpp
		Source/Data/MasterEQ.h
		Source/Data/FormantFilter.h
		Source/Data/FormantFilter.cpp
		Source/UI/SpectrogramMatrixItem.cpp
		Source/UI/SpectrogramMatrixItem.h
		Source/UI/SampleBrowserItem.cpp
		Source/UI/SampleBrowserItem.h
		Source/Components/CustomMouseComponent.h
		Source/Components/SpectrogramCanvas.h
		Source/Components/SpectrogramCanvas.cpp
		Source/Components/SampleBrowserComponent.h
		Source/Components/HelpTextItemComponent.h
		Source/Helpers/PixelFIFO.h
		Source/Helpers/CBuffer.cpp
		Source/Helpers/CBuffer.h
		Source/Helpers/GuiUtils.h
		Source/Helpers/GuiUtils.cpp
		Source/Helpers/FileHandling.h
		Source/Helpers/FileHandling.cpp
		Source/UI/HelpTextItem.cpp
		Source/UI/HelpTextItem.h
)

# package resources into BinaryData (compiled directly into the binary for the release)
juce_add_binary_data(Kryptosync_data
		SOURCES
		Resources/magic.xml
		Resources/Instructions.Text.txt
		Resources/License.Text.txt
		Resources/factory.1.preset
		Resources/factory.2.preset
		Resources/factory.3.preset
		Resources/decibel.png
		Resources/adsrGitter.png
		Resources/mySlider.svg
		Resources/blueLED.new.png
		Resources/greenLED.png
		Resources/meterBg.png
		Resources/Pfeil.png
		Resources/ChorusTxt.png
		Resources/FlangerTxt.png
		Resources/DelayTxt.png
		Resources/ReverbTxt.png
		Resources/SpectroSld.png
		Resources/modDiagramm.png
		Resources/Synth.Name.png
		Resources/Instructions.png
		Resources/meterColorMask.png
		Resources/meterColorMaskSpec.png
		Resources/strip.slider.128.bgr.png
		Resources/strip.slider.128.ggr.png
		Resources/strip.slider.128.org.png
		Resources/strip.slider.128.bgr.small.png
		Resources/strip.slider.128.ggr.small.png
		Resources/strip.slider.128.org.small.png
)

# --- SynthLab source files ---
set(SYNTHLAB_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/SDKs/SynthLab)
set(SYNTHLAB_SRC_DIR ${SYNTHLAB_ROOT}/source)

set(SYNTHLAB_SOURCES
		"${SYNTHLAB_SRC_DIR}/wtoscillator.cpp"
		"${SYNTHLAB_SRC_DIR}/wtoscillator.h"
		"${SYNTHLAB_SRC_DIR}/classicwtcore.cpp"
		"${SYNTHLAB_SRC_DIR}/classicwtcore.h"
		"${SYNTHLAB_SRC_DIR}/morphwtcore.cpp"
		"${SYNTHLAB_SRC_DIR}/morphwtcore.h"
		"${SYNTHLAB_SRC_DIR}/fourierwtcore.cpp"
		"${SYNTHLAB_SRC_DIR}/fourierwtcore.h"
		"${SYNTHLAB_SRC_DIR}/synthbase.cpp"
		"${SYNTHLAB_SRC_DIR}/synthbase.h"
		"${SYNTHLAB_SRC_DIR}/synthfunctions.h"
		"${SYNTHLAB_SRC_DIR}/synthconstants.h"
		"${SYNTHLAB_SRC_DIR}/synthstructures.h"
		"${SYNTHLAB_SRC_DIR}/synthlabparams.h"
		"${SYNTHLAB_SRC_DIR}/basiclookuptables.cpp"
		"${SYNTHLAB_SRC_DIR}/basiclookuptables.h"
		"${SYNTHLAB_SRC_DIR}/bleptables.h"
		"${SYNTHLAB_SRC_DIR}/wtsource.h"
		"${SYNTHLAB_SRC_DIR}/synthlabwtsource.h"
		"${SYNTHLAB_SRC_DIR}/dynamictablesource.h"
		"${SYNTHLAB_SRC_DIR}/sfxwtcore.cpp"
		"${SYNTHLAB_SRC_DIR}/sfxwtcore.h"
		"${SYNTHLAB_SRC_DIR}/drumwtcore.cpp"
		"${SYNTHLAB_SRC_DIR}/drumwtcore.h"
)

if(NOT APPLE)
	list(FILTER SYNTHLAB_SOURCES EXCLUDE REGEX "dm_support/.*")
	list(FILTER SYNTHLAB_SOURCES EXCLUDE REGEX "synthlabdm\\.cpp")
	list(FILTER SYNTHLAB_SOURCES EXCLUDE REGEX "module_templates/.*")
endif()

add_library(SynthLab STATIC ${SYNTHLAB_SOURCES})
target_compile_definitions(SynthLab PUBLIC SYNTHLAB_WT=1)
set_target_properties(SynthLab PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Definitions for the main project
# here, we use generator expressions:
# If CONFIG:Debug is active, FOLEYS_SHOW_GUI_EDITOR_PALLETTE is set to 1, otherwise to 0.
target_compile_definitions(Kryptosync PUBLIC
		FOLEYS_ENABLE_BINARY_DATA=1
		FOLEYS_ENABLE_OPEN_GL_CONTEXT=0
		$<$<CONFIG:Debug>:FOLEYS_SHOW_GUI_EDITOR_PALLETTE=1>
		$<$<NOT:$<CONFIG:Debug>>:FOLEYS_SHOW_GUI_EDITOR_PALLETTE=0>
)

# without generator expressions – safe, clean, and doesn't break compiler probing
target_compile_definitions(Kryptosync PRIVATE
		"SYNTHLAB_SAMPLES_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/SynthLabSamples\""
)

# make header paths known

target_include_directories(SynthLab PUBLIC
		${SYNTHLAB_SRC_DIR}
		${SYNTHLAB_SRC_DIR}/core_templates
		${SYNTHLAB_SRC_DIR}/module_templates
		${SYNTHLAB_ROOT}/wavetables
		${SYNTHLAB_ROOT}/examples/programming_guide
		${SYNTHLAB_ROOT}/examples/synthlab_examples
)

target_include_directories(Kryptosync PRIVATE Source)
target_include_directories(Kryptosync PRIVATE
		${CMAKE_CURRENT_BINARY_DIR}/juce_binarydata_Kryptosync_data/JuceLibraryCode
)

# link to libraries
target_link_libraries(Kryptosync PUBLIC
		SynthLab
		Kryptosync_data
		juce::juce_audio_utils
		juce::juce_audio_basics
		juce::juce_audio_formats
		juce::juce_audio_processors
		juce::juce_audio_devices
		juce::juce_gui_extra
		juce::juce_gui_basics
		juce::juce_data_structures
		juce::juce_graphics
		juce::juce_events
		juce::juce_core
		juce::juce_dsp
		juce::juce_cryptography
		foleys_gui_magic
)

# Linux-specific configurations
if(UNIX AND NOT APPLE)
	set_target_properties(Kryptosync PROPERTIES VST3_OUTPUT_DIRECTORY "$ENV{HOME}/.vst3")
	add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)

	# --- POST-BUILD AUTOMATION ---
	add_custom_command(TARGET Kryptosync POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/SynthLabSamples"
			"$<TARGET_FILE_DIR:Kryptosync>/SynthLabSamples"
			COMMENT "Kopiere lokale SynthLabSamples in das Build-Verzeichnis..."
	)
endif()