//
// Created by tommibe on 13.02.26.
//

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

/**
 to update the adsr data
 */

class AdsrData:public juce::ADSR
{
public:
    void updateADSR(const float attack, const float decay, const float sustain, const float release);
private:
    juce::ADSR::Parameters adsrParams;
    float lastAttack = -1.0f, lastDecay = -1.0f, lastSustain = -1.0f, lastRelease = -1.0f;

};
