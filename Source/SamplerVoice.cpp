//
// Created by tommibe on 13.02.26.
//

#include "SamplerVoice.h"
#include "PluginProcessor.h"



void SamplerVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int)
{
    noteVelocity = velocity;
    samplePointer = 0.0f;

    if (auto* zoneSound = dynamic_cast<SamplerZoneSound*> (sound))
    {
        activeSound = zoneSound;
        int noteDelta = midiNoteNumber - zoneSound->rootMidiNote;
        pitchFactor = std::pow (2.0f, static_cast<float> (noteDelta) / 12.0f);
        // start envelopes!:
        sampleGainAdsr.noteOn();
        sampleFilterAdsr.noteOn();
        samplerFilter->reset();

        /*std::cout << "[Voice Start] Note: " << midiNoteNumber
          << " | Root: " << zoneSound->rootMidiNote
          << " | Speed-Faktor: " << pitchFactor
          << " | Buffer-Dauer (s): " << (zoneSound->audioBuffer.getNumSamples() / zoneSound->originalSampleRate)
          << std::endl;*/
    }
    else {
        activeSound = nullptr;
    }
}

void SamplerVoice::stopNote (float, bool allowTailOff)
{
    //std::cout << "[Voice Stop] NoteOff empfangen (allowTailOff=" << (allowTailOff ? "true" : "false") << ")" << std::endl;
    if (allowTailOff) {
        sampleGainAdsr.noteOff();
        sampleFilterAdsr.noteOff();
    }
    else {
        sampleGainAdsr.reset();
        sampleFilterAdsr.reset();
        clearCurrentNote();
        activeSound = nullptr;
    }
}


void SamplerVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    sampleGainAdsr.setSampleRate (sampleRate);

    juce::ADSR::Parameters samplGparams;
    samplGparams.attack = 0.001f;
    samplGparams.decay = 0.9f;
    samplGparams.sustain = 0.05f;
    samplGparams.release = 0.2f;
    sampleGainAdsr.setParameters(samplGparams);
    sampleGainAdsr.reset();
    sampleFilterAdsr.setSampleRate (sampleRate);

    juce::ADSR::Parameters samplFparams;
    samplFparams.attack = 0.001f;
    samplFparams.decay = 0.9f;
    samplFparams.sustain = 0.05f;
    samplFparams.release = 0.2f;
    sampleFilterAdsr.setParameters(samplFparams);
    sampleFilterAdsr.reset();

    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = static_cast<uint32_t>(samplesPerBlock);
    spec.numChannels      = static_cast<uint32_t>(outputChannels);
    currentFilterSpec = spec; // save for later slot changes

    if (samplerFilter == nullptr)
        samplerFilter = std::make_unique<FilterData>();

    samplerFilter->prepare (spec);
}

void SamplerVoice::setFilterType (int type)
{
    const bool wantFormant = (type == 3); // 3 = "formant"
    const bool isFormant   = dynamic_cast<FormantFilter*> (samplerFilter.get()) != nullptr;

    if (wantFormant != isFormant)
    {
        /*
        * Slot type is changing—replacing the old instance.
        NOTE: This can potentially happen while active voices are playing.
        Without precautions, this could cause an audible click because the
        new filter instance starts at 0 instead of picking up from the previous state.
        It's functionally fine for now; let me know if you notice any clicks,
        and we can implement something like a short crossfade during the slot switch.
         */
        if (wantFormant)
            samplerFilter = std::make_unique<FormantFilter>();
        else
            samplerFilter = std::make_unique<FilterData>();

        samplerFilter->prepare (currentFilterSpec);
        samplerFilter->reset();
    }

    if (!wantFormant)
        samplerFilter->setSubType (type);
}


