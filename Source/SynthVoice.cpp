//
// Created by tommibe on 13.02.26.
//

#include "SynthVoice.h"
#include "SamplerVoice.h"
#include "fourierwtcore.h"
#include "morphwtcore.h"
#include "sfxwtcore.h"
#include "juce_dsp/juce_dsp.h"
#include "../SDKs/SynthLab/source/synthstructures.h"


void SynthVoice::setWavetableDatabase(const std::shared_ptr<SynthLab::MidiInputData>& _midiData,
        std::shared_ptr<SynthLab::WavetableDatabase> db) {
    _db = db;
    midiData = _midiData;
    uint32_t blockSize = 64;
    for (int inx = 0; inx < 4; inx++) {
        wtParams[inx] = std::make_shared<SynthLab::WTOscParameters>();
        // create oscillator
        wtOscillators[inx] = std::make_shared<SynthLab::WTOscillator>(midiData, wtParams[inx], db, blockSize);
        // register core
        const auto classicCore = std::make_shared<SynthLab::ClassicWTCore>();
        const auto morphingCore = std::make_shared<SynthLab::MorphWTCore>();
        const auto sfxCore = std::make_shared<SynthLab::SFXWTCore>();
        const auto furrierCore = std::make_shared<SynthLab::FourierWTCore>();
        wtOscillators[inx]->addModuleCore(std::static_pointer_cast<SynthLab::ModuleCore>(classicCore));
        wtOscillators[inx]->addModuleCore(std::static_pointer_cast<SynthLab::ModuleCore>(morphingCore));
        wtOscillators[inx]->addModuleCore(std::static_pointer_cast<SynthLab::ModuleCore>(sfxCore));
        wtOscillators[inx]->addModuleCore(std::static_pointer_cast<SynthLab::ModuleCore>(furrierCore));
        wtOscillators[inx]->reset(getSampleRate());
    }
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

void SynthVoice::startNote(const int midiNoteNumber, const float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    for (int ind = 0; ind < 4; ind++)
    {
        SynthLab::MIDINoteEvent noteEvent;
        noteEvent.midiNoteNumber = midiNoteNumber;
        // frequency is calculated as: f = 440 * 2^((n-69)/12):
        noteEvent.midiPitch = SynthLab::midiNoteNumberToOscFrequency(midiNoteNumber);
        noteEvent.midiNoteVelocity = static_cast<uint32_t> (velocity * 127.0f);
        gainWtAdsrs[ind].noteOn();
        filterAdsrs[ind].noteOn();
        filters[ind]->reset();
        if (wtOscillators[ind] != nullptr)
            wtOscillators[ind]->doNoteOn (noteEvent);
    }
}


void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    if (allowTailOff)
    {
        for (int i = 0; i < 4; i++){
            gainWtAdsrs[i].noteOff();
            filterAdsrs[i].noteOff();
            // Notify SynthLab oscillator (velocity 0 signals note-off)
            SynthLab::MIDINoteEvent noteEvent;
            noteEvent.midiNoteNumber = getCurrentlyPlayingNote();
            noteEvent.midiNoteVelocity = 0;
            wtOscillators[i]->doNoteOff(noteEvent);
        }
        // IMPORTANT: Do NOT use clearCurrentNote() here,
        // this only happens in renderToIndividualBuffers() when all ADSRs are inactive.
    } else {
        // hard termination (e.g., voice stealing): Kill everything immediately
        for (int i = 0; i < 4; i++) { gainWtAdsrs[i].reset(); filterAdsrs[i].reset(); }
        clearCurrentNote(); // release note for JUCE immediately
    }
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{

}

void SynthVoice::pitchWheelMoved(int newPitchWheelPosition)
{

}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    for (int ind = 0; ind < 4; ++ind) {
        gainWtAdsrs[ind].setSampleRate(sampleRate);
        juce::ADSR::Parameters wTparams;
        wTparams.attack = 0.001f;
        wTparams.decay = 0.9f;
        wTparams.sustain = 0.05f;
        wTparams.release = 0.2f;
        gainWtAdsrs[ind].setParameters(wTparams);
        gainWtAdsrs[ind].reset();
        if (wtOscillators[ind]!= nullptr) {
            wtOscillators[ind]->reset(sampleRate);
        }
        filterAdsrs[ind].setSampleRate(sampleRate);
        juce::ADSR::Parameters filterParams;
        filterParams.attack  = 0.01f;
        filterParams.decay   = 0.8f;
        filterParams.sustain = 0.5f;
        filterParams.release = 1.0f;
        filterAdsrs[ind].setParameters(filterParams);
        filterAdsrs[ind].reset();
    }

    // prepare filter
    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = static_cast<uint32_t>(samplesPerBlock);
    spec.numChannels      = static_cast<uint32_t>(outputChannels);
    currentFilterSpec = spec; // fuer spaetere Slot-Wechsel merken

    for (int ind = 0; ind < 4; ++ind)
    {
        // on the very first call, no instance exists yet -> create default
        // For every subsequent prepareToPlay() (e.g., sample rate change),
        // the currently selected filter type is retained but re-prepared.
        if (filters[ind] == nullptr)
            filters[ind] = std::make_unique<FilterData>();

        filters[ind]->prepare (spec);
    }
    mixBuffer.setSize (outputChannels, samplesPerBlock); // WICHTIG!

    for (int i = 0; i < 4; ++i){
        wtMeterBuffers[i].setSize(outputChannels, samplesPerBlock);
    }
    isPrepared = true;
}

