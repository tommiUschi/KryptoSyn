//
// Created by tommibe on 22.03.26.
//

#pragma once

#include <juce_dsp/juce_dsp.h>
#include "FilterBase.h"

/**
 * handle the data for 3 filter-types
 */

class FilterData : public FilterBase
{
public:
    void prepareFilterToPlay(juce::dsp::ProcessSpec spec);
    float processSample(int channel, float inputValue);
    void process(juce::AudioBuffer<float>& buffer);

    float process(float sample);
    void setFilterType(int type);
    void updateFilterParameters(float frequency, float resonance);
    void updateFilterFrequency(float frequency) override;
    void updateFilterResonance(float resonance) override;
    void reset() override;

    void prepare (const juce::dsp::ProcessSpec& spec) override { prepareFilterToPlay (spec); }
    void setSubType (int type) override { setFilterType (type); }

    float process (float sample, int channel) override { return processSample (channel, sample); }

private:
    // IMPORTANT: two separate filter instances for left (0) and right (1)
    juce::dsp::StateVariableTPTFilter<float> filters[2];
    // caching variables for performance
    float lastFrequency = -1.0f;
    float lastResonance = -1.0f;
};
