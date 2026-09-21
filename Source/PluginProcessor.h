/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <foleys_gui_magic/foleys_gui_magic.h>
#include "../SDKs/SynthLab/source/synthengine.h"
#include "Assets/BinaryData.h"
#include "UI/ADSRPlot.h"
#include "UI/AbsynthTabLookAndFeel.h"
#include <juce_dsp/juce_dsp.h>
#include "SynthSound.h"
#include "SynthVoice.h"
#include "SamplerVoice.h"
#include "UI/CustomLNF.h"
#include "UI/CustomLNFButton.h"
#include "UI/CustomLNFTextButton.h"
#include "UI/SpectrogramMatrixItem.h"
#include "UI/SampleBrowserItem.h"
#include "UI/HelpTextItem.h"
#include "Data/MasterEQ.h"
#include "Components/CustomMouseComponent.h"
#include "Helpers/PixelFIFO.h"
#include "Helpers/CBuffer.h"
#include <algorithm>
#include "Helpers/FileHandling.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlib.h>
#include <dlfcn.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "pluginterfaces/base/futils.h"
#include "Helpers/GuiUtils.h"
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include <thread>
#include <atomic>

#pragma region STRUCTS_AND_STUFF
struct PropertyLambdaListener;
struct PropertyActiveListener;
struct PropertySamplerListener;
struct PropertySynthFilterActiveListener;
struct PropertySamplerFilterListener;
struct PropertyEffectsListener;
struct PropertyButtonListener;
struct PropertyTutorialListener;
struct PropertyLicenceListener;
struct PropertyPlusButListener;
struct PropertyMedButListener;
struct PropertyMinusButListener;
struct PropertyAnalysButListener;
struct PropertyGainFilterListener;
struct PropertyKeybBigSmallListener;
struct PropertySamplerBrowseListener;
struct PixelUpdate;
class PropertySpectroListener : public juce::Value::Listener
{
public:
    PropertySpectroListener (juce::Value v, std::function<void(bool)> callback)
        : valueToListen (v), onChangedCallback (std::move(callback))
    {
        valueToListen.addListener (this);
    }

    ~PropertySpectroListener() override
    {
        valueToListen.removeListener (this);
    }

    void valueChanged (juce::Value& v) override
    {
        if (onChangedCallback)
            onChangedCallback (static_cast<bool>(v.getValue()));
    }

private:
    juce::Value valueToListen;
    std::function<void(bool)> onChangedCallback;
};
#pragma endregion STRUCTS_AND_STUFF

#pragma region GUI_HELPER_CLASSES
class WindowSizeLocker : public juce::ComponentListener
{
public:
    WindowSizeLocker (juce::Component& comp, int w, int h)
        : target (comp), targetW (w), targetH (h)
    {
        target.addComponentListener (this);
    }

    ~WindowSizeLocker() override
    {
        target.removeComponentListener (this);
    }

    void setTargetSize (int w, int h) { targetW = w; targetH = h; }

    void componentMovedOrResized (juce::Component& comp, bool /*wasMoved*/, bool wasResized) override
    {
        if (!wasResized || isGuard) { return; }

        if (comp.getWidth() != targetW || comp.getHeight() != targetH) {
            isGuard = true;
            comp.setSize (targetW, targetH);
            isGuard = false;
        }
    }
private:
    juce::Component& target;
    int targetW, targetH;
    bool isGuard { false };
};
class ScaledPluginEditor : public foleys::MagicPluginEditor
{
public:
    using foleys::MagicPluginEditor::MagicPluginEditor;

    void setFixedNativeSize (int w, int h) { nativeWidth = w; nativeHeight = h; }

    void setTargetScale (float s)
    {
        fixedScale = s;
        setResizable (false, false);   // verhindert Constrainer-Eingriffe
        setSize (juce::roundToInt (nativeWidth * fixedScale),
                  juce::roundToInt (nativeHeight * fixedScale));
        resized();
    }

    void resized() override
    {
        getGUIBuilder().updateLayout ({ 0, 0, nativeWidth, nativeHeight });

        if (auto* root = getChildComponent (0))
            root->setTransform (juce::AffineTransform::scale (fixedScale));
    }

private:
    int nativeWidth = 1164, nativeHeight = 720;
    float fixedScale = 1.0f;
};
#pragma endregion GUI_HELPER_CLASSES

#pragma region SFZ_PARSER
struct SfzOpcode
{
    juce::String key;
    juce::String value;
};

