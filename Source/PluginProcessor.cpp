/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include <foleys_gui_magic/foleys_gui_magic.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

#include <iostream>
#include "UI/ADSRPlot.h"
#include "Helpers/PixelFIFO.h"
#include <iterator> // needed for std::ssize


const juce::Identifier ADSRPlot::typeId { "ADSRPlot" };
struct SpectroParams;
struct SamplerZone;

#pragma region GLOBAL_STRUCTS
struct PropertyLambdaListener : public juce::Value::Listener {
    PropertyLambdaListener (juce::Value v, std::function<void(bool)> callback)
        : valueToListen (v), onChanchedCallback (std::move(callback)) {
        valueToListen.addListener (this);
    }
    ~PropertyLambdaListener() override {
        valueToListen.removeListener (this);
    }
    void valueChanged (juce::Value& v) override {
        if (onChanchedCallback)
            onChanchedCallback (static_cast<bool>(v.getValue()));
    }
    juce::Value valueToListen;
    std::function<void(bool)> onChanchedCallback;
};
struct PropertyActiveListener : public juce::Value::Listener {
    PropertyActiveListener (juce::Value v, std::function<void(bool)> callback)
        : valueToListen (v), onChanchedCallback (std::move(callback)) {
        valueToListen.addListener (this);
    }
    ~PropertyActiveListener() override {
        valueToListen.removeListener (this);
    }
    void valueChanged (juce::Value& v) override {
        if (onChanchedCallback)
            onChanchedCallback (static_cast<bool>(v.getValue()));
    }
    juce::Value valueToListen;
    std::function<void(bool)> onChanchedCallback;
};
struct PropertySamplerListener : public juce::Value::Listener {
    PropertySamplerListener (juce::Value v, std::function<void(bool)> callback)
        : valueToListen (v), onChanchedCallback (std::move(callback)) {
        valueToListen.addListener (this);
    }
    ~PropertySamplerListener() override {
        valueToListen.removeListener (this);
    }
    void valueChanged (juce::Value& v) override {
        if (onChanchedCallback)
            onChanchedCallback (static_cast<bool>(v.getValue()));
    }
    juce::Value valueToListen;
    std::function<void(bool)> onChanchedCallback;
};
struct PropertySamplerFilterListener : public juce::Value::Listener {
    PropertySamplerFilterListener (juce::Value v, std::function<void(bool)> callback)
        : valueToListen (v), onChanchedCallback (std::move(callback)) {
        valueToListen.addListener (this);
    }
    ~PropertySamplerFilterListener() override {
        valueToListen.removeListener (this);
    }
    void valueChanged (juce::Value& v) override {
        if (onChanchedCallback)
            onChanchedCallback (static_cast<bool>(v.getValue()));
    }
    juce::Value valueToListen;
    std::function<void(bool)> onChanchedCallback;
};
struct PropertyEffectsListener : public juce::Value::Listener {
    PropertyEffectsListener (juce::Value v, std::function<void(bool)> callback)
        : valueToListen (v), onChanchedCallback (std::move(callback)) {
        valueToListen.addListener (this);
    }
    ~PropertyEffectsListener() override {
        valueToListen.removeListener (this);
    }
    void valueChanged (juce::Value& v) override {
        if (onChanchedCallback)
            onChanchedCallback (static_cast<bool>(v.getValue()));
    }
    juce::Value valueToListen;
    std::function<void(bool)> onChanchedCallback;
};
struct PropertyButtonListener : public juce::Value::Listener {
    PropertyButtonListener (juce::Value v, std::function<void()> callback)
        : valueToListen (v), onClickCallback (std::move(callback)) {
        valueToListen.addListener (this);
    }
    ~PropertyButtonListener() override {
        valueToListen.removeListener (this);
    }
    void valueChanged (juce::Value& v) override {
        if (static_cast<bool>(v.getValue()))
        {
            if (onClickCallback)
                onClickCallback();

            v.setValue (false);
        }
    }
    juce::Value valueToListen;
    std::function<void()> onClickCallback;
};
struct PropertyTutorialListener : public juce::Value::Listener {
    PropertyTutorialListener (juce::Value v, std::function<void()> callback)
        : valueToListen (v), onClickCallback (std::move(callback)) {
        valueToListen.addListener (this);
    }
    ~PropertyTutorialListener() override {
        valueToListen.removeListener (this);
    }
    void valueChanged (juce::Value& v) override {
        // only fire if the button has been pressed (value becomes true)
        if (static_cast<bool>(v.getValue()))
        {
            if (onClickCallback)
                onClickCallback();
            v.setValue (false); // magical reset: makes the ToggleButton to PushButton!
        }
    }
    juce::Value valueToListen;
    std::function<void()> onClickCallback;
};
struct PropertyLicenceListener : public juce::Value::Listener {
    PropertyLicenceListener (juce::Value v, std::function<void()> callback)
        : valueToListen (v), onClickCallback (std::move(callback)) {
        valueToListen.addListener (this);
    }
    ~PropertyLicenceListener() override {
        valueToListen.removeListener (this);
    }
    void valueChanged (juce::Value& v) override {
        if (static_cast<bool>(v.getValue()))
        {
            if (onClickCallback)
                onClickCallback();

            v.setValue (false);
        }
    }
    juce::Value valueToListen;
    std::function<void()> onClickCallback;
};
struct PropertyPlusButListener : public juce::Value::Listener {
    PropertyPlusButListener (juce::Value v, std::function<void()> callback)
        : valueToListen (v), onClickCallback (std::move(callback)) {
        valueToListen.addListener (this);
    }
    ~PropertyPlusButListener() override {
        valueToListen.removeListener (this);
    }
    void valueChanged (juce::Value& v) override {
        if (static_cast<bool>(v.getValue()))
        {
            if (onClickCallback)
                onClickCallback();

            v.setValue (false);
        }
    }
    juce::Value valueToListen;
    std::function<void()> onClickCallback;
};
struct PropertyMedButListener : public juce::Value::Listener {
    PropertyMedButListener (juce::Value v, std::function<void()> callback)
        : valueToListen (v), onClickCallback (std::move(callback)) {
        valueToListen.addListener (this);
    }
    ~PropertyMedButListener() override {
        valueToListen.removeListener (this);
    }
    void valueChanged (juce::Value& v) override {
        if (static_cast<bool>(v.getValue()))
        {
            if (onClickCallback)
                onClickCallback();

            v.setValue (false);
        }
    }
    juce::Value valueToListen;
    std::function<void()> onClickCallback;
};
struct PropertyMinusButListener : public juce::Value::Listener {
    PropertyMinusButListener (juce::Value v, std::function<void()> callback)
        : valueToListen (v), onClickCallback (std::move(callback)) {
        valueToListen.addListener (this);
    }
    ~PropertyMinusButListener() override {
        valueToListen.removeListener (this);
    }
    void valueChanged (juce::Value& v) override {
        if (static_cast<bool>(v.getValue()))
        {
            if (onClickCallback)
                onClickCallback();

            v.setValue (false);
        }
    }
    juce::Value valueToListen;
    std::function<void()> onClickCallback;
};
struct PropertyAnalysButListener : public juce::Value::Listener {
    PropertyAnalysButListener (juce::Value v, std::function<void(bool)> callback)
        : valueToListen (v), onChanchedCallback (std::move(callback)) {
        valueToListen.addListener (this);
    }
    ~PropertyAnalysButListener() override {
        valueToListen.removeListener (this);
    }
    void valueChanged (juce::Value& v) override {
        if (onChanchedCallback)
            onChanchedCallback (static_cast<bool>(v.getValue()));
    }
    juce::Value valueToListen;
    std::function<void(bool)> onChanchedCallback;
};
struct PropertyGainFilterListener : public juce::Value::Listener {
    PropertyGainFilterListener (juce::Value v, std::function<void(bool)> callback)
        : valueToListen (v), onChanchedCallback (std::move(callback)) {
        valueToListen.addListener (this);
    }
    ~PropertyGainFilterListener() override {
        valueToListen.removeListener (this);
    }
    void valueChanged (juce::Value& v) override {
        if (onChanchedCallback)
            onChanchedCallback (static_cast<bool>(v.getValue()));
    }
    juce::Value valueToListen;
    std::function<void(bool)> onChanchedCallback;
};
struct PropertyKeybBigSmallListener : public juce::Value::Listener {
    PropertyKeybBigSmallListener (juce::Value v, std::function<void(bool)> callback)
        : valueToListen (v), onChanchedCallback (std::move(callback)) {
        valueToListen.addListener (this);
    }
    ~PropertyKeybBigSmallListener() override {
        valueToListen.removeListener (this);
    }
    void valueChanged (juce::Value& v) override {
        if (onChanchedCallback)
            onChanchedCallback (static_cast<bool>(v.getValue()));
    }
    juce::Value valueToListen;
    std::function<void(bool)> onChanchedCallback;
};
struct PropertySynthFilterActiveListener : public juce::Value::Listener {
    PropertySynthFilterActiveListener (juce::Value v, std::function<void(bool)> callback)
        : valueToListen (v), onChanchedCallback (std::move(callback)) {
        valueToListen.addListener (this);
    }
    ~PropertySynthFilterActiveListener() override {
        valueToListen.removeListener (this);
    }
    void valueChanged (juce::Value& v) override {
        if (onChanchedCallback)
            onChanchedCallback (static_cast<bool>(v.getValue()));
    }
    juce::Value valueToListen;
    std::function<void(bool)> onChanchedCallback;
};
#pragma endregion GLOBAL_STRUCTS

#pragma region SAMPLER_FINDINGS_HELPER
// locates the main SFZ library folder (searches for 'samples' or 'include' folders)
static juce::File findLibraryRoot (const juce::File& sfzFile)
{
    juce::File current = sfzFile.getParentDirectory();

    for (int depth = 0; depth < 4 && current.exists() && current != current.getParentDirectory(); ++depth)
    {
        for (const auto& entry : juce::RangedDirectoryIterator (current, false, "*", juce::File::findDirectories))
        {
            auto name = entry.getFile().getFileName();
            if (name.equalsIgnoreCase ("samples") || name.equalsIgnoreCase ("include"))
            {
                return current; // Root-Ordner gefunden!
            }
        }
        current = current.getParentDirectory();
    }

    return sfzFile.getParentDirectory(); // Fallback
}

// case-insensitive path resolver with underscore/space tolerance
static juce::File findPathCaseInsensitive (const juce::File& root, const juce::String& relativePath)
{
    juce::String cleanPath = relativePath.replaceCharacter ('\\', '/').trim();
    if (cleanPath.isEmpty()) return {};

    juce::File direct = root.getChildFile (cleanPath);
    if (direct.exists())
        return direct;

    auto normalizeStr = [] (juce::String s) {
        return s.toLowerCase().replaceCharacter ('_', ' ').trim();
    };

    juce::StringArray components;
    components.addTokens (cleanPath, "/", "");

    juce::File current = root;
    for (const auto& comp : components)
    {
        if (comp.isEmpty() || comp == ".") continue;
        if (comp == "..") { current = current.getParentDirectory(); continue; }

        juce::File exactChild = current.getChildFile (comp);
        if (exactChild.exists())
        {
            current = exactChild;
        }
        else
        {
            bool found = false;
            for (const auto& entry : juce::RangedDirectoryIterator (current, false, "*", juce::File::findFilesAndDirectories))
            {
                if (normalizeStr (entry.getFile().getFileName()) == normalizeStr (comp))
                {
                    current = entry.getFile();
                    found = true;
                    break;
                }
            }
            if (!found) return {};
        }
    }
    return current.exists() ? current : juce::File();
}
#pragma endregion SAMPLER_FINDINGS_HELPER

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : foleys::MagicProcessor(BusesProperties()
#if! JucePlugin_IsMidiEffect
#if! JucePlugin_IsSynth
         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      )
#endif
{
    // ACTIVATION AUTO-SAVE:
    // tells PGM where the source code files are located.
    FOLEYS_SET_SOURCE_PATH(__FILE__);
    // standalone or vst3 - replacement preprocessor for building
    currentPreProcessorReplacement = PreProcessorReplacement::Standalone;

    wavetableDatabase = std::make_shared<SynthLab::WavetableDatabase>();
    midiInputData = std::make_shared<SynthLab::MidiInputData>();

    // for rev. spectrogram synth.:
    audioGridData.assign (static_cast<size_t> (numColumns * numRows), 0.0f);
    analyser = magicState.createAndAddObject<foleys::MagicAnalyser>("analyser");
    // initialize the Synthesizers
    synth.addSound(new SynthSound());
    myVoices.clear();
    synth.clearVoices();

    for (int i = 0; i < maxVoices; ++i)
    {
        auto* newVoice = new SynthVoice();
        newVoice->setWavetableDatabase(midiInputData, wavetableDatabase);
        synth.addVoice(newVoice);
        myVoices.add(newVoice);
    }
    // IMPORTANT: Register WAV and AIFF, otherwise files cannot be read!:
    formatManager.registerBasicFormats();

    // positions measured exactly on 'anzeige.skala.png'(for all logarithm. paramaters):
    const std::vector<std::pair<float, float>> controlPoints = {
        { 0.00f, -60.0f }, // bottom
        { 0.04126f, -40.0f }, // ~41% hight
        { 0.12136f, -30.0f },
        { 0.21845f, -20.0f },
        { 0.3386f, -15.0f },
        { 0.46845f, -10.0f },
        { 0.6007f,  -5.0f },
        { 0.7354f,   0.0f },
        { 0.8738f,   6.0f },
        { 1.00f,  12.0f }  // top
    };// - for the vertical linear master-sliders

#pragma region SamplerParamInit//Sampler Volume Param
    InitSamplerParams(controlPoints);
    for (int i = 0; i < 16; ++i)
        samplerSynth.addVoice (new SamplerVoice (samplerVolumeParam));
#pragma endregion SamplerParamInit

    InitLfoParameters();
    InitJuceWtFilterParameters();

#pragma region HELPER_WINDOW
    bgColorWindow = juce::Colour (0xFF264049);
#pragma endregion HELPER_WINDOW

    if (currentPreProcessorReplacement == PreProcessorReplacement::VST3)
        setExplicitGuiSize (0.85f);

#pragma region SynthLabParams
    InitParamsWToscs(controlPoints);
#pragma endregion SynthLabParams


#pragma region MainParamsInit
    InitParamsMasterSound(controlPoints);
#pragma endregion MainParamsInit
    //= gui/keyboard ==================================================================
    auto pNameGibSmallKeybParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"keyBoard bigSmall "));
    addParameter(switchBigSmall.ptrKeyBoardBigSmallSwitch = new juce::AudioParameterBool
        (juce::ParameterID ("KEYB_BIG_SMALL", 1), pNameGibSmallKeybParam, false));
    //=================================================================================

    //== master modulation ============================================================
    InitMasterModualtionParams();
    //==================================================================================
    //=== effects ======================================================================
    InitEffekteParameters();
    //===== Spectral ===================================================================
    clearCanvasImage();
    InitSpectroParameters(controlPoints);
    //==================================================================================

#pragma region GUI_TRIGGERS
    auto pNameViewIndexParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"View Index "));
    addParameter(viewIndexParam = new juce::AudioParameterChoice(juce::ParameterID ("view_index", 1), pNameViewIndexParam,
                                                                 juce::StringArray{
                                                                     "Wavetables", "Lfos", "Samples",
                                                                     "SpectroSynth", "Modulation", "Effects", "Master" }, 0));
    // not yet used
    auto pNameSwitchTimerParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Switch Timer "));
    addParameter(switchTimerParamBool = new juce::AudioParameterBool(juce::ParameterID ("SWITCHTIMER", 1), pNameSwitchTimerParam, false));

    // register top-buttons
    // page back (<<)
    magicState.addTrigger ("preset_prev", [this] { cyclePreset (-1); });

    // page forward (>>)
    magicState.addTrigger ("preset_next", [this] { cyclePreset (1); });

    // direct load button
    magicState.addTrigger ("preset_load_file", [this] {
        triggerLoadFileDialog();
        updatePresetList(); // Refresh the list if the user has loaded data elsewhere
    });
    // load factory-preset buttons
    magicState.addTrigger ("onFac1", [this] {
        triggerLoadPreset1();
    });

    magicState.addTrigger ("onFac2", [this] {
        triggerLoadPreset2();
    });

    magicState.addTrigger ("onFac3", [this] {
        triggerLoadPreset3();
    });
    //
    // DIRECT SAVE BUTTON
    magicState.addTrigger ("preset_save_file", [this] {
        triggerSaveFileDialog();
        updatePresetList(); // Refresh the list so that the new preset appears in the cycle
    });

    magicState.addTrigger("show_tutorial", [this]
    {
        TriggerTutWindow();
    });

    magicState.addTrigger("show_license", [this]
    {
        TriggerLicenseWindow();
    });

    // Registers the action for the Cancel button in the XML
    magicState.addTrigger ("closeTutorial", [this]
    {
        if (helpTutWindow != nullptr)
        {
            helpTutWindow->setVisible (false);
            helpTutWindow.reset(); // Löscht das Fenster sicher aus dem Speicher
        }
    });

    // Registers the action for the Cancel button in the XML
    magicState.addTrigger ("closeLicense", [this]
    {
        if (helpLicWindow != nullptr)
        {
            helpLicWindow->setVisible (false);
            helpLicWindow.reset(); // Löscht das Fenster sicher aus dem Speicher
        }
    });

    // perform initial preset search when the plugin starts
    updatePresetList();
    //== ende of serialization ================================================

    // trigger for the fake-buttons for the main-tabs
    magicState.addTrigger("Wavetables", [this] {
        if (viewIndexParam) {
            viewIndexParam->operator=(0);
        }
    });

    magicState.addTrigger("Lfos", [this] {
        if (viewIndexParam) {
            viewIndexParam->operator=(1);
        }
    });

    magicState.addTrigger("Samples", [this] {
        if (viewIndexParam) {
            viewIndexParam->operator=(2);
        }
    });

    magicState.addTrigger("SpectroSynth", [this] {
        if (viewIndexParam) {
            viewIndexParam->operator=(3);
        }
    });

    magicState.addTrigger("Modulation", [this] {
        if (viewIndexParam) {
            viewIndexParam->operator=(4);
        }
    });

    magicState.addTrigger("Effects", [this] {
        if (viewIndexParam) {
            viewIndexParam->operator=(5);
        }
    });

    magicState.addTrigger("Master", [this] {
        if (viewIndexParam) {
            viewIndexParam->operator=(6);
        }
    });

    magicState.addTrigger ("tabChanged", [this] {
        const int tab = magicState.getGuiTree().getProperty ("pagesSeite.currentTab");
        if (switchTimerParamBool) {
            const bool value = (tab == 1); // Beispiel: nur bei ADSR-Tab true
            switchTimerParamBool->setValueNotifyingHost (value ? 1.0f : 0.0f);
            switchTimerBreak = value;
        }
    });
    //
    // trigger for the gui-size
    magicState.addTrigger ("size_plus", [this]
    {
        if (currentPreProcessorReplacement == PreProcessorReplacement::Standalone)
            setExplicitGuiSize (1.0f);
        else if (currentPreProcessorReplacement == PreProcessorReplacement::VST3)
            setExplicitGuiSize (1.0f);
    });

    magicState.addTrigger ("size_medium", [this]
    {
        if (currentPreProcessorReplacement == PreProcessorReplacement::Standalone)
            setExplicitGuiSize (0.85f);
        else if (currentPreProcessorReplacement == PreProcessorReplacement::VST3)
            setExplicitGuiSize (0.85f);
    });

    magicState.addTrigger ("size_minus", [this]
    {
        if (currentPreProcessorReplacement == PreProcessorReplacement::Standalone)
            setExplicitGuiSize (0.7f);
        else if (currentPreProcessorReplacement == PreProcessorReplacement::VST3)
            setExplicitGuiSize (0.70f);
    });
    //
#pragma endregion GUI_TRIGGERS

    magicState.updateParameterMap();
    magicState.setPlayheadUpdateFrequency(24);
#ifdef _DEBUG
    // path to the XML file on the hard drive (during development)
    if (juce::File devXmlFile ("/home/tommibe/Development/newAudio/KryptoSyn/Resources/magic.xml"); !devXmlFile.existsAsFile())
        magicState.setGuiValueTree (BinaryData::magic_xml, BinaryData::magic_xmlSize); // Fallback
    else
        magicState.setGuiValueTree (devXmlFile); /// Fallback
#else
    // In the final release product: directly from RAM, without a hard drive check
    magicState.setGuiValueTree (BinaryData::magic_xml, BinaryData::magic_xmlSize);
#endif
    InitializeWTOscTriggers();
    InitializeGainFilterTriggers();
    InitializeSamplerActiveTriggers();
    InitializeSamplerFilterActiveTriggers();
    InitializeModActiveTriggers();
    InitializeEffectsTriggers();
    InitializeAnalysTrigger();
    InitializeKeybBigSmallTriggers();
    InitializeSynthFilterActiveTriggers();
    *bothAnalysersActiveParam = false;
    // =====================================/
#ifdef _DEBUG
    presetFactoryDirectory = juce::File ("/home/tommibe/Development/newAudio/KryptoSyn/Resources/");
#else
    auto pluginBinary = juce::File::getSpecialLocation (juce::File::currentExecutableFile);
    presetFactoryDirectory = pluginBinary.getParentDirectory()
                                         .getChildFile ("Resources");
#endif
    presetDirectory = juce::File::getSpecialLocation (juce::File::userDocumentsDirectory)
                      .getChildFile ("MeinSynthesizer")
                      .getChildFile ("Presets");

    // create the folder and explicitly ignore the result due to the warning.
    auto result = presetDirectory.createDirectory();
    juce::ignoreUnused (result);
    // initialize preset bar and sprctro on/off
    InitializeTutWindowTriggers();
    InitializeLicenseWindowTriggers();
    InitializePresetBarTriggers();
    InitializeSpectroTriggers();
    updatePresetList();
    InitializePlusTrigger();
    InitializeMediumTrigger();
    InitializeMinusTrigger();
    // =============================================
    startTimerHz(24);
}



AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
    if (sampleLoadingThread.joinable())
        sampleLoadingThread.join();
}

//==============================================================================
#pragma region OVERRIDE_METHODS

void AudioPluginAudioProcessor::updateStatusMessage (const juce::String& newMessage)
{
    // accessing the property in the ValueTree
    auto statusValue = magicState.getPropertyAsValue ("ui:statusText");
    // setting the new value immediately updates all bound labels.
    statusValue.setValue (newMessage);
}


void AudioPluginAudioProcessor::postSetStateInformation()
{
}

//==============================================================================
// Preset-Stuff
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // create main root nodes
    juce::ValueTree presetRoot ("PRESET_ROOT");
    // call all module methods one after another.
    saveWtOscParams (presetRoot);
    saveWtFilterParams (presetRoot);
    saveLfoParams (presetRoot);
    SaveMainParams (presetRoot);
    saveSamplerParams (presetRoot);
    saveSpectrogramParams (presetRoot);
    saveEnvModulationParam (presetRoot);
    saveEffectParam (presetRoot); // now fully integrated
    // serialize to XML and write in binary to the DAW
    if (auto xmlState = presetRoot.createXml()) {
        copyXmlToBinary (*xmlState, destData);
    }
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{

    if (auto xmlState = getXmlFromBinary (data, sizeInBytes))
    {
        auto presetRoot = juce::ValueTree::fromXml (*xmlState);
        if (presetRoot.isValid() && presetRoot.hasType ("PRESET_ROOT")) {
            // restore all modules sequentially
            loadWtOscParams (presetRoot);
            loadWtFilterParams (presetRoot);
            loadLfoParams (presetRoot);
            loadMainParams (presetRoot);
            loadSamplerParams (presetRoot);
            loadSpectrogramParams (presetRoot);
            loadEnvModulationParam (presetRoot);
            loadEffectParam (presetRoot); // now fully integrated
        }
    }
    currentPresetNameValue = magicState.getPropertyAsValue ("currentPresetText");
    loadMostRecentPreset();
}

const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================

