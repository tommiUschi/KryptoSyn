#include "FormantFilter.h"
#include <cmath>

// rough reference values for F1/F2/F3 in Hz, plus relative gains per band.
// order: A, E, I, O, U
const std::array<FormantFilter::VowelFormants, FormantFilter::numVowels> FormantFilter::vowelTable
{{
    { {  700.f, 1220.f, 2600.f }, { 1.0f, 0.6f,  0.3f  } }, // A
    { {  400.f, 1700.f, 2600.f }, { 1.0f, 0.5f,  0.25f } }, // E
    { {  300.f, 2300.f, 3000.f }, { 1.0f, 0.4f,  0.2f  } }, // I
    { {  500.f,  900.f, 2600.f }, { 1.0f, 0.6f,  0.25f } }, // O
    { {  350.f,  700.f, 2600.f }, { 1.0f, 0.5f,  0.2f  } }, // U
}};

void FormantFilter::prepare (const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    // similar to FilterData::prepareFilterToPlay(): each channel gets a
    // dedicated mono-prepared filter chain.
    juce::dsp::ProcessSpec monoSpec = spec;
    monoSpec.numChannels = 1;

    for (auto& channelBands : bandsPerChannel)
    {
        for (auto& band : channelBands)
        {
            band.filter.prepare (monoSpec);
            band.filter.reset();
        }
    }

    updateCoefficients();
}

void FormantFilter::reset()
{
    for (auto& channelBands : bandsPerChannel)
        for (auto& band : channelBands)
            band.filter.reset();
}

void FormantFilter::updateFilterFrequency (float freq)
{
    const float safeFreq = juce::jlimit (20.0f, 20000.0f, freq);

    // instead of normalizing over the full 20-20000 Hz range (of which the
    // ADSR-modulated segment from SynthVoice only a small of the value range
    // is used), here we map to a narrower, more practical area -- so
    // the envelope actually traverses the full range of vowels.
    constexpr float mapMinHz = 150.0f;
    constexpr float mapMaxHz = 6000.0f;
    const float clamped = juce::jlimit (mapMinHz, mapMaxHz, safeFreq);
    const float normalized = std::log (clamped / mapMinHz) / std::log (mapMaxHz / mapMinHz);
    setVowelMorph (normalized * static_cast<float> (numVowels - 1));

}

void FormantFilter::setVowelMorph (float morphPosition)
{
    currentMorph = juce::jlimit (0.0f, static_cast<float> (numVowels - 1), morphPosition);
    updateCoefficients();
}

void FormantFilter::updateFilterResonance (float resonance)
{
    // the valid parameter range is 1–10 (shared with the other filter types).
    // to achieve a more pronounced vowel character, we internally map this to a
    // higher Q range -- the controller remains unchanged for the user,
    // but sounds noticeably sharper with the formant filter.
    const float mapped = juce::jmap (juce::jlimit (1.0f, 10.0f, resonance),
                                      1.0f, 10.0f, 6.0f, 22.0f);
    q = juce::jlimit (0.1f, 30.0f, mapped);
    updateCoefficients();
}

float FormantFilter::process (float sample, int channel)
{
    const int ch = juce::jlimit (0, 1, channel);
    float output = 0.0f;

    for (auto& band : bandsPerChannel[static_cast<size_t> (ch)])
        output += band.filter.processSample (sample) * band.gain;

    return output * outputGain;
}

void FormantFilter::updateCoefficients()
{
    // interpolate between two adjacent vowels.
    const int   indexLow  = static_cast<int> (currentMorph);
    const int   indexHigh = juce::jmin (indexLow + 1, numVowels - 1);
    const float fraction  = currentMorph - static_cast<float> (indexLow);

    const auto& vowelLow  = vowelTable[static_cast<size_t> (indexLow)];
    const auto& vowelHigh = vowelTable[static_cast<size_t> (indexHigh)];

    for (int i = 0; i < numBands; ++i)
    {
        const auto idx = static_cast<size_t> (i);

        const float freq = juce::jmap (fraction, vowelLow.freqs[idx], vowelHigh.freqs[idx]);
        const float gain = juce::jmap (fraction, vowelLow.gains[idx], vowelHigh.gains[idx]);

        // the coefficients are identical for both channels (only the filter history
        // is separate within the band object itself.) -- a shared Coefficients::Ptr
        // suffices; no redundant calculation needed.
        auto coeffs = juce::dsp::IIR::Coefficients<float>::makeBandPass (sampleRate, freq, q);

        for (auto& channelBands : bandsPerChannel)
        {
            channelBands[idx].gain = gain;
            channelBands[idx].filter.coefficients = coeffs;
        }
    }

    // automatic volume compensation: the higher the sum of the band gains,
    // the more the result is reduced.
    float gainSum = 0.0f;
    for (auto& band : bandsPerChannel[0])
        gainSum += band.gain;

    outputGain = gainSum > 0.0f ? (4.8f / gainSum) : 1.0f;
}