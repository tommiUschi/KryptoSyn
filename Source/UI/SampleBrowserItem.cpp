
#include <foleys_gui_magic/foleys_gui_magic.h>
#include "SampleBrowserItem.h"
#include "PluginProcessor.h"

SampleBrowserItem::SampleBrowserItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node)
    : foleys::GuiItem (builder, node)
{
    // access to the processor via the current PGM API
    if (auto* proc = dynamic_cast<AudioPluginAudioProcessor*> (builder.getMagicState().getProcessor()))
    {
        // use the correct variable name (onInstrumentSelected)
        browserComponent.onInstrumentSelected = [proc](const juce::File& instrumentFolder)
        {
            // tnow triggers the debug output in the processor
            proc->loadSampleFromFile (instrumentFolder);
        };
    }
    // only now that lambda exists do we trigger the
    // folder search. This causes the start trigger to fire directly into the ready-to-go Lambda!
    browserComponent.initialise();

    // extracting colors from the XML node with safe fallbacks
    auto bgStr      = node.getProperty ("combo-background").toString();
    auto textStr    = node.getProperty ("combo-text").toString();
    auto outlineStr = node.getProperty ("combo-outline").toString();
    auto arrowStr   = node.getProperty ("combo-arrow").toString();

    auto bg      = bgStr.isNotEmpty()      ? juce::Colour::fromString (bgStr)      : juce::Colour (0xff011511);
    auto text    = textStr.isNotEmpty()    ? juce::Colour::fromString (textStr)    : juce::Colour (0xffeafeb5);
    auto outline = outlineStr.isNotEmpty() ? juce::Colour::fromString (outlineStr) : juce::Colour (0xff00816A);
    auto arrow   = arrowStr.isNotEmpty()   ? juce::Colour::fromString (arrowStr)   : juce::Colour (0xff00e3e9);

    //apply colors to the UI component
    browserComponent.setCustomColors (bg, text, outline, arrow);

    addAndMakeVisible (browserComponent);
}

void SampleBrowserItem::update()
{
    // (if the GUI layout is updated at runtime)
}

juce::Component* SampleBrowserItem::getWrappedComponent()
{
    return &browserComponent;
}

