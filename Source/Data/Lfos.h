//
// Created by tommibe on 13.02.26.
//

#pragma once

//#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
//#include "../SDKs/SynthLab/source/lfo.h"

/**
 * Class for ther LFO'S - multiple usages
 */

class Lfos : public juce::dsp::Oscillator<float>
{
public:
    void setLfoWave (int mode);
    void prepareToPlayOsc(juce::dsp::ProcessSpec spec);
    void RenderFmLfoInLocalBuffer(const juce::dsp::AudioBlock<float>& subBlock);
    void RenderAmLfoInLocalBuffer(const juce::dsp::AudioBlock<float>& subBlock);
    void setWaveType(int type);
    void setWaveFrequenzy(const int midiNoteNumber);

    float getNextFMSample();
    float getNextAMSample();
    void setFMParams(float freq, float depth);
    void setAMParams(float freq, float depth);
    void setAMFrequency(float freq);

private:
    juce::dsp::Oscillator<float> osc;
    float carrierPhaseFm {0.0f};
    float carrierPhaseAm {0.0f};

    // own LFO phases and saved frequencies
    float fmLfoPhase  = 0.0f;
    float amLfoPhase  = 0.0f;
    float fmFrequency = 1.0f; // is stored in setFMParams
    float amFrequency = 1.0f; // is stored in setAMParams

    // for frequenzy-modulation:
    juce::dsp::Oscillator<float> oscFM {[](float x) { return std::sin (x); }};
    // for amplitude modulation:
    juce::dsp::Oscillator<float> oscAM {[](float x) { return std::sin (x); }};
    float fmMod {0.0f}; float fmDepth {0.0f};
    float amMod {0.0f}; float amDepth {0.0f};
    // to save the current OSC mode to avoid unnecessary re-initializations:
    double currentSampleRate { 44100.0 };
    int lastModeFm = -1;
    int lastModeAm = -1;
    int lastMidiNoteNumber {0};
};