// extracts SFZ key-value pairs (e.g., sample=Piano 1/c3.wav key=c3), even with spaces in paths
static juce::Array<SfzOpcode> parseSfzLineOpcodes (const juce::String& line)
{
    juce::Array<SfzOpcode> opcodes;

    juce::Array<int> eqIndices;
    int searchIdx = 0;
    while ((searchIdx = line.indexOfChar (searchIdx, '=')) != -1)
    {
        eqIndices.add (searchIdx);
        searchIdx++;
    }

    if (eqIndices.isEmpty()) return opcodes;

    for (int i = 0; i < eqIndices.size(); ++i)
    {
        int eqIdx = eqIndices[i];

        // key-Name vor '=' rückwärts ermitteln
        int keyStart = eqIdx - 1;
        while (keyStart >= 0 && (juce::CharacterFunctions::isLetterOrDigit (line[keyStart]) || line[keyStart] == '_' || line[keyStart] == '#'))
        {
            keyStart--;
        }
        keyStart++;

        juce::String key = line.substring (keyStart, eqIdx).trim();
        if (key.isEmpty()) continue;

        // determine the value following '=' (up to the next key before the next '=', or until the end of the line)
        int valStart = eqIdx + 1;
        int valEnd = line.length();

        if (i + 1 < eqIndices.size())
        {
            int nextEqIdx = eqIndices[i + 1];
            int nextKeyStart = nextEqIdx - 1;
            while (nextKeyStart >= valStart && (juce::CharacterFunctions::isLetterOrDigit (line[nextKeyStart]) || line[nextKeyStart] == '_' || line[nextKeyStart] == '#'))
            {
                nextKeyStart--;
            }
            valEnd = nextKeyStart + 1;
        }

        juce::String value = line.substring (valStart, valEnd).trim();
        opcodes.add ({ key, value });
    }

    return opcodes;
}
#pragma endregion SFZ_PARSER

class AudioPluginAudioProcessor : public foleys::MagicProcessor, public juce::ChangeBroadcaster, private juce::Timer
{
public:
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;
    void updateStatusMessage(const juce::String& newMessage);
    void postSetStateInformation() override;
    // for Presets:
    void getStateInformation (juce::MemoryBlock& destData) override; // Speichern
    void setStateInformation (const void* data, int sizeInBytes) override; // Laden
    void savePresetToFile(const juce::File& file) const;
    void loadPresetFromFile(const juce::File& file);

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    void timerCallback() override;
    void updateAdsrPlot(int inx);
    //============ reverse spectrogram ===================================================
    PixelFIFO& getPixelFIFO() { return pixelFifo; }
    // reads the FIFO and updates the audio copy of the grid
    void processIncomingPixelUpdates();
    // the actual synthesis logic (here we pass the buffer)
    void renderSpectrogramToBuffer (juce::AudioBuffer<float>& buffer);
    FFTFIFO& getOutputFFTFIFO() { return outputFftFifo; }
    // access methods for the UI:
    juce::Image& getCanvasImage() { return canvasImage; }
    const juce::Image& getCanvasImage() const { return canvasImage; }
    // helper method for clearing the image (e.g., for clear_canvas)
    void clearCanvasImage()
    {
        canvasImage.clear (canvasImage.getBounds(), juce::Colour (0xff001500));
    }
    void updateAudioAndUIFromLoadedImage (const juce::Image& loadedImage);
    // ================================================================================
    juce::AudioProcessorEditor* createEditor() override;
    void reassertWmSizeHints (juce::ComponentPeer* peer, int logicalW, int logicalH);