#pragma region RENDER_NEXT_BLOCK_SYNTH
void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer,
                                  const int startSample, const int numSamples)
{
    jassert(isPrepared);
    if (!isVoiceActive()) return;

    mixBuffer.clear();
    const uint32_t kWtBlockSize = 64;

    for (int ind = 0; ind < 4; ++ind)
    {
        if (wtOscillators[ind] == nullptr) continue;
        wtMeterBuffers[ind].clear();

        // render only active channels
        if (!wtActive[ind] || wtOscillators[ind] == nullptr)
            continue;

        int samplesRemaining = numSamples;
        int outputOffset     = 0;

        while (samplesRemaining > 0)
        {
            const uint32_t samplesToRender = static_cast<uint32_t>(
                std::min(samplesRemaining, static_cast<int>(kWtBlockSize)));

                // ── 1. FM: fineDetune modulate once per sub-block ─────────────
                // (LFO < 20 Hz, 64 samples granularity is entirely sufficient)
                {
                    const float fmSignal = lfoFmData[ind].getNextFMSample();
                    const float fmCents  = fmSignal * fmDepth[ind];
                    auto p = wtOscillators[ind]->getParameters();

                    // Kanal-FM + Master-FM zusammenrechnen:
                    p->fineDetune = baseTune[ind] + fmCents + masterFmCents;
                    wtOscillators[ind]->update();
                }

                // ── 2. render WaveTable  ──────────────────────────────────────────
                wtOscillators[ind]->render(samplesToRender);

                auto wtAudioBuffers = wtOscillators[ind]->getAudioBuffers();
                if (wtAudioBuffers == nullptr) break;

                float* wtLeft  = wtAudioBuffers->getOutputBuffer(0);
                float* wtRight = wtAudioBuffers->getOutputBuffer(1);
                if (wtLeft == nullptr) break;

                // ── 3. Filter-ADSR: average value for this sub-block ────────
                // setCutoffFrequency ist expensive (tan, sqrt) → not per Sample!
                // for an envelope, once every 64 samples is perfectly sufficient
                // executed ONLY if the filter for this channel is active!
                if (filterIsEnabled[ind] && filters[ind] != nullptr)
                {
                    float filterEnvVal = 0.0f;
                    for (uint32_t s = 0; s < samplesToRender; ++s)
                        filterEnvVal += filterAdsrs[ind].getNextSample(); // ADSR schreitet vor
                    filterEnvVal /= static_cast<float>(samplesToRender); // Mittelwert

                    const float modFilterFreq = juce::jlimit(
                        20.0f, 20000.0f,
                        filterFreq[ind] + filterEnvVal * filterEnvAmount[ind]
                        );
                filters[ind]->updateFilterFrequency(modFilterFreq);
            }


            // ── 4. Sample-Loop: Gain-ADSR, AM, Filter ────────────────────────
            for (uint32_t s = 0; s < samplesToRender; ++s)
            {
                const float gainEnv    = gainWtAdsrs[ind].getNextSample();
                const float activeMult = wtActive[ind] ? 1.0f : 0.0f;

                // AM: LFO-Signal [-1,1], amDepth 0..200
                // amDepth=0 → no mod, amDepth=200 → Amplitude 0..2× Basic
                const float amSignal = lfoAmData[ind].getNextAMSample();
                const float amMod = juce::jlimit(0.0f, 2.0f,
                    1.0f + amSignal * (amDepth[ind] / 100.0f));

                // const float dbVal = velocityGain[ind];
                // from -60 dB downwards, the signal becomes exactly 0.0f (true silence!).
                const float oscGain = velocityGain[ind];//juce::Decibels::decibelsToGain(velocityGain[ind], -60.0f);

                float left  = wtLeft[s] * gainEnv * 0.25f * activeMult * amMod * oscGain;
                float right = (wtRight ? wtRight[s] : wtLeft[s])
                              * gainEnv * 0.25f * activeMult * amMod * oscGain;

                // apply filter to each sample
                // ── apply filter if active ──
                if (filterIsEnabled[ind] && filters[ind] != nullptr)
                {
                    left  = filters[ind]->process(left, 0);
                    right = filters[ind]->process(right, 1);
                }

                mixBuffer.getWritePointer(0)[outputOffset + s] += left;
                if (mixBuffer.getNumChannels() > 1)
                    mixBuffer.getWritePointer(1)[outputOffset + s] += right;

                wtMeterBuffers[ind].getWritePointer(0)[outputOffset + s] = left;
                if (wtMeterBuffers[ind].getNumChannels() > 1)
                    wtMeterBuffers[ind].getWritePointer(1)[outputOffset + s] = right;
            }

            samplesRemaining -= static_cast<int>(samplesToRender);
            outputOffset     += static_cast<int>(samplesToRender);
        }
    }

    for (int ch = 0; ch < outputBuffer.getNumChannels(); ++ch)
        outputBuffer.addFrom(ch, startSample, mixBuffer, ch, 0, numSamples);

    bool allInactive = true;
    for (int i = 0; i < 4; ++i)
        if (wtActive[i] && gainWtAdsrs[i].isActive()) { allInactive = false; break; }
    // if the button is still pressed, the voice remains active
    // even when all internal wavetable oscillators are asleep!
    if (isKeyDown()) {
        allInactive = false;
    }
    if (allInactive)
        clearCurrentNote();
}
#pragma  endregion

