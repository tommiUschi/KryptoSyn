//
// Created by tommibe on 13.02.26.
//
#pragma once

#include <juce_dsp/juce_dsp.h>
#include "Data/AdsrData.h"
#include "Data/FilterData.h"
#include "Data/FormantFilter.h"
#include "Data/Lfos.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>

/**
 * This class is the equivalent of SynthVoice, but for
 * samples instead of SynthLab's wavetable oscillators.
 */

// the structure is now located HERE in the global namespace
struct SamplerZone
{
    juce::File file;
    int rootNote = 60;
    juce::AudioBuffer<float> audioBuffer;
    double originalSampleRate = 44100.0;
};

// a reference-counted sound that protects the audio buffer packet in RAM
class SamplerZoneSound : public juce::SynthesiserSound
{
public:
    SamplerZoneSound (int rootNote, juce::AudioBuffer<float>&& bufferToTake, double sampleRate)
        : rootMidiNote (rootNote), audioBuffer (std::move (bufferToTake)), originalSampleRate (sampleRate) {}

    bool appliesToNote (int midiNoteNumber) override
    {
        return midiNoteNumber >= minMidiNote && midiNoteNumber <= maxMidiNote;
    }

    bool appliesToChannel (int) override { return true; }

    int rootMidiNote;
    int minMidiNote = 0;
    int maxMidiNote = 127;
    juce::AudioBuffer<float> audioBuffer;
    double originalSampleRate;

    // loop properties:
    bool isLooping = false;
    int loopStart = 0;
    int loopEnd = 0;

    using Ptr = juce::ReferenceCountedObjectPtr<SamplerZoneSound>;
};

class SamplerVoice : public juce::SynthesiserVoice
{
public:
    SamplerVoice (juce::AudioParameterFloat* volParam) : volumeParam (volParam) {}

    // --- ANALOG TO SYNTHVOICE ---
    void prepareToPlay (double sampleRate, int samplesPerBlock, int outputChannels);
    void updateGainAdsr (const float attack, const float decay, const float sustain, const float release);
    // central location for type changes — replaces getFilter(inx).setFilterType(...)
    void setFilterType (int type);
    void updateFilterAdsr (const float attack, const float decay, const float sustain, const float release);
    //

    void updateFilterParams (float filterFreqParam, float filterResoParam, float filtEnvAmParam);
    void setFilterEnabled(bool active);
    void updateVolumenParam(float vol);

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<SamplerZoneSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int) override;
    void stopNote (float, bool allowTailOff) override;

    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

private:
    juce::AudioParameterFloat* volumeParam;
    juce::dsp::ProcessSpec currentFilterSpec;
    std::unique_ptr<FilterBase> samplerFilter;
    AdsrData sampleGainAdsr;
    AdsrData sampleFilterAdsr;
    bool filterIsEnabled = {true};
    bool lastFilterIsEnabled = {true};
    float filterFreq = {0.0f};
    float filterReso = {0.0f};
    float filterEnvAmount = {0.0f};
    // IMPORTANT: A smart pointer! protects the object from being deleted by the GUI thread
    juce::SynthesiserSound::Ptr activeSound;
    float samplePointer = 0.0f;
    float pitchFactor = 1.0f;
    float noteVelocity = 0.0f;
};



