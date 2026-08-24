//
// Created by tommibe on 13.02.26.
//
#pragma once

#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Assets/BinaryData.h"
#include <functional>

/**
 * this class ist used for the keyboard-change-button-design
 */
class CustomSwitchButton : public juce::LookAndFeel_V3
{
    static constexpr uint32_t BG_OFF   = 0xFF1c4f45;
    static constexpr uint32_t BG_ON    = 0xFF1c4f45;
    static constexpr uint32_t TEXT_OFF = 0xFFd1cf9d;
    static constexpr uint32_t TEXT_ON  = 0xFFd1cf9d;

public:
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override
    {

        auto area = button.getLocalBounds().toFloat().reduced (0.5f);
        const juce::Colour borderCol = juce::Colour::fromRGB (0, 255, 70.8);
        // it works natively now, since PGM links the property to the ToggleState!
        const bool isOn = button.getToggleState();
        juce::Colour base (isOn ? BG_ON : BG_OFF);

        if      (shouldDrawButtonAsDown)        base = base.brighter (1.0f);
        else if (shouldDrawButtonAsHighlighted) base = base.brighter (0.30f);

        auto bounds = button.getLocalBounds().toFloat().reduced (1.8f);
        juce::Path outline;

        const juce::ColourGradient grad (
            base.brighter (0.15f), area.getTopLeft(),
            base.darker   (0.60f), area.getBottomLeft(),
            false
        );

        g.setGradientFill (grad);
        g.fillRoundedRectangle (area, 3.4f);
        outline.scaleToFit(button.getPosition().getX(), button.getPosition().getY(),
                            button.getWidth(),button.getHeight(), true);
        outline.addRoundedRectangle (bounds, 2.5f);

        // 2. Text parsen und je nach Zustand auswählen
        juce::String rawText = button.getButtonText();
        juce::String displayText = rawText;

        if (rawText.containsChar ('|'))
        {
            // left = State false (OFF), right = state true (ON)
            auto offText = rawText.upToFirstOccurrenceOf ("|", false, false).trim();
            auto onText  = rawText.fromFirstOccurrenceOf ("|", false, false).trim();

            displayText = isOn ? onText : offText;
        }

        // draw text
        g.setFont (13.8f);
        g.setColour (isOn ? juce::Colour (TEXT_ON) : juce::Colour (TEXT_OFF));

        // drawFittedText slightly scales down the font if there is insufficient space
        // and allows for up to two lines of text wrapping
        g.drawFittedText (displayText,
                          button.getLocalBounds().reduced (4, 2),
                          juce::Justification::centred,
                          2,       // max. rows
                          0.8f);   // min. scaling
    }
};

//=============================================================================