    // the scale function:
    void setExplicitGuiSize(float factor);
    // ================================================================================
    juce::ValueTree keyBoardNode;
    juce::ValueTree guiTreeKeyBoard;
    foleys::GuiItem* keyboardItem;
    juce::MidiKeyboardComponent* keyboardComp;
    void ChangeKeyBoard();
    // === Sampling ===================================================================
    // The loading method triggered by the PGM item is not related to the 4 WaveTableOSCs
    void loadSampleFromFile (const juce::File& sampleSource);
    juce::String samplParentDir, samplInstrFolder;
    juce::File getSampleBaseDir() const;
    juce::String samplFile;
    //__________________________________________________
    // helper function: Finds files on Linux, ignoring case and backslashes
    static juce::File findPathCaseInsensitive (const juce::File& root, const juce::String& relativePath)
    {
        juce::String cleanPath = relativePath.replaceCharacter ('\\', '/').trim();
        if (cleanPath.isEmpty()) return {};

        juce::File direct = root.getChildFile (cleanPath);
        if (direct.existsAsFile())
            return direct;

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
                    if (entry.getFile().getFileName().equalsIgnoreCase (comp))
                    {
                        current = entry.getFile();
                        found = true;
                        break;
                    }
                }
                if (!found) return {};
            }
        }
        return current.existsAsFile() ? current : juce::File();
    }

    // struct for the clean encapsulation of zone parameters
    struct SfzParseState
    {
        int rootNote = 60;
        int loKey = 0;
        int hiKey = 127;
        float volumeDb = 0.0f;
        juce::String defaultPath = "";

        // SFZ looping-Parameters:
        bool isLooping = false;
        int loopStart = 0;
        int loopEnd = 0;
    };
    //================================================================================
    void initialiseBuilder (foleys::MagicGUIBuilder& builder) override
    {// customized components and LoolAndFeels
        myBuilder = &builder;
        lookAndFeelTab = std::make_unique<AbsynthTabLookAndFeel>();
        lookAndFeelTabV2 = std::make_unique<AbsynthTabLookAndFeel_V2>();
        lookAndFeelScroll = std::make_unique<AbsynthScrollbarLookAndFeel>();
        customLNF = std::make_unique<CustomLNF>();
        customButtonLNF = std::make_unique<CustomLNFButton>();
        customNavButtonLNF = std::make_unique<CustomLNFButton>();
        customButtonSwitch = std::make_unique<CustomSwitchButton>();
        lookAndFeelLed = std::make_unique<CustomLedLNF>();
        lookAndFeelTitle = std::make_unique<AbsynthTitelLookAndFeel>();
        builder.registerJUCEFactories();
        builder.registerJUCELookAndFeels();
        builder.registerLookAndFeel("AbsynthScrollLookAndFeel", std::move(lookAndFeelScroll));
        builder.registerLookAndFeel("AbsynthTabLookAndFeel", std::move(lookAndFeelTab));
        builder.registerLookAndFeel("AbsynthTabLookAndFeel_V2", std::move(lookAndFeelTabV2));
        builder.registerLookAndFeel("customLNF", std::move(customLNF));
        builder.registerLookAndFeel("CustomLNFButton", std::move(customButtonLNF));
        builder.registerLookAndFeel("CustomSwitchButton", std::move(customButtonSwitch));
        builder.registerLookAndFeel("CustomLedLNF", std::move(lookAndFeelLed));
        builder.registerLookAndFeel("AbsynthTitelLookAndFeel", std::move(lookAndFeelTitle));
        builder.registerFactory ("SpectrogramMatrix", &SpectrogramMatrixItem::factory);
        builder.registerFactory ("HelpTextItem", &HelpTextItem::factory);
        builder.registerFactory ("OutputSpectrogramItem", &OutputSpectrogramItem::factory);
        builder.registerFactory ("SampleBrowser", &SampleBrowserItem::factory);
    }
private:
#pragma region LAMBDA_METHODS
    std::vector<std::unique_ptr<PropertyLambdaListener>> MyPropertyListeners;
    std::vector<std::unique_ptr<PropertyActiveListener>> MyPropertyActiveListeners;
    std::vector<std::unique_ptr<PropertySamplerListener>> MyPropertySamplerListeners;
    std::vector<std::unique_ptr<PropertyEffectsListener>> MyPropertyEffectsListeners;
    std::vector<std::unique_ptr<PropertyButtonListener>> MyPresetButtonListeners;
    std::vector<std::unique_ptr<PropertyTutorialListener>> MyPropertyTutorialListener;
    std::vector<std::unique_ptr<PropertyLicenceListener>> MyPropertyLicenceListener;
    std::vector<std::unique_ptr<PropertySpectroListener>> MyPropertySpectroListeners;
    std::vector<std::unique_ptr<PropertyPlusButListener>> MyPropertyPlusButListeners;
    std::vector<std::unique_ptr<PropertyMedButListener>> MyPropertyMedButListeners;
    std::vector<std::unique_ptr<PropertyMinusButListener>> MyPropertyMinusButListeners;
    std::vector<std::unique_ptr<PropertyAnalysButListener>> MyPropertyAnalysButListeners;
    std::vector<std::unique_ptr<PropertyGainFilterListener>> MyPropertyGainFilterListeners;
    std::vector<std::unique_ptr<PropertyKeybBigSmallListener>> MyPropertyKeybBigSmallListeners;
    std::vector<std::unique_ptr<PropertySamplerFilterListener>> MyPropertySamplerFilterListeners;
    std::vector<std::unique_ptr<PropertySynthFilterActiveListener>> MyPropertySynthFilterActiveListeners;
#pragma endregion LAMBDA_METHODS

#pragma region TEXT_EDITOR
    std::unique_ptr<juce::DocumentWindow> helpTutWindow;
    std::unique_ptr<juce::DocumentWindow> helpLicWindow;
    std::unique_ptr<foleys::GuiItem> pgmContentLic = nullptr;
    std::unique_ptr<foleys::GuiItem> pgmContentTut = nullptr;
    juce::ValueTree myTutTree;
    juce::ValueTree myLicTree;
    juce::ValueTree guiTutNode;
    juce::ValueTree guiLicNode;
    juce::Colour bgColorWindow;
    void TriggerTutWindow();
    void TriggerLicenseWindow();
