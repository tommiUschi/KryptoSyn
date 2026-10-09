#include "ParameterValueLabelItem.h"

// similar like 'DynamicLabelItem'

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

ParameterValueLabelItem::ParameterValueLabelItem(foleys::MagicGUIBuilder& builder, const juce::ValueTree& node)
    : foleys::GuiItem(builder, node)
{
    addAndMakeVisible(label);

    // Label selbst transparent halten – Hintergrund wird von PGM gezeichnet
    label.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    label.setColour(juce::Label::textColourId, juce::Colour::fromRGB(0x00, 0xFF, 0x66)); // Digital-Grün
    label.setJustificationType(juce::Justification::centred);
}

ParameterValueLabelItem::~ParameterValueLabelItem()
{
    cancelPendingUpdate();
    if (attachedParameter != nullptr)
        attachedParameter->removeListener(this);
}

std::vector<foleys::SettableProperty> ParameterValueLabelItem::getSettableProperties() const
{
    std::vector<foleys::SettableProperty> props;

    // Lambda für die Parameter-Dropdownliste im PGM GUI Designer
    auto menuLambda = [this](juce::ComboBox& combo)
    {
        auto* nonConstThis = const_cast<ParameterValueLabelItem*>(this);
        combo.addItemList(nonConstThis->getMagicState().getParameterNames(), 1);
    };

    props.push_back({ configNode, foleys::IDs::parameter, foleys::SettableProperty::Choice, {}, menuLambda });
    props.push_back({ configNode, "label-text",       foleys::SettableProperty::Colour, {}, {} });
    props.push_back({ configNode, "font-size",        foleys::SettableProperty::Number, {}, {} });
    props.push_back({ configNode, "justification",    foleys::SettableProperty::Choice, {}, {} });

    return props;
}

void ParameterValueLabelItem::update()
{
    // 1. Alter Listener aufräumen & neuen Parameter koppeln
    if (attachedParameter != nullptr)
    {
        attachedParameter->removeListener(this);
        attachedParameter = nullptr;
    }

    const auto paramID = getProperty(foleys::IDs::parameter).toString();
    if (paramID.isNotEmpty())
    {
        attachedParameter = getMagicState().getParameter(paramID);
        if (attachedParameter != nullptr)
        {
            attachedParameter->addListener(this);
            updateLabelText();
        }
        else
        {
            label.setText("N/A: " + paramID, juce::dontSendNotification);
        }
    }
    else
    {
        label.setText("---", juce::dontSendNotification);
    }

    // 2. Schriftfarbe ("label-text")
    const auto colorVar = getProperty("label-text");
    if (!colorVar.isVoid())
        label.setColour(juce::Label::textColourId, juce::Colour::fromString(colorVar.toString()));

    // 3. Schriftgröße ("font-size")
    const auto sizeVar = getProperty("font-size");
    if (!sizeVar.isVoid())
        label.setFont(juce::Font(static_cast<float>(sizeVar)));

    // 4. Textausrichtung ("justification")
    const auto justVar = getProperty("justification");
    if (!justVar.isVoid())
        label.setJustificationType(parseJustification(justVar.toString()));
}

void ParameterValueLabelItem::resized()
{
    // Absolut essenziell für die Sichtbarkeit:
    foleys::GuiItem::resized();
    label.setBounds(getClientBounds());
}

juce::Component* ParameterValueLabelItem::getWrappedComponent()
{
    return &label;
}

void ParameterValueLabelItem::parameterValueChanged(int parameterIndex, float newValue)
{
    // Wird evtl. vom Audio-Thread aufgerufen!
    // triggerAsyncUpdate() ist thread-sicher, sperrfrei und fasst schnelle Events zusammen.
    triggerAsyncUpdate();
}

void ParameterValueLabelItem::handleAsyncUpdate()
{
    // Läuft garantiert sicher auf dem JUCE Message Thread (UI)
    updateLabelText();
}

void ParameterValueLabelItem::updateLabelText()
{
    if (attachedParameter != nullptr)
    {
        // Holt exakt die Formatierung aus dem AudioProcessorParameter (z.B. "0.65 Hz", "80 %")
        label.setText(attachedParameter->getCurrentValueAsText(), juce::dontSendNotification);
    }
}