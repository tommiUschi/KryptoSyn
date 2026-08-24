//
// Created by tommibe on 13.02.26.
//


#pragma once

#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../Assets/BinaryData.h"
#include <functional>


class CustomLNFButton : public juce::LookAndFeel_V3
{
    static constexpr uint32_t BG_OFF   = 0xFF1c4f45;
    static constexpr uint32_t BG_ON    = 0xFF338382;
    static constexpr uint32_t TEXT_OFF = 0xFFd1cf9d;
    static constexpr uint32_t TEXT_ON  = 0xFFA6FF00;

public:
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                           bool shouldDrawButtonAsHighlighted,
                           bool shouldDrawButtonAsDown) override
    {

        auto area = button.getLocalBounds().toFloat().reduced (0.5f);
        const juce::Colour borderCol = juce::Colour::fromRGB (0, 255, 70.8);
        // It works natively now, since PGM links the property to the ToggleState!
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

        // Draw text centered on the button (the standard checkbox is ignored)
        g.setFont (14.0f);
        g.setColour (isOn ? juce::Colour (TEXT_ON) : juce::Colour (TEXT_OFF));
        g.drawText (button.getButtonText(),
                    button.getLocalBounds(),
                    juce::Justification::centred, false);
    }

};

//=============================================================================
/**
 * class CustomLedLNF is used to fake a LED in the GUI
 */

class CustomLedLNF : public juce::LookAndFeel_V3
{
public:
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                           bool /*shouldDrawButtonAsHighlighted*/,
                           bool /*shouldDrawButtonAsDown*/) override
    {
        // Clicks pass through the LED to the underlying GUI.
        button.setInterceptsMouseClicks (false, false);

        const bool isOn = button.getToggleState();
        juce::Image ledImage;

        if (isOn) {
            // Oscillator AKTIV -> load the green LED
            ledImage = juce::ImageCache::getFromMemory (BinaryData::greenLED_png,
                                                        BinaryData::greenLED_pngSize);
        } else {
            // Oscllator INAKTIV -> load the blue LED
            ledImage = juce::ImageCache::getFromMemory (BinaryData::blueLED_new_png,
                                                        BinaryData::blueLED_new_pngSize);
        }

        // If the images have been successfully loaded from the binary data, we draw them
        if (ledImage.isValid())
        {
            // Now that we have two dedicated images for on/off,
            // we draw both with full opacity (1.0f).
            g.setOpacity (1.0f);

            g.drawImageWithin (ledImage,
                               0, 0, button.getWidth(), button.getHeight(),
                               juce::RectanglePlacement::centred);
        }
    }
};

//==============================================================================