#pragma endregion TEXT_EDITOR

#pragma region EFFECTS
    // --- DSP effects master chain ---
    juce::dsp::Chorus<float>    chorusModule;
    juce::dsp::Chorus<float>    flangerModule; // Uses the same class, but is tuned differently
    juce::dsp::Reverb           reverbModule;

    // --- parameter-pointer (lock-free for the audio-thread) ---
    juce::AudioParameterFloat* chorusRateParam    = nullptr;
    juce::AudioParameterFloat* chorusDepthParam   = nullptr;
    juce::AudioParameterFloat* chorusFeedBack   = nullptr;
    juce::AudioParameterFloat* chorusCentreDelay   = nullptr;
    juce::AudioParameterFloat* flangerRateParam   = nullptr;
    juce::AudioParameterFloat* flangerDepthParam  = nullptr;
    juce::AudioParameterFloat* flangerFeedBackParam  = nullptr;
    juce::AudioParameterFloat* flangerCentreDelay  = nullptr;
    juce::AudioParameterFloat* reverbRoomParam    = nullptr;
    juce::AudioParameterFloat* reverbDampingParam    = nullptr;

    juce::AudioParameterFloat* chorusDryWetParam    = nullptr;
    juce::AudioParameterFloat* flangerDryWetParam   = nullptr;
    juce::AudioParameterFloat* reverbDryWetParam    = nullptr;
    // delay:
    float CalcDelayTime(const float BPM, const int syncSetting)
    {
        const float quarterNoteDelay = 60.0 / BPM;
        const float fullNoteDelay = quarterNoteDelay * 4;
        const float fullNoteDotted = fullNoteDelay * 1.5;
        const float fullNoteTriplet = fullNoteDelay / 1.5;
        const float halfNoteDelay = quarterNoteDelay * 2;
        const float halfNoteDotted = halfNoteDelay * 1.5;
        const float halfNoteTriplet = halfNoteDelay / 1.5;
        const float quarterNoteDotted = quarterNoteDelay * 1.5;
        const float quarterNoteTriplet = quarterNoteDelay / 1.5;
        const float eighthNoteDelay = quarterNoteDelay / 2.0;
        const float eighthNoteDotted = eighthNoteDelay * 1.5;
        const float eighthNoteTripletDelay = quarterNoteDelay / 3.0;
        const float sixteenthNoteDelay = quarterNoteDelay / 4.0;
        const float sixteenthNoteDotted = sixteenthNoteDelay * 1.5;
        const float sixteenthNoteTripletDelay = quarterNoteDelay / 5.0;
        const float thirtytwothNoteDelay = quarterNoteDelay / 6.0;

        const double delayNotes[16] = { fullNoteDelay, fullNoteDotted, fullNoteTriplet,
            halfNoteDelay, halfNoteDotted, halfNoteTriplet, quarterNoteDelay,
            quarterNoteDotted, quarterNoteTriplet, eighthNoteDelay, eighthNoteDotted,
            eighthNoteTripletDelay, sixteenthNoteDelay, sixteenthNoteDotted, sixteenthNoteTripletDelay,
            thirtytwothNoteDelay };

        return (delayNotes[syncSetting] * 1000);
    }
    juce::AudioParameterFloat* delayMaster = nullptr;
    juce::AudioParameterFloat* delayMix = nullptr;
    juce::AudioParameterFloat* delayFeedback = nullptr;
    juce::AudioParameterFloat* delayTimeMsParam = nullptr;
    float FinalDelayTime = 0.0f;
    juce::AudioParameterBool* PingPongEnabled = nullptr;
    juce::AudioParameterBool* delaySyncEnabled = nullptr;
    juce::AudioParameterChoice* delaySyncSetting = nullptr;
    juce::AudioPlayHead::CurrentPositionInfo Playhead;
    CBuffer BufferL, BufferR;
    //
    struct EffekteActiveParam {
        juce::AudioParameterBool* effectIsActive = nullptr;
    };
    std::array<EffekteActiveParam, 4> effekteActiveParams;

    void InitEffekteParameters();
    void preProcessEffects (const int channelNumbers, const int totalNumOutputCh, juce::AudioBuffer<float>& bufferParam);
    std::array<CBuffer, 2> arrDelayBuffer;
    int mWritePos { 0 };
    // storage for the sample rate (required for time-to-sample calculations)
    double currentSampleRate = 44100.0;
    // === end off effects ================================================================
#pragma endregion EFFECTS