std::shared_ptr<SynthLab::WTOscillator> SynthVoice::GetWtOscillator(const int index)
{
    if (index >= 0 && index < 4) {
        return wtOscillators[index];
    }
    return wtOscillators[index];
}

Lfos& SynthVoice::getFmLfo(const int indexosc)
{
    if (indexosc >= 0 && indexosc < 4) {
        return lfoFmData[indexosc];
    }
    return lfoFmData[0];
}

Lfos& SynthVoice::getAmLfo(const int indexosc)
{
    if (indexosc >= 0 && indexosc < 4) {
        return lfoAmData[indexosc];
    }
    return lfoAmData[0];
}

void SynthVoice::setFilterType (int indexosc, int type)
{
    if (indexosc < 0 || indexosc >= 4)
        return;

    const bool wantFormant = (type == 3); // 3 = "formant"
    const bool isFormant   = dynamic_cast<FormantFilter*> (filters[indexosc].get()) != nullptr;

    if (wantFormant != isFormant)
    {
        // slot type is changing
        if (wantFormant)
            filters[indexosc] = std::make_unique<FormantFilter>();
        else
            filters[indexosc] = std::make_unique<FilterData>();

        filters[indexosc]->prepare (currentFilterSpec);
        filters[indexosc]->reset();
    }

    if (!wantFormant)
        filters[indexosc]->setSubType (type);
}


void SynthVoice::updateGainAdsr(const float attack, const float decay, const float sustain, const float release, int indexosc)
{
    if (indexosc >= 0 && indexosc < 4)
        lofAdsrs[indexosc].updateADSR(attack, decay, sustain, release);
}

void SynthVoice::updateGainAdsrWt(const float attack, const float decay, const float sustain, const float release, int indexosc)
{
    if (indexosc >= 0 && indexosc < 4)
        gainWtAdsrs[indexosc].updateADSR(attack, decay, sustain, release);
}


