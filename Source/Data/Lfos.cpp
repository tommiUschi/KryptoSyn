//
// Created by tommibe on 13.02.26.
//

#include "Lfos.h"

void Lfos::prepareToPlayOsc(juce::dsp::ProcessSpec spec)
{
    currentSampleRate = spec.sampleRate;
    osc.prepare(spec);
    oscFM.prepare(spec);
    oscAM.prepare(spec);
}

void Lfos::setWaveType(int type){
    lastModeFm = type;
}

void Lfos::RenderFmLfoInLocalBuffer(const juce::dsp::AudioBlock<float>& subBlock)
{
    auto baseFreq = juce::MidiMessage::getMidiNoteInHertz(lastMidiNoteNumber);

    for (int s = 0; s < subBlock.getNumSamples(); ++s)
    {
        // modulator signal
        float modSignal = oscFM.processSample(0.0f);

        // increment basic phase (0.0 to 1.0)
        float phaseIncrement = baseFreq / static_cast<float>(currentSampleRate);
        carrierPhaseFm += phaseIncrement;
        if (carrierPhaseFm >= 1.0f) carrierPhaseFm -= 1.0f;

        // apply phase modulation
        // we add the modulation to the phase
        // limit phase after modulation
        float phaseModulation = modSignal * (fmDepth / 100.0f);
        // limit modulation to a sensible range:
        phaseModulation = std::clamp(phaseModulation, -1.0f, 1.0f);
        float modulatedPhase = carrierPhaseFm + phaseModulation;

        // wrap around the modulated phase (to keep it in range)
        modulatedPhase = std::fmod(modulatedPhase, 1.0f);
        if (modulatedPhase < 0.0f) modulatedPhase += 1.0f;

        // Waveform selection ('lastMode')
        float sample = 0.0f;

        switch (lastModeFm)
        {
        case 0: // SINUS
            sample = std::sin(modulatedPhase * 2.0f * juce::MathConstants<float>::pi);
            break;

        case 1: // SAW(linear from -1 until 1)
            sample = (modulatedPhase * 2.0f) - 1.0f;
            break;

        case 2: // SQUARE
            sample = (modulatedPhase < 0.5f) ? 1.0f : -1.0f;
            break;

        default:
            sample = std::sin(modulatedPhase * 2.0f * juce::MathConstants<float>::pi);
            break;
        }
        // at the very end of RenderOscInLocalBuffer, after the switch:
        sample = std::clamp(sample, -1.0f, 1.0f); // ← Output always normalized
        // write to the buffer
        for (int ch = 0; ch < subBlock.getNumChannels(); ++ch)
            subBlock.setSample(ch, s, sample);
    }

}

void Lfos::RenderAmLfoInLocalBuffer(const juce::dsp::AudioBlock<float>& subBlock)
{//Analogous to the previous method
    auto baseFreq = juce::MidiMessage::getMidiNoteInHertz(lastMidiNoteNumber);

    for (int s = 0; s < subBlock.getNumSamples(); ++s)
    {
        float modSignal = oscAM.processSample(0.0f);

        float phaseIncrement = baseFreq / static_cast<float>(currentSampleRate);
        carrierPhaseAm += phaseIncrement;
        if (carrierPhaseAm >= 1.0f) carrierPhaseAm -= 1.0f;

        float phaseModulation = modSignal * (amDepth / 100.0f);
        phaseModulation = std::clamp(phaseModulation, -1.0f, 1.0f);
        float modulatedPhase = carrierPhaseAm + phaseModulation;

        modulatedPhase = std::fmod(modulatedPhase, 1.0f);
        if (modulatedPhase < 0.0f) modulatedPhase += 1.0f;

        float sample = 0.0f;

        switch (lastModeAm)
        {
        case 0:
            sample = std::sin(modulatedPhase * 2.0f * juce::MathConstants<float>::pi);
            break;

        case 1:
            sample = (modulatedPhase * 2.0f) - 1.0f;
            break;

        case 2:
            sample = (modulatedPhase < 0.5f) ? 1.0f : -1.0f;
            break;

        default:
            sample = std::sin(modulatedPhase * 2.0f * juce::MathConstants<float>::pi);
            break;
        }

        sample = std::clamp(sample, -1.0f, 1.0f);

        for (int ch = 0; ch < subBlock.getNumChannels(); ++ch)
            subBlock.setSample(ch, s, sample);
    }

}

float Lfos::getNextFMSample()
{
    if (currentSampleRate <= 0.0f) return 0.0f;

    fmLfoPhase += fmFrequency / static_cast<float>(currentSampleRate);
    if (fmLfoPhase >= 1.0f) fmLfoPhase -= 1.0f;

    switch (lastModeFm)
    {
    case 0: // SINUS
        return std::sin(fmLfoPhase * 2.0f * juce::MathConstants<float>::pi);
    case 1: // SAW
        return (fmLfoPhase * 2.0f) - 1.0f;
    case 2: // SQUARE
        return (fmLfoPhase < 0.5f) ? 1.0f : -1.0f;
    default:
        return std::sin(fmLfoPhase * 2.0f * juce::MathConstants<float>::pi);
    }
}

float Lfos::getNextAMSample()
{
    if (currentSampleRate <= 0.0f) return 0.0f;

    amLfoPhase += amFrequency / static_cast<float>(currentSampleRate);
    if (amLfoPhase >= 1.0f) amLfoPhase -= 1.0f;

    switch (lastModeAm)
    {
    case 0: return std::sin(amLfoPhase * 2.0f * juce::MathConstants<float>::pi);
    case 1: return (amLfoPhase * 2.0f) - 1.0f;
    case 2: return (amLfoPhase < 0.5f) ? 1.0f : -1.0f;
    default: return std::sin(amLfoPhase * 2.0f * juce::MathConstants<float>::pi);
    }
}

void Lfos::setWaveFrequenzy(const int midiNoteNumber)
{
    // we only remember the base note.
    // we do the frequency calculation in the render loop or here once
    lastMidiNoteNumber = midiNoteNumber;
    carrierPhaseFm = 0.0f;
    carrierPhaseAm = 0.0f;
}

void Lfos::setFMParams(float freq, float depth)
{
    fmDepth     = depth;
    fmFrequency = freq;       // save for getNextFMSample()
    oscFM.setFrequency(freq);
}

void Lfos::setAMParams(float freq, float depth)
{
    amDepth     = depth;
    amFrequency = freq;       // save for getNextAMSample()
    oscAM.setFrequency(freq);
}

void Lfos::setAMFrequency(float freq)
{
    amFrequency = freq;       // save for getNextAMSample()
    oscAM.setFrequency(freq);
}

void Lfos::setLfoWave (int mode)
{
    // the math happens live in the render loop.
    lastModeFm = mode;
    lastModeAm = mode;
}








