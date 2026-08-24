//
// Created by tommibe on 13.02.26.
//



#include "FilterData.h"
//#include "juce_dsp/juce_dsp.h"

void FilterData:: prepareFilterToPlay(juce::dsp::ProcessSpec spec)
{
    // the spec we get is mostly for stereo (numChannels = 2).
    // since we split the instances manually, we prepare both with a mono spec
    juce::dsp::ProcessSpec monoSpec = spec;
    monoSpec.numChannels = 1;

    filters[0].reset();
    filters[0].prepare(monoSpec);

    filters[1].reset();
    filters[1].prepare(monoSpec);

    lastFrequency = -1.0f;
}


float FilterData::processSample (int channel, float inputValue)
{
    // making sure we don't fly out of bounds (0 = Left, 1 = Right)
    const int ch = juce::jlimit(0, 1, channel);
    // each sample goes into ITS OWN filter history!
    return filters[ch].processSample (0, inputValue);
}

void FilterData::process(juce::AudioBuffer<float>& buffer) {
    juce::dsp::AudioBlock<float> block {buffer};
    const int numChannels = buffer.getNumChannels();
    // channel 0 (left) passed separately to filters[0]
    if (numChannels > 0) {
        auto leftBlock = block.getSingleChannelBlock(0);
        filters[0].process(juce::dsp::ProcessContextReplacing<float> { leftBlock });
    }
    // channel 1 (Right) passed separately to filters[1]
    if (numChannels > 1){
         auto rightBlock = block.getSingleChannelBlock(1);
        filters[1].process(juce::dsp::ProcessContextReplacing<float> { rightBlock });
    }
}

float FilterData::process(float sample)
{
    return filters[0].processSample(0, sample);
}


void FilterData::setFilterType(const int type)
{
    // set type for both filter instances
    for (auto& f : filters)
    {
        switch (type)
        {
        case 0: f.setType(juce::dsp::StateVariableTPTFilterType::lowpass);  break;
        case 1: f.setType(juce::dsp::StateVariableTPTFilterType::bandpass); break;
        case 2: f.setType(juce::dsp::StateVariableTPTFilterType::highpass); break;
        default: f.setType(juce::dsp::StateVariableTPTFilterType::lowpass); break;
        }
    }
}

void FilterData::updateFilterParameters(float frequency, const float resonance)
{
    updateFilterFrequency(frequency);
    updateFilterResonance(resonance);
}

void FilterData::updateFilterFrequency(float frequency)
{
    // protection against invalid frequencies (Nyquist safety)
    const float safeFreq = juce::jlimit(20.0f, 20000.0f, frequency);

    // passed directly to the TPT filter at audio rate for modulation
    filters[0].setCutoffFrequency(safeFreq);
    filters[1].setCutoffFrequency(safeFreq);
    lastFrequency = safeFreq;
}

void FilterData::updateFilterResonance(const float resonance)
{
    const float safeResonance = juce::jlimit(0.1f, 15.0f, resonance);

    if (std::abs(safeResonance - lastResonance) > 0.01f)
    {
        filters[0].setResonance(safeResonance);
        filters[1].setResonance(safeResonance);
        lastResonance = safeResonance;
    }
}

void FilterData::reset()
{
    filters[0].reset();
    filters[1].reset();
}





