
#include <foleys_gui_magic/foleys_gui_magic.h>
#include "SampleBrowserItem.h"
#include "PluginProcessor.h"

struct PropertySamplerBrowseListener : public juce::Value::Listener {
    PropertySamplerBrowseListener (juce::Value v, std::function<void(const juce::var&)> callback)
        : valueToListen (v), onChangedCallback (std::move (callback))
    {
        valueToListen.addListener (this);
    }

    ~PropertySamplerBrowseListener() override
    {
        valueToListen.removeListener (this);
    }

    void valueChanged (juce::Value& v) override
    {
        if (onChangedCallback)
            onChangedCallback (v.getValue());
    }

    juce::Value valueToListen;
    std::function<void(const juce::var&)> onChangedCallback;
};

SampleBrowserItem::~SampleBrowserItem() = default;
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

    // apply colors to the UI component
    browserComponent.setCustomColors (bg, text, outline, arrow);

    auto updateGUI = [this] (const juce::var&)
    {
        auto parent = getMagicState().getPropertyAsValue ("samplParentDir").toString();
        auto instr  = getMagicState().getPropertyAsValue ("samplInstrFolder").toString();
        auto file   = getMagicState().getPropertyAsValue ("samplFile").toString();

        if (parent.isNotEmpty() && instr.isNotEmpty())
        {
            juce::MessageManager::callAsync ([this, parent, instr, file]()
            {
                // TEST: If the GUI already displays this state (because the user clicked),
                // we and prevent the quadruple loading cascade!
                if (browserComponent.getCurrentCategory() == parent &&
                    browserComponent.getCurrentSubCategory() == instr &&
                    browserComponent.getCurrentFile() == file)
                {
                    return;
                }

                // the UI is synchronized and audio reloaded only when loading an external preset:
                browserComponent.selectFoldersByName (parent, instr, file, false);
            });
        }
    };

    // register listeners for all three variables:
    MyPropertySamplerBrowseListener.push_back (std::make_unique<PropertySamplerBrowseListener> (
        getMagicState().getPropertyAsValue ("samplParentDir"), updateGUI));

    MyPropertySamplerBrowseListener.push_back (std::make_unique<PropertySamplerBrowseListener> (
        getMagicState().getPropertyAsValue ("samplInstrFolder"), updateGUI));

    MyPropertySamplerBrowseListener.push_back (std::make_unique<PropertySamplerBrowseListener> (
        getMagicState().getPropertyAsValue ("samplFile"), updateGUI));

    addAndMakeVisible (browserComponent);
}

void SampleBrowserItem::update()
{
    // (if the GUI layout is updated at runtime)
    // is called when the PGM GUI layout is updated)
    if (auto* proc = dynamic_cast<AudioPluginAudioProcessor*> (getMagicState().getProcessor()))
    {
        auto currentCategory   = proc->samplParentDir;
        auto currentInstrument = proc->samplInstrFolder;
        auto currentFile       = proc->samplFile; // retrieves the saved file name (e.g., "Guitar.sfz")

        if (currentCategory.isNotEmpty() && currentInstrument.isNotEmpty())
        {
            // jetzt stimmen die 4 Parameter: Category, Instrument, File, triggerAudioLoad (false)
            browserComponent.selectFoldersByName (currentCategory, currentInstrument, currentFile, false);
        }
    }
}

juce::Component* SampleBrowserItem::getWrappedComponent()
{
    return &browserComponent;
}

