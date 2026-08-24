//
// Created by tommibe on 13.02.26.
//
#include "MasterEQ.h"

void MasterEQ::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSampleRate = spec.sampleRate;

    juce::dsp::ProcessSpec monoSpec = spec;
    monoSpec.numChannels = 1;

    for (int ch = 0; ch < 2; ++ch)
    {
        lowShelfFilters[ch].prepare(monoSpec);
        midPeakFilters[ch].prepare(monoSpec);
        highShelfFilters[ch].prepare(monoSpec);
    }

    reset();
}

void MasterEQ::updateEQParameters(float bassDb, float midDb, float highDb)
{
    //std::cout << "EQ update: bass=" << bassDb << " mid=" << midDb << " high=" << highDb << std::endl;
    // efficiency Gate: Recalculate only when controllers have moved
    if (std::abs(bassDb - lastBassDb) > 0.05f ||
        std::abs(midDb - lastMidDb)   > 0.05f ||
        std::abs(highDb - lastHighDb) > 0.05f)
    {
        // frequencies firmly defined for a classic 3-band EQ
        // fass up to 200 Hz, mids centered at 1.0 kHz, treble from 5.0 kHz
        // the quality factor (Q) defaults to the musical value 0.707f
        auto lowCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            currentSampleRate, 200.0f, 0.707f, juce::Decibels::decibelsToGain(bassDb));
            
        auto midCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            currentSampleRate, 1000.0f, 0.707f, juce::Decibels::decibelsToGain(midDb));
            
        auto highCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
            currentSampleRate, 5000.0f, 0.707f, juce::Decibels::decibelsToGain(highDb));

        // pass coefficients atomically to the channels (JUCE internally thread-safe)
        for (int ch = 0; ch < 2; ++ch)
        {
            lowShelfFilters[ch].coefficients  = lowCoeffs;
            midPeakFilters[ch].coefficients   = midCoeffs;
            highShelfFilters[ch].coefficients = highCoeffs;
        }

        lastBassDb = bassDb;
        lastMidDb  = midDb;
        lastHighDb = highDb;
    }
}

void MasterEQ::process(juce::AudioBuffer<float>& buffer)
{
    juce::dsp::AudioBlock<float> block(buffer);
    const int channelsToProcess = std::min(static_cast<int>(buffer.getNumChannels()), 2);

    for (int ch = 0; ch < channelsToProcess; ++ch)
    {
        // JUCE IIR requires an appropriate context per channel
        auto singleChannelBlock = block.getSingleChannelBlock(static_cast<size_t>(ch));
        juce::dsp::ProcessContextReplacing<float> context(singleChannelBlock);

        // the signal passes through all 3 bands serially
        lowShelfFilters[ch].process(context);
        midPeakFilters[ch].process(context);
        highShelfFilters[ch].process(context);
    }
}

void MasterEQ::reset()
{
    for (int ch = 0; ch < 2; ++ch)
    {
        lowShelfFilters[ch].reset();
        midPeakFilters[ch].reset();
        highShelfFilters[ch].reset();
    }
    lastBassDb = -999.0f;
    lastMidDb  = -999.0f;
    lastHighDb = -999.0f;
}