#pragma  region INIT
    void InitLfoParameters();
    void InitMasterModualtionParams();
    void InitJuceWtFilterParameters();
    void InitializeWTOscTriggers();
    void InitializeGainFilterTriggers();
    void InitializeSynthFilterActiveTriggers();
    void InitializeKeybBigSmallTriggers();
    void InitializeAnalysTrigger();
    void InitializeModActiveTriggers();
    void InitializeSamplerActiveTriggers();
    void InitializeSamplerFilterActiveTriggers();
    void InitializeEffectsTriggers();
    void InitializeTutWindowTriggers();
    void InitializeLicenseWindowTriggers();
    void InitializeSpectroTriggers();
    void InitParamsMasterSound(const std::vector<std::pair<float, float>> controlPointsParam);
    void InitSpectroParameters(const std::vector<std::pair<float, float>> controlPointsParam);
    void InitSamplerParams(std::vector<std::pair<float, float>> controlPointsParam);
    void InitParamsWToscs(std::vector<std::pair<float, float>> controlPointsParam);
#pragma  endregion INIT

#pragma region SPECTROGRAMM
    //=========== reverse spectrogram synthesis: ======================================//
    PixelFIFO pixelFifo; // high-speed circular buffer
    // the private copy of the grid for the audio thread,
    // grid dimensions must match the UI exactly
    int numColumns = 512;
    int numRows = 256;
    std::vector<float> audioGridData; // Initialized to 0.0f in the constructor
    // play variables
    double currentSamplePosition = 0.0; // Current position in the loop (in samples)
    double loopDurationSeconds = 3.0;   // Total duration of the canvas from left to right
    // FFT object: 2^9 = 512 points (yields exactly 256 frequency bins)
    juce::dsp::FFT fft { 9 };
    // the overlap-add buffer (512 samples in size)
    std::vector<float> overlapBuffer = std::vector<float>(512, 0.0f);
    // counter for when the next 256 samples for the next FFT hop are reached
    int hopCounter = 0;
    //================= End Reverse Part ==========================================
    // Spectro analyze:
    // FFT tools (Order 9 = 2^9 = 512-sample FFT -> yields 256 frequency bands)
    juce::dsp::FFT outputFFT { 9 };
    juce::dsp::WindowingFunction<float> window { 512, juce::dsp::WindowingFunction<float>::hann };
    std::array<float, 512> fftAccumulatorBuffer;
    int fftAccumulatorIndex = 0;
    FFTFIFO outputFftFifo;
    void processOutputSpectrogram(juce::AudioBuffer<float>& bufferParam);
    struct SpectroParams {
        juce::AudioParameterFloat* spectrogramVolumeParam = nullptr;
        juce::AudioParameterFloat* spectrogramPlayTime = nullptr;
        juce::AudioParameterBool* spectroIsActiveParam = nullptr;
    }; SpectroParams spectroParams;
    // scratch buffer for level measurement without real-time allocation
    juce::AudioBuffer<float> specScratchBuffer;
    juce::Image canvasImage { juce::Image::ARGB, 512, 256, false };
    int spectroActiveNoteCount = 0; // counter for Spectrogram keys held
    // auxiliary method for Spectrogram MIDI tracking:
    void processMidiForSpectrogram (const juce::MidiBuffer& midiMessages);
    // helper after preset-load:
    void pushImageToPixelFIFO (const juce::Image& img);
    std::atomic<bool> needFullRefresh { false };
    //=================================================================================//
#pragma  endregion SPECTROGRAMM

#pragma region SAMPLING
    //=========== Sampling =================================================================
    static int parseRootNoteFromFilename (const juce::File& file);
    juce::CriticalSection vectorLock;
    juce::AudioParameterFloat* samplerVolumeParam = nullptr;
    juce::Synthesiser samplerSynth;
    // manages audio formats (WAV, AIFF, etc.):
    juce::AudioFormatManager formatManager;
    // gain/adsr sampler
    struct SamplerGainParams {
        juce::AudioParameterFloat* attackSample = nullptr;
        juce::AudioParameterFloat* decaySample = nullptr;
        juce::AudioParameterFloat* sustainSample = nullptr;
        juce::AudioParameterFloat* releaseSample = nullptr;
        juce::AudioParameterBool* isActivated = nullptr;
        ADSRPlot* samplerAdsrGainPtr = nullptr;
    };
    SamplerGainParams samplerGainParams;

    // filter sampler
    struct SamplerFilterParams {
        juce::AudioParameterChoice* filterSampleTypeParam = nullptr;
        juce::AudioParameterFloat* sampleFilterFreq = nullptr;
        juce::AudioParameterFloat* sampleFilterReso = nullptr;
        juce::AudioParameterFloat* sampleFilterEnvAmount = nullptr;
        juce::AudioParameterFloat* attackFilterSample = nullptr;
        juce::AudioParameterFloat* decayFilterSample = nullptr;
        juce::AudioParameterFloat* sustainFilterSample = nullptr;
        juce::AudioParameterFloat* releaseFilterSample = nullptr;
        juce::AudioParameterBool* isActivated = nullptr;
        ADSRPlot* samplerAdsrFilterPtr = nullptr;
    };
    SamplerFilterParams samplerFilterParams;
    juce::AudioBuffer<float> samplerScratchBuffer;
    int parseSfzNote (const juce::String& noteStr);
    std::atomic<bool> isLoadingSample { false };
    std::thread sampleLoadingThread;
    // background worker for the actual file I/O
    void loadSampleSourceInBackground (const juce::File& sampleSource);
    // the SFZ parser writes directly to the passed array
    void loadSfzPresetToBuffer (const juce::File& sfzFile,
                                juce::Array<juce::SynthesiserSound::Ptr>& targetArray);
    //================  end sampling =====================================================
    //======================================================================================
