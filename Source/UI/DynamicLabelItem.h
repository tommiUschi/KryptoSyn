//
// Created by tommibe on 06.04.26.
//
#pragma once

#include <foleys_gui_magic/foleys_gui_magic.h>
//#include "../Components/DynamicLabelItemComponent.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>


class DynamicLabelItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY(DynamicLabelItem)

    DynamicLabelItem(foleys::MagicGUIBuilder& builder, const juce::ValueTree& node);

    std::vector<foleys::SettableProperty> getSettableProperties() const override;

    void update() override;
    juce::Component* getWrappedComponent() override;

private:
    juce::Label label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DynamicLabelItem)
};