#pragma region RENDER_NEXT_BLOCK
void SamplerVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (activeSound == nullptr) return;

    auto* zone = dynamic_cast<SamplerZoneSound*> (activeSound.get());
    if (zone == nullptr) return;

    const auto& srcBuffer = zone->audioBuffer;
    const int srcSamples = srcBuffer.getNumSamples();
    const int srcChannels = srcBuffer.getNumChannels();
    const int destChannels = outputBuffer.getNumChannels();

    if (srcSamples <= 0 || srcChannels <= 0)
    {
        sampleGainAdsr.reset();
        sampleFilterAdsr.reset();
        clearCurrentNote();
        activeSound = nullptr;
        return;
    }

    double synthSampleRate = getSampleRate();
    if (synthSampleRate <= 0.0) synthSampleRate = 44100.0;

    float baseGain = juce::Decibels::decibelsToGain (volumeParam->get(), -60.0f) * noteVelocity;
    float speed = pitchFactor * static_cast<float> (zone->originalSampleRate / synthSampleRate);
    baseGain = baseGain / 2.0f;

    if (std::isnan (speed) || std::isinf (speed) || speed <= 0.0f)
    {
        sampleGainAdsr.reset();
        sampleFilterAdsr.reset();
        clearCurrentNote();
        activeSound = nullptr;
        return;
    }

    // --- SUB-BLOCK RENDERING (ANALOG TO SYNTHVOICE) ---
    const int kWtBlockSize = 64;
    int samplesRemaining = numSamples;
    int outputOffset = 0;

    while (samplesRemaining > 0)
    {
        const uint32_t samplesToRender = static_cast<uint32_t> (std::min (samplesRemaining, kWtBlockSize));

        if (filterIsEnabled)
        {
            // ── filter-ADSR: calculate the average value for this sub-block ──
            float filterEnvVal = 0.0f;
            for (uint32_t s = 0; s < samplesToRender; ++s)
                filterEnvVal += sampleFilterAdsr.getNextSample();
            filterEnvVal /= static_cast<float> (samplesToRender);

            // exponential cutoff modulation (for a noticeable frequency shift)
            float envCents = filterEnvVal * filterEnvAmount;
            const float modFilterFreq = juce::jlimit (
                20.0f,
                20000.0f,
                filterFreq * std::pow (2.0f, envCents / 1200.0f)
            );

            // Dynamic resonance modulation!
            // the further the envelope opens, the sharper (more resonant) the filter becomes.
            // We take the base resonance from the filter and add a portion of the envelope to it
            const float modFilterReso = juce::jlimit
                (1.0f,
                10.0f,
                filterReso + (filterEnvVal * 0.3f));

            // Pass both values to the filter
            samplerFilter->updateFilterFrequency(modFilterFreq);
            samplerFilter->updateFilterResonance(modFilterReso);
        }

        for (uint32_t s = 0; s < samplesToRender; ++s)
        {
            int idxCurrent = static_cast<int> (samplePointer);

            // puffer-Ende / Looping-Behandlung
            if (zone->isLooping && sampleGainAdsr.isActive() && zone->loopEnd > zone->loopStart)
            {
                if (idxCurrent >= zone->loopEnd)
                {
                    // jump seamlessly back to the loop start point
                    float loopLen = static_cast<float> (zone->loopEnd - zone->loopStart);
                    samplePointer -= loopLen;
                    if (samplePointer < static_cast<float> (zone->loopStart))
                        samplePointer = static_cast<float> (zone->loopStart);
                    idxCurrent = static_cast<int> (samplePointer);
                }
            }
            else
            {
                // one-shot behavior (drums/percussion) OR note released and buffer finished
                if (idxCurrent < 0 || idxCurrent >= srcSamples)
                {
                    sampleGainAdsr.reset();
                    sampleFilterAdsr.reset();
                    clearCurrentNote();
                    activeSound = nullptr;
                    return; // stop voice completely
                }
            }

            float gainEnv = sampleGainAdsr.getNextSample();

            // volume envelope has fully completed (tail-off finished)
            if (!sampleGainAdsr.isActive())
            {
                sampleFilterAdsr.reset();
                clearCurrentNote();
                activeSound = nullptr;
                return; // stop voic
            }

            int idxNext = idxCurrent + 1;
            if (zone->isLooping && idxNext >= zone->loopEnd)
                idxNext = zone->loopStart;
            else if (idxNext >= srcSamples)
                idxNext = idxCurrent;

            float alpha = samplePointer - static_cast<float> (idxCurrent);
            float finalGain = baseGain * gainEnv;

            for (int ch = 0; ch < destChannels; ++ch)
            {
                int srcCh = ch % srcChannels;
                float s1 = srcBuffer.getSample (srcCh, idxCurrent);
                float s2 = srcBuffer.getSample (srcCh, idxNext);
                float sampleVal = (s1 + alpha * (s2 - s1)) * finalGain;
                if (filterIsEnabled)
                {
                    sampleVal = samplerFilter->process (sampleVal, ch);
                }
                outputBuffer.addSample (ch, startSample + outputOffset + s, sampleVal);
            }
            samplePointer += speed;
        }
        samplesRemaining -= static_cast<int> (samplesToRender);
        outputOffset     += static_cast<int> (samplesToRender);
    }
}
#pragma endregion

void SamplerVoice::updateGainAdsr(const float attack, const float decay, const float sustain, const float release)
{
    sampleGainAdsr.updateADSR(attack, decay, sustain, release);
}



void SamplerVoice::updateFilterAdsr(const float attack, const float decay, const float sustain, const float release)
{
    juce::ADSR::Parameters filterParams;
    filterParams.attack = attack;
    filterParams.decay = decay;
    filterParams.sustain = sustain;
    filterParams.release = release;
    sampleFilterAdsr.setParameters(filterParams);
}

void SamplerVoice::updateFilterParams(const float filterFreqParam, const float filterResoParam, const float filtEnvAmParam)
{
    filterFreq = filterFreqParam; filterReso = filterResoParam;
    filterEnvAmount= filtEnvAmParam;
    samplerFilter->updateFilterFrequency(filterFreqParam);
    samplerFilter->updateFilterResonance(filterResoParam);
}


void SamplerVoice::setFilterEnabled(const bool active)
{
    bool currentActivationState = active;
    if (currentActivationState != lastFilterIsEnabled)
    {
        filterIsEnabled = currentActivationState;
        samplerFilter->reset();
    }
    lastFilterIsEnabled = currentActivationState;
}

void SamplerVoice::updateVolumenParam(float vol)
{
    *volumeParam = vol;
}



