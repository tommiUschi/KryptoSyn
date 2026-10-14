//
// Created by tommibe on 09.10.26.
//

#pragma once

#include <juce_dsp/juce_dsp.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <foleys_gui_magic/foleys_gui_magic.h>
#include <juce_core/juce_core.h>


struct WavetableGeneratorConfig
{
    float spectralTilt    = 0.5f;  // 0.5 = Hyper-Saw, 1.0 = Standard Saw, 2.0 = Mellow
    float oddOnlyRatio  = 0.0f;  // 0.0 = Sägezahn-Typ, 1.0 = Rechteck/Hohl-Typ
    float phaseSpread   = 0.0f;  // 0.0 = Nullphase (konzentriert), 1.0 = Zufallsphase
    float morphDrift    = 0.05f; // Max. Änderung pro Frame (Random Walk Schrittweite)
    bool startGen = false;
    uint32_t randomSeed   = 1337;  // Reproduzierbarer Zufall
};
inline WavetableGeneratorConfig WTConfig;

class AdditiveNoteSampleGenerator
{
public:
    /**
     * Generiert WAV-Dateien für alle Töne von C2 (MIDI 36) bis B4 (MIDI 71)
     * basierend auf den mathematischen Additiv-Parametern.
     */
    static bool generateNoteSet(const juce::File& targetDirectory,
                                const WavetableGeneratorConfig& config,
                                float noteDurationSeconds = 2.0f,
                                double sampleRate = 44100.0)
    {
        if (!targetDirectory.isDirectory() && !targetDirectory.createDirectory())
            return false;

        const int startMidiNote = 36; // C2
        const int endMidiNote   = 71; // B4
        const int totalSamples  = static_cast<int>(sampleRate * noteDurationSeconds);

        juce::WavAudioFormat wavFormat;

        for (int midiNote = startMidiNote; midiNote <= endMidiNote; ++midiNote)
        {
            const double f0 = juce::MidiMessage::getMidiNoteInHertz(midiNote);
            const juce::String noteName = juce::MidiMessage::getMidiNoteName(midiNote, true, true, 4);

            // 1. Maximale Obertöne ohne Aliasing berechnen (Nyquist-Grenze)
            const int maxHarmonics = static_cast<int>((sampleRate * 0.5) / f0);
            if (maxHarmonics < 1) continue;

            juce::Random rng(config.randomSeed + midiNote); // Eigener Seed pro Note

            // 2. Initialisiere Amplituden & Phasen für diese Note
            std::vector<float> currentAmps(maxHarmonics);
            std::vector<float> currentPhases(maxHarmonics);

            for (int h = 0; h < maxHarmonics; ++h)
            {
                int n = h + 1; // 1-basierte Harmonische (1 = Grundton)

                // Spektraler Abfall (Spectral Tilt)
                float baseAmp = 1.0f / std::pow(static_cast<float>(n), config.spectralTilt);

                // Ungerade/Gerade Gewichtung (Rechteck vs. Sägezahn)
                bool isOdd = (n % 2 != 0);
                float mask = isOdd ? 1.0f : (1.0f - config.oddOnlyRatio);

                currentAmps[h] = baseAmp * mask * rng.nextFloat();
                currentPhases[h] = config.phaseSpread * rng.nextFloat() * juce::MathConstants<float>::twoPi;
            }

            // 3. AudioBuffer allokieren
            juce::AudioBuffer<float> buffer(1, totalSamples);
            float* writePtr = buffer.getWritePointer(0);

            // Phasenakkumulator für die Grundfrequenz
            double basePhase = 0.0;
            const double basePhaseIncrement = (juce::MathConstants<double>::twoPi * f0) / sampleRate;

            // 4. Sample-weise Additive Synthese mit Random Walk (Morphing)
            for (int sampleIdx = 0; sampleIdx < totalSamples; ++sampleIdx)
            {
                double sampleVal = 0.0;

                for (int h = 0; h < maxHarmonics; ++h)
                {
                    float amp = currentAmps[h];
                    if (amp <= 0.00001f) continue;

                    int n = h + 1;
                    // Phasenberechnung: n * Grundphase + Offset
                    sampleVal += amp * std::sin(n * basePhase + currentPhases[h]);

                    // Random Walk Drift während der Ton anhält
                    if (config.morphDrift > 0.0f)
                    {
                        float baseAmp = 1.0f / std::pow(static_cast<float>(n), config.spectralTilt);
                        bool isOdd = (n % 2 != 0);
                        float mask = isOdd ? 1.0f : (1.0f - config.oddOnlyRatio);
                        float maxAllowedAmp = baseAmp * mask;

                        // Leichtes Driften der Amplituden und Phasen pro Sample
                        float ampDelta = (rng.nextFloat() * 2.0f - 1.0f) * (config.morphDrift / sampleRate) * baseAmp;
                        currentAmps[h] = juce::jlimit(0.0f, maxAllowedAmp, currentAmps[h] + ampDelta);

                        float phaseDelta = (rng.nextFloat() * 2.0f - 1.0f) * (config.morphDrift / sampleRate) * juce::MathConstants<float>::pi;
                        currentPhases[h] += phaseDelta;
                    }
                }

                writePtr[sampleIdx] = static_cast<float>(sampleVal);

                // Grundphase weiterzählen
                basePhase += basePhaseIncrement;
                if (basePhase >= juce::MathConstants<double>::twoPi)
                    basePhase -= juce::MathConstants<double>::twoPi;
            }

            // 5. Normalisierung & Fade-Out (10ms gegen Knackser am Ende)
            const float maxVal = buffer.getMagnitude(0, totalSamples);
            if (maxVal > 0.00001f)
                buffer.applyGain(0.95f / maxVal); // Auf -0.45 dB skalierten Peak

            const int fadeSamples = static_cast<int>(sampleRate * 0.01);
            buffer.applyGainRamp(totalSamples - fadeSamples, fadeSamples, 1.0f, 0.0f);

            // 6. WAV-Datei exportieren
            /*juce::String fileName = juce::String::formatted("SynthNote__%d_Midi_%s.wav",
                                                           noteName.toRawUTF8(),
                                                           midiNote);*/
            juce::String fileName = "SynthNote__"
                      + juce::String(midiNote)
                      + "_Midi_"
                      + noteName.toRawUTF8()
                      + ".wav";
            juce::File outputFile = targetDirectory.getChildFile(fileName);

            std::unique_ptr<juce::FileOutputStream> fileStream(outputFile.createOutputStream());
            if (fileStream != nullptr)
            {
                std::unique_ptr<juce::AudioFormatWriter> writer(
                    wavFormat.createWriterFor(fileStream.release(),
                                              sampleRate,
                                              1,   // Mono
                                              24,  // 24-Bit PCM
                                              {},
                                              0));

                if (writer != nullptr)
                    writer->writeFromAudioSampleBuffer(buffer, 0, totalSamples);
            }
        }

        return true;
    }
};