void SynthVoice::updateFilterAdsr(const float attack, const float decay, const float sustain, const float release, int indexosc)
{
    juce::ADSR::Parameters filterParams;
    filterParams.attack = attack;
    filterParams.decay = decay;
    filterParams.sustain = sustain;
    filterParams.release = release;
    if (indexosc >= 0 && indexosc < 4)
        filterAdsrs[indexosc].setParameters(filterParams);
}


void SynthVoice::updateVelocity(const float velo, const int indexosc)
{
    if (indexosc >= 0 && indexosc < 4) {
        velocityGain[indexosc] = std::clamp(velo, 0.0f, 1.0f); // ← safety
    }
}


void SynthVoice::updateVelocityDb(const float velo, const int indexosc)
{
    if (indexosc >= 0 && indexosc < 4) {
        velocityGain[indexosc] = juce::Decibels::decibelsToGain(velo, -60.0f);
    }
}


void SynthVoice::updateFmParams(const float fmFreqParam, const float fmDepthParam, const int indexosc)
{
    if (indexosc >= 0 && indexosc < 4) {
        fmFreq[indexosc] = fmFreqParam; fmDepth[indexosc] = fmDepthParam;
        lfoFmData[indexosc].setFMParams(fmFreqParam, fmDepthParam);
    }
}

void SynthVoice::updateAmParams(const float amFreqParam, const float amDepthParam, const int indexosc)
{
    if (indexosc >= 0 && indexosc < 4) {
        amFreq[indexosc] = amFreqParam; amDepth[indexosc] = amDepthParam;
        lfoAmData[indexosc].setAMParams(amFreqParam, amDepthParam);
    }
}


void SynthVoice::updateFilterParams(const float filterFreqParam, const float filterResoParam, const float filtEnvAmParam, int indexosc)
{
    if (indexosc >= 0 && indexosc < 4) {
        filterFreq[indexosc] = filterFreqParam; filterReso[indexosc] = filterResoParam;
        filterEnvAmount[indexosc] = filtEnvAmParam;
        filters[indexosc]->updateFilterFrequency(filterFreqParam);
        filters[indexosc]->updateFilterResonance(filterResoParam);
    }
}

void SynthVoice::setWtActive(const int index, const bool active)
{
    wtActive[index]  = active;
}

void SynthVoice::setFilterEnabled(int index, bool active)
{
    std::array<bool, 4> currentActivationState;
    currentActivationState[index] = active;
    if (currentActivationState[index] != lastFiterIsEnabled[index])
    {
        filterIsEnabled[index]  = currentActivationState[index];
        filters[index]->reset();
    }
    lastFiterIsEnabled[index] = currentActivationState[index];

}

void SynthVoice::prepare (double sampleRate, int maxBlockSize, int outputChannels)
{
    setCurrentPlaybackSampleRate (sampleRate);

    // reset ADSR envelopes & SynthLab oscillators
    for (int ind = 0; ind < 4; ++ind)
    {
        gainWtAdsrs[ind].setSampleRate (sampleRate);
        juce::ADSR::Parameters wTparams { 0.001f, 0.9f, 0.05f, 0.2f };
        gainWtAdsrs[ind].setParameters (wTparams);
        gainWtAdsrs[ind].reset();

        filterAdsrs[ind].setSampleRate (sampleRate);
        juce::ADSR::Parameters filterParams { 0.01f, 0.8f, 0.5f, 1.0f };
        filterAdsrs[ind].setParameters (filterParams);
        filterAdsrs[ind].reset();

        if (wtOscillators[ind] != nullptr)
            wtOscillators[ind]->reset (sampleRate);
    }

    // preparing JUCE DSP filters for maxBlockSize
    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = static_cast<uint32_t> (maxBlockSize);
    spec.numChannels      = static_cast<uint32_t> (outputChannels);
    currentFilterSpec     = spec;

    for (int ind = 0; ind < 4; ++ind)
    {
        if (filters[ind] == nullptr)
            filters[ind] = std::make_unique<FilterData>();

        filters[ind]->prepare (spec);
    }

    // allocate audio buffer with a fixed safety margin (no re-allocation)
    mixBuffer.setSize (outputChannels, maxBlockSize, false, false, true);

    for (int i = 0; i < 4; ++i) {
        wtMeterBuffers[i].setSize (outputChannels, maxBlockSize, false, false, true);
        wtMeterBuffers[i].clear();
    }
    isPrepared = true;
}







