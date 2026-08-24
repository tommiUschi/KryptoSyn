#include "SpectrogramMatrixItem.h"
#include "PluginProcessor.h"

SpectrogramMatrixItem::SpectrogramMatrixItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node)
    : foleys::GuiItem (builder, node)
{
    if (auto* proc = dynamic_cast<AudioPluginAudioProcessor*> (builder.getMagicState().getProcessor()))
    {
        canvasComponent.setFIFO (&proc->getPixelFIFO());
        canvasComponent.setProcessor (proc);

        // clear-Trigger integration
        builder.getMagicState().addTrigger ("clear_canvas", [this, proc]()
        {
            proc->clearCanvasImage();
            canvasComponent.clearCanvasMemory();
            proc->getPixelFIFO().pushClearCommand();
            canvasComponent.repaint();
        });
    }

    addAndMakeVisible (canvasComponent);
}

void SpectrogramMatrixItem::update()
{
}

juce::Component* SpectrogramMatrixItem::getWrappedComponent()
{
    return &canvasComponent;
}
//============================================================================

OutputSpectrogramItem::OutputSpectrogramItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node)
: foleys::GuiItem (builder, node)
{
    if (auto* proc = dynamic_cast<AudioPluginAudioProcessor*> (builder.getMagicState().getProcessor()))
    {
        canvasOutputComponent.setFIFO (&proc->getOutputFFTFIFO());
    }
    addAndMakeVisible (canvasOutputComponent);
}

juce::Component* OutputSpectrogramItem::getWrappedComponent()
{
    return &canvasOutputComponent;
}