#pragma endregion SAMPLING

#pragma region LFO_UND_FILTER
    //Lfo AM-Modulation Params
    struct juceLfoAmParameters
    {
        juce::AudioParameterFloat* lfoAmGainParam = nullptr;
        juce::AudioParameterChoice* lfoAmWaveFormParam = nullptr;
        juce::AudioParameterFloat* lfoAmFreqParam = nullptr;
        juce::AudioParameterFloat* lfoAmDepthParam = nullptr;
    };
    std::array<juceLfoAmParameters, 4> juceLfoAmParams;

    //Lfo Fm-Modulation Params
    struct juceLfoFmParameters
    {
        juce::AudioParameterFloat* lfoFmGainParam = nullptr;
        juce::AudioParameterChoice* lfoFmWaveFormParam = nullptr;
        juce::AudioParameterFloat* lfoFmFreqParam = nullptr;
        juce::AudioParameterFloat* lfoFmDepthParam = nullptr;
    };
    std::array<juceLfoFmParameters, 4> juceLfoFmParams;

    foleys::MagicLevelSource* outputWtMeterOscA = nullptr;
    foleys::MagicLevelSource* outputWtMeterOscB = nullptr;
    foleys::MagicLevelSource* outputWtMeterOscC = nullptr;
    foleys::MagicLevelSource* outputWtMeterOscD = nullptr;
    foleys::MagicLevelSource* outputSamplerMeter = nullptr;
    foleys::MagicLevelSource* outputSpecMeter = nullptr;
#pragma endregion LFO_UND_FILTER

#pragma region SYNTHLAB_DECL
    struct SynthLabOscParams {
        juce::AudioParameterFloat* attackGwT = nullptr;
        juce::AudioParameterFloat* decayGwT = nullptr;
        juce::AudioParameterFloat* sustainGwT = nullptr;
        juce::AudioParameterFloat* releaseGwT = nullptr;
        juce::AudioParameterChoice* coreParam = nullptr;  // classic, morph, etc.
        juce::AudioParameterChoice* waveParam = nullptr;  // Index 0 to 15
        // the 4 mod knobs (A, B, C, D) as continuous values
        std::array<juce::AudioParameterFloat*, 4> modKnobs;

        juce::AudioParameterFloat* volumeWTParams = nullptr;
        juce::AudioParameterFloat* oscillatorShape = nullptr;
        juce::AudioParameterFloat* tuneWTParams = nullptr;
        juce::AudioParameterFloat* panWTParams = nullptr;
        juce::AudioParameterFloat* reserveWTParams = nullptr;
        juce::AudioParameterBool* isActivated = nullptr;

        juce::AudioParameterChoice* filterTypeParam = nullptr;
        juce::AudioParameterFloat* filterFreq = nullptr;
        juce::AudioParameterFloat* filterReso = nullptr;
        juce::AudioParameterFloat* attackFilterParam  = nullptr;
        juce::AudioParameterFloat* decayFilterParam   = nullptr;
        juce::AudioParameterFloat* sustainFilterParam = nullptr;
        juce::AudioParameterFloat* releaseFilterParam = nullptr;
        juce::AudioParameterFloat* filterEnvelopeAmount = nullptr;
        juce::AudioParameterBool* filterIsActivated = nullptr;

        ADSRPlot* adsrWtPtr = nullptr;
    };
    std::array<SynthLabOscParams, 4> synthLabOscParams;
    // the central database for all wavetables
    std::shared_ptr<SynthLab::WavetableDatabase> wavetableDatabase;
    std::shared_ptr<SynthLab::MidiInputData> midiInputData;
    // individual buffer for each of the 7 oscillators (for the individual Meters)
    std::array<juce::AudioBuffer<float>, 6> lfoBuffers;
    std::array<juce::AudioBuffer<float>, 4> wtOscBuffers;
    // summation buffer (submixe)
    juce::AudioBuffer<float> classicSubmixBus;
    juce::AudioBuffer<float> wtSubmixBus;
    juce::dsp::Limiter<float> safetyLimiter;
    juce::dsp::Limiter<float> safetyLimiterSpectrogram; // the reverse Spectrogramm needs a special threatment
    juce::AudioBuffer<float> dspTimerDummyBuffer { 1, 1 };
