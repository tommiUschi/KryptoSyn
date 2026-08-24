//
// Created by tommibe on 19.08.26.
//

#pragma once

#include <juce_dsp/juce_dsp.h>

/**
    A common interface for all filter types intended to be swappable via the combo
    box at the voice or channel level (e.g., FilterData, FormantFilter).

    Deliberately kept lightweight to minimize the overhead of virtual dispatch
    within the sample loop.
*/

class FilterBase
{
public:
    virtual ~FilterBase() = default;

    // it's named intentionally "prepare"—not "prepare FilterToPlay" like in "FilterData" --
    // filterData receives a thin wrapper for this purpose (see FilterData.h/.cpp),
    // so that existing direct calls to prepareFilterToPlay() continue to run unchanged.
    virtual void prepare (const juce::dsp::ProcessSpec& spec) = 0;
    virtual void reset() = 0;

    // currently, all 64 samples are accessed (filter ADSR block).
    // for FilterData: Cutoff frequency in Hz.
    // for FormantFilter: repurposed as vowel morph position 0..(numVowels-1).
    virtual void updateFilterFrequency (float frequency) = 0;

    // for FilterData: Resonance/Q.
    // for FormantFilter: Q, shared across all formant bands.
    virtual void updateFilterResonance (float resonance) = 0;

    // per-sample processing. Channel: 0 = left/mono, 1 = right.
    // important for filters with internal state (feedback/history, e.g., resonance)
    virtual float process (float sample, int channel) = 0;

    // for filter types with an internal sub-selection (e.g., FilterData: lowpass/bandpass/highpass).
    // default = no-op, so that FormantFilter (no subtype) does not have to overwrite anything.
    virtual void setSubType (int type) { juce::ignoreUnused (type); }
};
