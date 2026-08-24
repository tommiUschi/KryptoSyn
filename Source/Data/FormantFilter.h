//
// Created by tommibe on 13.08.26.
//

#pragma once

#include "FilterBase.h"
#include <array>

/**
    Handles the Data for the FormantFilter

    Formant Filter: Several parallel bandpass filters whose
    center frequencies/gains are continuously interpolated between 5 vowel presets
    (A, E, I, O, U).

    Control parameter: Internally, a vowel morph position in the range [0, numVowels-1].
    0.0 = pure "A", 0.5 = exactly between A and E, 1.0 = pure "E", etc.

    Externally (FilterBase interface), FormantFilter behaves like FilterData:
    updateFilterFrequency() expects an actual frequency in Hz (20...20000) and
    maps it logarithmically to the morph position internally—so the same
    FilterFreq parameter/control can be used unchanged for both filter types.

    updateFilterResonance() controls Q, also 1:1 like FilterData.

    Volume compensation: the sum of the parallel added bands would
    result in different volumes depending on the vowel and Q (bandpass filters only allow a narrow range to pass through -> less overall energy than with a
    low-pass/high-pass filter of the same order). Instead of a hard-coded gain factor,
    the output volume is automatically normalized based on the sum of the currently active
    band gains (see updateCoefficients()/outputGain) -- it adjusts
    automatically if the vowel table or Q is later modified.

    setVowelMorph() remains available as a direct access if you later want to control the morph
    independently of a Hz value (e.g., your own).
    Modulationsziel-Eintrag statt ueber den bestehenden FilterFreq-Parameter).
*/
class FormantFilter : public FilterBase
{
public:
    static constexpr int numBands  = 3; // F1, F2, F3
    static constexpr int numVowels = 5; // A, E, I, O, U

    void prepare (const juce::dsp::ProcessSpec& spec) override;
    void reset() override;

    // frequency: ACTUAL frequency in Hz (20–20,000); see class comment.
    void updateFilterFrequency (float frequency) override;

    // Q, jointly for all three formant bands
    void updateFilterResonance (float resonance) override;

    // channel: 0 = left/mono, 1 = right -- separate internal state per channel,
    // analogous to FilterData::processSample()
    float process (float sample, int channel) override;

    // direct, more descriptive access — internally identical to updateFilterFrequency()
    void setVowelMorph (float morphPosition);

private:
    struct Band
    {
        juce::dsp::IIR::Filter<float> filter;
        float gain = 1.0f;
    };

    struct VowelFormants
    {
        std::array<float, numBands> freqs;
        std::array<float, numBands> gains;
    };

    static const std::array<VowelFormants, numVowels> vowelTable;

    // separate bands per channel (0 = left/mono, 1 = right) so that the
    // filter history is not mixed between L and R — see chat discussion.
    // the coefficients (frequency/gain) are per band index for both channels.
    // identical; only the internal state (history) is separate.
    std::array<std::array<Band, numBands>, 2> bandsPerChannel;

    double sampleRate    = 44100.0;
    float  q             = 10.0f;
    float  currentMorph  = 0.0f;
    float  outputGain    = 1.0f; // automatic volume compensation, see class comment
    void updateCoefficients();
};