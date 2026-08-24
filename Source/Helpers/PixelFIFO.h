//
// Created by tommibe on 08.06.26.
//

#pragma once
#include <juce_core/juce_core.h>
#include <array>
#include <foleys_gui_magic/foleys_gui_magic.h>
#include "../Components/SpectrogramCanvas.h"

struct PixelUpdate
{
    int x = 0;
    int y = 0;
    float value = 0.0f;
};

/** Lock-free FIFO (First-In, First-Out) ring buffer system
 * it is for reversem- mand outputspectrogram
 */

class PixelFIFO
{
public:
    PixelFIFO() : abstractFifo (capacity)
    {
        buffer.resize (capacity);
    }

    // Called by the UI thread
    bool push (int x, int y, float value)
    {
        int start1, size1, start2, size2;
        abstractFifo.prepareToWrite (1, start1, size1, start2, size2);

        if (size1 > 0)
        {
            buffer[static_cast<size_t>(start1)] = { x, y, value };
            abstractFifo.finishedWrite (1);
            return true;
        }
        
        return false; // Buffer full (should never happen with 8192 elements when drawing)
    }

    // Called by the audio thread
    bool pop (PixelUpdate& result)
    {
        int start1, size1, start2, size2;
        abstractFifo.prepareToRead (1, start1, size1, start2, size2);

        if (size1 > 0)
        {
            result = buffer[static_cast<size_t>(start1)];
            abstractFifo.finishedRead (1);
            return true;
        }

        return false; // no new updates available
    }

    // Special command for quick deletion (X = -1 triggers "Clear" in the audio thread)
    void pushClearCommand()
    {
        push (-1, 0, 0.0f);
    }

private:
    static constexpr int capacity = 8192; // Big enough for quick mouse movements
    juce::AbstractFifo abstractFifo;
    std::vector<PixelUpdate> buffer;
};

class FFTFIFO
{
public:
    static constexpr int numBins = 256;
    using FFTFrame = std::array<float, numBins>;

    FFTFIFO() : abstractFifo (capacity) {}

    // Called from the audio thread
    bool push (const FFTFrame& frame)
    {
        int start1, size1, start2, size2;
        abstractFifo.prepareToWrite (1, start1, size1, start2, size2);

        if (size1 > 0)
        {
            buffer[static_cast<size_t>(start1)] = frame;
            abstractFifo.finishedWrite (1);
            return true;
        }
        return false;
    }

    // Called by UI thread (timer)
    bool pop (FFTFrame& result)
    {
        int start1, size1, start2, size2;
        abstractFifo.prepareToRead (1, start1, size1, start2, size2);

        if (size1 > 0)
        {
            result = buffer[static_cast<size_t>(start1)];
            abstractFifo.finishedRead (1);
            return true;
        }
        return false;
    }

    // sends a signal to the UI thread to delete the image
    bool pushClearCommand()
    {
        FFTFrame clearFrame;
        clearFrame[0] = -1.0f; // Sentinel value (otherwise, valid values are 0.0f to 1.0f)
        return push (clearFrame);
    }

private:
    static constexpr int capacity = 32; // A small buffer is sufficient for visualization
    juce::AbstractFifo abstractFifo;
    std::array<FFTFrame, capacity> buffer {};

};
