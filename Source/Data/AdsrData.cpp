//
// Created by tommibe on 13.02.26.
//

#include "AdsrData.h"
//#include "juce_dsp/juce_dsp.h"

/**
 to update the adsr data
 */

void AdsrData::updateADSR(const float attack, const float decay, const float sustain, const float release)
{
    // ptimization: only update if something has changed
    if (lastAttack == attack && lastDecay == decay &&
        lastSustain == sustain && lastRelease == release)
        return;
    juce::ADSR::Parameters adsrParams;
    adsrParams.attack  = attack;
    adsrParams.decay   = decay;
    adsrParams.sustain = sustain;
    adsrParams.release = release;

    setParameters(adsrParams);
}


