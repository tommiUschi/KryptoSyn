//
// Created by tommibe on 13.02.26.
//


#pragma once

#include <juce_dsp/juce_dsp.h>
#include "Data/AdsrData.h"
#include "Data/FilterData.h"
#include "Data/Lfos.h"
#include "Data/FilterBase.h"
#include "Data/FormantFilter.h"
#include "../SDKs/SynthLab/source/wtoscillator.h"
#include "../SDKs/SynthLab/source/classicwtcore.h"

/**
 * Handles the synthesis and audio rendering for an individual active note.
 * Processes voice-level DSP modules (Synthlab::WT-oscillators, envelopes, filters)
 * and tracks note state (pitch, velocity, gate) during triggering and release.
 */


    class SynthVoice : public juce::SynthesiserVoice
    {
    public:
        SynthVoice(){};
        void setWavetableDatabase(const std::shared_ptr<SynthLab::MidiInputData>& _midiData, std::shared_ptr<SynthLab::WavetableDatabase> db);
        bool canPlaySound(juce::SynthesiserSound * sound) override;
        void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
        void stopNote(float velocity, bool allowTailOff) override;
        void controllerMoved(int controllerNumber, int newControllerValue) override;
        void pitchWheelMoved(int newPitchWheelValue) override;
        void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels);
        std::shared_ptr<SynthLab::WTOscillator> GetWtOscillator(int index);

        void updateGainAdsr(const float attack, const float decay, const float sustain, const float release, int indexosc);
        void updateGainAdsrWt(float attack, float decay, float sustain, float release, int indexosc);
        void updateFilterAdsr(const float attack, const float decay, const float sustain, const float release, int indexosc);

        void renderNextBlock(juce::AudioBuffer<float> &outputBuffer,int startSample, int numSamples) override;
        Lfos& getFmLfo(int indexosc);
        Lfos& getAmLfo(int indexosc);

        // central location for type changes
        void setFilterType (int indexosc, int type);

        void updateVelocity(float velo, int indexos);
        void updateVelocityDb(float velo, int indexosc);
        void updateFmParams(float fmFreqParam, float fmDepthParam, int indexos);
        void updateAmParams(float amFreqParam, float amDepthParam, int indexos);
        void updateFilterParams(float filterFreqParam, float filterResoParam, float filtEnvAmParam, int indexos);
        void setWtActive(int index, bool active);

        const juce::AudioBuffer<float>& getWtMeterBuffer(int index) const {
            return wtMeterBuffers[index];
        }
        void setBaseTune(float tune, int ind)
        {
            if (ind >= 0 && ind < 4)
                baseTune[ind] = tune;
        }
        void setLfoActive(const int index, const bool active) { lfoActive[index] = active; }
        //void setWtActive(const int index, const bool active) { /*wtActive[index]  = active; */}

    private:
        std::shared_ptr<SynthLab::MidiInputData> midiData;
        std::shared_ptr<SynthLab::WavetableDatabase> _db;
        std::array<std::shared_ptr<SynthLab::WTOscParameters>, 4> wtParams;
        std::array<std::shared_ptr<SynthLab::WTOscillator>, 4> wtOscillators;
        juce::AudioBuffer<float> synthBuffer; // a local buffer for this voice
        juce::AudioBuffer<float> mixBuffer;    // Collect all 7 oscillators

        // the baseTune array is necessary so that the FM modulation is always relative to the
        // operates using the set tune value (and does not permanently shift it):
        std::array<float, 4> baseTune = {0.0f, 0.0f, 0.0f, 0.0f}; // basic-fineDetune

        // measurement buffer: each oscillator writes its output here
        // (for level display only – is NOT mixed into outputBuffer)
        std::array<juce::AudioBuffer<float>, 4> wtMeterBuffers;   // WT-oszillators

        std::array<Lfos, 4> lfoFmData;
        std::array<Lfos, 4> lfoAmData;

        // envelope controls LFO frequency/amplitude
        std::array<AdsrData, 8> lofAdsrs;
        std::array<AdsrData, 4> filterAdsrs;

        juce::dsp::ProcessSpec currentFilterSpec;
        std::array<std::unique_ptr<FilterBase>, 4> filters;
        std::array<FilterData, 4> filtersForAdsr;
        std::array<AdsrData, 4> gainWtAdsrs;

        bool isPrepared = {false};
        std::array<float, 4> velocityGain = {0.68f, 0.68f, 0.68f};
        std::array<float, 4> fmFreq = {0.0f, 0.0f, 0.0f};
        std::array<float, 4> fmDepth = {0.0f, 0.0f, 0.0f};
        std::array<float, 4> amFreq = {0.0f, 0.0f, 0.0f};
        std::array<float, 4> amDepth = {0.0f, 0.0f, 0.0f};
        std::array<float, 4> filterFreq = {0.0f, 0.0f, 0.0f};
        std::array<float, 4> filterReso = {0.0f, 0.0f, 0.0f};
        std::array<float, 4> filterEnvAmount = {0.0f, 0.0f, 0.0f};

        std::array<bool, 6> lfoActive = {true, true, true, true, true, true};
        std::array<bool, 4> wtActive  = {true, true, true, true};
        //==============================================================================
        // sine wave: return std::sin(x);
        // saw wave: return x / juce::MathConstants<float>::pi;
        // square wave: return x < 0.0f ? -1.0f : 1f
    };