void AudioPluginAudioProcessor::prepareToPlay (const double sampleRate, const int samplesPerBlock)
{
    if (analyser != nullptr)
        analyser->prepareToPlay (sampleRate, samplesPerBlock);
    // secure initialization of all buffer channels present in the array
    for (size_t channel = 0; channel < arrDelayBuffer.size(); ++channel)
    {
        arrDelayBuffer[channel].Initialize(sampleRate, 2000.0f);
        arrDelayBuffer[channel].Clear();
    }
    const int numChannels = juce::jmax (1, getTotalNumOutputChannels());
    const int maxBlockSize = juce::jmax (samplesPerBlock, 4096);
    // size the scratch buffer to the maximum block size
    samplerScratchBuffer.setSize (numChannels, maxBlockSize, false, false, true);
    specScratchBuffer.setSize (numChannels, maxBlockSize, false, false, true);
    zeroBuffer.setSize (numChannels, maxBlockSize, false, false, true);
    zeroBuffer.clear();

    // pre-allocate 4 wavetable meter buffers
    for (auto& buf : wtMeterSum)
    {
        buf.setSize (numChannels, maxBlockSize, false, false, true);
        buf.clear();
    }

    // Meter setup: channels, sampling rate, peak hold time in ms (e.g., 500 ms)
    if (outputWtMeterOscA!= nullptr)
        outputWtMeterOscA->setupSource (getTotalNumOutputChannels(), sampleRate, 500);
    if (outputWtMeterOscB!= nullptr)
        outputWtMeterOscB->setupSource (getTotalNumOutputChannels(), sampleRate, 500);
    if (outputWtMeterOscC!= nullptr)
        outputWtMeterOscC->setupSource (getTotalNumOutputChannels(), sampleRate, 500);
    if (outputWtMeterOscD!= nullptr)
        outputWtMeterOscD->setupSource (getTotalNumOutputChannels(), sampleRate, 500);
    if (outputSamplerMeter!= nullptr)
        outputSamplerMeter->setupSource (getTotalNumOutputChannels(), sampleRate, 500);
    if (outputSpecMeter!= nullptr)
        outputSpecMeter->setupSource (getTotalNumOutputChannels(), sampleRate, 500);
    // setup of the meter: channels, sampling rate, peak hold time in ms (e.g., 500 ms)
    if (outputMeterMasterMain!= nullptr)
        outputMeterMasterMain->setupSource (getTotalNumOutputChannels(), sampleRate, 500);
    synth.setCurrentPlaybackSampleRate(sampleRate);
    for (int ind = 0; ind < 4; ++ind)
    {
        synthLabOscParams[ind].adsrWtPtr->prepareToPlay(sampleRate, samplesPerBlock);
        synthLabOscParams[ind].adsrWtPtr->timerInjection();
    }
    // prepare each voice individually so that 'isPrepared = true'
    for (int indx = 0; indx < synth.getNumVoices(); ++indx) {
        if (auto* v = dynamic_cast<SynthVoice*>(synth.getVoice(indx))) {
            v->prepare (sampleRate, maxBlockSize, numChannels);
        }
    }
    // prepare master-gain
    juce::dsp::ProcessSpec masterSpec;
    masterSpec.sampleRate       = sampleRate;
    masterSpec.maximumBlockSize = static_cast<uint32_t>(samplesPerBlock);
    masterSpec.numChannels      = static_cast<uint32_t>(getTotalNumOutputChannels());
    // EASILY REUSE for all master components:
    filterGlitch.prepareFilterToPlay (masterSpec); // <-- Just put masterSpec in here!
    filterGlitch.setFilterType (1); // Bandpass
    // Prepare Lfos classes
    lfoMasterFilterModSlow.prepareToPlayOsc(masterSpec);
    lfoMasterFilterMod.prepareToPlayOsc(masterSpec);
    //master-fm/am-modulation:
    lfoFmDataMaster.prepareToPlayOsc(masterSpec);
    lfoAmDataMaster.prepareToPlayOsc(masterSpec);
    // feed all modules with the 'Spec'
    chorusModule.prepare (masterSpec);
    flangerModule.prepare (masterSpec);
    reverbModule.prepare (masterSpec);
    masterGain.prepare(masterSpec);
    masterGain.setRampDurationSeconds(0.005); // 5ms Glättung gegen Knackser
    magicState.prepareToPlay(sampleRate, samplesPerBlock);
    masterEQ.prepare(masterSpec);
    safetyLimiter.prepare (masterSpec);
    // define limiter properties
    // -0.1 dB is the studio standard for a "safety ceiling"
    safetyLimiter.setThreshold (-0.1f);
    // short release time (in milliseconds) so that the limiter
    // lets go again quickly and unnoticed after a peak.
    safetyLimiter.setRelease (10.0f);
    safetyLimiter.reset();
    // inform the sampler of the current environment sample rate
    samplerSynth.setCurrentPlaybackSampleRate (sampleRate);
    // prepare each sampler voice individually, analogous to the WT voices
    for (int indx = 0; indx < samplerSynth.getNumVoices(); ++indx) {
        if (auto* v = dynamic_cast<SamplerVoice*> (samplerSynth.getVoice (indx))) {
            v->prepareToPlay (sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }
    //gain/filter-switches:
    (*switchAdrsParams[0].switchGainFilterParam) = true;
    (*switchAdrsParams[1].switchGainFilterParam) = true;
    (*switchAdrsParams[2].switchGainFilterParam) = true;
    (*switchAdrsParams[3].switchGainFilterParam) = true;
    *bothAnalysersActiveParam = false;

    // prepare and flush a zero-filled buffer of maximum block size:
    zeroBuffer.setSize (getTotalNumOutputChannels(), samplesPerBlock);
    zeroBuffer.clear();

    wasAnalyserActive = false;
    silenceBlocksToPush = 0;
    // absolutely reliable reset for the FFT analyzer buffer
    fftAccumulatorIndex = 0;
    fftAccumulatorBuffer.fill (0.0f);
    // set the slow LFO (which replaces the ADSR) to a slow modulation frequency
    lfoMasterFilterModSlow.setAMFrequency (0.75f);
    filterGlitch.reset();
    lfoMasterFilterModSlow.reset();
    lfoMasterFilterMod.reset();
    chorusModule.reset();
    flangerModule.reset();
    // delayModule.reset(); -> for delay we use an other solution
    reverbModule.reset();
    // Pre-allocate memory (channels, samples)
    samplerScratchBuffer.setSize (getTotalNumOutputChannels(), samplesPerBlock);
    specScratchBuffer.setSize (getTotalNumOutputChannels(), samplesPerBlock);
    // for glitch-mode (special modulation)
    // set the initial value to the current base cutoff (prevents power-on sweep)
    smoothedCutoff = *masterFilterModulat.filterEnvMasterModulParam;
    // calculate alpha based on the sample rate (e.g., for a smoothing time of approx. 1 ms)
    // formel: alpha = 1.0 - exp(-2 * pi * f_cutoff / sampleRate)
    // for smoothing micro-glitches with a sharp cutoff at ~500 Hz:
    constexpr float smoothingCutoffHz = 300.0f;
    cutoffAlpha = 1.0f - std::exp(-2.0f * juce::MathConstants<float>::pi * smoothingCutoffHz / static_cast<float>(sampleRate));
}


void AudioPluginAudioProcessor::releaseResources()
{
    // when playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // this checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif
    return true;
  #endif
}
#endif
#pragma endregion OVERRIDE_METHODS

#pragma region PROCESS_BLOCK
void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    buffer.clear();
    //==============================================================================//
    //=== begin reverse spectrogram synthesis get data ======================//
    processIncomingPixelUpdates();
    //=== end reverse spectrogram synthesis ========================================//
    //==============================================================================/
    struct LocalParams {
        int coreP, waveP;
        float attack, decay, sustain, release;
        float pan, vol, volDb, tune, reserve, oscilShape;
        float mods[4];
        bool filterIsActive;
    };
    std::array<LocalParams, 4> locSnapShot;

    int currentLfoFmWaveForm[4];
    int currentLfoAmWaveForm[4]; int currentFilterType[4];
    for (int inx = 0; inx < 4; inx++){
        currentLfoFmWaveForm[inx] = juceLfoFmParams[inx].lfoFmWaveFormParam->getIndex();
        currentLfoAmWaveForm[inx] = juceLfoAmParams[inx].lfoAmWaveFormParam->getIndex();
        currentFilterType[inx] = synthLabOscParams[inx].filterTypeParam->getIndex();
    }

    for (int inx = 0; inx < 4; inx++) {
        const float _freqLfoFm = (*juceLfoFmParams[inx].lfoFmFreqParam);
        const float _depthLfoFm = (*juceLfoFmParams[inx].lfoFmDepthParam);
        const float _freqLfoAm = (*juceLfoAmParams[inx].lfoAmFreqParam);
        const float _depthLfoAm = (*juceLfoAmParams[inx].lfoAmDepthParam);
        const int _currentLfoFmWaveFormIndex = currentLfoFmWaveForm[inx];
        if (_currentLfoFmWaveFormIndex != lastLfoFmWaveForm[inx])
        {
            for (auto* voice : myVoices)
                voice->getFmLfo(inx).setLfoWave(currentLfoFmWaveForm[inx]);
        }
        lastLfoFmWaveForm[inx] = currentLfoFmWaveForm[inx];
        for (auto* voice : myVoices) {
            voice->getFmLfo(inx).setFMParams(_freqLfoFm, _depthLfoFm);

        }
        const int _currentLfoAmWaveFormIndex = currentLfoAmWaveForm[inx];
        if (_currentLfoAmWaveFormIndex != lastLfoAmWaveForm[inx]) {
            for (auto* voice : myVoices)
                voice->getAmLfo(inx).setLfoWave(currentLfoAmWaveForm[inx]);
        }
        lastLfoAmWaveForm[inx] = _currentLfoAmWaveFormIndex;
        for (auto* voice : myVoices){
            voice->getAmLfo(inx).setAMParams(_freqLfoAm, _depthLfoAm);
        }
    }
    for (int inx = 0; inx < 4; inx++) {
        const float _filterFreq = (*synthLabOscParams[inx].filterFreq);
        const float _filterReso = (*synthLabOscParams[inx].filterReso);
        const float _envFilter = (*synthLabOscParams[inx].filterEnvelopeAmount);
        const float _filtAttack = (*synthLabOscParams[inx].attackFilterParam);
        const float _filtDecay = (*synthLabOscParams[inx].decayFilterParam);
        const float _filtSustain = (*synthLabOscParams[inx].sustainFilterParam);
        const float _filtRelease = (*synthLabOscParams[inx].releaseFilterParam);
        const int _currentFilterType = currentFilterType[inx];
        if (_currentFilterType != lastFilterType[inx]) {
            for (auto* voice : myVoices)
                voice->setFilterType(inx, _currentFilterType);
        }
        lastFilterType[inx] = _currentFilterType;
        for (auto* voice : myVoices) {
            //if (voice->isVoiceActive()) // not practicable
            //{
                voice->updateFilterAdsr(_filtAttack, _filtDecay, _filtSustain, _filtRelease, inx);
                voice->updateFilterParams(_filterFreq, _filterReso, _envFilter,inx);
            //}
        }
    }

    for (int i = 0; i < 4; ++i) {
        locSnapShot[i].attack = (*synthLabOscParams[i].attackGwT);
        locSnapShot[i].decay = (*synthLabOscParams[i].decayGwT);
        locSnapShot[i].sustain = (*synthLabOscParams[i].sustainGwT);
        locSnapShot[i].release = (*synthLabOscParams[i].releaseGwT);
        locSnapShot[i].waveP = synthLabOscParams[i].waveParam->getIndex();
        locSnapShot[i].coreP = synthLabOscParams[i].coreParam->getIndex();
        locSnapShot[i].pan  = (*synthLabOscParams[i].panWTParams);
        locSnapShot[i].vol = (*synthLabOscParams[i].volumeWTParams);//* 0.25f;
        locSnapShot[i].tune = (*synthLabOscParams[i].tuneWTParams);
        locSnapShot[i].reserve = (*synthLabOscParams[i].reserveWTParams);
        locSnapShot[i].filterIsActive = synthLabOscParams[i].filterIsActivated->get();
        for (int m = 0; m < 4; ++m)
            locSnapShot[i].mods[m] = (*synthLabOscParams[i].modKnobs[m]);
        locSnapShot[i].oscilShape = (*synthLabOscParams[i].oscillatorShape);
    }

    for (int ind = 0; ind < 4; ind++) {

        const bool _currentIsActive = synthLabOscParams[ind].isActivated->get();
        for (auto* voiceWt : myVoices) {

            if (auto osc = voiceWt->GetWtOscillator(ind))
            {
                if (auto p = osc->getParameters())
                {
                    voiceWt->updateGainAdsrWt(locSnapShot[ind].attack, locSnapShot[ind].decay,
                            locSnapShot[ind].sustain, locSnapShot[ind].release, ind);
                    voiceWt->setWtActive(ind,  _currentIsActive); // activated state
                    voiceWt->setFilterEnabled(ind, locSnapShot[ind].filterIsActive);
                    p->waveIndex = locSnapShot[ind].waveP;
                    p->moduleIndex = locSnapShot[ind].coreP;
                    p->panValue = locSnapShot[ind].pan;
                    p->outputAmplitude_dB = locSnapShot[ind].vol;
                    p->fineDetune = locSnapShot[ind].tune;
                    voiceWt->setBaseTune(locSnapShot[ind].tune, ind);
                    p->coarseDetune = locSnapShot[ind].reserve;
                    for (int m = 0; m < 4; ++m)
                        p->modKnobValue[m] = locSnapShot[ind].mods[m];
                    p->oscillatorShape = locSnapShot[ind].oscilShape;
                    voiceWt->updateVelocityDb(locSnapShot[ind].vol, ind);
                    voiceWt->GetWtOscillator(ind)->update();
                }
            }
        }
    }
    // this state is automatically linked to the keyboard widget in the UI
    magicState.getKeyboardState().processNextMidiBuffer (midiMessages, 0, buffer.getNumSamples(), true);
    // create an exact copy for the sampler BEFORE the wavetables touch the buffer
    juce::MidiBuffer samplerMidiMessages = midiMessages;
    // render the wavetables (consumes midiMessages)
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    // ==============================================================================
    // === Transfer sampler ADSR parameters to voices =============================
    // ==============================================================================
    bool samplerIsActive = samplerGainParams.isActivated->get();
    const float samplerAtt  = (*samplerGainParams.attackSample);
    const float samplerDec  = *(samplerGainParams.decaySample);
    const float samplerSus  = (*samplerGainParams.sustainSample);
    const float samplerRel  = (*samplerGainParams.releaseSample);

    int currentSampleFilterType = samplerFilterParams.filterSampleTypeParam->getIndex();
    const float sampleFilterFreq = (*samplerFilterParams.sampleFilterFreq);
    const float sampleFilterReso = (*samplerFilterParams.sampleFilterReso);
    const float samleFilterEnv = (*samplerFilterParams.sampleFilterEnvAmount);
    const float samplerFiltAtt  = (*samplerFilterParams.attackFilterSample);
    const float samplerFiltDec  = (*samplerFilterParams.decayFilterSample);
    const float samplerFiltSus  = (*samplerFilterParams.sustainFilterSample);
    const float samplerFiltRel  = (*samplerFilterParams.releaseFilterSample);
    const bool sampleFiltActive = samplerFilterParams.isActivated->get();
    if (currentSampleFilterType != lastSamplerFilterType) {for (int i = 0; i < samplerSynth.getNumVoices(); ++i) {
        if (auto* voiceSam = dynamic_cast<SamplerVoice*> (samplerSynth.getVoice(i))) {
            voiceSam->setFilterType(currentSampleFilterType);
        }}
    }
    lastSamplerFilterType = currentSampleFilterType;
    // we loop through the voices of the samplerSynth (not myVoices!)
    for (int i = 0; i < samplerSynth.getNumVoices(); ++i) {
        if (auto* voiceSam = dynamic_cast<SamplerVoice*> (samplerSynth.getVoice(i))) {
            voiceSam->updateGainAdsr (samplerAtt, samplerDec, samplerSus, samplerRel);
            voiceSam->updateFilterAdsr (samplerFiltAtt, samplerFiltDec, samplerFiltSus, samplerFiltRel);
            voiceSam->updateFilterParams(sampleFilterFreq, sampleFilterReso, samleFilterEnv);
            voiceSam->setFilterEnabled(sampleFiltActive);
            if (!samplerIsActive) {
                (*samplerVolumeParam) = -60.0f;
                voiceSam->updateVolumenParam(-60.0);
            }
        }
    }
    // ==============================================================================//
    // === begin polyphonic Sampler Rendering =======================================//
    samplerScratchBuffer.clear();
    // the sampler reads the same MIDI data and adds its audio data to the buffer
    // the sampler renders in isolation into our scratch buffer
    samplerSynth.renderNextBlock (samplerScratchBuffer, samplerMidiMessages, 0, buffer.getNumSamples());
    //= ==================================================================================

    juce::MidiBuffer spectralMidiMessages = midiMessages;
    // now send the isolated sampler signal to the Meter
    if (outputSamplerMeter != nullptr)
        outputSamplerMeter->pushSamples (samplerScratchBuffer);
    // add the isolated signal to the main buffer
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
        buffer.addFrom (ch, 0, samplerScratchBuffer, ch, 0, buffer.getNumSamples());
    }
    // === End Polyphonic Sampler Rendering =========================================//
    //==============================================================================//

    //=== Begin reverse spectrogram synthesis render daten ========================//
    // analyze MIDI for the spectrogram:
    processMidiForSpectrogram (spectralMidiMessages);
    specScratchBuffer.clear();
    // the spectrogram renders in isolation into its own buffer
    renderSpectrogramToBuffer(specScratchBuffer);

    // now send the isolated spectrogram signal to the meter
    if (outputSpecMeter != nullptr)
        outputSpecMeter->pushSamples (specScratchBuffer);
    // Add the spectrogram to the main buffer
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
        buffer.addFrom (ch, 0, specScratchBuffer, ch, 0, buffer.getNumSamples());
    }
    //=== End Reverse Spectrogram Synthesis ========================================//
    //==============================================================================/

    // ==============================================================================
    // === MASTER AM & FM MODULATION ================================================
    // ==============================================================================
    // update master LFO parameters
    const int currentMasterFmWave = lfoFmWaveFormParamMaster->getIndex();
    lfoFmDataMaster.setLfoWave(currentMasterFmWave);
    lfoFmDataMaster.setFMParams(fmFreqMaster->get(), fmDepthMaster->get());

    const int currentMasterAmWave = lfoAmWaveFormParamMaster->getIndex();
    lfoAmDataMaster.setLfoWave(currentMasterAmWave);
    lfoAmDataMaster.setAMParams(amFreqMaster->get(), amDepthMaster->get());

    // ------------------------------------------------------------------------------
    // master-FM (Path A): Pass the signal through to the voices
    // ------------------------------------------------------------------------------
    const float masterFmDepthVal = fmDepthMaster->get();
    if (masterFmDepthVal > 0.0f)
    {
        // We retrieve the current LFO value (granularity per block is sufficient for LFO < 20Hz,
        // (with audio-rate FM up to 1000 Hz, this can also take place in the voice's sub-block)
        const float masterFmSignal = lfoFmDataMaster.getNextFMSample();
        const float masterFmCents  = masterFmSignal * masterFmDepthVal;

        for (auto* voice : myVoices)
        {
            // transmits the master FM offset to the voice
            voice->setMasterFmCents(masterFmCents);
        }
    }
    else
    {
        for (auto* voice : myVoices)
            voice->setMasterFmCents(0.0f);
    }
    // ------------------------------------------------------------------------------
    // master-AM: Direct multiplexing to the summation buffer
    // ------------------------------------------------------------------------------
    const float masterAmDepthVal = amDepthMaster->get(); // 0..100
    if (masterAmDepthVal > 0.0f)
    {
        const int numSamples  = buffer.getNumSamples();
        const int numChannels = buffer.getNumChannels();

        for (int s = 0; s < numSamples; ++s)
        {
            const float amSignal = lfoAmDataMaster.getNextAMSample(); // [-1.0, 1.0]

            // amDepth=0 -> 1.0 (no change), amDepth=100 -> Amplitude 0.0 .. 2.0
            const float amMod = juce::jlimit(0.0f, 2.0f, 1.0f + amSignal * (masterAmDepthVal / 100.0f));

            for (int ch = 0; ch < numChannels; ++ch)
            {
                buffer.setSample(ch, s, buffer.getSample(ch, s) * amMod);
            }
        }
    }



    // ==============================================================================
    // === EXPERIMENTAL GLITCH MODE =================================================
    // ==============================================================================
    if (glitchActiveParam != nullptr && glitchActiveParam->get())
    {
        int currentModFilterType = masterFilterModulat.filterTypeParam->getIndex();

        const float baseCutoff     = *masterFilterModulat.filterEnvMasterModulParam;
        const float resonance      = *masterFilterModulat.filterResoMasterModulParam;
        const float depthInOctaves = *lfoMastModParam.lfoMasterDepthParam;

        float dryWetMix = masterFilterModulat.filterDryWetMmp->get();
        if (dryWetMix > 1.0f) dryWetMix /= 100.0f;

        // calculate safe resonance (limiting for bandpass to prevent resonance-induced popping)
        float safeResonance = juce::jlimit(0.1f, 3.0f, resonance);
        /*if (currentModFilterType == 1) // 1 = Bandpass
        {
            safeResonance = juce::jmin(safeResonance, 0.2f);
            (*masterFilterModulat.filterResoMasterModulParam) = 0.2f;
        }*/
        filterGlitch.updateFilterResonance(safeResonance);

        const int numSamples  = buffer.getNumSamples();
        const int numChannels = buffer.getNumChannels();

        // maximum safe cutoff frequency (staying below Nyquist)
        const float maxSafeCutoff = static_cast<float>(getSampleRate()) * 0.45f;
        // sample-Schleife für modulierte Audioraten-Verarbeitung
        for (int s = 0; s < numSamples; ++s) {
            // slow LFO (scaled to 0.0 to 1.0)
            lfoMasterFilterModSlow.setAMFrequency(*masterFilterModulat.envelopeFrequencyP);
            float slowLfoNormalized = (lfoMasterFilterModSlow.getNextAMSample() + 1.0f) * 0.5f;
            // dynamically control the frequency of the fast LFO (0.2 Hz to 18.0 Hz)
            float dynamicFastFreq = 0.2f + (slowLfoNormalized * (*masterFilterModulat.wobbleUpperLimitP));
            lfoMasterFilterMod.setAMFrequency(dynamicFastFreq);
            // query fast LFO & switch filter type if necessary
            float fastGlitchMod = lfoMasterFilterMod.getNextAMSample();
            if (currentModFilterType != lastModFilterType) {
                filterGlitch.setFilterType(currentModFilterType);
            }
            lastModFilterType = currentModFilterType;
            // calculate the exponential target frequency and limit it within the safe frequency band
            float targetCutoff = baseCutoff * std::pow(2.0f, fastGlitchMod * depthInOctaves);
            targetCutoff = juce::jlimit(20.0f, maxSafeCutoff, targetCutoff);
            // single-pole smoothing per sample to prevent micro-glitches/clicking.
            smoothedCutoff += cutoffAlpha * (targetCutoff - smoothedCutoff);
            // pass the smoothed frequency to the filter
            filterGlitch.updateFilterFrequency(smoothedCutoff);
            // apply filter to all audio channels and perform the mix.
            for (int ch = 0; ch < numChannels; ++ch)
            {
                float inputSample = buffer.getSample(ch, s);
                float filteredSample = filterGlitch.processSample(ch, inputSample);

                float finalSample = (inputSample * (1.0f - dryWetMix)) + (filteredSample * dryWetMix);
                buffer.setSample(ch, s, finalSample);
            }
        }
    }
    else {
        // Reset filter state when glitch mode is inactive to avoid popping sounds upon power-up.
        filterGlitch.reset();
    }

    //===============================================================================

    //== Effects ===========================================================
    preProcessEffects(totalNumInputChannels, totalNumOutputChannels, buffer);
    //======================================================================
    // Read out parameter in unnormalized form (returns -12.0 to +12.0)
    float bass = *masterEqBass;
    float mid  = *masterEqMid;
    float highs = *masterEqHigh;

    // update filter coefficients and process buffer directly
    masterEQ.updateEQParameters(bass, mid, highs);
    masterEQ.process(buffer);
    // now apply master gain:
    const float masterDb  = masterVolumeMainParam->get();
    float masterLin = juce::Decibels::decibelsToGain(masterDb, -60.0f);
    masterLin = masterLin / 60.0f;
    masterGain.setGainLinear(masterLin);

    const juce::dsp::AudioBlock<float> masterBlock(buffer);
    // process only the relevant area:
    auto masterSubBlock = masterBlock.getSubBlock(
        static_cast<size_t>(0),
        static_cast<size_t>(buffer.getNumSamples()));

    masterGain.process(juce::dsp::ProcessContextReplacing<float>(masterSubBlock));
    // ==============================================================================
    // === SAFETY BRICKWALL LIMITER =================================================
    // ==============================================================================
    // we run the fully calculated sub-block through the limiter.
    // anything exceeding -0.1 dB is no caught—mercilessly, yet smoothly..
    safetyLimiter.process (juce::dsp::ProcessContextReplacing<float> (masterSubBlock));

    const bool analys1IsActive = bothAnalysersActiveParam->get();
    // detect status change (edge):
    if (analys1IsActive != wasAnalyserActive) {
        wasAnalyserActive = analys1IsActive;
        if (!analys1IsActive) {
            outputFftFifo.pushClearCommand();
            fftAccumulatorIndex = 0;
        }
    }
    if (analys1IsActive) {
        processOutputSpectrogram(buffer); //Spectro-analyzer
    }

    // ==============================================================================
    //= begin meter ================================================================
    // for the Meter:
    // ── Individual meters: sum all voices ─────────────────────────────────
    const int numSamples  = buffer.getNumSamples();
    const int numChannels = getTotalNumOutputChannels();

    // empty all 4 buffers at lightning speed (no heap allocation)
    for (auto& buf : wtMeterSum)
        buf.clear();

    for (const auto* voice : myVoices) {
        if (!voice->isVoiceActive()) continue;
        for (int i = 0; i < 4; ++i) {
            const auto& src = voice->getWtMeterBuffer(i);
            if (src.getNumChannels() >= numChannels && wtMeterSum[i].getNumSamples() >= numSamples) {
                for (int ch = 0; ch < numChannels; ++ch)
                    wtMeterSum[i].addFrom(ch, 0, src, ch, 0, numSamples);
            }
        }
    }
    // create sub-buffers (views) without heap allocation and send them to the GUI meters
    if (synthLabOscParams[0].isActivated->get() && outputWtMeterOscA != nullptr) {
        juce::AudioBuffer<float> view (wtMeterSum[0].getArrayOfWritePointers(), numChannels, numSamples);
        outputWtMeterOscA->pushSamples (view);
    }

    if (synthLabOscParams[1].isActivated->get() && outputWtMeterOscB != nullptr) {
        juce::AudioBuffer<float> view (wtMeterSum[1].getArrayOfWritePointers(), numChannels, numSamples);
        outputWtMeterOscB->pushSamples (view);
    }

    if (synthLabOscParams[2].isActivated->get() && outputWtMeterOscC != nullptr) {
        juce::AudioBuffer<float> view (wtMeterSum[2].getArrayOfWritePointers(), numChannels, numSamples);
        outputWtMeterOscC->pushSamples (view);
    }

    if (synthLabOscParams[3].isActivated->get() && outputWtMeterOscD != nullptr) {
        juce::AudioBuffer<float> view (wtMeterSum[3].getArrayOfWritePointers(), numChannels, numSamples);
        outputWtMeterOscD->pushSamples (view);
    }
    //= end of meters ====================================================================
    // ── Master meter: directly from the finished buffer ──────────────────────────
    outputMeterMasterMain->pushSamples(buffer);
    //= end of meters ====================================================================
    if (analyser != nullptr)
    {
        const bool analys2IsActive = bothAnalysersActiveParam->get();
        // detect status change (edge):
        if (analys2IsActive != wasAnalyserActive)
        {
            wasAnalyserActive = analys2IsActive;
            analyser->setActive (analys2IsActive);
        }
        if (analys2IsActive)
        {
            // CHECK 1: Is there a signal at all? (Silence Gate)
            // get the maximum magnitude across all channels from sample 0 to the end.
            auto currentMagnitude = buffer.getMagnitude (0, buffer.getNumSamples());
            if (currentMagnitude > 0.0001f) // Entspricht ca. -80 dB (Noise Floor)
            {
                // CHECK 2: Visual pacing (throttling)
                // we sum up the samples.
                samplesSinceLastUpdate += buffer.getNumSamples();
                // only when enough samples have been collected (approximately every 20-30 ms),
                // we send the data to the FFT calculation
                if (samplesSinceLastUpdate >= 1024) {
                    analyser->pushSamples (buffer);
                    samplesSinceLastUpdate = 0; // Zähler zurücksetzen
                }
            } else {
                // during silence, we send absolutely nothing to the analyzer.
                // this completely relieves the CPU
                samplesSinceLastUpdate = 0;
            }
        } else {
            if (zeroBuffer.getNumSamples() != buffer.getNumSamples())
                zeroBuffer.setSize (buffer.getNumChannels(), buffer.getNumSamples(), false, false, true);
            analyser->pushSamples (zeroBuffer);
        }

    }

}
#pragma endregion PROCESS_BLOCK