#pragma endregion SYNTHLAB_DECL

#pragma region MASTER
    // masterSound:
    MasterEQ masterEQ;
    juce::AudioParameterFloat* masterEqBass = nullptr;
    juce::AudioParameterFloat* masterEqMid = nullptr;
    juce::AudioParameterFloat* masterEqHigh = nullptr;
    // for  master-output:
    juce::AudioParameterFloat* masterVolumeMainParam = nullptr;
    juce::dsp::Gain<float> masterGain; // Der DSP-Gain-Prozessor
    foleys::MagicLevelSource* outputMeterMasterMain = nullptr;

    // filter master modulation ================================================================
    Lfos lfoMasterFilterMod;
    Lfos lfoMasterFilterModSlow; // handles the ADSR function
    FilterData filterGlitch;
    struct MasterFilterModulation {
        float baseMasterCutoff = {1000};
        juce::AudioParameterChoice* filterTypeParam = nullptr;
        juce::AudioParameterFloat* filterResoMasterModulParam = nullptr;
        juce::AudioParameterFloat* filterEnvMasterModulParam = nullptr;
        juce::AudioParameterFloat* filterDryWetMmp = nullptr;
        juce::AudioParameterFloat* envelopeFrequencyP = nullptr;
        juce::AudioParameterFloat* wobbleUpperLimitP = nullptr;
    };  MasterFilterModulation  masterFilterModulat;
    float smoothedCutoff { 1000.0f };
    float cutoffAlpha { 0.08f };
    struct LfoMasterModulat
    {
        juce::AudioParameterFloat* lfoMasterDepthParam = nullptr;
    }; LfoMasterModulat lfoMastModParam;
    juce::AudioParameterBool* glitchActiveParam = nullptr;
    //==========================================================================================
    juce::AudioBuffer<float> zeroBuffer;
    // 4 buffers for the individual wavetable oscillators (meters 1-4)
    std::array<juce::AudioBuffer<float>, 4> wtMeterSum;
    // note-counter:
    int activeNoteCount { 0 };
    juce::AudioParameterBool* bothAnalysersActiveParam = nullptr;
    Lfos lfoFmDataMaster, lfoAmDataMaster;
    juce::AudioParameterFloat* fmFreqMaster = nullptr;
    juce::AudioParameterFloat* fmDepthMaster = nullptr;
    juce::AudioParameterFloat* amFreqMaster = nullptr;
    juce::AudioParameterFloat* amDepthMaster = nullptr;
    juce::AudioParameterChoice* lfoFmWaveFormParamMaster = nullptr;
    juce::AudioParameterChoice* lfoAmWaveFormParamMaster = nullptr;

#pragma endregion MASTER

#pragma region GUI
    // for the GUI:
    juce::AudioParameterChoice* viewIndexParam = nullptr;
    juce::AudioParameterInt* viewIndexParamInt = nullptr;
    juce::AudioParameterBool* switchTimerParamBool = nullptr;
    foleys::MagicGUIBuilder* myBuilder = nullptr;
    std::unique_ptr<juce::LookAndFeel> lookAndFeelTab = nullptr;
    std::unique_ptr<juce::LookAndFeel> lookAndFeelTabV2 = nullptr;
    std::unique_ptr<juce::LookAndFeel> lookAndFeelScroll = nullptr;
    std::unique_ptr<juce::LookAndFeel> customLNF = nullptr;
    std::unique_ptr<juce::LookAndFeel> customButtonLNF = nullptr;
    std::unique_ptr<juce::LookAndFeel> customButtonSwitch = nullptr;
    std::unique_ptr<juce::LookAndFeel> customNavButtonLNF = nullptr;
    std::unique_ptr<CustomLedLNF> lookAndFeelLed = nullptr;
    std::unique_ptr<AbsynthTitelLookAndFeel> lookAndFeelTitle = nullptr;
    void InitializePlusTrigger();
    void InitializeMediumTrigger();
    void InitializeMinusTrigger();
    void TriggerKeyBoard();
    // the 3 stagers of size:
    enum class GuiScaleStages  { Max, Med, Min };
    GuiScaleStages currentScale = GuiScaleStages::Max;
    enum class PreProcessorReplacement { Standalone, VST3 };
    PreProcessorReplacement currentPreProcessorReplacement = PreProcessorReplacement::Standalone;
    static constexpr int maxGuiWidth = 1152;
    static constexpr int maxGuiHeight = 748;
    struct SwitchAdrsParams
    {
        juce::AudioParameterBool* switchGainFilterParam; // to switsch plot between ADSR's
    };
    std::array<SwitchAdrsParams, 4> switchAdrsParams;
    float currentScaleFactor = {1.0f};
    struct KeyBoardSwitch{
    juce::AudioParameterBool* ptrKeyBoardBigSmallSwitch = nullptr;
    };
    KeyBoardSwitch switchBigSmall;
    bool boolKeyBSwitch = { true };
    float convertFrom0To1(std::vector<std::pair<float, float>> controlPointsParam);
    float convertTo0To1(std::vector<std::pair<float, float>> controlPointsParam);
