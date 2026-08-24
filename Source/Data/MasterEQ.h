//
// Created by tommibe on 13.02.26.
//

#pragma once

#include <juce_dsp/juce_dsp.h>
#include <cmath>

//for the global master controls
//ToDo: it does not work yet
class MasterEQ
{
public:
    MasterEQ() = default;
    ~MasterEQ() = default;

    void prepare(const juce::dsp::ProcessSpec& spec);
    void updateEQParameters(float bassDb, float midDb, float highDb);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

private:
    double currentSampleRate = 44100.0;

    // 3 bands x 2 channels (stereo)
    juce::dsp::IIR::Filter<float> lowShelfFilters[2];
    juce::dsp::IIR::Filter<float> midPeakFilters[2];
    juce::dsp::IIR::Filter<float> highShelfFilters[2];

    // caching against unnecessary recalculation in the audio thread
    float lastBassDb = -999.0f;
    float lastMidDb  = -999.0f;
    float lastHighDb = -999.0f;
};