#pragma region HELPER_METHODS
juce::File AudioPluginAudioProcessor::getSampleBaseDir() const
{
#ifdef _DEBUG
    return juce::File ("/home/tommibe/Development/newAudio/KryptoSyn/SynthLabSamples/");
#else
    auto dir = juce::File::getSpecialLocation (juce::File::currentExecutableFile)
                    .getParentDirectory()   // Standalone
                    .getParentDirectory()   // Release
                    .getChildFile ("SynthLabSamples");
    if (! dir.exists())
        dir = juce::File ("/home/tommibe/Development/newAudio/KryptoSyn/SynthLabSamples/");
    return dir;
#endif
}


float AudioPluginAudioProcessor::convertFrom0To1(std::vector<std::pair<float, float>> controlPointsParam)
{
    return -60.0f;
}

float AudioPluginAudioProcessor::convertTo0To1(std::vector<std::pair<float, float>> controlPointsParam)
{
    return 0.0f;
}

// for spectro preset
void AudioPluginAudioProcessor::updateAudioAndUIFromLoadedImage (const juce::Image& loadedImage)
{
    // save local picture
    canvasImage = loadedImage;
    // move pixel to FIFO for the audio-thread
    pushImageToPixelFIFO (canvasImage);
    // Notify all registered GUI components
    sendChangeMessage();
}
// for spectro preset
void AudioPluginAudioProcessor::pushImageToPixelFIFO (const juce::Image& img)
{
    if (! img.isValid())
        return;
    // discard pending old drawing commands
    pixelFifo.pushClearCommand();
    // iterate through all grid points of the spectrogram
    for (int x = 0; x < numColumns; ++x) {
        for (int y = 0; y < numRows; ++y) {
            // y-inversion analogous to the SpectrogramCanvas::setPixelValue:
            // in juce::Image, Y=0 is at the top (graphics coordinates)
            // in the frequency grid, Y=0 is at the bottom (bass/low frequencies)
            int imageY = (numRows - 1) - y;

            juce::Colour color = img.getPixelAt (x, imageY);
            float brightness = color.getBrightness(); // returns a float (0.0f to 1.0f)

            pixelFifo.push (x, y, brightness);
        }
    }
}



void AudioPluginAudioProcessor::processMidiForSpectrogram (const juce::MidiBuffer& midiMessages)
{
    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();

        if (msg.isNoteOn())
        {
            // with EVERY new keystroke we reset the playhead,
            // so that the spectrogram immediately starts from the beginning (column 0)!
            currentSamplePosition = 0.0;
            hopCounter = 0;
            std::fill (overlapBuffer.begin(), overlapBuffer.end(), 0.0f);
            spectroActiveNoteCount++;
        }
        else if (msg.isNoteOff())
        {
            spectroActiveNoteCount = std::max (0, spectroActiveNoteCount - 1);
        }
        else if (msg.isAllNotesOff() || msg.isAllSoundOff())
        {
            spectroActiveNoteCount = 0;
        }
    }
}

void AudioPluginAudioProcessor::timerCallback()
{// we need this to diplay ADSR-PLots
    {
        magicState.updatePlayheadInformation (getPlayHead());

        switch (_counterOscWt1) {
        case 0: {
                updateAdsrPlot(0);
                //synthMainParams.synthLabAdsrPtr->pushSamples(dspTimerDummyBuffer);
                //synthMainParams.synthLabAdsrPtr->timerInjection();
                samplerFilterParams.samplerAdsrFilterPtr->pushSamples(dspTimerDummyBuffer);
                samplerFilterParams.samplerAdsrFilterPtr->timerInjection();
                _counterOscWt1++;
                break; }
        case 1: {
                updateAdsrPlot(1);
                samplerGainParams.samplerAdsrGainPtr->pushSamples(dspTimerDummyBuffer);
                samplerGainParams.samplerAdsrGainPtr->timerInjection();
                _counterOscWt1++;
                break; }
        case 2: {
                updateAdsrPlot(2);
                //synthMainParams.synthLabAdsrPtr->pushSamples(dspTimerDummyBuffer);
                //synthMainParams.synthLabAdsrPtr->timerInjection();
                samplerFilterParams.samplerAdsrFilterPtr->pushSamples(dspTimerDummyBuffer);
                samplerFilterParams.samplerAdsrFilterPtr->timerInjection();
                _counterOscWt1++;
                break; }
        case 3: {
                updateAdsrPlot(3);
                samplerGainParams.samplerAdsrGainPtr->pushSamples(dspTimerDummyBuffer);
                samplerGainParams.samplerAdsrGainPtr->timerInjection();
                _counterOscWt1++;
                break; }
        case 4: {
                updateAdsrPlot(0);
                //synthMainParams.synthLabAdsrPtr->pushSamples(dspTimerDummyBuffer);
                //synthMainParams.synthLabAdsrPtr->timerInjection();
                samplerFilterParams.samplerAdsrFilterPtr->pushSamples(dspTimerDummyBuffer);
                samplerFilterParams.samplerAdsrFilterPtr->timerInjection();
                _counterOscWt1++;
                break; }
        case 5: {
                updateAdsrPlot(1);
                samplerFilterParams.samplerAdsrFilterPtr->pushSamples(dspTimerDummyBuffer);
                samplerFilterParams.samplerAdsrFilterPtr->timerInjection();
                _counterOscWt1++;
                break; }
        case 6: {
                updateAdsrPlot(2);
                //synthMainParams.synthLabAdsrPtr->pushSamples(dspTimerDummyBuffer);
                //synthMainParams.synthLabAdsrPtr->timerInjection();
                samplerFilterParams.samplerAdsrFilterPtr->pushSamples(dspTimerDummyBuffer);
                samplerFilterParams.samplerAdsrFilterPtr->timerInjection();
                _counterOscWt1++;
                break; }
        case 7: {
                updateAdsrPlot(3);
                samplerGainParams.samplerAdsrGainPtr->pushSamples(dspTimerDummyBuffer);
                samplerGainParams.samplerAdsrGainPtr->timerInjection();
                _counterOscWt1 = 0;
                break; }
        default:
            _counterOscWt1 = 0;
        }
        switch (_counterOscWt2) {
        case 0: {
                updateAdsrPlot(0);
                //synthMainParams.synthLabAdsrPtr->pushSamples(dspTimerDummyBuffer);
                //synthMainParams.synthLabAdsrPtr->timerInjection();
                samplerFilterParams.samplerAdsrFilterPtr->pushSamples(dspTimerDummyBuffer);
                samplerFilterParams.samplerAdsrFilterPtr->timerInjection();
                _counterOscWt2++;
                break; }
        case 1: {
                updateAdsrPlot(1);
                samplerGainParams.samplerAdsrGainPtr->pushSamples(dspTimerDummyBuffer);
                samplerGainParams.samplerAdsrGainPtr->timerInjection();
                _counterOscWt2++;
                break; }
        case 2: {
                updateAdsrPlot(2);
                //synthMainParams.synthLabAdsrPtr->pushSamples(dspTimerDummyBuffer);
                //synthMainParams.synthLabAdsrPtr->timerInjection();
                samplerFilterParams.samplerAdsrFilterPtr->pushSamples(dspTimerDummyBuffer);
                samplerFilterParams.samplerAdsrFilterPtr->timerInjection();
                _counterOscWt2++;
                break; }
        case 3: {
                updateAdsrPlot(3);
                samplerGainParams.samplerAdsrGainPtr->pushSamples(dspTimerDummyBuffer);
                samplerGainParams.samplerAdsrGainPtr->timerInjection();
                _counterOscWt2++;
                break; }
        case 4: {
                updateAdsrPlot(0);
                //synthMainParams.synthLabAdsrPtr->pushSamples(dspTimerDummyBuffer);
                //synthMainParams.synthLabAdsrPtr->timerInjection();
                samplerFilterParams.samplerAdsrFilterPtr->pushSamples(dspTimerDummyBuffer);
                samplerFilterParams.samplerAdsrFilterPtr->timerInjection();
                _counterOscWt2++;
                break; }
        case 5: {
                updateAdsrPlot(1);
                samplerGainParams.samplerAdsrGainPtr->pushSamples(dspTimerDummyBuffer);
                samplerGainParams.samplerAdsrGainPtr->timerInjection();
                _counterOscWt2++;
                break; }
        case 6:
                {
                updateAdsrPlot(2);
                //synthMainParams.synthLabAdsrPtr->pushSamples(dspTimerDummyBuffer);
                //synthMainParams.synthLabAdsrPtr->timerInjection();
                samplerFilterParams.samplerAdsrFilterPtr->pushSamples(dspTimerDummyBuffer);
                samplerFilterParams.samplerAdsrFilterPtr->timerInjection();
                _counterOscWt2++;
                break;}
        case 7:
            {
                updateAdsrPlot(3);
                _counterOscWt2 = 0;
                break;}
        default:
            _counterOscWt1 = 0;
        }
    }
}

// initial state. set some of the parameters randomly
void AudioPluginAudioProcessor::updateAdsrPlot(int inx)
{
    auto* plot = synthLabOscParams[inx].adsrWtPtr;
    if (plot == nullptr) return;

    const bool showGain = switchAdrsParams[inx].switchGainFilterParam->get();
    if (showGain) {
        plot->setParameters(
            synthLabOscParams[inx].attackGwT,
            synthLabOscParams[inx].decayGwT,
            synthLabOscParams[inx].sustainGwT,
            synthLabOscParams[inx].releaseGwT
        );
    } else {
        plot->setParameters(
            synthLabOscParams[inx].attackFilterParam,
            synthLabOscParams[inx].decayFilterParam,
            synthLabOscParams[inx].sustainFilterParam,
            synthLabOscParams[inx].releaseFilterParam
        );
    }
    synthLabOscParams[inx].adsrWtPtr->pushSamples(dspTimerDummyBuffer);
    synthLabOscParams[inx].adsrWtPtr->timerInjection();
}

void AudioPluginAudioProcessor::preProcessEffects (const int channelNumbers, const int totalNumOutputCh, juce::AudioBuffer<float>& bufferParam)
{
    // basic context for the in-place modules (Chorus, Flanger, Reverb)
    juce::dsp::AudioBlock<float> mainBlock (bufferParam);
    juce::dsp::ProcessContextReplacing<float> mainContext (mainBlock);
    const float dryWetMixReverb = reverbDryWetParam->get();
    // ==== Effects =================================================================
    // CHORUS
    if (effekteActiveParams[0].effectIsActive->get()) {
        chorusModule.setRate (*chorusRateParam);
        chorusModule.setDepth (*chorusDepthParam);
        chorusModule.setCentreDelay(30);
        chorusModule.setFeedback(0.1);
        const float chorusMix = (*chorusDryWetParam) / 100.0f;
        chorusModule.setMix (chorusMix);
        // compensation: The levels sum up most strongly in the middle (at a mix of 0.5)
        // we lower the level slightly the closer we get to the 50/50 mix
        const float chorusGainComp = 1.0f - (chorusMix * (1.0f - chorusMix) * 0.4f);
        bufferParam.applyGain (chorusGainComp);
        chorusModule.process (mainContext);
    }
    if (effekteActiveParams[1].effectIsActive->get()) {
        flangerModule.setRate (*flangerRateParam);
        flangerModule.setDepth (*flangerDepthParam);
        flangerModule.setCentreDelay (2.0f);
        flangerModule.setFeedback (0.7f);
        const float flangerMix = (*flangerDryWetParam) / 100.0f;
        flangerModule.setMix (flangerMix);
        const float flangerGainComp = 1.0f - (flangerMix * (1.0f - flangerMix) * 0.4f);
        bufferParam.applyGain (flangerGainComp);
        flangerModule.process (mainContext);
    }

    // ==== DELAY PARAMETER EINLESEN ====
    // by dereferencing (*), we retrieve the actual values from the JUCE parameters
    float delayTimeMs = *delayTimeMsParam;
    float feedback    = *delayFeedback;
    float rawMix      = *delayMix;       // Returns 0.0f to 1.0f directly (no /100 needed!)
    float masterGain  = *delayMaster;

    if (effekteActiveParams[2].effectIsActive->get())
    {
        // safety net for feedback against runaway oscillation
        feedback = std::clamp(feedback, 0.0f, 0.95f);
        // integration of the master gain into the wet path
        // the master control scales the overall volume of the echo without affecting the dry signal
        float wetMix = rawMix * masterGain;
        float dryMix = 1.0f - rawMix; // Klassischer linearer Crossfade für das Originalsignal
        // ==== AUDIO PROCESSING LOOP ====
        const int dynamicChannels = bufferParam.getNumChannels();
        const int channelsToProcess = std::min (dynamicChannels, static_cast<int>(arrDelayBuffer.size()));

        for (int channel = 0; channel < channelsToProcess; ++channel) {

            float* channelData = bufferParam.getWritePointer(channel);
            for (int sample = 0; sample < bufferParam.getNumSamples(); ++sample) {
                float inputSample = channelData[sample];
                // 1. read the delayed signal from the past
                float delayedSample = arrDelayBuffer[channel].ReadBuffer(delayTimeMs);
                // 2. feedback loop: Attenuate the old signal and add it to the new input
                float bufferInput = inputSample + (delayedSample * feedback);
                // 3. write the combination back into the circular buffer
                arrDelayBuffer[channel].WriteBuffer(bufferInput);
                // 4. mix output signal (Dry/Wet + Master volume)
                channelData[sample] = (inputSample * dryMix) + (delayedSample * wetMix);
            }
        }
    }

    // REVERB (uses a dedicated parameter structure object)
    if (effekteActiveParams[3].effectIsActive){
        juce::dsp::Reverb::Parameters reverbSettings;
        reverbSettings.roomSize = (*reverbRoomParam);
        reverbSettings.damping = (*reverbDampingParam);
        if (!effekteActiveParams[3].effectIsActive) {
            reverbSettings.wetLevel = 0.0f;
            reverbSettings.dryLevel = 1.0f;
        } else {
        reverbSettings.wetLevel = dryWetMixReverb;
        reverbSettings.dryLevel = (1.0f - dryWetMixReverb);
    }
    reverbModule.setParameters (reverbSettings);
    reverbModule.process (mainContext);
    }
    //===============================================================================
}

//for reverse spectrogram
void AudioPluginAudioProcessor::processIncomingPixelUpdates()
{
    // clear the FIFO and update the matrix's audio copy
    PixelUpdate update;
    while (pixelFifo.pop (update)) {
        if (update.x == -1) {// our signal for "Clear Canvas"
            std::fill (audioGridData.begin(), audioGridData.end(), 0.0f);
        }
        else {
            // calculate index (column-wise/time-first)
            size_t index = static_cast<size_t>(update.x) * static_cast<size_t>(numRows) + static_cast<size_t>(update.y);
            if (index < audioGridData.size()) {
                audioGridData[index] = update.value;
            }
        }
    }
}

void AudioPluginAudioProcessor::renderSpectrogramToBuffer (juce::AudioBuffer<float>& buffer)
{
    if (spectroParams.spectroIsActiveParam != nullptr && spectroParams.spectroIsActiveParam->get())
    {
        if (audioGridData.empty()) return;
        // gating via the voices
        bool anyVoiceActive = false;
        for (const auto* voice : myVoices) {
            if (voice->isVoiceActive()) { anyVoiceActive = true; break; }
        }
        // IMPORTANT FOR THE ONE-SHOT-RESET:
        // when NO midiMessage is active (all keys released),
        // we zero out the buffer and reset the playhead to 0.0.
        // this causes the next pressed note to start over again from the very beginning
        if (spectroActiveNoteCount == 0) {
            std::fill (overlapBuffer.begin(), overlapBuffer.end(), 0.0f);
            hopCounter = 0;
            currentSamplePosition = 0.0;
            return;
        }
        loopDurationSeconds = static_cast<double>(*spectroParams.spectrogramPlayTime);

        auto numSamples = buffer.getNumSamples();
        auto numChannels = buffer.getNumChannels();

        double sampleRate = getSampleRate();
        double totalLoopSamples = sampleRate * loopDurationSeconds;

        const float specDb = spectroParams.spectrogramVolumeParam->get();
        float specGain = juce::Decibels::decibelsToGain(specDb, -50.0f);
        //specGain = specGain * 0.003f;
        if (specGain <= 0.0002f) return;



        static std::vector<float> persistentPhases (static_cast<size_t>(numRows), 0.0f);

        for (int sample = 0; sample < numSamples; ++sample) {
            // =================================================================
            // 1. THE IFFT COMPUTATION BLOCK
            // we only calculate new iFFTs when the playhead
            // has not yet reached the end of the drawing window (totalLoopSamples)
            // =================================================================
            if (hopCounter == 0 && currentSamplePosition < totalLoopSamples)
            {
                double progress0to1 = currentSamplePosition / totalLoopSamples;
                int currentX = static_cast<int> (progress0to1 * numColumns);
                currentX = juce::jlimit (0, numColumns - 1, currentX);

                std::array<float, 1024> fftData;
                fftData.fill (0.0f);

                const float* columnPtr = &audioGridData[static_cast<size_t>(currentX) * static_cast<size_t>(numRows)];

                for (int y = 0; y < numRows; ++y)
                {
                    float amplitude = columnPtr[y];
                    float phase = persistentPhases[static_cast<size_t>(y)];

                    if (amplitude > 0.0f)
                    {
                        fftData[static_cast<size_t>(2 * y)]     = amplitude * std::cos (phase);
                        fftData[static_cast<size_t>(2 * y + 1)] = amplitude * std::sin (phase);
                    }

                    persistentPhases[static_cast<size_t>(y)] += juce::MathConstants<float>::pi * static_cast<float>(y);
                    persistentPhases[static_cast<size_t>(y)] = std::fmod (persistentPhases[static_cast<size_t>(y)],
                                                                          juce::MathConstants<float>::twoPi);
                }

                fft.performRealOnlyInverseTransform (fftData.data());

                for (int i = 0; i < 512; ++i)
                {
                    float window = 0.5f * (1.0f - std::cos (2.0f * juce::MathConstants<float>::pi * static_cast<float>(i) / 511.0f));
                    //float safeSample = fftData[static_cast<size_t>(i)] * 512.0f;
                    float safeSample = fftData[static_cast<size_t>(i)]  * 5.11f;

                    overlapBuffer[static_cast<size_t>(i)] += safeSample * window;
                }
            }
            // =================================================================
            // EXTRACT AUDIO
            // it keeps running so that the remaining data in the overlapBuffer can decay.
            // when the end is reached and the buffer has been emptied,
            // we automatically read only 0.0f (silence) from this point on.
            // =================================================================
            float currentSpectrogramSample = overlapBuffer[static_cast<size_t>(hopCounter)];
            for (int channel = 0; channel < numChannels; ++channel) {
                buffer.getWritePointer (channel)[sample] += (currentSpectrogramSample * specGain);
            }
            // CORRECTION: The playhead remains at the end of the window and
            // is no longer reset to 0.0 (prevents loops).
            if (currentSamplePosition < totalLoopSamples) {
                currentSamplePosition += 1.0;
            }
            // The hop counter must necessarily continue to run unstoppably,
            // to maintain the shifting of the overlap buffer!
            hopCounter++;
            if (hopCounter == 256) {
                for (int i = 0; i < 256; ++i)
                {
                    overlapBuffer[static_cast<size_t>(i)] = overlapBuffer[static_cast<size_t>(i + 256)];
                    overlapBuffer[static_cast<size_t>(i + 256)] = 0.0f;
                }
                hopCounter = 0;
            }
        }
    }
}

void AudioPluginAudioProcessor::processOutputSpectrogram(juce::AudioBuffer<float>& buffer)
{
    // ==============================================================================
    // === OUTPUT SPECTROGRAM ANALYZER FEEDING ======================================
    // ==============================================================================
    const int numSamplesForAnalyzer = buffer.getNumSamples();
    auto* readL = buffer.getReadPointer (0);
    auto* readR = buffer.getNumChannels() > 1 ? buffer.getReadPointer (1) : readL;

    for (int i = 0; i < numSamplesForAnalyzer; ++i)
    {
        // downmixing stereo to mono for frequency analysis
        fftAccumulatorBuffer[static_cast<size_t>(fftAccumulatorIndex)] = (readL[i] + readR[i]) * 0.5f;
        fftAccumulatorIndex++;

        // when our analysis block is full (512 samples)
        if (fftAccumulatorIndex == 512) {
            // Due to the complex numbers, the FFT requires double the buffer size internally.
            std::array<float, 1024> fftData;
            fftData.fill (0.0f);
            std::copy (fftAccumulatorBuffer.begin(), fftAccumulatorBuffer.end(), fftData.begin());
            //apply window function (prevents frequency artifacts at block boundaries)
            window.multiplyWithWindowingTable (fftData.data(), 512);
            // perform real forward FFT
            outputFFT.performRealOnlyForwardTransform (fftData.data());
            // calculate magnitudes (loudness levels) and scale logarithmically
            FFTFIFO::FFTFrame currentFrame;

            for (int bin = 0; bin < 256; ++bin)
            {
                float real = fftData[static_cast<size_t>(2 * bin)];
                float imag = fftData[static_cast<size_t>(2 * bin + 1)];
                // calculate the amplitude
                float magnitude = std::sqrt (real * real + imag * imag);
                // convert to meaningful dB values (-60 dB to 0 dB)
                float db = juce::Decibels::gainToDecibels (magnitude, -60.0f);
                // map to a clean range from 0.0f (silence) to 1.0f (full level)
                currentFrame[static_cast<size_t>(bin)] = juce::jmap (db, -60.0f, 0.0f, 0.0f, 1.0f);
            }
            // push into the lock-free FIFO
            outputFftFifo.push (currentFrame);
            // reset index for the next block
            fftAccumulatorIndex = 0;
        }
    }
}


// not used, may be it's not working
void AudioPluginAudioProcessor::updateLabelColor (const juce::String& elementId, const juce::String& hexColorWithAlpha)
{
    // GuiValueTree get from the state
    const auto& guiTree = magicState.getGuiTree();
    // search for the corresponding node by ID:
    if (auto node = findNodeById(guiTree, elementId); node.isValid())
    {
        // change property.
        // "background-color" is the property read by the decorator
        node.setProperty("background-color", hexColorWithAlpha, nullptr);
    }
}

// helper function for searching the tree
juce::ValueTree AudioPluginAudioProcessor::findNodeById(juce::ValueTree tree, const juce::String& targetId)
{
    if (tree.getProperty("id").toString() == targetId)
        return tree;

    for (int i = 0; i < tree.getNumChildren(); ++i)
    {
        auto found = findNodeById(tree.getChild(i), targetId);
        if (found.isValid())
            return found;
    }
    return {};
}
#pragma endregion HELPER_METHODS

#pragma region INIT_PARAMS
void AudioPluginAudioProcessor::InitMasterModualtionParams()
{
    lfoMasterFilterMod.setWaveType(0); // sinus
    lfoMasterFilterModSlow.setWaveType(0); //sinus

    auto pNameMastModFilterReso = juce::String::fromUTF8(reinterpret_cast<const char*>(u8"MasterMod FilterReso"));
    juce::NormalisableRange<float> resoRange(0.1f, 2.0f, 0.01f);
    resoRange.setSkewForCentre(1.0f); // 1.0f is exactly in the middle of the slider
    addParameter(masterFilterModulat.filterResoMasterModulParam = new juce::AudioParameterFloat(
        juce::ParameterID("MASTER_MOD_FILTERRESO", 1),
        pNameMastModFilterReso,
        resoRange,
        0.707f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));

    // filter type:
    auto pNameFilterTypeMod = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Filter Type Mod"));
    addParameter(masterFilterModulat.filterTypeParam = new juce::AudioParameterChoice(
        juce::ParameterID ("FILTERTYPE_MOD", 1), pNameFilterTypeMod,
        juce::StringArray{"lowpass", "bandpass", "highpass", "formant"}, 3));


    juce::NormalisableRange<float> envAmountMastModRange(10.0f, 20000.0f, 1.0f);
    auto pNameMastModFilterEnv = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"MasterMod EnvAm Flt "));
    addParameter(masterFilterModulat.filterEnvMasterModulParam = new juce::AudioParameterFloat(
        juce::ParameterID ("MASTER_MOD_ENVWTFLT", 1), pNameMastModFilterEnv,
        envAmountMastModRange, 5000,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 0); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));

    juce::NormalisableRange<float> dryWetMastModRange(0.0f, 100, 0.01f);
    auto pNameMastModFilterDryWet = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"MasterMod DryWet Flt "));
    addParameter(masterFilterModulat.filterDryWetMmp = new juce::AudioParameterFloat(
        juce::ParameterID ("MASTER_MOD_DRYWET", 1), pNameMastModFilterDryWet,
        dryWetMastModRange, 50,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 0); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));

    auto pNameLfoMasterDepth = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"LfoMaster Depth "));
    juce::NormalisableRange<float> lfoMasterDepthRange(0.0f, 8.0f, 0.1f);
    addParameter(lfoMastModParam.lfoMasterDepthParam = new juce::AudioParameterFloat(
        juce::ParameterID ("LFO_MASTER_DEPTH", 1),
        pNameLfoMasterDepth, lfoMasterDepthRange, 2.5f, juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 0); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));



    auto pNameMastModEnvFreq = juce::String::fromUTF8(reinterpret_cast<const char*>(u8"Mod EnvFreq"));
    juce::NormalisableRange<float> envFreqRange(0.2f, 1.4f, 0.01f);
    resoRange.setSkewForCentre(1.0f); // 1.0f liegt genau in der Mitte des Reglers!
    addParameter(masterFilterModulat.envelopeFrequencyP = new juce::AudioParameterFloat(
        juce::ParameterID("ENVELOPE_MOD_FREQ", 1),
        pNameMastModEnvFreq,
        envFreqRange,
        0.75f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int)
        {
            return juce::String(value, 2) + " Hz"; })
        .withValueFromStringFunction([](const juce::String& text){
            return text.getFloatValue();
        })
    ));

    auto pNameModVobbleLimit = juce::String::fromUTF8(reinterpret_cast<const char*>(u8"Mod WobbleLimit"));
    juce::NormalisableRange<float> modVobbleRange(0.2f, 180.0f, 0.1f);
    resoRange.setSkewForCentre(30.0f); // 1.0f liegt genau in der Mitte des Reglers!
    addParameter(masterFilterModulat.wobbleUpperLimitP = new juce::AudioParameterFloat(
        juce::ParameterID("WOBBLE_MOD_LIMIT", 1),
        pNameModVobbleLimit,
        modVobbleRange,
        16.0f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int)
        {
            return juce::String(value, 1) + " Hz"; })
        .withValueFromStringFunction([](const juce::String& text){
            return text.getFloatValue();
        })
    ));

    auto pNameGlitchIsActivatedParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Glitch Active "));
    addParameter(glitchActiveParam = new juce::AudioParameterBool
        (juce::ParameterID ("GLITCH_ACTIVE", 1), pNameGlitchIsActivatedParam, false));

    //=============================================================================================//
    //=================  fm/am-modulation: ========================================================
    // fm-mod.
    auto pNameLfoFmWaveMaster = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Lfo Fm WaveMaster "));
    addParameter(lfoFmWaveFormParamMaster = new juce::AudioParameterChoice(
        juce::ParameterID ("LFO_FM_WAVE_MASTER", 1), pNameLfoFmWaveMaster,
        juce::StringArray{"sine", "saw", "square"}, 0));
    //Lfo Freq
    juce::NormalisableRange<float> fmFreqRangeMaster(0.0f, 1000.0f, 0.1f);
    auto pNameFmFreqMaster = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"LfoFmFreqMaster "));
    addParameter(fmFreqMaster = new juce::AudioParameterFloat(
        juce::ParameterID ("LFOFMFREQ_MASTER", 1), pNameFmFreqMaster,
        fmFreqRangeMaster, 7.0f, juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int)
        {
            return juce::String(value, 0) + " Hz";
        })
        .withValueFromStringFunction([](const juce::String& text){
            return text.getFloatValue();
        })));

    auto pNameFmDepthMaster = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"FmDepth Master"));
    juce::NormalisableRange<float> fmDepthRangeMaster(0.0f, 200.0f, 0.1f, 0.4f);
    addParameter(fmDepthMaster = new juce::AudioParameterFloat(
        juce::ParameterID ("FMDEPTH_MASTER", 1), pNameFmDepthMaster, fmDepthRangeMaster,
        20.0f, juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int)
        {
            if (value >= 1000.0f)
                return juce::String(value / 1000.0f, 2);
            return juce::String(value, 0);
        })
        .withValueFromStringFunction([](const juce::String& text)
        {
            return text.getFloatValue();
        })));

    // am-modulation
    auto pNameLfoAmWaveMaster = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Lfo Am WaveMaster "));
    addParameter(lfoAmWaveFormParamMaster = new juce::AudioParameterChoice(
        juce::ParameterID ("LFO_AM_WAVE_MASTER", 1), pNameLfoAmWaveMaster,
        juce::StringArray{"sine", "saw", "square"}, 0));
    //Lfo Freq
    juce::NormalisableRange<float> amFreqRangeMaster(0.0f, 20.0f, 0.01f, 0.35f);
    auto pNameAmFreqMaster = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"LfoAmFreqMaster "));
    addParameter(amFreqMaster = new juce::AudioParameterFloat(
        juce::ParameterID ("LFOAMFREQ_MASTER", 1),
        pNameAmFreqMaster,  amFreqRangeMaster, 2.5f, juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int)
        {
            return juce::String(value, 0) + " Hz"; })
            .withValueFromStringFunction([](const juce::String& text){
                return text.getFloatValue();
            })
        ));

    auto pNameAmDepthMaster = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"AmDepth Master "));
    juce::NormalisableRange<float> amDepthRangeMaster(0.0f, 100.0f, 0.1f, 0.4f);
    addParameter(amDepthMaster = new juce::AudioParameterFloat(
        juce::ParameterID ("AMDEPTH_MASTER", 1),
        pNameAmDepthMaster, amDepthRangeMaster, 18.0f, juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int)
        {
            if (value >= 1000.0f)
                return juce::String(value / 1000.0f, 2);
            return juce::String(value, 0);
        })
        .withValueFromStringFunction([](const juce::String& text)
        {
            return text.getFloatValue();
        })));
}

