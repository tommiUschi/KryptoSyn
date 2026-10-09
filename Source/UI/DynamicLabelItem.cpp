
#include <foleys_gui_magic/foleys_gui_magic.h>
#include "DynamicLabelItem.h"


// small helper function for parsing JUCE text alignment
static juce::Justification parseJustification(const juce::String& str)
{
    if (str == "top-left")       return juce::Justification::topLeft;
    if (str == "top-right")      return juce::Justification::topRight;
    if (str == "top-center" || str == "top-centred") return juce::Justification::centredTop;
    if (str == "bottom-left")    return juce::Justification::bottomLeft;
    if (str == "bottom-right")   return juce::Justification::bottomRight;
    if (str == "bottom-center" || str == "bottom-centred") return juce::Justification::centredBottom;
    if (str == "left" || str == "centred-left")  return juce::Justification::centredLeft;
    if (str == "right" || str == "centred-right") return juce::Justification::centredRight;

    return juce::Justification::centred; // Fallback
}


DynamicLabelItem::DynamicLabelItem(foleys::MagicGUIBuilder& builder, const juce::ValueTree& node)
    : foleys::GuiItem(builder, node)
{
    addAndMakeVisible(label);
}

std::vector<foleys::SettableProperty> DynamicLabelItem::getSettableProperties() const
{
    return {
            { configNode, "property",      foleys::SettableProperty::Text,   {}, {} },
            { configNode, "label-text", foleys::SettableProperty::Colour, {}, {} },
            { configNode, "font-size",  foleys::SettableProperty::Number, {}, {} },
            { configNode, "justification",  foleys::SettableProperty::Choice, {}, {} }
    };
}

void DynamicLabelItem::update()
{
    // read which property was specified in the XML (e.g., property="SL MOD_A_1_caption")
    const auto propName = getProperty("property").toString();

    if (propName.isNotEmpty())
    {
        // JUCE natively links the label's text to the value from magicState
        label.getTextValue().referTo(getMagicState().getPropertyAsValue(propName));
    }

    // read text color from "label-text" (property in 'foleys_gui_magic')
    const auto colorVar = getProperty("label-text");
    if (!colorVar.isVoid())
    {
        const auto col = juce::Colour::fromString(colorVar.toString());
        label.setColour(juce::Label::textColourId, col);
    }

    const auto sizeVar = getProperty("font-size");
    if (!sizeVar.isVoid())
    {
        const float fontSize = static_cast<float>(sizeVar);
        label.setFont(juce::Font(fontSize));
    }

    const auto allignVar = getProperty("justification");
    if (!allignVar.isVoid())
    {
        const juce::String& justification = allignVar.toString();
        label.setJustificationType(parseJustification(allignVar.toString()));
    }

}

juce::Component* DynamicLabelItem::getWrappedComponent()
{
    return &label;
}