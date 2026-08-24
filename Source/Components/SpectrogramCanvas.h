
#pragma once

#include <foleys_gui_magic/foleys_gui_magic.h>
#include "../Helpers/PixelFIFO.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>

/**
 * for reverse sprectrogram (drawing) and Spectrogram (displaying) [GUI component]
 */

class AudioPluginAudioProcessor;

class SpectrogramCanvas : public juce::Component, public juce::ChangeListener
{
public:
    SpectrogramCanvas();
    ~SpectrogramCanvas() override;

    void setProcessor (AudioPluginAudioProcessor* processorToUse);
    void setFIFO (PixelFIFO* fifoToUse) { processorFifo = fifoToUse; }

    // JUCE callback-func
    void changeListenerCallback (juce::ChangeBroadcaster* source) override;
    void syncGridFromProcessorImage();
    void initGrid (int columns, int rows);
    void clearCanvasMemory(); // resets local gridData

    void mouseMove (const juce::MouseEvent& e) override { evaluateMouse (e, false); }
    void mouseDrag (const juce::MouseEvent& e) override { evaluateMouse (e, true); }
    void mouseDown (const juce::MouseEvent& e) override { evaluateMouse (e, true); }

    void paint (juce::Graphics& g) override;

private:
    void evaluateMouse (const juce::MouseEvent& e, bool isDrawing);
    void setPixelValue (int x, int y, float value);
    void drawSoftBrush (int centerX, int centerY, int radius);

    AudioPluginAudioProcessor* processor = nullptr;
    PixelFIFO* processorFifo = nullptr;

    int numColumns = 512;
    int numRows = 256;

    const juce::Colour canvasBgColour = juce::Colour (0xff001500);
    const juce::Colour myCyan         = juce::Colour (0xff20ffd5);

    std::vector<float> gridData; // local audio storage for amplitude verification
};
//=============================================================================
class OutputSpectrogramCanvas : public juce::Component, private juce::Timer
{ // for spectral frquency analyzer on mastert page
public:
    OutputSpectrogramCanvas()
    {
        initCanvas (512, 256);
        startTimerHz (54); // 60 FPS for silky-smooth scrolling
    }

    ~OutputSpectrogramCanvas() override
    {
        stopTimer();
    }

    // Deletes the entire drawing
    void clearCanvas()
    {
        canvasOutputImage.clear (canvasOutputImage.getBounds(), canvasBgColour);
        repaint();
    }

    void initCanvas (int columns, int rows)
    {
        numColumns = columns;
        numRows = rows;
        canvasOutputImage = juce::Image (juce::Image::PixelFormat::ARGB, numColumns, numRows, true);
        canvasOutputImage.clear (canvasOutputImage.getBounds(), juce::Colours::black);
    }

    void setFIFO (FFTFIFO* fifoToUse) { audioFifo = fifoToUse; }

    void paint (juce::Graphics& g) override
    {
        g.drawImage (canvasOutputImage, getLocalBounds().toFloat(),
                     juce::RectanglePlacement::stretchToFit, false);

        g.setColour (juce::Colours::darkgreen.darker(1.0));
        g.drawRect (getLocalBounds(), 1);
    }

    juce::Image GetCanvas(){ return canvasOutputImage;}

private:
    void timerCallback() override
    {
        if (audioFifo == nullptr) return;

        FFTFIFO::FFTFrame incomingFrame;
        bool dataChanged = false;

        // Extract all pending FFT frames from the FIFO and render them
        while (audioFifo->pop (incomingFrame))
        {
            // check whether a clear signal has been received:
            if (incomingFrame[0] < 0.0f) {
                canvasOutputImage.clear (canvasOutputImage.getBounds(), juce::Colours::black);
                dataChanged = true;
                continue; // Weiter zum nächsten Frame, falls vorhanden
            }
            // shift the existing image 1 pixel to the left
            canvasOutputImage.moveImageSection (0, 0, 1, 0, numColumns - 1, numRows);

            // draw the new column on the far right (X = numColumns - 1)
            int targetX = numColumns - 1;

            for (int y = 0; y < numRows; ++y)
            {
                float intensity = incomingFrame[static_cast<size_t>(y)];

                // invert Y so that the bass is at the bottom and the treble is at the top
                int imageY = numRows - 1 - y;

                // generate the color based on the line height (frequency) and intensity
                juce::Colour pixelColour = getColourForFrequencyRow (y, intensity);
                canvasOutputImage.setPixelAt (targetX, imageY, pixelColour);
            }
            dataChanged = true;
        }

        if (dataChanged)
            repaint();
    }

    // calculates a harmonious color spectrum: bass (red/orange) -> mids (green/yellow) -> highs (cyan/blue)
    juce::Colour getColourForFrequencyRow (int row, float intensity)
    {
        float factor = static_cast<float>(row) / static_cast<float>(numRows);

        float r = 0.0f, g = 0.0f, b = 0.0f;

        if (factor < 0.33f) // low frequencies / Bass: Red to yellow
        {
            r = 1.0f;
            g = factor / 0.33f;
        }
        else if (factor < 0.66f) // mid-frequencies: Yellow to green/cyan
        {
            r = 1.0f - ((factor - 0.33f) / 0.33f);
            g = 1.0f;
            b = (factor - 0.33f) / 0.33f;
        }
        else // high frequencies: Cyan to rich blue
        {
            g = 1.0f - ((factor - 0.66f) / 0.34f);
            b = 1.0f;
        }

        // the intensity (volume) controls the brightening from the deep black
        return juce::Colour::fromRGBA (
            static_cast<juce::uint8>(r * intensity * 255.0f),
            static_cast<juce::uint8>(g * intensity * 255.0f),
            static_cast<juce::uint8>(b * intensity * 255.0f),
            255
        );
    }
    const juce::Colour canvasBgColour = juce::Colour (0xff000000);

    FFTFIFO* audioFifo = nullptr;
    int numColumns = 0;
    int numRows = 0;
    juce::Image canvasOutputImage;
};