//==============================================================================
// for the left global sidebar
void AudioPluginAudioProcessor::InitParamsMasterSound(const std::vector<std::pair<float, float>> controlPointsParam)
{
    auto pNameAnalysersActivatedParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"active Analys "));
    addParameter(bothAnalysersActiveParam = new juce::AudioParameterBool
        (juce::ParameterID ("ANALYS_ACTIVE_", 1), pNameAnalysersActivatedParam, false));


    //===================================================================================
    outputWtMeterOscA = magicState.createAndAddObject<foleys::MagicLevelSource>("outputWtMet_M_OscA");
    outputWtMeterOscB = magicState.createAndAddObject<foleys::MagicLevelSource>("outputWtMet_M_OscB");
    outputWtMeterOscC = magicState.createAndAddObject<foleys::MagicLevelSource>("outputWtMet_M_OscC");
    outputWtMeterOscD = magicState.createAndAddObject<foleys::MagicLevelSource>("outputWtMet_M_OscD");
    outputSamplerMeter = magicState.createAndAddObject<foleys::MagicLevelSource>("outputSamplerMet");
    outputSpecMeter = magicState.createAndAddObject<foleys::MagicLevelSource>("outputSpectroMet");
    outputMeterMasterMain = magicState.createAndAddObject<foleys::MagicLevelSource>("outputWtMet_M_Osc");

    // parameter master Volume
    auto pNameMasterVolumeParam = juce::String::fromUTF8(
            reinterpret_cast<const char*>(u8"Master Volume "));
    addParameter(masterVolumeMainParam = new juce::AudioParameterFloat(
        juce::ParameterID("MASTER_VOLUME", 1),
        pNameMasterVolumeParam,
        GuiUtils::createControlPointRange(controlPointsParam, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float val, int) {
                return val <= -59.0f ? "-INF" : juce::String(val, 1) + " dB";
            })
            .withValueFromStringFunction([](const juce::String& text) {
                return text.getFloatValue();
            })
    ));
    //=========

    juce::NormalisableRange<float> eqRange(-12.0f, 12.0f, 0.1f);
    auto pNameMasterEQbass = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"EQ Bass "));
    addParameter(masterEqBass = new juce::AudioParameterFloat(
        juce::ParameterID ("MASTER_EQ_BASS", 1), pNameMasterEQbass,
        eqRange, 0.0f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 1); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

    auto pNameMasterEQmid = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"EQ Mid "));
    addParameter(masterEqMid = new juce::AudioParameterFloat(
        juce::ParameterID ("MASTER_EQ_MID", 1), pNameMasterEQmid,
        eqRange, 0.0f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

    auto pNameMasterEQhigh = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"EQ High "));
    addParameter(masterEqHigh = new juce::AudioParameterFloat(
        juce::ParameterID ("MASTER_EQ_HIGH", 1), pNameMasterEQhigh,
        eqRange, 0.0f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));
}

void AudioPluginAudioProcessor::InitParamsWToscs (const std::vector<std::pair<float, float>> controlPointsParam)
{
    //==================================================================================
    // for SynthLab:
    bool _currDefaultSetted = false;
    juce::String knobMode = "A";
    for (int inx = 0; inx < 4; inx++)
    {
        if (inx == 0){ _currDefaultSetted = true; }
        else { _currDefaultSetted = false; }

        auto suffixWT = juce::String(inx + 1);
        // core selection (fixed):
        auto pNameWtCoreParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Modus "));
        addParameter(synthLabOscParams[inx].coreParam = new juce::AudioParameterChoice(
            juce::ParameterID ("SL_CORE_" + suffixWT, 1), pNameWtCoreParam,
            juce::StringArray{"Classic", "Morphing", "SFX", "Fourier"}, inx));

        // since different cores have varying numbers of waves:
        // a list ranging from "Wave 1" to "Wave 16" is usually used here:
        juce::StringArray waveIndices;
        for (int i = 1; i <= 16; ++i) waveIndices.add("Slot_" + juce::String(i));
        auto pNameWtWaveParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Waveform Index"));
        addParameter(synthLabOscParams[inx].waveParam = new juce::AudioParameterChoice(
            juce::ParameterID ("SL_WAVE_" + suffixWT, 1), pNameWtWaveParam, waveIndices, inx));


        auto pNameWtAttack = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Attack Wt ")) + suffixWT;
        addParameter(synthLabOscParams[inx].attackGwT = new juce::AudioParameterFloat(
            juce::ParameterID ("ATTACK_WT_" + suffixWT, 1), pNameWtAttack,
            juce::NormalisableRange<float>(0.0f, 2.0f, 0.001f, 0.5f), 1.0f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

        auto pNameWtDecay = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Decay Wt ")) + suffixWT;
        addParameter(synthLabOscParams[inx].decayGwT = new juce::AudioParameterFloat(
            juce::ParameterID ("DECAY_WT_" + suffixWT, 1), pNameWtDecay,
            juce::NormalisableRange<float>(0.0f, 2.0f, 0.001f, 0.5f), 1.0f,juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

        auto pNameWtSustain = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Sustain Wt ")) + suffixWT;
        addParameter(synthLabOscParams[inx].sustainGwT = new juce::AudioParameterFloat(
            juce::ParameterID ("SUSTAIN_WT_" + suffixWT, 1), pNameWtSustain,
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 0.5f), 0.7f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

        auto pNameWtRelease = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Release Wt ")) + suffixWT;
        addParameter(synthLabOscParams[inx].releaseGwT = new juce::AudioParameterFloat(
            juce::ParameterID ("RELEASE_WT_" + suffixWT, 1), pNameWtRelease,
            juce::NormalisableRange<float>(0.0f, 2.0f, 0.001f, 0.5f), 1.5f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

        // Mod-Knobs A, B, C, D
        for (int i = 0; i < 4; ++i) {
            switch (i) {
                case 0: knobMode = "A";
                    break;
                case 1: knobMode = "B";
                    break;
                case 2: knobMode = "C";
                    break;
                case 3: knobMode = "D";
                    break;
                default: knobMode = "A";
            }
            auto pNameModeKnob = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Mod Knob ")) + juce::String(static_cast<char>('A' + i));
            addParameter(synthLabOscParams[inx].modKnobs[i] = new juce::AudioParameterFloat(
                juce::ParameterID ("SL_MOD_" + knobMode + "_" + suffixWT, 1), pNameModeKnob,
                0.0f, 1.0f, 0.2f));
        }
        // wt volume-params:
        auto pNameWtVolumeParam = juce::String::fromUTF8(
            reinterpret_cast<const char*>(u8"VolumeWt ")) + suffixWT;
        addParameter(synthLabOscParams[inx].volumeWTParams = new juce::AudioParameterFloat(
            juce::ParameterID("VOLUMEwT_" + suffixWT, 1),
            pNameWtVolumeParam,
            GuiUtils::createControlPointRange(controlPointsParam, 0.1f),
            -5.0f,
            juce::AudioParameterFloatAttributes()
                .withStringFromValueFunction([](const float val, int) {
                    return val <= -59.0f ? "-INF" : juce::String(val, 1) + " dB";
                })
                .withValueFromStringFunction([](const juce::String& text) {
                    return text.getFloatValue();
                })
        ));

        auto pNameWtTuneParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Tune ")) + suffixWT;
        addParameter(synthLabOscParams[inx].tuneWTParams = new juce::AudioParameterFloat(
            juce::ParameterID ("TUNE_" + suffixWT, 1), pNameWtTuneParam,
            juce::NormalisableRange<float>(0.0f, 2.0f, 0.001), 0.1f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

        auto pNameWtPanParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Pan ")) + suffixWT;
        addParameter(synthLabOscParams[inx].panWTParams = new juce::AudioParameterFloat(
            juce::ParameterID ("PAN_" + suffixWT, 1), pNameWtPanParam,
            juce::NormalisableRange<float>(-1.0f, 1.0f, 0.001f), 0.8f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));


        auto pNameWtReserveParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Reserve ")) + suffixWT;
        addParameter(synthLabOscParams[inx].reserveWTParams = new juce::AudioParameterFloat(
            juce::ParameterID ("RESERVE_" + suffixWT, 1), pNameWtReserveParam,
            juce::NormalisableRange<float>(0.0f, 2.0f, 0.001f), 0.4f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

        auto rangeOscShape = juce::NormalisableRange<float>(-1.0f, 1.0f, 0.005f);
        auto oscillatorShape = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Oscillator Shape ")) + suffixWT;
        addParameter(synthLabOscParams[inx].oscillatorShape = new juce::AudioParameterFloat(
            juce::ParameterID ("OSCSHAPE" + suffixWT, 1), oscillatorShape,
            rangeOscShape, 0.4f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));


        auto pNameWtIsActivatedParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"active WT ")) + suffixWT;
        addParameter(synthLabOscParams[inx].isActivated = new juce::AudioParameterBool
            (juce::ParameterID ("WT_ACTIVE_" + suffixWT, 1), pNameWtIsActivatedParam, false));

        // for switching GUI plot-adsr
        auto pNameWtGainFilterParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"switch gainFilter WT ")) + suffixWT;
        addParameter(switchAdrsParams[inx].switchGainFilterParam = new juce::AudioParameterBool
            (juce::ParameterID ("WT_SWITCH_GAIN_FILTER_" + suffixWT, 1), pNameWtGainFilterParam, true));

        auto pNameAdsrWt = juce::String::fromUTF8(reinterpret_cast<const char*>(u8"adsr-Wt")) + suffixWT;
        auto* plotWt = synthLabOscParams[inx].adsrWtPtr = magicState.createAndAddObject<ADSRPlot>(pNameAdsrWt);
        magicState.addBackgroundProcessing(plotWt);

        if (plotWt != nullptr)
        {
            plotWt->setActive(true);
            plotWt->setGuiState(magicState);
        }
    }
}

// parameters for filter modulation:
void AudioPluginAudioProcessor::InitJuceWtFilterParameters()
{
    for (int i = 0; i < 4; ++i)
    {
        auto suffix = juce::String(i + 1);

        // filter type:
        auto pNameFilterType = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Filter Type ")) + suffix;
        addParameter(synthLabOscParams[i].filterTypeParam = new juce::AudioParameterChoice(
            juce::ParameterID ("FILTERTYPE" + suffix, 1), pNameFilterType,
            juce::StringArray{"lowpass", "bandpass", "highpass", "formant"}, std::min(i, 2)));


        // define range
        juce::NormalisableRange<float> filterRange(20.0f, 20000.0f, 0.1f);
        auto pNameFilterFreq = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"FilterFreq ")) + suffix;
        // Set the scaling so that 1000 Hz is at the center of the slider
        filterRange.setSkewForCentre(1000.0f);
        // Use in the parameter
        addParameter(synthLabOscParams[i].filterFreq = new juce::AudioParameterFloat(
            juce::ParameterID("FILTERFREQ" + suffix, 1),
            pNameFilterFreq,
            filterRange, // Use the prepared range here
            300, // Default-value
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int)
            {
                // display Hz as a suffix
                if (value >= 1000.0f)
                    return juce::String(value / 1000.0f, 2) + " kHz";
                return juce::String(value, 0) + " Hz";
            })
            .withValueFromStringFunction([](const juce::String& text)
            {
                return text.getFloatValue();
            })
        ));

        auto pNameFilterReso = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"FilterReso ")) + suffix;
        addParameter(synthLabOscParams[i].filterReso = new juce::AudioParameterFloat(
            juce::ParameterID ("FILTERRESO" + suffix, 1), pNameFilterReso,
            juce::NormalisableRange<float>(1.0f, 10.0f, 0.01f, 1.5f), 2.5f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

        // filter envelopes:
        auto pNameAttackFlt = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Attack Flt ")) + suffix;
        addParameter(synthLabOscParams[i].attackFilterParam = new juce::AudioParameterFloat(
            juce::ParameterID ("ATTACKFLT" + suffix, 1), pNameAttackFlt,
            juce::NormalisableRange<float>(0.05f, 0.2f, 0.001f, 0.5f), 0.1f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

        auto pNameDecayFlt = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Decay Flt ")) + suffix;
        addParameter(synthLabOscParams[i].decayFilterParam = new juce::AudioParameterFloat(
            juce::ParameterID ("DECAYFLT" + suffix, 1), pNameDecayFlt,
            juce::NormalisableRange<float>(0.0f, 2.0f, 0.001f, 0.5f), 0.8f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

        auto pNameSustainFlt = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Sustain Flt ")) + suffix;
        addParameter(synthLabOscParams[i].sustainFilterParam = new juce::AudioParameterFloat(
            juce::ParameterID ("SUSTAINFLT" + suffix, 1), pNameSustainFlt,
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 0.5f), 0.5f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

        auto pNameReleaseFlt = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Release Flt ")) + suffix;
        addParameter(synthLabOscParams[i].releaseFilterParam = new juce::AudioParameterFloat(
            juce::ParameterID ("RELEASEFLT" + suffix, 1), pNameReleaseFlt,
            juce::NormalisableRange<float>(0.0f, 2.0f, 0.001f, 0.5f), 0.8f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));


        juce::NormalisableRange<float> anvAmountRange(0.0f, 4800.0f, 1.0f);
        auto pNameEnvAmFlt = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"EnvAm Flt ")) + suffix;
        anvAmountRange.setSkewForCentre(1000.0f);
        addParameter(synthLabOscParams[i].filterEnvelopeAmount = new juce::AudioParameterFloat(
            juce::ParameterID ("ENVWTFLT" + suffix, 1), pNameEnvAmFlt,
            anvAmountRange, 3600,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 0); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

        auto pNameWtFilterActiveParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Wt Filter_active")) + suffix;
        addParameter(synthLabOscParams[i].filterIsActivated = new juce::AudioParameterBool
            (juce::ParameterID ("WT_FILTER_ACTIVE_" + suffix, 1), pNameWtFilterActiveParam, true));
    }
}

// parameters frequency - und amplitude modulation:
void AudioPluginAudioProcessor::InitLfoParameters()
{
    // fm-modulation
    for (int ind = 0; ind < 4; ++ind)
    {
        auto suffixLfo = juce::String(ind + 1);
        auto rangeLFOvolume = juce::NormalisableRange<float>(-60.0f, 12.0f, 0.1f);
        rangeLFOvolume.skew = std::log(0.5f) /
                       std::log((-8.5f - (-60.0f)) / (12.0f - (-60.0f)));
        auto pNameLfoVolumeParam = juce::String::fromUTF8(
            reinterpret_cast<const char*>(u8"Gain FM Lfo ")) + suffixLfo;
        addParameter(juceLfoFmParams[ind].lfoFmGainParam =
            new juce::AudioParameterFloat(
                juce::ParameterID("GAIN_Fm_Lfo_" + suffixLfo, 1),
                pNameLfoVolumeParam,
                rangeLFOvolume,
                -8.5f,
                juce::AudioParameterFloatAttributes()
                    .withStringFromValueFunction([](const float val, int) {
                        return val <= -59.0f ? "-INF" : juce::String(val, 1) + " dB";
                    })
                    .withValueFromStringFunction([](const juce::String& text) {
                        return text.getFloatValue();
                    })
            ));

        // Lfo Waveform
        auto pNameLfoFmWave = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Lfo Fm Wave ")) + suffixLfo;
        addParameter(juceLfoFmParams[ind].lfoFmWaveFormParam = new juce::AudioParameterChoice(
        juce::ParameterID ("LFO_FM_WAVE" + suffixLfo, 1), pNameLfoFmWave,
        juce::StringArray{"sine", "saw", "square"}, std::min(ind, 2)));
        //Lfo Freq
        juce::NormalisableRange<float> fmFreqRange(0.0f, 1000.0f, 0.1f);
        auto pNameFmFreq = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"LfoFmFreq ")) + suffixLfo;
        addParameter(juceLfoFmParams[ind].lfoFmFreqParam = new juce::AudioParameterFloat(
            juce::ParameterID ("LFOFMFREQ" + suffixLfo, 1),
            pNameFmFreq,
            fmFreqRange, 7.0f, juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int)
            {
                return juce::String(value, 0) + " Hz"; })
            .withValueFromStringFunction([](const juce::String& text){
                return text.getFloatValue();
            })
        ));

        auto pNameFmDepth = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"FmDepth ")) + suffixLfo;
        juce::NormalisableRange<float> fmDepthRange(0.0f, 200.0f, 0.1f, 0.4f);
        addParameter(juceLfoFmParams[ind].lfoFmDepthParam = new juce::AudioParameterFloat(
            juce::ParameterID ("FMDEPTH" + suffixLfo, 1),
            pNameFmDepth, fmDepthRange,
            20.0f, juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int)
            {
                if (value >= 1000.0f)
                    return juce::String(value / 1000.0f, 2);
                return juce::String(value, 0);
            })
            .withValueFromStringFunction([](const juce::String& text)
            {
                return text.getFloatValue();
            })
        ));
    }
    // am-modulation
    for (int ind = 0; ind < 4; ++ind)
    {
        auto suffixLfo = juce::String(ind + 1);
        auto rangeLFOvolume = juce::NormalisableRange<float>(-60.0f, 12.0f, 0.1f);
        rangeLFOvolume.skew = std::log(0.5f) /
                       std::log((-8.5f - (-60.0f)) / (12.0f - (-60.0f)));
        auto pNameLfoVolumeParam = juce::String::fromUTF8(
            reinterpret_cast<const char*>(u8"Gain AM Lfo ")) + suffixLfo;
        addParameter(juceLfoAmParams[ind].lfoAmGainParam =
            new juce::AudioParameterFloat(
                juce::ParameterID("GAIN_Am_Lfo_" + suffixLfo, 1),
                pNameLfoVolumeParam, rangeLFOvolume, -8.5f, juce::AudioParameterFloatAttributes()
                    .withStringFromValueFunction([](const float val, int) {
                        return val <= -59.0f ? "-INF" : juce::String(val, 1) + " dB";
                    })
                    .withValueFromStringFunction([](const juce::String& text) {
                        return text.getFloatValue();
                    })
            ));
        // Lfo Waveform
        auto pNameLfoAmWave = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Lfo Am Wave ")) + suffixLfo;
        addParameter(juceLfoAmParams[ind].lfoAmWaveFormParam = new juce::AudioParameterChoice(
        juce::ParameterID ("LFO_AM_WAVE" + suffixLfo, 1), pNameLfoAmWave,
        juce::StringArray{"sine", "saw", "square"}, 3-ind));
        //Lfo Freq
        juce::NormalisableRange<float> amFreqRange(0.0f, 20.0f, 0.01f, 0.35f);
        auto pNameAmFreq = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"LfoAmFreq ")) + suffixLfo;
        addParameter(juceLfoAmParams[ind].lfoAmFreqParam = new juce::AudioParameterFloat(
            juce::ParameterID ("LFOAMFREQ" + suffixLfo, 1),
            pNameAmFreq,  amFreqRange, 2.5f, juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int)
            {
                return juce::String(value, 0) + " Hz"; })
            .withValueFromStringFunction([](const juce::String& text){
                return text.getFloatValue();
            })
        ));

        auto pNameAmDepth = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"AmDepth ")) + suffixLfo;
        juce::NormalisableRange<float> amDepthRange(0.0f, 100.0f, 0.1f, 0.4f);
        addParameter(juceLfoAmParams[ind].lfoAmDepthParam = new juce::AudioParameterFloat(
            juce::ParameterID ("AMDEPTH" + suffixLfo, 1),
            pNameAmDepth, amDepthRange, 18.0f, juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int)
            {
                if (value >= 1000.0f)
                    return juce::String(value / 1000.0f, 2);
                return juce::String(value, 0);
            })
            .withValueFromStringFunction([](const juce::String& text)
            {
                return text.getFloatValue();
            })
        ));
    }

}


void AudioPluginAudioProcessor::InitSpectroParameters(const std::vector<std::pair<float, float>> controlPointsParam)
{
    // range from -60 dB to +6 dB, NO skew (skew = 1.0f is standard)
    auto spectroRange = juce::NormalisableRange<float>(-50.0f, 3.0f, 0.1f);

    auto pNameSpectroGain = juce::String::fromUTF8(reinterpret_cast<const char*>(u8"Spectro Volume "));

    addParameter(spectroParams.spectrogramVolumeParam = new juce::AudioParameterFloat(
        juce::ParameterID("SPECTROGRAM_VOLUME", 1),
        pNameSpectroGain,
        spectroRange,
        0.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float val, int) {
                return val <= -59.5f ? "-INF" : juce::String(val, 1) + " dB";
            })
            .withValueFromStringFunction([](const juce::String& text) {
                return text.getFloatValue();
            })
    ));

    auto pNameSpectroIsActivatedParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Spec Active "));
    addParameter(spectroParams.spectroIsActiveParam = new juce::AudioParameterBool
        (juce::ParameterID ("SPEC_ACTIVE", 1), pNameSpectroIsActivatedParam, false));

    juce::NormalisableRange<float> spectroTimeRange(0.7f, 4.0, 0.1f);
    auto pNameSpectroTime = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Spectro Time "));
    addParameter(spectroParams.spectrogramPlayTime = new juce::AudioParameterFloat(
        juce::ParameterID ("SPECTRO_TIME", 1), pNameSpectroTime,
        spectroTimeRange, 3.0f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 1); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));
}