#pragma  endregion GUI

#pragma region SERIALIZATION_DECL
    // --- serialization: saving ---
    void triggerSaveFileDialog();
    void saveWtOscParams   (juce::ValueTree& parent) const;
    void saveWtFilterParams(juce::ValueTree& parent) const;
    void saveLfoParams(juce::ValueTree& parent) const;
    void SaveMainParams(juce::ValueTree& parent) const;
    void saveSamplerParams(juce::ValueTree& parent) const;
    void saveSpectrogramParams(juce::ValueTree& parent) const;
    void saveEnvModulationParam(juce::ValueTree& parent) const;
    void saveKeyBoardParam(juce::ValueTree& parent) const;
    void saveEffectParam(juce::ValueTree& parent) const;
    //
    std::shared_ptr<juce::FileChooser> fileChooser = nullptr;
    // --- serialization: loading ---
    void triggerLoadFileDialog();
    void triggerLoadPreset1();
    void triggerLoadPreset2();
    void triggerLoadPreset3();
    void LoadFactoryPreset(const int presetNumber);
    void loadWtOscParams   (const juce::ValueTree& parent);
    void loadWtFilterParams(const juce::ValueTree& parent);
    void loadLfoParams(const juce::ValueTree& parent);
    void loadMainParams(const juce::ValueTree& parent);
    void loadSamplerParams(const juce::ValueTree& parent);
    void loadSpectrogramParams(const juce::ValueTree& parent);
    void loadEnvModulationParam(const juce::ValueTree& parent);
    void loadEffectParam(const juce::ValueTree& parent);
    void loadKeyBoardParams(const juce::ValueTree& parent);
    juce::File presetDirectory, presetFactoryDirectory;  // whe default folders for presets
    juce::Array<juce::File> presetFiles; // list of all *.preset files in the folder
    int currentPresetIndex = -1;         // which preset is currently loaded?
    // we link this JUCE value to the PGM label for the text display
    juce::Value currentPresetNameValue;
    // felper methods
    void InitializePresetBarTriggers();
    void updatePresetList();
    juce::String currentFactoryFileName = {""};
    void cyclePreset (int direction);
    void loadMostRecentPreset();
    void selectPresetByIndex (int index);
    juce::File newestPreset;
    //= end serialization =====================================
#pragma endregion SERIALIZATION_DECL

#pragma region OTHER_STUFF
    void updateLabelColor(const juce::String& elementId, const juce::String& hexColorWithAlpha);
    static juce::ValueTree findNodeById(juce::ValueTree tree, const juce::String& targetId);

    static constexpr int maxVoices = 32; //we determine: 16 voices
    juce::Synthesiser synth;
    // an array that holds only the pointers to the specific voices:
    juce::Array<SynthVoice*> myVoices;
    juce::AudioParameterFloat* scopeTimeParam = nullptr;
    foleys::MagicAnalyser* analyser = nullptr;
    // counter to throttle (down) the graphics updates
    int samplesSinceLastUpdate = 0;

    std::unique_ptr<WindowSizeLocker> sizeLocker;
    juce::Component::SafePointer<juce::DocumentWindow> standaloneWindow;

    int _counterOscWt1 = {0};
    int _counterOscWt2 = {0};
    int limit_counter = {5};
    bool switchTimerBreak = false;
    int lastSamplerFilterType = {0};
    int lastModFilterType = {1};
    // status tracking for edge detection (On -> Off):
    bool wasAnalyserActive = false;
    int silenceBlocksToPush = 0;
    //
    std::array<int, 4> lastFilterType = {0, 0, 0, 0};
    std::array<int, 4> lastLfoFmWaveForm = {0, 0, 0, 0};
    std::array<int, 4> lastLfoAmWaveForm = {0, 0, 0, 0};
    std::array<bool, 4> lastIswtActive = {true, true, true, true};

#pragma endregion OTHER_STUFF

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor);
};
//=================================================================================

