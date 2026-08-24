//
// Created by tommibe on 19.04.26.
//

#pragma once
#include <algorithm>
#include <cmath>
#include <memory>
#include <juce_core/juce_core.h> // for jassert

/**
 * Class CBuffer (circular buffer) is needed for the delay effect
 */

class CBuffer
{
public:
    CBuffer();
    ~CBuffer();

    // allow move, prevent copy (due to unique_ptr)
    CBuffer(CBuffer&&) = default;
    CBuffer& operator=(CBuffer&&) = default;
    CBuffer(const CBuffer&) = delete;
    CBuffer& operator=(const CBuffer&) = delete;

    void Initialize(double sampleRate, float maxDelTimeMs);
    // mark critical functions as inline in the audio thread
    inline void WriteBuffer(float val) noexcept
    {
        jassert(delaybuffer != nullptr); // prevent crashes in debug builds
        delaybuffer[writeInd] = val;
        writeInd = (writeInd + 1) & inxMask;
    }

    // marked as const because the state of the buffer does not change during reading
    float ReadBuffer(float delTimeMs) const noexcept;
    float ReadSamples(int samplesDelay) const noexcept;

    void Clear();

private:
    int writeInd = 0;
    double currentSampleRate = 0.0;
    int bufferLen = 0;
    int inxMask = 0;
    std::unique_ptr<float[]> delaybuffer = nullptr;
};