void AudioPluginAudioProcessor::InitEffekteParameters()
{
    juce::NormalisableRange<float> chorusRateRange(0.1f, 10.0f, 0.02f);
    auto pNameChorusRate = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Chorus Rate "));
    addParameter(chorusRateParam = new juce::AudioParameterFloat(
        juce::ParameterID ("EFF_CHORUS_RATE", 1), pNameChorusRate,
        chorusRateRange, 1.0,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));

    juce::NormalisableRange<float> chorusDepthRange(0.0f, 1.0f, 0.01f);
    auto pNameChorusDepth = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Chorus Depth "));
    addParameter(chorusDepthParam = new juce::AudioParameterFloat(
        juce::ParameterID ("EFF_CHORUS_DEPTH", 1), pNameChorusDepth,
        chorusDepthRange, 0.25,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));

    juce::NormalisableRange<float> chorusFeedbackRange(-0.9f, 0.9f, 0.01f);
    auto pNameChorusFeedback = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Chorus Feedback "));
    addParameter(chorusFeedBack = new juce::AudioParameterFloat(
        juce::ParameterID ("EFF_CHORUS_FEEDBACK", 1), pNameChorusFeedback,
        chorusFeedbackRange, 0.7,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));

    juce::NormalisableRange<float> ChorusCDelayRange(10.0f, 40.0f, 0.02f);
    auto pNameChorusCDelay = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Chorus Centre Delay "));
    addParameter(flangerCentreDelay= new juce::AudioParameterFloat(
        juce::ParameterID ("EFF_CHORUS_CDELAY", 1), pNameChorusCDelay,
        ChorusCDelayRange, 30.0,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));

    juce::NormalisableRange<float> flangerRateRange(0.01f, 5.0f, 0.1f);
    auto pNameFlangerRate = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Flanger Rate "));
    addParameter(flangerRateParam = new juce::AudioParameterFloat(
        juce::ParameterID ("EFF_FLANGER_RATE", 1), pNameFlangerRate,
        flangerRateRange, 0.5f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 1); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));

    juce::NormalisableRange<float> flangerDepthRange(0.0, 1.0f, 0.02f);
    auto pNameFlangerDepth = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Flanger Depth "));
    addParameter(flangerDepthParam = new juce::AudioParameterFloat(
        juce::ParameterID ("EFF_FLANGER_DEPTH", 1), pNameFlangerDepth,
        flangerDepthRange, 0.5f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));

    juce::NormalisableRange<float> flangerFeedbackRange(-0.9f, 0.9f, 0.01f);
    auto pNameFlangerFeedback = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Flanger Feedback "));
    addParameter(flangerFeedBackParam = new juce::AudioParameterFloat(
        juce::ParameterID ("EFF_FLANGER_FEEDBACK", 1), pNameFlangerFeedback,
        flangerFeedbackRange, 0.7,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));

    juce::NormalisableRange<float> flangerCDelayRange(0.1f, 9.0f, 0.02f);
    auto pNameFlangerCDelay = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Flanger Centre Delay "));
    addParameter(flangerCentreDelay= new juce::AudioParameterFloat(
        juce::ParameterID ("EFF_FLANGER_CDELAY", 1), pNameFlangerCDelay,
        flangerCDelayRange, 2.5,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));


    juce::NormalisableRange<float> delayMasterRange(0.0f, 1.0f, 0.01f);
    auto pNameDelayMasterParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Delay Master "));
    addParameter(delayMaster = new juce::AudioParameterFloat(
        juce::ParameterID ("EFF_DELAY_MASTER", 1), pNameDelayMasterParam,
        delayMasterRange, 0.5f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 0); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));

    juce::NormalisableRange<float> delayTimeMsRange(20.0f, 2000.0f, 2.0f);
    auto pNameDelayTimeMsParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Delay Time Ms "));
    addParameter(delayTimeMsParam = new juce::AudioParameterFloat(
        juce::ParameterID ("EFF_DELAY_TIME_MS", 1), pNameDelayTimeMsParam,
        delayTimeMsRange, 200.0f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 0); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));

    juce::NormalisableRange<float> dryWetDelayRange(0.0f, 1.0, 0.01f);
    auto pNameDelayDryWet = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Delay DryWet "));
    addParameter(delayMix = new juce::AudioParameterFloat(
        juce::ParameterID ("DELAY_DRYWET", 1), pNameDelayDryWet,
        dryWetDelayRange, 0.7,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 0); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));

    juce::NormalisableRange<float> feedBackDelayRange(0.0f, 1.0, 0.01f);
    auto pNameDelayfeedBack = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Delay Feedback "));
    addParameter(delayFeedback = new juce::AudioParameterFloat(
        juce::ParameterID ("DELAY_FEEDBACK", 1), pNameDelayfeedBack,
        feedBackDelayRange, 0.4,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 0); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));

    auto pNameDelayIsPingPongParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Delay PingPong "));
    addParameter(PingPongEnabled = new juce::AudioParameterBool
        (juce::ParameterID ("DELAY_IS_PINGPONG", 1), pNameDelayIsPingPongParam, false));

    auto pNameIsSyncParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Delay Sync "));
    addParameter(delaySyncEnabled = new juce::AudioParameterBool
        (juce::ParameterID ("DELAY_IS_SYNC", 1), pNameIsSyncParam, false));


    auto pNameSyncSettingType = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Sync Setting Type "));
    addParameter(delaySyncSetting = new juce::AudioParameterChoice(
        juce::ParameterID ("DELAY_SYNC_SETTING",1), pNameSyncSettingType,
        juce::StringArray{"1/1", "1/1D", "1/1T", "1/2", "1/2D", "1/2T", "1/4", "1/4D", "1/4T", "1/8", "1/8D", "1/8T",
                    "1/16", "1/16D", "1/16T", "1/32"}, 1));

    juce::NormalisableRange<float> reverbRoomRange(0.0f, 1.0f, 0.01f);
    auto pNameReverbRoom = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Reverb Room "));
    addParameter(reverbRoomParam = new juce::AudioParameterFloat(
        juce::ParameterID ("EFF_REVERB_ROOM", 1), pNameReverbRoom,
        reverbRoomRange, 0.5f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));

    juce::NormalisableRange<float> reverbDampingRange(0.0f, 1.0f, 0.01f);
    auto pNameReverbDamping = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Reverb Damping "));
    addParameter(reverbDampingParam = new juce::AudioParameterFloat(
        juce::ParameterID ("EFF_REVERB_DAMPIMG", 1), pNameReverbDamping,
        reverbDampingRange, 0.5f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));

    auto pNameChorusIsActivatedParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Chorus Active "));
    addParameter(effekteActiveParams[0].effectIsActive = new juce::AudioParameterBool
        (juce::ParameterID ("CHORUS_ACTIVE", 1), pNameChorusIsActivatedParam, false));

    auto pNameFlangerIsActivatedParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Flanger Active "));
    addParameter(effekteActiveParams[1].effectIsActive = new juce::AudioParameterBool
        (juce::ParameterID ("FLANGER_ACTIVE", 1), pNameFlangerIsActivatedParam, false));

    auto pNameDelayIsActivatedParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Delay Active "));
    addParameter(effekteActiveParams[2].effectIsActive = new juce::AudioParameterBool
        (juce::ParameterID ("DELAY_ACTIVE", 1), pNameDelayIsActivatedParam, false));

    auto pNameReverbIsActivatedParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Reverb Active "));
    addParameter(effekteActiveParams[3].effectIsActive = new juce::AudioParameterBool
        (juce::ParameterID ("REVERB_ACTIVE", 1), pNameReverbIsActivatedParam, false));

    juce::NormalisableRange<float> dryWetChorusRange(0.0f, 100, 0.01f);
    auto pNameChorusDryWet = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Chorus DryWet "));
    addParameter(chorusDryWetParam = new juce::AudioParameterFloat(
        juce::ParameterID ("CHORUS_DRYWET", 1), pNameChorusDryWet,
        dryWetChorusRange, 50,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 0); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));

    juce::NormalisableRange<float> dryWetFlangerRange(0.0f, 100, 0.01f);
    auto pNameFlangerDryWet = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Flanger DryWet "));
    addParameter(flangerDryWetParam = new juce::AudioParameterFloat(
        juce::ParameterID ("FLANGER_DRYWET", 1), pNameFlangerDryWet,
        dryWetFlangerRange, 50,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 0); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));


    juce::NormalisableRange<float> dryWetReverbRange(0.0f, 100, 0.01f);
    auto pNameReverbDryWet = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Reverb DryWet "));
    addParameter(reverbDryWetParam = new juce::AudioParameterFloat(
        juce::ParameterID ("REVERB_DRYWET", 1), pNameReverbDryWet,
        dryWetReverbRange, 50,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 0); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
    ));

    for (int inx = 0; inx < 4; inx++)
    {
        auto suffixEffects = juce::String(inx + 1);
        auto pNameEffIsActivatedParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"active Eff ")) + suffixEffects;
        addParameter(effekteActiveParams[inx].effectIsActive = new juce::AudioParameterBool
            (juce::ParameterID ("EFF_ACTIVE_" + suffixEffects, 1), pNameEffIsActivatedParam, false));
    }
}
#pragma endregion INIT_PARAMS

#pragma region TRIGGER_METHODS
void AudioPluginAudioProcessor::InitializeWTOscTriggers()
{// for the 4 activation-buttons
    MyPropertyListeners.clear();

    for (int i = 0; i < 4; ++i)
    {
        juce::String propName = "wt" + juce::String (i) + "Active";

        // cleanly read the initial value from the parameter
        const bool initVal = synthLabOscParams[i].isActivated->get();

        auto propValue = magicState.getPropertyAsValue (propName);

        // FIRST create and register the listener
        MyPropertyListeners.push_back (std::make_unique<PropertyLambdaListener> (
            propValue,
            [this, i] (bool newValue)
            {
                if (auto* p = synthLabOscParams[i].isActivated)
                    p->setValueNotifyingHost (newValue ? 1.0f : 0.0f);
            }
        ));
        // THEN set the property in magicState (triggers the listener)
        propValue.setValue (initVal);
    }
}

// switch display plot adsr or filter
void AudioPluginAudioProcessor::InitializeGainFilterTriggers()
{
    // delete any old listeners (in case the tree is reloaded)
    MyPropertyGainFilterListeners.clear();

    for (int i = 0; i < 4; ++i) {
        juce::String propName = "switchWT" + juce::String(i) + "adsr";
        const bool initVal = switchAdrsParams[i].switchGainFilterParam->get();

        // set initial value in PGM (creates the entry in the XML root
        auto propValue = magicState.getPropertyAsValue(propName);
        // attach the lambda listener and save it in the vector
        MyPropertyGainFilterListeners.push_back (std::make_unique<PropertyGainFilterListener> (
            propValue,
            [this, i] (bool newValue)
            {
                if (auto* p = switchAdrsParams[i].switchGainFilterParam) {
                    p->setValueNotifyingHost (newValue ? 1.0f : 0.0f);
                }
            }
        ));
        propValue.setValue(initVal);
    }
}

// switch fiter function on off
void AudioPluginAudioProcessor::InitializeSynthFilterActiveTriggers()
{
    MyPropertySynthFilterActiveListeners.clear();

    for (int i = 0; i < 4; ++i)
    {
        juce::String propName = "wt" + juce::String (i) + "FilterActivated";

        const bool initVal = synthLabOscParams[i].filterIsActivated->get();

        auto propValue = magicState.getPropertyAsValue (propName);

        MyPropertySynthFilterActiveListeners.push_back (std::make_unique<PropertySynthFilterActiveListener> (
            propValue,
            [this, i] (bool newValue)
            {
                if (auto* p = synthLabOscParams[i].filterIsActivated)
                    p->setValueNotifyingHost (newValue ? 1.0f : 0.0f);
            }
        ));
        propValue.setValue (initVal);
    }
}

void AudioPluginAudioProcessor::InitializeKeybBigSmallTriggers()
{
    // delete any old listeners (in case the tree is reloaded)
    MyPropertyKeybBigSmallListeners.clear();
    juce::String propName = "btnkeyBoard";
    const bool initVal = switchBigSmall.ptrKeyBoardBigSmallSwitch->get();
    auto propValue = magicState.getPropertyAsValue(propName);

    MyPropertyKeybBigSmallListeners.push_back (std::make_unique<PropertyKeybBigSmallListener> (
        propValue,
        [this] (bool newValue)
        {
            if (auto* p = switchBigSmall.ptrKeyBoardBigSmallSwitch) {
                TriggerKeyBoard();
                p->setValueNotifyingHost (newValue ? 1.0f : 0.0f);}
        }));
    propValue.setValue(initVal);
}

void AudioPluginAudioProcessor::InitializeAnalysTrigger()
{
    MyPropertyAnalysButListeners.clear();
    const juce::String propName = "analyseActive";
    const bool initVal = bothAnalysersActiveParam->get();

    auto propVal = magicState.getPropertyAsValue(propName);

    MyPropertyAnalysButListeners.push_back (std::make_unique<PropertyAnalysButListener> (
    propVal,[this] (bool newVal)
    {
        if (juce::AudioParameterBool* p = bothAnalysersActiveParam) {
            p->setValueNotifyingHost (newVal ? 1.0f : 0.0f);
        }
    }));
    propVal.setValue(initVal);
}

void AudioPluginAudioProcessor::InitializeModActiveTriggers()
{
    MyPropertyActiveListeners.clear();
    juce::String propName = "bPActive";
    const bool initVal = glitchActiveParam->get();

    auto propValue = magicState.getPropertyAsValue(propName);

    MyPropertyActiveListeners.push_back (std::make_unique<PropertyActiveListener> (
    propValue,[this] (bool newValue)
    {
        if (auto* p = glitchActiveParam) {
            p->setValueNotifyingHost (newValue ? 1.0f : 0.0f);
        }
    }));
    propValue.setValue(initVal);
}

void AudioPluginAudioProcessor::InitializeSamplerActiveTriggers()
{
    MyPropertySamplerListeners.clear();
    juce::String propName = "sampleActivated";
    const bool initVal = samplerGainParams.isActivated->get();
    auto propValue = magicState.getPropertyAsValue(propName);

    MyPropertySamplerListeners.push_back (std::make_unique<PropertySamplerListener> (
    propValue,[this] (bool newValue)
    {
        if (auto* p = samplerGainParams.isActivated) {
            p->setValueNotifyingHost (newValue ? 1.0f : 0.0f);
        }
    }));
    propValue.setValue(initVal);
}

void AudioPluginAudioProcessor::InitializeSamplerFilterActiveTriggers()
{
    MyPropertySamplerFilterListeners.clear();
    juce::String propName = "sampleFilterActivated";
    const bool initVal = samplerGainParams.isActivated->get();
    auto propValue = magicState.getPropertyAsValue(propName);

    MyPropertySamplerFilterListeners.push_back (std::make_unique<PropertySamplerFilterListener> (
    propValue,[this] (bool newValue)
    {
        if (auto* p = samplerFilterParams.isActivated) {
            p->setValueNotifyingHost (newValue ? 1.0f : 0.0f);
        }
    }));
    propValue.setValue(initVal);
}

void AudioPluginAudioProcessor::InitializeEffectsTriggers()
{
    MyPropertyEffectsListeners.clear();
    for (int i = 0; i < 4; ++i) {
        juce::String propName = "effect" + juce::String(i) + "Active";
        const bool initVal = effekteActiveParams[i].effectIsActive->get();
        auto propValue = magicState.getPropertyAsValue(propName);
        MyPropertyEffectsListeners.push_back (std::make_unique<PropertyEffectsListener> (
            propValue,
            [this, i] (bool newValue)
            {
                if (auto* p = effekteActiveParams[i].effectIsActive) {
                    p->setValueNotifyingHost (newValue ? 1.0f : 0.0f);
                }
            }
        ));
        propValue.setValue(initVal);
    }
}

void AudioPluginAudioProcessor::InitializeTutWindowTriggers()
{
    MyPropertyTutorialListener.clear();
    auto propSave = magicState.getPropertyAsValue ("btnOpenTut");
    propSave.setValue (false);
    MyPropertyTutorialListener.push_back (std::make_unique<PropertyTutorialListener> (propSave, [this] { TriggerTutWindow(); }));
}

void AudioPluginAudioProcessor::InitializeLicenseWindowTriggers()
{
    MyPropertyLicenceListener.clear();
    auto propSave = magicState.getPropertyAsValue ("btnOpenLic");
    propSave.setValue (false);
    MyPropertyLicenceListener.push_back (std::make_unique<PropertyLicenceListener> (propSave, [this] { TriggerLicenseWindow(); }));
}

void AudioPluginAudioProcessor::InitializePresetBarTriggers()
{
    MyPresetButtonListeners.clear();
    //<< button
    auto propPrev = magicState.getPropertyAsValue ("btnPresetPrev");
    propPrev.setValue (false);
    MyPresetButtonListeners.push_back (std::make_unique<PropertyButtonListener> (propPrev, [this] { cyclePreset (-1); }));
    //>> button
    auto propNext = magicState.getPropertyAsValue ("btnPresetNext");
    propNext.setValue (false);
    MyPresetButtonListeners.push_back (std::make_unique<PropertyButtonListener> (propNext, [this] { cyclePreset (1); }));
    // save button
    auto propSave = magicState.getPropertyAsValue ("btnPresetSave");
    propSave.setValue (false);
    MyPresetButtonListeners.push_back (std::make_unique<PropertyButtonListener> (propSave, [this] { triggerSaveFileDialog(); }));
    // load button
    auto propLoad = magicState.getPropertyAsValue ("btnPresetLoad");
    propLoad.setValue (false);
    MyPresetButtonListeners.push_back (std::make_unique<PropertyButtonListener> (propLoad, [this] { triggerLoadFileDialog(); }));

    auto propLoadFact1 = magicState.getPropertyAsValue ("btnFac1");
    propLoadFact1.setValue (false);
    MyPresetButtonListeners.push_back (std::make_unique<PropertyButtonListener> (propLoadFact1, [this] { triggerLoadPreset1(); }));

    auto propLoadFact2 = magicState.getPropertyAsValue ("btnFac2");
    propLoadFact2.setValue (false);
    MyPresetButtonListeners.push_back (std::make_unique<PropertyButtonListener> (propLoadFact2, [this] { triggerLoadPreset2(); }));

    auto propLoadFact3 = magicState.getPropertyAsValue ("btnFac3");
    propLoadFact2.setValue (false);
    MyPresetButtonListeners.push_back (std::make_unique<PropertyButtonListener> (propLoadFact3, [this] { triggerLoadPreset3(); }));
}

void AudioPluginAudioProcessor::InitializeSpectroTriggers()
{
    MyPropertySpectroListeners.clear();
    juce::String propName = "specActivated";
    const bool initVal = spectroParams.spectroIsActiveParam->get();

    auto propVal = magicState.getPropertyAsValue(propName);

    MyPropertySpectroListeners.push_back (std::make_unique<PropertySpectroListener> (
        propVal,
        [this] (bool newValue)
        {
            if (auto* prop = spectroParams.spectroIsActiveParam) {
                prop->setValueNotifyingHost (newValue ? 1.0f : 0.0f);
            }
        }
    ));
    propVal.setValue(initVal);
}

void AudioPluginAudioProcessor::TriggerTutWindow()
{
    juce::String closeFunc = "closeTutorial";
#ifdef _DEBUG
    // if the window is already open, we simply bring it to the foreground
    if (helpTutWindow != nullptr) {
        helpTutWindow->toFront (true);
        return;
    }
    juce::String instructionstext = FileHandling::loadHelpText(HelpTextMode::Instructions);
#else
    juce::String instructionstext = juce::String::createStringFromData (BinaryData::Instructions_Text_txt,
    BinaryData::Instructions_Text_txtSize);
#endif
    juce::String tutorialXml = FileHandling::xmlPartTut;
    auto tutorial_Xml = tutorialXml.replace ("@TEXT@", instructionstext);
    tutorial_Xml = tutorial_Xml.replace ("@CLOSE@", closeFunc);

    const auto xmlTut = juce::XmlDocument::parse (tutorial_Xml);
    if (xmlTut == nullptr) {
        DBG ("XML error  in C++ Raw String!");
        jassertfalse;
        return;
    }
    guiTutNode = juce::ValueTree::fromXml (*xmlTut);
    if (myBuilder == nullptr) {
        DBG ("myBuilder-pointer is null!");
        return;
    }
    pgmContentTut = myBuilder->createGuiItem (guiTutNode);
    if (pgmContentTut != nullptr) {
        //DBG ("pgmContent erfolgreich erzeugt!");
        pgmContentTut->update(); // Zwingt PGM, die Kinder (Text, Button) zu bauen
    } else {
        DBG ("The builder could not generate the element from the C++ XML!");
        jassertfalse;
        return;
    }
    helpTutWindow = std::make_unique<juce::DocumentWindow> ("Tutorial", juce::Colour (0xFF264049), juce::DocumentWindow::allButtons);
    helpTutWindow->setSize (650, 550);
    helpTutWindow->setTopLeftPosition (250, 50);
    helpTutWindow->setColour (juce::DocumentWindow::backgroundColourId, bgColorWindow);
    pgmContentTut->setSize(600, 500);
    helpTutWindow->setContentOwned (pgmContentTut.release(), true);
    helpTutWindow->setVisible (true);
}

void AudioPluginAudioProcessor::TriggerLicenseWindow()
{
    juce::String closeFunc = "closeLicense";
#ifdef _DEBUG
    juce::String licensetext = FileHandling::loadHelpText(HelpTextMode::License);
#else
    juce::String licensetext = juce::String::createStringFromData (BinaryData::License_Text_txt,
    BinaryData::License_Text_txtSize);
#endif
    if (helpLicWindow != nullptr) {
        helpLicWindow->toFront (true);
        return;
    }
    juce::String licenseXml = FileHandling::xmlPartLic;
    auto license_Xml = licenseXml.replace ("@TEXT@", licensetext);
    license_Xml = license_Xml.replace ("@CLOSE@", closeFunc);
    const auto xmlLic = juce::XmlDocument::parse (license_Xml);
    if (xmlLic == nullptr) {
        jassertfalse;
        return;
    }
    // convert the XML into a ValueTree that PGM can read
    guiLicNode = juce::ValueTree::fromXml (*xmlLic);
    if (myBuilder == nullptr) {
        return;
    }
    // have the element generated dynamically via the builder
    pgmContentLic= myBuilder->createGuiItem (guiLicNode);

    if (pgmContentLic != nullptr) {
        pgmContentLic->update(); // Forces PGM to build the children (text, button)
    } else {
        jassertfalse;
        return;
    }
    helpLicWindow = std::make_unique<juce::DocumentWindow> ("License", juce::Colour (0xFF264049), juce::DocumentWindow::allButtons);
    helpLicWindow->setSize (650, 550);
    helpLicWindow->setTopLeftPosition (250, 100);
    helpLicWindow->setColour (juce::DocumentWindow::backgroundColourId, bgColorWindow);
    pgmContentLic->setSize(600, 500);
    helpLicWindow->setContentOwned (pgmContentLic.release(), true);
    helpLicWindow->setVisible (true);
}

void AudioPluginAudioProcessor::InitializePlusTrigger()
{
    MyPropertyPlusButListeners.clear();
    auto propSave = magicState.getPropertyAsValue ("btnPlus");
    propSave.setValue (false);
    MyPropertyPlusButListeners.push_back (std::make_unique<PropertyPlusButListener> (propSave, [this] { setExplicitGuiSize (1.0f); }));
}

void AudioPluginAudioProcessor::InitializeMediumTrigger()
{
    MyPropertyMedButListeners.clear();
    auto propSave = magicState.getPropertyAsValue ("btnMedium");
    propSave.setValue (false);
    MyPropertyMedButListeners.push_back (std::make_unique<PropertyMedButListener> (propSave, [this] { setExplicitGuiSize (0.8f);}));
}

void AudioPluginAudioProcessor::InitializeMinusTrigger()
{
    MyPropertyMinusButListeners.clear();
    auto propSave = magicState.getPropertyAsValue ("btnMinus");
    propSave.setValue (false);
    MyPropertyMinusButListeners.push_back (std::make_unique<PropertyMinusButListener> (propSave, [this] { setExplicitGuiSize (0.7f);; }));
}

// few or many keyboard-keys
void AudioPluginAudioProcessor::TriggerKeyBoard()
{
    ChangeKeyBoard();
}

//= begin serialization ======================================================================
// ── save ────────────────────────────────────────────────
// save preset as an XML file
void AudioPluginAudioProcessor::triggerSaveFileDialog()
{
    // we define the FileChooser (the pointer must be long-lived, either as a member or via std::shared_ptr)
    fileChooser = std::make_unique<juce::FileChooser> (
        "Preset speichern...",
        juce::File::getSpecialLocation (juce::File::userDocumentsDirectory),
        "*.preset"
    );

    fileChooser->launchAsync (juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
        [this] (const juce::FileChooser& chooser)
        {
            auto resultFile = chooser.getResult();
            if (resultFile != juce::File())
            {
                // If the user has forgotten the extension, we append it
                if (resultFile.getFileExtension() != ".preset")
                    resultFile = resultFile.withFileExtension (".preset");

                savePresetToFile (resultFile);
            }
        });
}

void AudioPluginAudioProcessor::triggerLoadFileDialog()
{
    fileChooser = std::make_unique<juce::FileChooser> (
        "Open preset ...",
        juce::File::getSpecialLocation (juce::File::userDocumentsDirectory),
        "*.preset"
    );

    fileChooser->launchAsync (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this] (const juce::FileChooser& chooser)
        {
            auto resultFile = chooser.getResult();
            if (resultFile.existsAsFile())
            {
                // update the list if a new file has been added.
                updatePresetList();

                // check whether the selected file exists in our presetFiles array
                int foundIndex = presetFiles.indexOf (resultFile);

                if (foundIndex != -1)
                {
                    // file is in the preset folder -> Load via index & update label
                    selectPresetByIndex (foundIndex);
                }
                else
                {
                    // the file is located in a completely different place on the disk:
                    loadPresetFromFile (resultFile);
                    currentPresetNameValue.setValue (resultFile.getFileNameWithoutExtension());
                    currentPresetIndex = -1; // -1 indicates: no active index in the default folder
                }
            }
        });
}

void AudioPluginAudioProcessor::triggerLoadPreset1()
{
    LoadFactoryPreset(1);
}

void AudioPluginAudioProcessor::triggerLoadPreset2()
{
    LoadFactoryPreset(2);
}

void AudioPluginAudioProcessor::triggerLoadPreset3()
{
    LoadFactoryPreset(3);
}


void AudioPluginAudioProcessor::LoadFactoryPreset(const int presetNumber)
{
#ifdef _DEBUG
    auto resultFile = FileHandling::loadFactoryPreset(presetNumber);
    if (resultFile != juce::File())
    {
        loadPresetFromFile (resultFile);
        currentFactoryFileName = resultFile.getFileNameWithoutExtension();
        currentPresetNameValue.setValue(currentFactoryFileName);
    }
    else
    {
        DBG("Preset"+juce::String(presetNumber)+" not found!");
    }
#else
    juce::String presetXmlText = "";
    // convert the raw data from RAM into a JUCE string
    if (presetNumber == 1) {
        presetXmlText = juce::String::createStringFromData (
            BinaryData::factory_1_preset,
            BinaryData::factory_1_presetSize
        );
    }
    if (presetNumber == 2) {
        presetXmlText = juce::String::createStringFromData (
            BinaryData::factory_2_preset,
            BinaryData::factory_2_presetSize
        );
    }
    if (presetNumber == 3) {
        presetXmlText = juce::String::createStringFromData (
            BinaryData::factory_3_preset,
            BinaryData::factory_3_presetSize
        );
    }
    auto xmlElement = juce::XmlDocument::parse (presetXmlText);
    if (xmlElement != nullptr) {
        juce::ValueTree presetState = juce::ValueTree::fromXml (*xmlElement);
        if (presetState.isValid() && presetState.hasType ("PRESET_ROOT"))
        {
            // feed all modules sequentially with the loaded tree
            loadWtOscParams (presetState);
            loadWtFilterParams (presetState);
            loadLfoParams (presetState);
            loadMainParams (presetState);
            loadSamplerParams (presetState);
            loadSpectrogramParams (presetState);
            loadEnvModulationParam (presetState);
            loadEffectParam (presetState);
        }
        currentPresetNameValue.setValue("factory."+juce::String(presetNumber));
    }
    else {
        juce::Logger::writeToLog ("Error: Factory preset " +juce::String(presetNumber)+"could not be parsed.");
    }
#endif
}
#pragma  endregion TRIGGER_METHODS

