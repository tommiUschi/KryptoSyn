#pragma once

#include <foleys_gui_magic/foleys_gui_magic.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>


class ParameterValueLabelItem : public foleys::GuiItem,
                                public juce::AudioProcessorParameter::Listener,
                                public juce::AsyncUpdater
{
public:
    FOLEYS_DECLARE_GUI_FACTORY(ParameterValueLabelItem)

    ParameterValueLabelItem(foleys::MagicGUIBuilder& builder, const juce::ValueTree& node);
    ~ParameterValueLabelItem() override;

    std::vector<foleys::SettableProperty> getSettableProperties() const override;
    void update() override;
    void resized() override;
    juce::Component* getWrappedComponent() override;

private:
    // juce::AudioProcessorParameter::Listener
    void parameterValueChanged(int parameterIndex, float newValue) override;
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {}

    // juce::AsyncUpdater (Thread-sichere GUI-Aktualisierung)
    void handleAsyncUpdate() override;

    void updateLabelText();

    juce::Label label;
    juce::AudioProcessorParameter* attachedParameter = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterValueLabelItem)
};