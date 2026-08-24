
#include "SpectrogramCanvas.h"
#include "PluginProcessor.h"

/**
 * for reverse sprectrogram (drawing) and Spectrogram (displaying) [GUI component]
 */

SpectrogramCanvas::SpectrogramCanvas()
{
    juce::Desktop::getInstance().addGlobalMouseListener (this);
    initGrid (512, 256);
}

SpectrogramCanvas::~SpectrogramCanvas()
{
    // important: Unregister from the processor when destroying the GUI component
    if (processor != nullptr)
        processor->removeChangeListener (this);
    juce::Desktop::getInstance().removeGlobalMouseListener (this);
}

void SpectrogramCanvas::setProcessor (AudioPluginAudioProcessor* processorToUse)
{
    // if a processor was already set, log out.
    if (processor != nullptr)
        processor->removeChangeListener (this);
    processor = processorToUse;
    // log in to the new processor
    if (processor != nullptr)
    {
        processor->addChangeListener (this);
        // synchronize immediately upon connecting
        syncGridFromProcessorImage();
    }
}

// this method is automatically called by JUICE when `send Change Message()` is fired:
void SpectrogramCanvas::changeListenerCallback (juce::ChangeBroadcaster* source)
{
    if (source == processor){
        syncGridFromProcessorImage();
    }
}

void SpectrogramCanvas::syncGridFromProcessorImage()
{
    if (processor == nullptr)
        return;
    const auto& img = processor->getCanvasImage();
    if (! img.isValid())
        return;

    // compare the grid memory with the values ​​of the loaded image
    for (int x = 0; x < numColumns; ++x) {
        for (int y = 0; y < numRows; ++y) {
            // y-inversion (in juce::Image, Y=0 is at the top; in the frequency grid, Y=0 is at the bottom)
            int imageY = (numRows - 1) - y;
            juce::Colour c = img.getPixelAt (x, imageY);
            size_t index = static_cast<size_t> (x) * static_cast<size_t> (numRows) + static_cast<size_t> (y);
            gridData[index] = c.getBrightness();
        }
    }
    repaint();
}

void SpectrogramCanvas::initGrid (int columns, int rows)
{
    numColumns = columns;
    numRows = rows;
    gridData.assign (static_cast<size_t> (numColumns * numRows), 0.0f);
    repaint();
}

void SpectrogramCanvas::clearCanvasMemory()
{
    std::fill (gridData.begin(), gridData.end(), 0.0f);
}

void SpectrogramCanvas::paint (juce::Graphics& g)
{
    if (processor != nullptr && processor->getCanvasImage().isValid())
    {
        g.drawImage (processor->getCanvasImage(),
                     getLocalBounds().toFloat(),
                     juce::RectanglePlacement::stretchToFit,
                     false);
    }
    else
    {
        g.fillAll (canvasBgColour);
    }

    g.setColour (juce::Colours::darkgreen.withAlpha (0.9f));
    g.drawRect (getLocalBounds(), 1);
}

void SpectrogramCanvas::evaluateMouse (const juce::MouseEvent& e, bool isDrawing)
{
    if (processor == nullptr || ! isShowing())
        return;

    auto localEvent = e.getEventRelativeTo (this);
    bool inside = getLocalBounds().contains (localEvent.getPosition());
    bool leftDown = e.mods.isLeftButtonDown();

    if (inside && leftDown && isDrawing)
    {
        int gridX = (localEvent.x * numColumns) / getWidth();
        // frequency Y: 0 is at the bottom
        int gridY = numRows - 1 - ((localEvent.y * numRows) / getHeight());

        if (gridX >= 0 && gridX < numColumns && gridY >= 0 && gridY < numRows) {
            drawSoftBrush (gridX, gridY, 7);
        }
    }
}

void SpectrogramCanvas::setPixelValue (int x, int y, float value)
{
    if (x < 0 || x >= numColumns || y < 0 || y >= numRows)
        return;

    size_t index = static_cast<size_t> (x) * static_cast<size_t> (numRows) + static_cast<size_t> (y);

    // damping/comparison: update only if the new value is greater
    float newValue = std::max (gridData[index], value);

    if (gridData[index] != newValue)
    {
        gridData[index] = newValue;
        // only push to FIFO on an actual change (prevents FIFO flooding)
        if (processorFifo != nullptr) {
            processorFifo->push (x, y, newValue);
        }
        // Set pixels in the processor image
        if (processor != nullptr)
        {
            auto& img = processor->getCanvasImage();
            if (img.isValid()) {
                // inversion for Y-axis (0 is at the top)
                int imageY = numRows - 1 - y;

                juce::Colour pixelColour = canvasBgColour.interpolatedWith (myCyan, newValue);
                img.setPixelAt (x, imageY, pixelColour);
            }
        }
        repaint();
    }
}

void SpectrogramCanvas::drawSoftBrush (int centerX, int centerY, int radius)
{
    for (int dx = -radius; dx <= radius; ++dx)
    {
        for (int dy = -radius; dy <= radius; ++dy)
        {
            float dist = std::sqrt (static_cast<float>(dx * dx + dy * dy));
            if (dist <= static_cast<float>(radius))
            {
                float value = 1.0f - (dist / static_cast<float>(radius + 1));
                if (value > 0.0f)
                {
                    setPixelValue (centerX + dx, centerY + dy, value);
                }
            }
        }
    }
}