#pragma region SERIALIZATION
void AudioPluginAudioProcessor::savePresetToFile (const juce::File& file) const
{
    // create main root nodes
    juce::ValueTree presetRoot ("PRESET_ROOT");

    // have all modules written to the tree
    saveWtOscParams (presetRoot);
    saveWtFilterParams (presetRoot);
    saveLfoParams (presetRoot);
    SaveMainParams (presetRoot);
    saveSamplerParams (presetRoot);
    saveSpectrogramParams (presetRoot);
    saveEnvModulationParam (presetRoot);
    saveEffectParam (presetRoot);
    saveKeyBoardParam(presetRoot);

    // convert the tree into an XML document
    if (std::unique_ptr<juce::XmlElement> xmlState = presetRoot.createXml()) {
        // write to the hard drive as a text file and check the return value
        if (! xmlState->writeTo (file)) {
            DBG ("Error: Could not write preset file!");
        }
    }
}

// load preset from XML file
void AudioPluginAudioProcessor::loadPresetFromFile (const juce::File& file)
{
    if (! file.existsAsFile()) return;
    // read the file as XML
    if (std::unique_ptr<juce::XmlElement> xmlState = juce::XmlDocument::parse (file))
    {
        // convert the XML back into a ValueTree
        auto presetRoot = juce::ValueTree::fromXml (*xmlState);

        if (presetRoot.isValid() && presetRoot.hasType ("PRESET_ROOT"))
        {
            // feed all modules sequentially with the loaded tree
            loadWtOscParams (presetRoot);
            loadWtFilterParams (presetRoot);
            loadLfoParams (presetRoot);
            loadMainParams (presetRoot);
            loadSamplerParams (presetRoot);
            loadSpectrogramParams (presetRoot);
            loadEnvModulationParam (presetRoot);
            loadEffectParam (presetRoot);
            loadKeyBoardParams(presetRoot);
        }
    }
}
//

void AudioPluginAudioProcessor::saveWtOscParams (juce::ValueTree& parent) const
{
    juce::ValueTree group ("OSCILLATORS");
    for (int i = 0; i < 4; ++i)
    {
        juce::ValueTree node ("OSC");
        node.setProperty ("index", i, nullptr);

        if (synthLabOscParams[i].coreParam) node.setProperty ("coreIndex", synthLabOscParams[i].coreParam->getIndex(), nullptr);
        if (synthLabOscParams[i].waveParam) node.setProperty ("waveIndex", synthLabOscParams[i].waveParam->getIndex(), nullptr);

        if(synthLabOscParams[i].attackGwT) node.setProperty ("attack",  static_cast<float> (*synthLabOscParams[i].attackGwT), nullptr);
        if(synthLabOscParams[i].decayGwT) node.setProperty ("decay",   static_cast<float> (*synthLabOscParams[i].decayGwT), nullptr);
        if(synthLabOscParams[i].sustainGwT) node.setProperty ("sustain", static_cast<float> (*synthLabOscParams[i].sustainGwT), nullptr);
        if(synthLabOscParams[i].releaseGwT) node.setProperty ("release", static_cast<float> (*synthLabOscParams[i].releaseGwT), nullptr);

        if(switchAdrsParams[i].switchGainFilterParam) node.setProperty ("switchGainFilter", static_cast<bool> (switchAdrsParams[i].switchGainFilterParam->get()), nullptr);

        juce::String knobLetters[] = { "A", "B", "C", "D" };
        for (int k = 0; k < 4; ++k)
        {
            juce::String propName = "modKnob" + knobLetters[k];
            if (synthLabOscParams[i].modKnobs[k]) node.setProperty (propName, static_cast<float> (*synthLabOscParams[i].modKnobs[k]), nullptr);
        }
        if (synthLabOscParams[i].volumeWTParams) node.setProperty ("volume",    static_cast<float> (*synthLabOscParams[i].volumeWTParams), nullptr);
        if (synthLabOscParams[i].tuneWTParams) node.setProperty ("tune",      static_cast<float> (*synthLabOscParams[i].tuneWTParams), nullptr);
        if (synthLabOscParams[i].panWTParams) node.setProperty ("pan",       static_cast<float> (*synthLabOscParams[i].panWTParams), nullptr);
        if (synthLabOscParams[i].reserveWTParams) node.setProperty ("reserve",   static_cast<float> (*synthLabOscParams[i].reserveWTParams), nullptr);
        if (synthLabOscParams[i].oscillatorShape) node.setProperty ("oscShape",  static_cast<float> (*synthLabOscParams[i].oscillatorShape), nullptr);
        if (synthLabOscParams[i].isActivated) node.setProperty ("isActivated",  synthLabOscParams[i].isActivated->get(), nullptr);

        group.addChild (node, -1, nullptr);
    }

    parent.addChild (group, -1, nullptr);
}


void AudioPluginAudioProcessor::saveWtFilterParams (juce::ValueTree& parent) const
{
    juce::ValueTree group ("FILTERS"); // Container für alle 4 Filter
    for (int i = 0; i < 4; ++i)
    {
        juce::ValueTree node ("FILTER");

        node.setProperty ("index", i, nullptr);

        if(synthLabOscParams[i].filterTypeParam) node.setProperty ("typeIndex", synthLabOscParams[i].filterTypeParam->getIndex(), nullptr);

        if(synthLabOscParams[i].filterFreq) node.setProperty ("frequency", static_cast<float> (*synthLabOscParams[i].filterFreq), nullptr);
        if(synthLabOscParams[i].filterReso) node.setProperty ("resonance", static_cast<float> (*synthLabOscParams[i].filterReso), nullptr);

        if(synthLabOscParams[i].attackFilterParam) node.setProperty ("attack",  static_cast<float> (*synthLabOscParams[i].attackFilterParam), nullptr); // Hinweis: Tippfehler im User-Code oben bei "decay" korrigiert
        if(synthLabOscParams[i].decayFilterParam) node.setProperty ("decay",   static_cast<float> (*synthLabOscParams[i].decayFilterParam), nullptr);
        if(synthLabOscParams[i].sustainFilterParam) node.setProperty ("sustain", static_cast<float> (*synthLabOscParams[i].sustainFilterParam), nullptr);
        if(synthLabOscParams[i].releaseFilterParam) node.setProperty ("release", static_cast<float> (*synthLabOscParams[i].releaseFilterParam), nullptr);
        if(synthLabOscParams[i].filterEnvelopeAmount) node.setProperty ("envAmount", static_cast<float> (*synthLabOscParams[i].filterEnvelopeAmount), nullptr);
        if(synthLabOscParams[i].filterIsActivated) node.setProperty ("filterIsActivated",  synthLabOscParams[i].filterIsActivated->get(), nullptr);

        group.addChild (node, -1, nullptr);
    }
    parent.addChild (group, -1, nullptr);
}


void AudioPluginAudioProcessor::saveLfoParams(juce::ValueTree& parent) const
{
    juce::ValueTree group ("LFOS"); // container for all LFOs
    for (int i = 0; i < 4; ++i) {
        juce::ValueTree node ("LFO");
        node.setProperty ("index", i, nullptr);
        if (juceLfoFmParams[i].lfoFmWaveFormParam) node.setProperty ("fmWaveIndex", juceLfoFmParams[i].lfoFmWaveFormParam->getIndex(), nullptr);
        if (juceLfoFmParams[i].lfoFmGainParam) node.setProperty ("fmGain",      static_cast<float> (*juceLfoFmParams[i].lfoFmGainParam), nullptr);
        if (juceLfoFmParams[i].lfoFmFreqParam) node.setProperty ("fmFreq",      static_cast<float> (*juceLfoFmParams[i].lfoFmFreqParam), nullptr);
        if (juceLfoFmParams[i].lfoFmDepthParam) node.setProperty ("fmDepth",     static_cast<float> (*juceLfoFmParams[i].lfoFmDepthParam), nullptr);

        if(juceLfoAmParams[i].lfoAmWaveFormParam) node.setProperty ("amWaveIndex", juceLfoAmParams[i].lfoAmWaveFormParam->getIndex(), nullptr);
        if(juceLfoAmParams[i].lfoAmGainParam) node.setProperty ("amGain",      static_cast<float> (*juceLfoAmParams[i].lfoAmGainParam), nullptr);
        if(juceLfoAmParams[i].lfoAmFreqParam) node.setProperty ("amFreq",      static_cast<float> (*juceLfoAmParams[i].lfoAmFreqParam), nullptr);
        if(juceLfoAmParams[i].lfoAmDepthParam) node.setProperty ("amDepth",     static_cast<float> (*juceLfoAmParams[i].lfoAmDepthParam), nullptr);
        group.addChild (node, -1, nullptr);
    }
    parent.addChild (group, -1, nullptr);
}


void AudioPluginAudioProcessor::SaveMainParams(juce::ValueTree& parent) const
{
    juce::ValueTree node ("GLOBAL");

    if(masterVolumeMainParam) node.setProperty ("masterVolume", static_cast<float> (*masterVolumeMainParam), nullptr);

    if(masterEqBass) node.setProperty ("eqBass", static_cast<float> (*masterEqBass), nullptr);
    if(masterEqMid) node.setProperty ("eqMid",  static_cast<float> (*masterEqMid), nullptr);
    if(masterEqHigh) node.setProperty ("eqHigh", static_cast<float> (*masterEqHigh), nullptr);

    parent.addChild (node, -1, nullptr);
}


void AudioPluginAudioProcessor::saveSamplerParams(juce::ValueTree& parent) const
{
    juce::ValueTree node ("SAMPLER");

    if (samplerVolumeParam) node.setProperty("samplVolume", static_cast<float> (*samplerVolumeParam), nullptr);
    if (samplerGainParams.attackSample) node.setProperty ("ampAttack",  static_cast<float> (*samplerGainParams.attackSample), nullptr);
    if (samplerGainParams.decaySample) node.setProperty ("ampDecay",   static_cast<float> (*samplerGainParams.decaySample), nullptr);
    if (samplerGainParams.sustainSample) node.setProperty ("ampSustain", static_cast<float> (*samplerGainParams.sustainSample), nullptr);
    if (samplerGainParams.releaseSample) node.setProperty ("ampRelease", static_cast<float> (*samplerGainParams.releaseSample), nullptr);

    if(samplerFilterParams.filterSampleTypeParam) node.setProperty ("filterTypeIndex", samplerFilterParams.filterSampleTypeParam->getIndex(), nullptr);

    if (samplerFilterParams.sampleFilterFreq) node.setProperty ("filterFrequency", static_cast<float> (*samplerFilterParams.sampleFilterFreq), nullptr);
    if (samplerFilterParams.sampleFilterReso) node.setProperty ("filterResonance", static_cast<float> (*samplerFilterParams.sampleFilterReso), nullptr);

    if (samplerFilterParams.attackFilterSample) node.setProperty ("filterAttack",  static_cast<float> (*samplerFilterParams.attackFilterSample), nullptr);
    if (samplerFilterParams.decayFilterSample) node.setProperty ("filterDecay",   static_cast<float> (*samplerFilterParams.decayFilterSample), nullptr);
    if (samplerFilterParams.sustainFilterSample) node.setProperty ("filterSustain", static_cast<float> (*samplerFilterParams.sustainFilterSample), nullptr);
    if (samplerFilterParams.releaseFilterSample) node.setProperty ("filterRelease", static_cast<float> (*samplerFilterParams.releaseFilterSample), nullptr);

    if (samplerFilterParams.sampleFilterEnvAmount) node.setProperty ("filterEnvAmount", static_cast<float> (*samplerFilterParams.sampleFilterEnvAmount), nullptr);
    if (samplerGainParams.isActivated) node.setProperty ("samplerActive",  static_cast<bool> (samplerGainParams.isActivated->get()), nullptr);

    if (samplParentDir.isNotEmpty()) node.setProperty ("samplParentDir",  static_cast<juce::String> (samplParentDir), nullptr);
    if (samplInstrFolder.isNotEmpty()) node.setProperty ("samplInstrFolder",  static_cast<juce::String> (samplInstrFolder), nullptr);
    if (samplerFilterParams.isActivated) node.setProperty ("samplerFilterActive",  static_cast<bool> (samplerFilterParams.isActivated->get()), nullptr);

    parent.addChild (node, -1, nullptr);
}


void AudioPluginAudioProcessor::saveSpectrogramParams(juce::ValueTree& parent) const
{
    juce::ValueTree node ("SPECTROGRAM");

    if (spectroParams.spectrogramVolumeParam) node.setProperty ("volume", static_cast<float> (*spectroParams.spectrogramVolumeParam), nullptr);

    if (spectroParams.spectrogramPlayTime) node.setProperty ("playTime", static_cast<float> (*spectroParams.spectrogramPlayTime), nullptr);

    if (spectroParams.spectroIsActiveParam) node.setProperty ("specActivated", spectroParams.spectroIsActiveParam->get(), nullptr);

    // save canvas image as PNG memoryBlock ================================================//
    if (canvasImage.isValid())
    {
        juce::MemoryBlock imageBlock;
        juce::MemoryOutputStream stream (imageBlock, false);

        juce::PNGImageFormat pngFormat;
        if (pngFormat.writeImageToStream (canvasImage, stream))
        {
            // JUCE automatically converts this memoryBlock to Base64 within the XML!
            node.setProperty ("canvasData", imageBlock, nullptr);
        }
    }
    //=====================================================================================================//
    parent.addChild (node, -1, nullptr);
}


void AudioPluginAudioProcessor::saveEnvModulationParam(juce::ValueTree& parent) const
{
    juce::ValueTree node ("ENVELOPE_MODULATION");

    if (masterFilterModulat.filterResoMasterModulParam) node.setProperty ("filterReso",      static_cast<float> (*masterFilterModulat.filterResoMasterModulParam), nullptr);
    if (masterFilterModulat.filterTypeParam) node.setProperty("filterTypeModIndex",  masterFilterModulat.filterTypeParam->getIndex(), nullptr);
    if (masterFilterModulat.filterEnvMasterModulParam) node.setProperty ("filterEnvAmount", static_cast<float> (*masterFilterModulat.filterEnvMasterModulParam), nullptr);
    if (masterFilterModulat.filterDryWetMmp) node.setProperty ("filterDryWet",    static_cast<float> (*masterFilterModulat.filterDryWetMmp), nullptr);
    if (masterFilterModulat.envelopeFrequencyP) node.setProperty ("envelopeFreq",    static_cast<float> (*masterFilterModulat.envelopeFrequencyP), nullptr);
    if (masterFilterModulat.wobbleUpperLimitP) node.setProperty ("wobbleLimit",    static_cast<float> (*masterFilterModulat.wobbleUpperLimitP), nullptr);

    if (lfoMastModParam.lfoMasterDepthParam) node.setProperty ("lfoDepth",        static_cast<float> (*lfoMastModParam.lfoMasterDepthParam), nullptr);

    if (glitchActiveParam) node.setProperty ("isActive",        glitchActiveParam->get(), nullptr);

    if (lfoFmWaveFormParamMaster) node.setProperty("fmWaveFormIndex",  lfoFmWaveFormParamMaster->getIndex(), nullptr);
    if (lfoAmWaveFormParamMaster) node.setProperty("amWaveFormIndex",  lfoAmWaveFormParamMaster->getIndex(), nullptr);
    if (fmFreqMaster) node.setProperty ("fmFreqMaster",    static_cast<float> (*fmFreqMaster), nullptr);
    if (fmDepthMaster) node.setProperty ("fmDepthMaster",    static_cast<float> (*fmDepthMaster), nullptr);
    if (amFreqMaster) node.setProperty ("amFreqMaster",    static_cast<float> (*amFreqMaster), nullptr);
    if (amDepthMaster) node.setProperty ("amDepthMaster",    static_cast<float> (*amDepthMaster), nullptr);

    parent.addChild (node, -1, nullptr);
}


void AudioPluginAudioProcessor::saveKeyBoardParam(juce::ValueTree& parent) const
{
    // save the keyboard bog or small size preset
    juce::ValueTree group ("GUI");
    juce::ValueTree keyBoardNode ("KEYB_BIG_SMALL");
    if (switchBigSmall.ptrKeyBoardBigSmallSwitch) keyBoardNode.setProperty("bigOrSmall", switchBigSmall.ptrKeyBoardBigSmallSwitch->get(), nullptr);
    group.addChild (keyBoardNode, -1, nullptr);
    parent.addChild (group, -1, nullptr);
}


void AudioPluginAudioProcessor::saveEffectParam(juce::ValueTree& parent) const
{
    juce::ValueTree group ("EFFECTS");

    juce::ValueTree chorusNode ("CHORUS");
    if (effekteActiveParams[0].effectIsActive) chorusNode.setProperty ("chorusIsActive", effekteActiveParams[0].effectIsActive->get(), nullptr);
    if (chorusRateParam)   chorusNode.setProperty ("rate",        static_cast<float> (*chorusRateParam), nullptr);
    if (chorusDepthParam)  chorusNode.setProperty ("depth",       static_cast<float> (*chorusDepthParam), nullptr);
    if (chorusFeedBack)    chorusNode.setProperty ("feedback",    static_cast<float> (*chorusFeedBack), nullptr);
    if (chorusCentreDelay) chorusNode.setProperty ("centreDelay", static_cast<float> (*chorusCentreDelay), nullptr); // Schutz gegen den Copy-Paste-Teufel!
    if (chorusDryWetParam) chorusNode.setProperty ("dryWet",      static_cast<float> (*chorusDryWetParam), nullptr);
    group.addChild (chorusNode, -1, nullptr);

    juce::ValueTree flangerNode ("FLANGER");
    if (effekteActiveParams[1].effectIsActive) flangerNode.setProperty ("flangerIsActive", effekteActiveParams[1].effectIsActive->get(), nullptr);
    if (flangerRateParam)     flangerNode.setProperty ("rate",        static_cast<float> (*flangerRateParam), nullptr);
    if (flangerDepthParam)    flangerNode.setProperty ("depth",       static_cast<float> (*flangerDepthParam), nullptr);
    if (flangerFeedBackParam) flangerNode.setProperty ("feedback",    static_cast<float> (*flangerFeedBackParam), nullptr);
    if (flangerCentreDelay)   flangerNode.setProperty ("centreDelay", static_cast<float> (*flangerCentreDelay), nullptr);
    if (flangerDryWetParam)   flangerNode.setProperty ("dryWet",      static_cast<float> (*flangerDryWetParam), nullptr);
    group.addChild (flangerNode, -1, nullptr);

    juce::ValueTree delayNode ("DELAY");
    if (effekteActiveParams[2].effectIsActive) delayNode.setProperty ("delayIsActive", effekteActiveParams[2].effectIsActive->get(), nullptr);
    if (delayMaster)        delayNode.setProperty ("master",   static_cast<float> (*delayMaster), nullptr);
    if (delayTimeMsParam)   delayNode.setProperty ("timeMs",   static_cast<float> (*delayTimeMsParam), nullptr);
    if (delayMix)           delayNode.setProperty ("mix",      static_cast<float> (*delayMix), nullptr);
    if (delayFeedback)      delayNode.setProperty ("feedback", static_cast<float> (*delayFeedback), nullptr);
    group.addChild (delayNode, -1, nullptr);

    juce::ValueTree reverbNode ("REVERB");
    if (effekteActiveParams[3].effectIsActive) reverbNode.setProperty ("reverbIsActive", effekteActiveParams[3].effectIsActive->get(), nullptr);
    if (reverbRoomParam)    reverbNode.setProperty ("room",    static_cast<float> (*reverbRoomParam), nullptr);
    if (reverbDampingParam) reverbNode.setProperty ("damping", static_cast<float> (*reverbDampingParam), nullptr);
    if (reverbDryWetParam)  reverbNode.setProperty ("dryWet",  static_cast<float> (*reverbDryWetParam), nullptr);
    group.addChild (reverbNode, -1, nullptr);

    parent.addChild (group, -1, nullptr);
}
//==============================================================================
// ── LOAD ────────────────────────────────────────────────────
void AudioPluginAudioProcessor::loadWtOscParams (const juce::ValueTree& parent)
{
    // search for the group in the tree – if it does not exist (old preset), cancel safely
    juce::ValueTree group = parent.getChildWithName ("OSCILLATORS");
    if (! group.isValid()) return;

    for (const auto& node : group)
    {
        if (! node.hasType ("OSC")) continue;

        const int i = node.getProperty ("index", -1);

        if (i < 0 || i >= 4) continue;

        // Restore choice parameters (casts to 'int')
        if (node.hasProperty ("coreIndex"))
            *synthLabOscParams[i].coreParam = static_cast<int> (node.getProperty ("coreIndex"));

        if (node.hasProperty ("waveIndex"))
            *synthLabOscParams[i].waveParam = static_cast<int> (node.getProperty ("waveIndex"));

        // ...
        if (node.hasProperty ("attack"))
            *synthLabOscParams[i].attackGwT = static_cast<float> (node.getProperty ("attack"));

        if (node.hasProperty ("decay"))
            *synthLabOscParams[i].decayGwT = static_cast<float> (node.getProperty ("decay"));

        if (node.hasProperty ("sustain"))
            *synthLabOscParams[i].sustainGwT = static_cast<float> (node.getProperty ("sustain"));

        if (node.hasProperty ("release"))
            *synthLabOscParams[i].releaseGwT = static_cast<float> (node.getProperty ("release"));

        if (node.hasProperty ("attack"))
            *synthLabOscParams[i].attackGwT = static_cast<float> (node.getProperty ("attack"));

        if (node.hasProperty ("switchGainFilter"))
            *switchAdrsParams[i].switchGainFilterParam = static_cast<bool> (node.getProperty ("switchGainFilter"));

        // dynamically read the 4 modulation knobs (A, B, C, D)
        juce::String knobLetters[] = { "A", "B", "C", "D" };
        for (int k = 0; k < 4; ++k)
        {
            juce::String propName = "modKnob" + knobLetters[k];
            if (node.hasProperty (propName))
                *synthLabOscParams[i].modKnobs[k] = static_cast<float> (node.getProperty (propName));
        }

        if (node.hasProperty ("volume"))
            *synthLabOscParams[i].volumeWTParams = static_cast<float> (node.getProperty ("volume"));

        if (node.hasProperty ("tune"))
            *synthLabOscParams[i].tuneWTParams = static_cast<float> (node.getProperty ("tune"));

        if (node.hasProperty ("pan"))
            *synthLabOscParams[i].panWTParams = static_cast<float> (node.getProperty ("pan"));

        if (node.hasProperty ("reserve"))
            *synthLabOscParams[i].reserveWTParams = static_cast<float> (node.getProperty ("reserve"));

        if (node.hasProperty ("oscShape"))
            *synthLabOscParams[i].oscillatorShape = static_cast<float> (node.getProperty ("oscShape"));

        if (node.hasProperty ("isActivated"))
        {
            // convert juce::var to bool from XML in a type-safe manner
            const auto val = node.getProperty ("isActivated");
            bool isAct = false;

            if (val.isBool())
                isAct = static_cast<bool> (val);
            else if (val.isInt() || val.isInt64())
                isAct = (static_cast<int> (val) != 0);
            else
                isAct = (val.toString() == "1" || val.toString().equalsIgnoreCase ("true"));

            // set the C++ AudioParameter
            if (auto* p = synthLabOscParams[i].isActivated)
                p->setValueNotifyingHost (isAct ? 1.0f : 0.0f);

            // update the property in magicState (controls the PGM button in the GUI)
            juce::String propName = "wt" + juce::String (i) + "Active";
            magicState.getPropertyAsValue (propName).setValue (isAct);
            if (synthLabOscParams[i].isActivated->get() == false)
            {
                *synthLabOscParams[i].volumeWTParams = -58.0f;
            }
        }
    }
}


void AudioPluginAudioProcessor::loadWtFilterParams (const juce::ValueTree& parent)
{
    juce::ValueTree group = parent.getChildWithName ("FILTERS");
    if (! group.isValid()) return;

    for (const auto& node : group)
    {
        if (! node.hasType ("FILTER")) continue;

        const int i = node.getProperty ("index", -1);
        if (i < 0 || i >= 4) continue;

        if (node.hasProperty ("typeIndex"))
            *synthLabOscParams[i].filterTypeParam = static_cast<int> (node.getProperty ("typeIndex"));

        if (node.hasProperty ("frequency"))
            *synthLabOscParams[i].filterFreq = static_cast<float> (node.getProperty ("frequency"));

        if (node.hasProperty ("resonance"))
            *synthLabOscParams[i].filterReso = static_cast<float> (node.getProperty ("resonance"));

        if (node.hasProperty ("attack"))
            *synthLabOscParams[i].attackFilterParam = static_cast<float> (node.getProperty ("attack"));

        if (node.hasProperty ("decay"))
            *synthLabOscParams[i].decayFilterParam = static_cast<float> (node.getProperty ("decay"));

        if (node.hasProperty ("sustain"))
            *synthLabOscParams[i].sustainFilterParam = static_cast<float> (node.getProperty ("sustain"));

        if (node.hasProperty ("release"))
            *synthLabOscParams[i].releaseFilterParam = static_cast<float> (node.getProperty ("release"));

        if (node.hasProperty ("envAmount"))
            *synthLabOscParams[i].filterEnvelopeAmount = static_cast<float> (node.getProperty ("envAmount"));

        if (node.hasProperty ("filterIsActivated"))
        {
            const auto val = node.getProperty ("filterIsActivated");
            bool isAct = false;

            if (val.isBool())
                isAct = static_cast<bool> (val);
            else if (val.isInt() || val.isInt64())
                isAct = (static_cast<int> (val) != 0);
            else
                isAct = (val.toString() == "1" || val.toString().equalsIgnoreCase ("true"));

            if (auto* p = synthLabOscParams[i].filterIsActivated)
                p->setValueNotifyingHost (isAct ? 1.0f : 0.0f);

            juce::String propName = "wt" + juce::String (i) + "FilterActivated";
            magicState.getPropertyAsValue (propName).setValue (isAct);
        }
    }
}


void AudioPluginAudioProcessor::loadLfoParams(const juce::ValueTree& parent)
{
    juce::ValueTree group = parent.getChildWithName ("LFOS");
    if (! group.isValid()) return;

    for (const auto& node : group)
    {
        if (! node.hasType ("LFO")) continue;

        const int i = node.getProperty ("index", -1);
        if (i < 0 || i >= 4) continue;

        if (node.hasProperty ("fmWaveIndex"))
            *juceLfoFmParams[i].lfoFmWaveFormParam = static_cast<int> (node.getProperty ("fmWaveIndex"));

        if (node.hasProperty ("fmGain"))
            *juceLfoFmParams[i].lfoFmGainParam = static_cast<float> (node.getProperty ("fmGain"));

        if (node.hasProperty ("fmFreq"))
            *juceLfoFmParams[i].lfoFmFreqParam = static_cast<float> (node.getProperty ("fmFreq"));

        if (node.hasProperty ("fmDepth"))
            *juceLfoFmParams[i].lfoFmDepthParam = static_cast<float> (node.getProperty ("fmDepth"));

        if (node.hasProperty ("amWaveIndex"))
            *juceLfoAmParams[i].lfoAmWaveFormParam = static_cast<int> (node.getProperty ("amWaveIndex"));

        if (node.hasProperty ("amGain"))
            *juceLfoAmParams[i].lfoAmGainParam = static_cast<float> (node.getProperty ("amGain"));

        if (node.hasProperty ("amFreq"))
            *juceLfoAmParams[i].lfoAmFreqParam = static_cast<float> (node.getProperty ("amFreq"));

        if (node.hasProperty ("amDepth"))
            *juceLfoAmParams[i].lfoAmDepthParam = static_cast<float> (node.getProperty ("amDepth"));
    }
}


