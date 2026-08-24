//
// Created by tommibe on 06.04.26.
//
#pragma once

#include <foleys_gui_magic/foleys_gui_magic.h>
#include "../Components/HelpTextItemComponent.h"

/**
 * Helper-Class for the texts in tutorial -and licence window
 */
class HelpTextItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (HelpTextItem)

    HelpTextItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node);

    std::vector<foleys::SettableProperty> getSettableProperties() const override
    {
        return { { configNode, "text", foleys::SettableProperty::Text, "text", {} },
        { configNode, "colour", foleys::SettableProperty::Colour, "colour", {} }};
    };
    void update() override;
    juce::Component* getWrappedComponent() override;
private:
    juce::TextEditor editor;
    juce::ValueTree myNode;
    HelpTextItemComponent heplTextComponent;
    juce::Colour colour;
    juce::Colour backGroundColor;
    juce::String editorText;
};