void AudioPluginAudioProcessor::loadMainParams( const juce::ValueTree& parent)
{
    auto node = parent.getChildWithName ("GLOBAL");
    if (! node.isValid()) return;

    if (node.hasProperty ("masterVolume"))
        *masterVolumeMainParam = static_cast<float> (node.getProperty ("masterVolume"));

    if (node.hasProperty ("eqBass"))
        *masterEqBass = static_cast<float> (node.getProperty ("eqBass"));

    if (node.hasProperty ("eqMid"))
        *masterEqMid = static_cast<float> (node.getProperty ("eqMid"));

    if (node.hasProperty ("eqHigh"))
        *masterEqHigh = static_cast<float> (node.getProperty ("eqHigh"));
}


void AudioPluginAudioProcessor::loadSamplerParams(const juce::ValueTree& parent)
{
    auto node = parent.getChildWithName ("SAMPLER");
    if (! node.isValid()) return;

    if (node.hasProperty ("samplVolume"))
        *samplerVolumeParam = static_cast<float> (node.getProperty ("samplVolume"));


    if (node.hasProperty ("ampAttack"))
        *samplerGainParams.attackSample = static_cast<float> (node.getProperty ("ampAttack"));

    if (node.hasProperty ("ampDecay"))
        *samplerGainParams.decaySample = static_cast<float> (node.getProperty ("ampDecay"));

    if (node.hasProperty ("ampSustain"))
        *samplerGainParams.sustainSample = static_cast<float> (node.getProperty ("ampSustain"));

    if (node.hasProperty ("ampRelease"))
        *samplerGainParams.releaseSample = static_cast<float> (node.getProperty ("ampRelease"));

    if (node.hasProperty ("filterTypeIndex"))
        *samplerFilterParams.filterSampleTypeParam = static_cast<int> (node.getProperty ("filterTypeIndex"));

    if (node.hasProperty ("filterFrequency"))
        *samplerFilterParams.sampleFilterFreq = static_cast<float> (node.getProperty ("filterFrequency"));

    if (node.hasProperty ("filterResonance"))
        *samplerFilterParams.sampleFilterReso = static_cast<float> (node.getProperty ("filterResonance"));

    if (node.hasProperty ("filterAttack"))
        *samplerFilterParams.attackFilterSample = static_cast<float> (node.getProperty ("filterAttack"));

    if (node.hasProperty ("filterDecay"))
        *samplerFilterParams.decayFilterSample = static_cast<float> (node.getProperty ("filterDecay"));

    if (node.hasProperty ("filterSustain"))
        *samplerFilterParams.sustainFilterSample = static_cast<float> (node.getProperty ("filterSustain"));

    if (node.hasProperty ("filterRelease"))
        *samplerFilterParams.releaseFilterSample = static_cast<float> (node.getProperty ("filterRelease"));

    if (node.hasProperty ("filterEnvAmount"))
        *samplerFilterParams.sampleFilterEnvAmount = static_cast<float> (node.getProperty ("filterEnvAmount"));

    if (node.hasProperty ("samplerActive"))
    {
        const auto val = node.getProperty ("samplerActive");
        bool isAct = false;

        if (val.isBool())
            isAct = static_cast<bool> (val);
        else if (val.isInt() || val.isInt64())
            isAct = (static_cast<int> (val) != 0);
        else
            isAct = (val.toString() == "1" || val.toString().equalsIgnoreCase ("true"));

        if (auto* p = samplerGainParams.isActivated)
            p->setValueNotifyingHost (isAct ? 1.0f : 0.0f);

        juce::String propName = "sampleActivated";
        magicState.getPropertyAsValue (propName).setValue (isAct);
    }

    if (node.hasProperty ("samplerFilterActive"))
    {
        const auto val = node.getProperty ("samplerFilterActive");
        bool isAct = false;

        if (val.isBool())
            isAct = static_cast<bool> (val);
        else if (val.isInt() || val.isInt64())
            isAct = (static_cast<int> (val) != 0);
        else
            isAct = (val.toString() == "1" || val.toString().equalsIgnoreCase ("true"));

        if (auto* p = samplerFilterParams.isActivated)
            p->setValueNotifyingHost (isAct ? 1.0f : 0.0f);

        juce::String propName = "sampleFilterActivated";
        magicState.getPropertyAsValue (propName).setValue (isAct);
    }

    // read folder names and pass them to the magic state (triggers the PropertySamplerBrowseListener).
    if (node.hasProperty ("samplParentDir"))
    {
        samplParentDir = node.getProperty ("samplParentDir").toString();
        magicState.getPropertyAsValue ("samplParentDir").setValue (samplParentDir);
    }

    if (node.hasProperty ("samplInstrFolder"))
    {
        samplInstrFolder = node.getProperty ("samplInstrFolder").toString();
        magicState.getPropertyAsValue ("samplInstrFolder").setValue (samplInstrFolder);
    }

    // update audio engine (load samples from hard drive)
    if (samplParentDir.isNotEmpty() && samplInstrFolder.isNotEmpty())
    {
        auto instrumentFolder = getSampleBaseDir()
                                    .getChildFile (samplParentDir)
                                    .getChildFile (samplInstrFolder);
        if (instrumentFolder.isDirectory()) {
            loadSampleFromFile (instrumentFolder);
        }
    }
}


void AudioPluginAudioProcessor::loadSpectrogramParams(const juce::ValueTree& parent)
{
    auto node = parent.getChildWithName ("SPECTROGRAM");
    if (! node.isValid()) return;

    if (node.hasProperty ("volume"))
        *spectroParams.spectrogramVolumeParam = static_cast<float> (node.getProperty ("volume"));

    if (node.hasProperty ("playTime"))
        *spectroParams.spectrogramPlayTime = static_cast<float> (node.getProperty ("playTime"));

    if (node.hasProperty ("specActivated"))
    {
        const auto val = node.getProperty ("specActivated");
        bool isAct = false;

        if (val.isBool())
            isAct = static_cast<bool> (val);
        else if (val.isInt() || val.isInt64())
            isAct = (static_cast<int> (val) != 0);
        else
            isAct = (val.toString() == "1" || val.toString().equalsIgnoreCase ("true"));

        if (auto* p = spectroParams.spectroIsActiveParam)
            p->setValueNotifyingHost (isAct ? 1.0f : 0.0f);

        juce::String propName = "specActivated";
        magicState.getPropertyAsValue (propName).setValue (isAct);
    }
    // --- read the canvas image from the XML ---
    if (node.hasProperty ("canvasData"))
    {
        auto varData = node.getProperty ("canvasData");
        // check whether binary data is present
        if (auto* block = varData.getBinaryData())
        {
            juce::MemoryInputStream stream (block->getData(), block->getSize(), false);

            juce::PNGImageFormat pngFormat;
            juce::Image loadedImage = pngFormat.decodeImage (stream);

            if (loadedImage.isValid()){
                // update local image
                canvasImage = loadedImage;
                // IMPORTANT: update synthesis & UI regarding the new image
                updateAudioAndUIFromLoadedImage (canvasImage);
            }
        }
    }
}


void AudioPluginAudioProcessor::loadEnvModulationParam(const juce::ValueTree& parent)
{
    auto node = parent.getChildWithName ("ENVELOPE_MODULATION");
    if (! node.isValid()) return;

    if (node.hasProperty ("filterReso"))
        *masterFilterModulat.filterResoMasterModulParam = static_cast<float> (node.getProperty ("filterReso"));

    if (node.hasProperty ("filterTypeModIndex"))
        *masterFilterModulat.filterTypeParam = static_cast<int> (node.getProperty ("filterTypeModIndex"));

    if (node.hasProperty ("filterEnvAmount"))
        *masterFilterModulat.filterEnvMasterModulParam = static_cast<float> (node.getProperty ("filterEnvAmount"));

    if (node.hasProperty ("filterDryWet"))
        *masterFilterModulat.filterDryWetMmp = static_cast<float> (node.getProperty ("filterDryWet"));

    if (node.hasProperty ("envelopeFreq"))
        *masterFilterModulat.envelopeFrequencyP = static_cast<float> (node.getProperty ("envelopeFreq"));

    if (node.hasProperty ("wobbleLimit"))
        *masterFilterModulat.wobbleUpperLimitP = static_cast<float> (node.getProperty ("wobbleLimit"));

    if (node.hasProperty ("lfoDepth"))
        *lfoMastModParam.lfoMasterDepthParam = static_cast<float> (node.getProperty ("lfoDepth"));

    if (node.hasProperty ("isActive"))
    {
        const auto val = node.getProperty ("isActive");
        bool isAct = false;

        if (val.isBool())
            isAct = static_cast<bool> (val);
        else if (val.isInt() || val.isInt64())
            isAct = (static_cast<int> (val) != 0);
        else
            isAct = (val.toString() == "1" || val.toString().equalsIgnoreCase ("true"));

        if (auto* p = glitchActiveParam)
            p->setValueNotifyingHost (isAct ? 1.0f : 0.0f);

        juce::String propName = "bPActive";
        magicState.getPropertyAsValue (propName).setValue (isAct);
    }

    if (node.hasProperty ("fmWaveFormIndex"))
        *lfoFmWaveFormParamMaster = static_cast<int> (node.getProperty ("fmWaveFormIndex"));

    if (node.hasProperty ("amWaveFormIndex"))
        *lfoAmWaveFormParamMaster = static_cast<int> (node.getProperty ("amWaveFormIndex"));

    if (node.hasProperty ("fmFreqMaster"))
        *fmFreqMaster = static_cast<float> (node.getProperty ("fmFreqMaster"));

    if (node.hasProperty ("fmDepthMaster"))
        *fmDepthMaster = static_cast<float> (node.getProperty ("fmDepthMaster"));

    if (node.hasProperty ("amFreqMaster"))
        *amFreqMaster = static_cast<float> (node.getProperty ("amFreqMaster"));

    if (node.hasProperty ("amDepthMaster"))
        *amDepthMaster = static_cast<float> (node.getProperty ("amDepthMaster"));
}


void AudioPluginAudioProcessor::loadEffectParam(const juce::ValueTree& parent)
{
    auto group = parent.getChildWithName ("EFFECTS");
    if (! group.isValid()) return;

    auto chorusNode = group.getChildWithName ("CHORUS");
    if (chorusNode.isValid())
    {
        if (chorusNode.hasProperty ("chorusIsActive"))
        {
            const auto val = chorusNode.getProperty ("chorusIsActive");
            bool isAct = false;

            if (val.isBool())
                isAct = static_cast<bool> (val);
            else if (val.isInt() || val.isInt64())
                isAct = (static_cast<int> (val) != 0);
            else
                isAct = (val.toString() == "1" || val.toString().equalsIgnoreCase ("true"));
            if (auto* p = effekteActiveParams[0].effectIsActive)
                p->setValueNotifyingHost (isAct ? 1.0f : 0.0f);
            juce::String propName = "effect0Active";
            magicState.getPropertyAsValue (propName).setValue (isAct);
        }

        if (chorusNode.hasProperty ("rate"))        *chorusRateParam = static_cast<float> (chorusNode.getProperty ("rate"));
        if (chorusNode.hasProperty ("depth"))       *chorusDepthParam = static_cast<float> (chorusNode.getProperty ("depth"));
        if (chorusNode.hasProperty ("feedback"))    *chorusFeedBack = static_cast<float> (chorusNode.getProperty ("feedback"));
        if (chorusNode.hasProperty ("centreDelay")) *chorusCentreDelay = static_cast<float> (chorusNode.getProperty ("centreDelay")); // Mit korrigierter Variable
        if (chorusNode.hasProperty ("dryWet"))      *chorusDryWetParam = static_cast<float> (chorusNode.getProperty ("dryWet"));
    }

    auto flangerNode = group.getChildWithName ("FLANGER");
    if (flangerNode.isValid())
    {
        if (flangerNode.hasProperty ("flangerIsActive"))
        {
            const auto val = flangerNode.getProperty ("flangerIsActive");
            bool isAct = false;

            if (val.isBool())
                isAct = static_cast<bool> (val);
            else if (val.isInt() || val.isInt64())
                isAct = (static_cast<int> (val) != 0);
            else
                isAct = (val.toString() == "1" || val.toString().equalsIgnoreCase ("true"));
            if (auto* p = effekteActiveParams[1].effectIsActive)
                p->setValueNotifyingHost (isAct ? 1.0f : 0.0f);
            juce::String propName = "effect1Active";
            magicState.getPropertyAsValue (propName).setValue (isAct);
        }

        if (flangerNode.hasProperty ("rate"))        *flangerRateParam = static_cast<float> (flangerNode.getProperty ("rate"));
        if (flangerNode.hasProperty ("depth"))       *flangerDepthParam = static_cast<float> (flangerNode.getProperty ("depth"));
        if (flangerNode.hasProperty ("feedback"))    *flangerFeedBackParam = static_cast<float> (flangerNode.getProperty ("feedback"));
        if (flangerNode.hasProperty ("centreDelay")) *flangerCentreDelay = static_cast<float> (flangerNode.getProperty ("centreDelay"));
        if (flangerNode.hasProperty ("dryWet"))      *flangerDryWetParam = static_cast<float> (flangerNode.getProperty ("dryWet"));
    }

    auto delayNode = group.getChildWithName ("DELAY");
    if (delayNode.isValid())
    {
        if (delayNode.hasProperty ("delayIsActive"))
        {
            const auto val = delayNode.getProperty ("delayIsActive");
            bool isAct = false;

            if (val.isBool())
                isAct = static_cast<bool> (val);
            else if (val.isInt() || val.isInt64())
                isAct = (static_cast<int> (val) != 0);
            else
                isAct = (val.toString() == "1" || val.toString().equalsIgnoreCase ("true"));
            if (auto* p = effekteActiveParams[2].effectIsActive)
                p->setValueNotifyingHost (isAct ? 1.0f : 0.0f);
            juce::String propName = "effect2Active";
            magicState.getPropertyAsValue (propName).setValue (isAct);
        }
        if (delayNode.hasProperty ("master"))   *delayMaster = static_cast<float> (delayNode.getProperty ("master"));
        if (delayNode.hasProperty ("timeMs"))   *delayTimeMsParam = static_cast<float> (delayNode.getProperty ("timeMs"));
        if (delayNode.hasProperty ("mix"))      *delayMix = static_cast<float> (delayNode.getProperty ("mix"));
        if (delayNode.hasProperty ("feedback")) *delayFeedback = static_cast<float> (delayNode.getProperty ("feedback"));
    }

    auto reverbNode = group.getChildWithName ("REVERB");
    if (reverbNode.isValid())
    {
        if (reverbNode.hasProperty ("reverbIsActive"))
        {
            const auto val = reverbNode.getProperty ("reverbIsActive");
            bool isAct = false;

            if (val.isBool())
                isAct = static_cast<bool> (val);
            else if (val.isInt() || val.isInt64())
                isAct = (static_cast<int> (val) != 0);
            else
                isAct = (val.toString() == "1" || val.toString().equalsIgnoreCase ("true"));
            if (auto* p = effekteActiveParams[3].effectIsActive)
                p->setValueNotifyingHost (isAct ? 1.0f : 0.0f);
            juce::String propName = "effect3Active";
            magicState.getPropertyAsValue (propName).setValue (isAct);
        }

        if (reverbNode.hasProperty ("room"))     *reverbRoomParam = static_cast<float> (reverbNode.getProperty ("room"));
        if (reverbNode.hasProperty ("damping"))  *reverbDampingParam = static_cast<float> (reverbNode.getProperty ("damping"));
        if (reverbNode.hasProperty ("dryWet"))   *reverbDryWetParam = static_cast<float> (reverbNode.getProperty ("dryWet"));
    }
}


void AudioPluginAudioProcessor::loadKeyBoardParams (const juce::ValueTree& parent)
{
    // loads the keyboard bog or small size preset
    juce::ValueTree group = parent.getChildWithName ("GUI");
    if (! group.isValid()) return;
    auto keyBoardNode = group.getChildWithName ("KEYB_BIG_SMALL");
    if (keyBoardNode.isValid())
    {
       if (keyBoardNode.hasProperty("bigOrSmall"))
       {
           const auto val = keyBoardNode.getProperty ("bigOrSmall");
           bool isAct = false;

           if (val.isBool())
               isAct = static_cast<bool> (val);
           else if (val.isInt() || val.isInt64())
               isAct = (static_cast<int> (val) != 0);
           else
               isAct = (val.toString() == "1" || val.toString().equalsIgnoreCase ("true"));

           if (auto* p = switchBigSmall.ptrKeyBoardBigSmallSwitch)
               p->setValueNotifyingHost (isAct ? 1.0f : 0.0f);

           juce::String propName = "btnkeyBoard";
           magicState.getPropertyAsValue (propName).setValue (isAct);
       }
    }
}


void AudioPluginAudioProcessor::updatePresetList()
{
    presetFiles.clear();
    // scans the folder for all .preset files
    presetFiles = presetDirectory.findChildFiles (juce::File::findFiles, false, "*.preset");
    // try to locate the currently loaded preset in the new list
    if (currentPresetIndex >= 0 && presetFiles.size() > 0)
    {
        // if the index has shifted due to deletion or addition
        currentPresetIndex = juce::jlimit (0, presetFiles.size() - 1, currentPresetIndex);
    }
}

void AudioPluginAudioProcessor::loadMostRecentPreset()
{
    // update the list if the user has moved files in Explorer while the application is running
    updatePresetList();
    if (presetFiles.isEmpty()) {
        currentPresetNameValue.setValue ("Keine Presets gefunden");
        currentPresetIndex = -1;
        return;
    }
    auto newestIt = std::max_element (presetFiles.begin(), presetFiles.end(),
        [] (const juce::File& a, const juce::File& b) {
            return a.getLastModificationTime() < b.getLastModificationTime();
        });
    if (newestIt != presetFiles.end())
    {
        // calculate the index of the found iterator in the array
        int index = static_cast<int> (std::distance (presetFiles.begin(), newestIt));
        // activate the preset and set the label
        selectPresetByIndex (index);
    }
}


void AudioPluginAudioProcessor::selectPresetByIndex (int index)
{
    if (index < 0 || index >= presetFiles.size())
        return;
    // define index & file
    currentPresetIndex = index;
    juce::File targetPreset = presetFiles[currentPresetIndex];
    // apply to the hard drive/processor
    loadPresetFromFile (targetPreset);
    // update GUI label (without the .preset extension)
    currentPresetNameValue.setValue (targetPreset.getFileNameWithoutExtension());
}


void AudioPluginAudioProcessor::cyclePreset (int direction)
{
    updatePresetList();
    if (presetFiles.isEmpty())
    {
        currentPresetNameValue.setValue ("No presets found");
        return;
    }
    currentPresetIndex += direction;
    // overflow protection (wrap-around)
    if (currentPresetIndex >= presetFiles.size())
        currentPresetIndex = 0;
    if (currentPresetIndex < 0)
        currentPresetIndex = presetFiles.size() - 1;
    // call central (preset)logic
    selectPresetByIndex (currentPresetIndex);
}
//=== end serialization ===============================================
// ======================================================================
#pragma endregion SERIALIZATION

#pragma region SAMPLING_METHODS
//========== Sampling ===================================================
void AudioPluginAudioProcessor::loadSampleFromFile (const juce::File& sampleSource)
{
    if (!sampleSource.exists()) return;

    // update state values for preset persistence
    if (sampleSource.existsAsFile() && sampleSource.getFileExtension() == ".sfz")
    {
        samplParentDir   = sampleSource.getParentDirectory().getParentDirectory().getFileNameWithoutExtension();
        samplInstrFolder = sampleSource.getParentDirectory().getFileNameWithoutExtension();
        samplFile        = sampleSource.getFileName();
    }
    else if (sampleSource.isDirectory())
    {
        samplParentDir   = sampleSource.getParentDirectory().getFileNameWithoutExtension();
        samplInstrFolder = sampleSource.getFileNameWithoutExtension();
        samplFile        = "";
    }

    magicState.getPropertyAsValue ("samplParentDir").setValue (samplParentDir);
    magicState.getPropertyAsValue ("samplInstrFolder").setValue (samplInstrFolder);
    magicState.getPropertyAsValue ("samplFile").setValue (samplFile);

    // clean up the previous loading thread if one is still running.
    if (sampleLoadingThread.joinable())
        sampleLoadingThread.detach();

    // Run I/O entirely in the background
    sampleLoadingThread = std::thread ([this, sampleSource]()
    {
        loadSampleSourceInBackground (sampleSource);
    });
}

void AudioPluginAudioProcessor::loadSfzPresetToBuffer (const juce::File& sfzFile,
                                                        juce::Array<juce::SynthesiserSound::Ptr>& targetArray)
{
    if (!sfzFile.existsAsFile()) return;

    juce::File rootDir = findLibraryRoot (sfzFile);
    std::map<juce::String, juce::String> defines;

    auto instantiateRegion = [&] (const SfzParseState& state, const juce::String& samplePath)
    {
        if (samplePath.isEmpty()) return;

        juce::String fullRelPath = state.defaultPath + samplePath;
        juce::File audioFile = findPathCaseInsensitive (rootDir, fullRelPath);

        if (!audioFile.existsAsFile())
            audioFile = findPathCaseInsensitive (rootDir, samplePath);

        if (!audioFile.existsAsFile())
            audioFile = findPathCaseInsensitive (sfzFile.getParentDirectory(), samplePath);

        if (audioFile.existsAsFile())
        {
            std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (audioFile));
            if (reader != nullptr)
            {
                juce::AudioBuffer<float> tempBuffer (static_cast<int> (reader->numChannels),
                                                     static_cast<int> (reader->lengthInSamples));
                reader->read (&tempBuffer, 0, static_cast<int> (reader->lengthInSamples), 0, true, true);

                if (state.volumeDb != 0.0f)
                    tempBuffer.applyGain (juce::Decibels::decibelsToGain (state.volumeDb));

                auto* newSound = new SamplerZoneSound (state.rootNote, std::move (tempBuffer), reader->sampleRate);
                newSound->minMidiNote = state.loKey;
                newSound->maxMidiNote = state.hiKey;

                // determine loop status:
                bool shouldLoop = state.isLooping;
                int lStart = state.loopStart;
                int lEnd   = state.loopEnd;

                // if no loop was defined in the SFZ: read WAV metadata (smpl chunk):
                if (!shouldLoop)
                {
                    const auto& metadata = reader->metadataValues;
                    int numLoops = metadata.getValue ("NumSampleLoops", "0").getIntValue();

                    if (numLoops > 0)
                    {
                        shouldLoop = true;
                        lStart = metadata.getValue ("Loop0Start", "0").getIntValue();
                        lEnd   = metadata.getValue ("Loop0End",   "0").getIntValue();
                    }
                }

                newSound->isLooping = shouldLoop;
                newSound->loopStart = lStart;
                newSound->loopEnd   = (lEnd > lStart) ? lEnd : static_cast<int> (reader->lengthInSamples);

                targetArray.add (newSound);
            }
        }
    };

    // recursive parsing with a three-level state hierarchy
    std::function<void(const juce::File&, SfzParseState)> parseSfzRecursive =
        [&] (const juce::File& fileToParse, SfzParseState inheritedState)
    {
        if (!fileToParse.existsAsFile()) return;
        juce::StringArray lines;
        fileToParse.readLines (lines);
        SfzParseState globalState        = inheritedState;
        SfzParseState groupState         = globalState;
        SfzParseState currentRegionState = groupState;
        juce::String currentSamplePath   = "";
        bool inGroup                     = false;
        bool inRegion                    = false;

        auto flushRegion = [&]()
        {
            if (inRegion && currentSamplePath.isNotEmpty()) {
                instantiateRegion (currentRegionState, currentSamplePath);
            }
            currentRegionState = groupState;
            currentSamplePath  = "";
            inRegion           = false;
        };
        // helper lambda for sorting macros by length (prevents substring replacements)
        auto getSortedDefines = [&]()
        {
            std::vector<std::pair<juce::String, juce::String>> sorted (defines.begin(), defines.end());
            std::sort (sorted.begin(), sorted.end(),
                       [](const auto& a, const auto& b) { return a.first.length() > b.first.length(); });
            return sorted;
        };
        for (auto rawLine : lines)
        {
            juce::String line = rawLine.replaceCharacter ('\t', ' ').trim();
            if (line.isEmpty() || line.startsWith ("//")) continue;

            // processing #define
            if (line.startsWithIgnoreCase ("#define"))
            {
                auto parts = juce::StringArray::fromTokens (line, " ", "\"");
                if (parts.size() >= 3)
                    defines[parts[1]] = parts[2].unquoted();
                continue;
            }
            // replace macros in descending order of length.
            auto sortedDefines = getSortedDefines();
            for (const auto& [varName, varVal] : sortedDefines)
                line = line.replace (varName, varVal);

            // manage SFZ header tags
            if (line.containsIgnoreCase ("<control>") || line.containsIgnoreCase ("<global>"))
            {
                flushRegion();
                inGroup = false;
            }
            else if (line.containsIgnoreCase ("<group>"))
            {
                flushRegion();
                inGroup = true;
                groupState = globalState; // Erbt vom Datei-weiten Global-Status!
                currentRegionState = groupState;
            }
            else if (line.containsIgnoreCase ("<region>"))
            {
                flushRegion();
                inRegion = true;
                if (!inGroup) groupState = globalState;
                currentRegionState = groupState;
            }
            // parsing key-Value opcodes
            auto opcodes = parseSfzLineOpcodes (line);
            for (const auto& op : opcodes)
            {
                if (op.key.equalsIgnoreCase ("default_path"))
                {
                    auto pathVal = op.value.unquoted().replaceCharacter ('\\', '/');
                    if (!pathVal.endsWithChar ('/')) pathVal += "/";
                    globalState.defaultPath        = pathVal;
                    groupState.defaultPath         = pathVal;
                    currentRegionState.defaultPath = pathVal;
                }
                else if (op.key.equalsIgnoreCase ("sample"))
                {
                    if (currentSamplePath.isNotEmpty())
                        flushRegion();
                    currentSamplePath = op.value.unquoted();
                    inRegion          = true;
                }
                else if (op.key.equalsIgnoreCase ("key"))
                {
                    int note = parseSfzNote (op.value);
                    currentRegionState.rootNote = currentRegionState.loKey = currentRegionState.hiKey = note;
                    if (!inRegion && inGroup) groupState.rootNote = groupState.loKey = groupState.hiKey = note;
                    else if (!inRegion && !inGroup) globalState.rootNote = globalState.loKey = globalState.hiKey = note;
                }
                else if (op.key.equalsIgnoreCase ("lokey"))
                {
                    int note = parseSfzNote (op.value);
                    currentRegionState.loKey = note;
                    if (!inRegion && inGroup) groupState.loKey = note;
                    else if (!inRegion && !inGroup) globalState.loKey = note;
                }
                else if (op.key.equalsIgnoreCase ("hikey"))
                {
                    int note = parseSfzNote (op.value);
                    currentRegionState.hiKey = note;
                    if (!inRegion && inGroup) groupState.hiKey = note;
                    else if (!inRegion && !inGroup) globalState.hiKey = note;
                }
                else if (op.key.equalsIgnoreCase ("pitch_keycenter"))
                {
                    int note = parseSfzNote (op.value);
                    currentRegionState.rootNote = note;
                    if (!inRegion && inGroup) groupState.rootNote = note;
                    else if (!inRegion && !inGroup) globalState.rootNote = note;
                }
                else if (op.key.equalsIgnoreCase ("volume"))
                {
                    float vol = op.value.getFloatValue();
                    currentRegionState.volumeDb = vol;
                    if (!inRegion && inGroup) groupState.volumeDb = vol;
                    else if (!inRegion && !inGroup) globalState.volumeDb = vol;
                }
                else if (op.key.equalsIgnoreCase ("loop_mode") || op.key.equalsIgnoreCase ("loopmode"))
                {
                    bool loop = op.value.containsIgnoreCase ("loop_continuous")
                             || op.value.containsIgnoreCase ("forward")
                             || (!op.value.containsIgnoreCase ("one_shot") && !op.value.containsIgnoreCase ("no_loop"));
                    currentRegionState.isLooping = loop;
                    if (!inRegion && inGroup) groupState.isLooping = loop;
                    else if (!inRegion && !inGroup) globalState.isLooping = loop;
                }
                else if (op.key.equalsIgnoreCase ("loop_start") || op.key.equalsIgnoreCase ("loopstart"))
                {
                    int start = op.value.getIntValue();
                    currentRegionState.loopStart = start;
                    if (!inRegion && inGroup) groupState.loopStart = start;
                    else if (!inRegion && !inGroup) globalState.loopStart = start;
                }
                else if (op.key.equalsIgnoreCase ("loop_end") || op.key.equalsIgnoreCase ("loopend"))
                {
                    int end = op.value.getIntValue();
                    currentRegionState.loopEnd = end;
                    if (!inRegion && inGroup) groupState.loopEnd = end;
                    else if (!inRegion && !inGroup) globalState.loopEnd = end;
                }
            }
            // recursively call #include
            if (line.containsIgnoreCase ("#include"))
            {
                flushRegion();
                auto idx = line.indexOfIgnoreCase ("#include");
                auto afterInc = line.substring (idx + 8).trim();
                juce::String incPath;

                if (afterInc.startsWith ("\""))
                    incPath = afterInc.fromFirstOccurrenceOf ("\"", false, false).upToFirstOccurrenceOf ("\"", false, false);
                else
                    incPath = afterInc.upToFirstOccurrenceOf (" ", false, false);

                incPath = incPath.unquoted().trim();
                juce::File incFile = findPathCaseInsensitive (rootDir, incPath);

                if (!incFile.existsAsFile())
                    incFile = findPathCaseInsensitive (fileToParse.getParentDirectory(), incPath);
                if (incFile.existsAsFile()) {
                    // pass the current group or global status to the include file!
                    parseSfzRecursive (incFile, inGroup ? groupState : globalState);
                }
                else{
                    //std::cout << "SFZ Warning: Include nicht gefunden: " << incPath.toStdString() << std::endl;
                }
            }
        }
        flushRegion();
    };
    SfzParseState initialState;
    parseSfzRecursive (sfzFile, initialState);
    /*std::cout << "SFZ loaded preset: " << sfzFile.getFileName().toStdString()
              << " | loaded sounds: " << samplerSynth.getNumSounds() << std::endl;*/
}

void AudioPluginAudioProcessor::loadSampleSourceInBackground (const juce::File& sampleSource)
{
    // local buffer for smart pointers (ReferenceCountedObjectPtr)
    juce::Array<juce::SynthesiserSound::Ptr> loadedSounds;
    // CASE A: SFZ file
    if (sampleSource.existsAsFile() && sampleSource.getFileExtension() == ".sfz")
    {
        loadSfzPresetToBuffer (sampleSource, loadedSounds);
    }
    // CASE B: folder
    else if (sampleSource.isDirectory())
    {
        juce::Array<juce::File> sfzFiles;
        for (const auto& entry : juce::RangedDirectoryIterator (sampleSource, true, "*.sfz", juce::File::findFiles))
            sfzFiles.add (entry.getFile());

        if (!sfzFiles.isEmpty())
        {
            loadSfzPresetToBuffer (sfzFiles.getFirst(), loadedSounds);
        }
        else {
            // the sampler structures without sfz (clean, using smart pointers)
            std::vector<SamplerZoneSound::Ptr> tempSounds;
            for (const auto& entry : juce::RangedDirectoryIterator (sampleSource, true, "*.wav", juce::File::findFiles))
            {
                auto file = entry.getFile();
                std::unique_ptr<juce::AudioFormatReader> reader (formatManager.createReaderFor (file));
                if (reader != nullptr)
                {
                    int rootNote = parseRootNoteFromFilename (file);
                    juce::AudioBuffer<float> tempBuffer (static_cast<int> (reader->numChannels),
                                                         static_cast<int> (reader->lengthInSamples));
                    reader->read (&tempBuffer, 0, static_cast<int> (reader->lengthInSamples), 0, true, true);

                    // creates a SamplerZoneSound::Ptr (ref-counted smart pointer)
                    tempSounds.push_back (new SamplerZoneSound (rootNote, std::move (tempBuffer), reader->sampleRate));
                }
            }
            if (!tempSounds.empty())
            {
                std::sort (tempSounds.begin(), tempSounds.end(), [](const SamplerZoneSound::Ptr& a, const SamplerZoneSound::Ptr& b) {
                    return a->rootMidiNote < b->rootMidiNote;
                });
                for (size_t i = 0; i < tempSounds.size(); ++i)
                {
                    int low = 0;
                    int high = 127;
                    if (i > 0)
                        low = (tempSounds[i - 1]->rootMidiNote + tempSounds[i]->rootMidiNote) / 2 + 1;
                    if (i < tempSounds.size() - 1)
                        high = (tempSounds[i]->rootMidiNote + tempSounds[i + 1]->rootMidiNote) / 2;

                    tempSounds[i]->minMidiNote = low;
                    tempSounds[i]->maxMidiNote = high;
                    // simply move it into the JUCE array (no messy delete required!)
                    loadedSounds.add (tempSounds[i]);
                }
            }
        }
    }
    // !! atomic exchange in the synthesizer (execute on the message thread)
    if (!loadedSounds.isEmpty())
    {
        juce::MessageManager::callAsync ([this, soundsToTransfer = std::move (loadedSounds)]()
        {
            samplerSynth.clearSounds();
            for (auto& sound : soundsToTransfer)
            {
                samplerSynth.addSound (sound);
            }
        });
    }
}


// helper function to convert SFZ notes (e.g., "c3" or "60") to int
int AudioPluginAudioProcessor::parseSfzNote (const juce::String& noteStr)
{
    auto str = noteStr.trim().toLowerCase();
    if (str.isEmpty())
        return 60; // Fallback C4 (MIDI 60)
    // is it already just a number? (e.g., "60")
    if (str.containsOnly ("0123456789-"))
        return str.getIntValue();
    // determine the note name and accidentals (semitones)
    int noteIndex = -1;
    int charOffset = 1;
    // first check 2-character notes with '#' or 'b' (e.g., "c#", "db")
    if (str.length() > 1 && (str[1] == '#' || str[1] == 'b'))
    {
        juce::String noteWithAcc = str.substring (0, 2);
        if      (noteWithAcc == "c#" || noteWithAcc == "db") noteIndex = 1;
        else if (noteWithAcc == "d#" || noteWithAcc == "eb") noteIndex = 3;
        else if (noteWithAcc == "f#" || noteWithAcc == "gb") noteIndex = 6;
        else if (noteWithAcc == "g#" || noteWithAcc == "ab") noteIndex = 8;
        else if (noteWithAcc == "a#" || noteWithAcc == "bb") noteIndex = 10;

        charOffset = 2;
    }
    // if no accidental was found, check the natural note (e.g., "c", "d")
    if (noteIndex == -1 && str.length() > 0) {
        char firstChar = str[0];
        if      (firstChar == 'c') noteIndex = 0;
        else if (firstChar == 'd') noteIndex = 2;
        else if (firstChar == 'e') noteIndex = 4;
        else if (firstChar == 'f') noteIndex = 5;
        else if (firstChar == 'g') noteIndex = 7;
        else if (firstChar == 'a') noteIndex = 9;
        else if (firstChar == 'b') noteIndex = 11;
    }
    // unknown format -> security fallback
    if (noteIndex == -1)
        return 60;
    // parse otave (e.g. "4" from "c4" or "-1" from "c-1")
    int octave = str.substring (charOffset).getIntValue();
    // in the SFZ standard, C4 typically corresponds to MIDI note 60: (octave + 1) * 12 + note
    int midiNote = (octave + 1) * 12 + noteIndex;
    return juce::jlimit (0, 127, midiNote);
}


int AudioPluginAudioProcessor::parseRootNoteFromFilename (const juce::File& file)
{
    auto name = file.getFileNameWithoutExtension().toUpperCase();
    // check notes with accidentals first, so that "C#" is matched before "C"!
    juce::StringArray noteNames { "C#", "D#", "F#", "G#", "A#", "C", "D", "E", "F", "G", "A", "B" };
    juce::StringArray orderedNotes { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

    for (int i = 0; i < name.length(); ++i) {
        for (const auto& note : noteNames)
        {
            if (name.substring (i).startsWith (note))
            {
                int digitPos = i + note.length();
                if (digitPos < name.length())
                {
                    auto nextChar = name[digitPos];
                    if (nextChar >= '0' && nextChar <= '9')
                    {
                        int octave = nextChar - '0'; // cleanly converts, for example, ASCII '1' (49) into int 1
                        int noteIndex = orderedNotes.indexOf (note);
                        return (octave + 1) * 12 + noteIndex;
                    }
                }
            }
        }
    }
    return 60; // Robust fallback (C3)
}


void AudioPluginAudioProcessor::InitSamplerParams(std::vector<std::pair<float, float>> controlPointsParam)
{
    // parameter sampler volume
    auto pNameSamplerVolumeParam = juce::String::fromUTF8(
            reinterpret_cast<const char*>(u8"Sampler Volume "));
    addParameter(samplerVolumeParam = new juce::AudioParameterFloat(
        juce::ParameterID("SAMPLER_VOLUME", 1),
        pNameSamplerVolumeParam,
        GuiUtils::createControlPointRange(controlPointsParam, 0.1f),
        -5.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float val, int) {
                return val <= -59.0f ? "-INF" : juce::String(val, 1) + " dB";
            })
            .withValueFromStringFunction([](const juce::String& text) {
                return text.getFloatValue();
            })
    ));

    // adsr params ============================================================================
    auto pNameSampleAttack = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Attack Sample "));
    addParameter(samplerGainParams.attackSample = new juce::AudioParameterFloat(
        juce::ParameterID ("ATTACK_SAMPLE_", 1), pNameSampleAttack,
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.001f, 0.5f), 1.0f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

    auto pNameSampleDecay = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Decay Sample "));
    addParameter(samplerGainParams.decaySample = new juce::AudioParameterFloat(
        juce::ParameterID ("DECAY_SAMPLE_", 1), pNameSampleDecay,
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.001f, 0.5f), 1.0f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

    auto pNameSampleSustain = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Sustain Sample "));
    addParameter(samplerGainParams.sustainSample = new juce::AudioParameterFloat(
        juce::ParameterID ("SUSTAIN_SAMPLE_", 1), pNameSampleSustain,
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 0.5f), 1.0f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

    auto pNameSampleRelease = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Release Sample "));
    addParameter(samplerGainParams.releaseSample = new juce::AudioParameterFloat(
        juce::ParameterID ("RELEASE_SAMPLE_", 1), pNameSampleRelease,
        juce::NormalisableRange<float>(0.0f, 2.0f, 0.001f, 0.5f), 2.0f,
        juce::AudioParameterFloatAttributes()
        .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
        .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

    auto pNameGainSampleAdsr = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"gain-SampleAdsr"));
    auto* plotGainSample = samplerGainParams.samplerAdsrGainPtr = magicState.createAndAddObject<ADSRPlot>(pNameGainSampleAdsr);
    magicState.addBackgroundProcessing(plotGainSample);
    if (samplerGainParams.samplerAdsrGainPtr != nullptr)
    {
        samplerGainParams.samplerAdsrGainPtr->setActive(true);
        samplerGainParams.samplerAdsrGainPtr->setParameters(
        samplerGainParams.attackSample,
        samplerGainParams.decaySample,
         samplerGainParams.sustainSample,
         samplerGainParams.releaseSample
        );
    }
    samplerGainParams.samplerAdsrGainPtr->setGuiState(magicState);

    auto pNameSamplerIsActivatedParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Sampler Active "));
    addParameter(samplerGainParams.isActivated = new juce::AudioParameterBool
        (juce::ParameterID ("SAMPLER_ACTIVE", 1), pNameSamplerIsActivatedParam, true));

    // filter params ========================================================
        auto pNameSampleFilterType = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Sample Filter Type "));
        addParameter(samplerFilterParams.filterSampleTypeParam= new juce::AudioParameterChoice(
            juce::ParameterID ("SAMPLE_FILTERTYPE",1), pNameSampleFilterType,
            juce::StringArray{"lowpass", "bandpass", "highpass", "formant"}, 1));



        juce::NormalisableRange<float> sampleFilterRange(20.0f, 20000.0f, 0.1f);
        auto pNameSampleFilterFreq = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Sample FilterFreq "));
        sampleFilterRange.setSkewForCentre(1000.0f);
        addParameter(samplerFilterParams.sampleFilterFreq = new juce::AudioParameterFloat(
            juce::ParameterID("SAMPLE_FILTERFREQ", 1), pNameSampleFilterFreq, sampleFilterRange, // Hier die vorbereitete Range nutzen
            290, juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int)
            {
                if (value >= 1000.0f)
                    return juce::String(value / 1000.0f, 2) + " kHz";
                return juce::String(value, 0) + " Hz";
            })
            .withValueFromStringFunction([](const juce::String& text)
            {
                return text.getFloatValue();
            })
        ));

        auto pNameSampleFilterReso = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Sample FilterReso "));
        addParameter(samplerFilterParams.sampleFilterReso = new juce::AudioParameterFloat(
            juce::ParameterID ("SAMPLE_FILTERRESO", 1), pNameSampleFilterReso,
            juce::NormalisableRange<float>(1.0f, 10.0f, 0.05f), 2.5f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));


        auto pNameSampleAttackFlt = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Sample Attack Flt "));
        addParameter(samplerFilterParams.attackFilterSample = new juce::AudioParameterFloat(
            juce::ParameterID ("SAMPLE_ATTACKFLT", 1), pNameSampleAttackFlt,
            juce::NormalisableRange<float>(0.05f, 0.2f, 0.001f, 0.5f), 0.1f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

        auto pNameSampleDecayFlt = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Sample Decay Flt "));
        addParameter(samplerFilterParams.decayFilterSample = new juce::AudioParameterFloat(
            juce::ParameterID ("SAMPLE_DECAYFLT", 1), pNameSampleDecayFlt,
            juce::NormalisableRange<float>(0.0f, 2.0f, 0.001f, 0.5f), 0.8f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

        auto pNameSampleSustainFlt = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Sample Sustain Flt "));
        addParameter(samplerFilterParams.sustainFilterSample = new juce::AudioParameterFloat(
            juce::ParameterID ("SAMPLE_SUSTAINFLT", 1), pNameSampleSustainFlt,
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f, 0.5f), 0.5f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

        auto pNameSampleReleaseFlt = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Sample Release Flt "));
        addParameter(samplerFilterParams.releaseFilterSample = new juce::AudioParameterFloat(
            juce::ParameterID ("SAMPLE_RELEASEFLT", 1), pNameSampleReleaseFlt,
            juce::NormalisableRange<float>(0.0f, 2.0f, 0.001f, 0.5f), 0.8,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 2); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));


        juce::NormalisableRange<float> envAmountSampleRange(0.0f, 4800.0f, 1.0f);
        auto pNameSampleEnvAmFlt = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Sample EnvAm Flt "));
        addParameter(samplerFilterParams.sampleFilterEnvAmount = new juce::AudioParameterFloat(
            juce::ParameterID ("SAMPLE_ENVWTFLT", 1), pNameSampleEnvAmFlt,
            envAmountSampleRange, 3600,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction([](const float value, int) { return juce::String(value, 0); })
            .withValueFromStringFunction([](const juce::String& text) { return text.getFloatValue(); })
        ));

        auto pNameSampleFilterAdsr = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"sample-filter-adsr"));
        auto* plotSamplerFilter = samplerFilterParams.samplerAdsrFilterPtr = magicState.createAndAddObject<ADSRPlot>(pNameSampleFilterAdsr);
        magicState.addBackgroundProcessing(plotSamplerFilter);
        if (samplerFilterParams.samplerAdsrFilterPtr != nullptr)
        {
            samplerFilterParams.samplerAdsrFilterPtr->setActive(true);
            samplerFilterParams.samplerAdsrFilterPtr->setParameters(
                samplerFilterParams.attackFilterSample,
                samplerFilterParams.decayFilterSample,
                 samplerFilterParams.sustainFilterSample,
                 samplerFilterParams.releaseFilterSample
            );
        }
        samplerFilterParams.samplerAdsrFilterPtr->setGuiState(magicState);

    auto pNameSamplerFilterIsActivatedParam = juce::String::fromUTF8 (reinterpret_cast<const char*> (u8"Sampler Filter Active "));
    addParameter(samplerFilterParams.isActivated = new juce::AudioParameterBool
        (juce::ParameterID ("SAMPLER_FILTER_ACTIVE", 1), pNameSamplerFilterIsActivatedParam, true));
}
#pragma  endregion SAMPLING_METHODS

#pragma region GUI_SIZE_AND_KEYBOARD
void AudioPluginAudioProcessor::setExplicitGuiSize (float factor)
{
    if (currentPreProcessorReplacement == PreProcessorReplacement::Standalone) {
        currentScaleFactor = factor;
        if (auto* editor = getActiveEditor())
        {
            const int targetWidth  = juce::roundToInt (maxGuiWidth * factor);
            const int targetHeight = juce::roundToInt (maxGuiHeight * factor);

            if (standaloneWindow != nullptr && sizeLocker != nullptr)  {
                sizeLocker->setTargetSize (targetWidth, targetHeight);
                standaloneWindow->setSize (targetWidth, targetHeight);
            }
            editor->setResizeLimits (targetWidth, targetHeight, targetWidth, targetHeight);
            editor->setSize (maxGuiWidth, maxGuiHeight);
            editor->setScaleFactor (factor);

            if (standaloneWindow != nullptr) {
                juce::Component::SafePointer<juce::Component> safeWindow (standaloneWindow);
                juce::MessageManager::callAsync ([this, safeWindow, targetWidth, targetHeight]
                {
                    if (safeWindow != nullptr) {
                        if (auto* peer = safeWindow->getPeer()) {
                            reassertWmSizeHints (peer, targetWidth, targetHeight);
                        }
                    }
                });
            }
        }
    }
    else if (currentPreProcessorReplacement == PreProcessorReplacement::VST3) {
        currentScaleFactor = factor;

        if (auto* editor = dynamic_cast<ScaledPluginEditor*> (getActiveEditor())) {
            const float targetWidthInPixels = maxGuiWidth * factor;
            const float scale = targetWidthInPixels / static_cast<float> (maxGuiWidth);
            editor->setTargetScale (scale);
        }
    }
}

void AudioPluginAudioProcessor::reassertWmSizeHints (juce::ComponentPeer* peer, int logicalW, int logicalH)
{
    if (peer == nullptr) { return; }

    const double scale = peer->getPlatformScaleFactor();
    const int physW = juce::roundToInt (logicalW * scale);
    const int physH = juce::roundToInt (logicalH * scale);

    using DisplayPtr = void*;
    using WindowHandle = unsigned long;

    struct LocalXSizeHints {
        long flags;
        int x, y;
        int width, height;
        int min_width, min_height;
        int max_width, max_height;
        int width_inc, height_inc;
        struct { int x; int y; } min_aspect, max_aspect;
        int base_width, base_height;
        int win_gravity;
    };

    constexpr long localPMinSize = (1L << 4);
    constexpr long localPMaxSize = (1L << 5);

    using XOpenDisplayFunc      = DisplayPtr (*)(const char*);
    using XCloseDisplayFunc     = int (*)(DisplayPtr);
    using XAllocSizeHintsFunc   = LocalXSizeHints* (*)();
    using XSetWMNormalHintsFunc = void (*)(DisplayPtr, WindowHandle, LocalXSizeHints*);
    using XFreeFunc             = void (*)(void*);
    using XFlushFunc            = int (*)(DisplayPtr);

    void* x11Lib = dlopen("libX11.so.6", RTLD_LAZY | RTLD_NOLOAD);
    if (!x11Lib) { x11Lib = dlopen("libX11.so.6", RTLD_LAZY); }
    if (!x11Lib) { return; }

    auto pXOpenDisplay      = reinterpret_cast<XOpenDisplayFunc>(dlsym(x11Lib, "XOpenDisplay"));
    auto pXCloseDisplay     = reinterpret_cast<XCloseDisplayFunc>(dlsym(x11Lib, "XCloseDisplay"));
    auto pXAllocSizeHints   = reinterpret_cast<XAllocSizeHintsFunc>(dlsym(x11Lib, "XAllocSizeHints"));
    auto pXSetWMNormalHints = reinterpret_cast<XSetWMNormalHintsFunc>(dlsym(x11Lib, "XSetWMNormalHints"));
    auto pXFree             = reinterpret_cast<XFreeFunc>(dlsym(x11Lib, "XFree"));
    auto pXFlush            = reinterpret_cast<XFlushFunc>(dlsym(x11Lib, "XFlush"));

    if (pXOpenDisplay && pXCloseDisplay && pXAllocSizeHints && pXSetWMNormalHints && pXFree) {
        if (DisplayPtr display = pXOpenDisplay(nullptr)) {
            auto window = static_cast<WindowHandle>(reinterpret_cast<uintptr_t>(peer->getNativeHandle()));
            if (LocalXSizeHints* hints = pXAllocSizeHints()) {
                hints->flags = localPMinSize | localPMaxSize;
                hints->min_width  = hints->max_width  = physW;
                hints->min_height = hints->max_height = physH;

                pXSetWMNormalHints(display, window, hints);
                pXFree(hints);
            }
            if (pXFlush) { pXFlush(display); }
            pXCloseDisplay(display);
        }
    }
}

void AudioPluginAudioProcessor::ChangeKeyBoard()
{
    // toggle elegantly (instead of if/else)
    boolKeyBSwitch = !boolKeyBSwitch;
    // set values ​​based on the state
    const float keyWidth        = boolKeyBSwitch ? 50.0f : 30.0f;
    const float ratio           = boolKeyBSwitch ? 0.6f  : 0.4f;
    const int rangeLimitLowest  = boolKeyBSwitch ? 24    : 0;
    const int rangeLimitHighest = boolKeyBSwitch ? 84    : 96;
    // security checks: Do the tree and builder even exist?
    // (The builder is only active when the editor window is open!)
    if (myBuilder == nullptr){ return; }

    const auto guiTree = magicState.getGuiTree();
    const auto node = findNodeById(guiTree, "mainKeyBoard");

    if (!node.isValid()){ return; }

    if (auto* item = myBuilder->findGuiItem(node)) {
        if (auto* keyboardComp = dynamic_cast<juce::MidiKeyboardComponent*>(item->getWrappedComponent()))
        {
            keyboardComp->setKeyWidth(keyWidth);
            keyboardComp->setAccessible(true);
            keyboardComp->setAvailableRange(rangeLimitLowest, rangeLimitHighest);
        }
    }
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    auto* editor = foleys::MagicProcessor::createEditor();

    if (currentPreProcessorReplacement == PreProcessorReplacement::Standalone)
    {
        // --- STANDALONE PATH (With flexible scaling & X11 lock) ---
        juce::Component::SafePointer<juce::AudioProcessorEditor> safeEditor (editor);

        juce::MessageManager::callAsync ([this, safeEditor]
        {
            if (safeEditor == nullptr)
                return;

            if (auto* docWindow = dynamic_cast<juce::DocumentWindow*> (safeEditor->getTopLevelComponent()))
            {
                docWindow->setUsingNativeTitleBar (false);
                docWindow->setResizable (false, false);

                standaloneWindow = docWindow;

                const int initW = juce::roundToInt (maxGuiWidth * currentScaleFactor);
                const int initH = juce::roundToInt (maxGuiHeight * currentScaleFactor);
                sizeLocker = std::make_unique<WindowSizeLocker> (*docWindow, initW, initH);

                setExplicitGuiSize (currentScaleFactor);
            }
        });
        setExplicitGuiSize (currentScaleFactor);
    }
    else if (currentPreProcessorReplacement == PreProcessorReplacement::VST3)
    {
        magicState.updateParameterMap();

        auto builder = std::make_unique<foleys::MagicGUIBuilder> (magicState);
        initialiseBuilder (*builder);

        if (magicState.getGuiTree().getChildWithName (foleys::IDs::view).isValid() == false)
            magicState.setGuiValueTree (createGuiValueTree());

        auto* editorVst = new ScaledPluginEditor (magicState, std::move (builder));

        constexpr float targetWidthInPixels = 700.0f;
        const float scale = targetWidthInPixels / static_cast<float> (maxGuiWidth);

        editorVst->setFixedNativeSize (maxGuiWidth, maxGuiHeight);
        editorVst->setTargetScale (scale);

        return editorVst;
    }


    return editor;
}
#pragma endregion GUI_SIZE_AND_KEYBOARD

//=======================================================================
// this creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
//========================================================================