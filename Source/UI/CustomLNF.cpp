//
// Created by tommibe on 13.02.26.
//


#include "CustomLNF.h"

CustomLNF::CustomLNF()
{
    // Text box background
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::black.withAlpha(1.0f));
    // Textbox border (neon-green)
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::green.withAlpha(0.8f).brighter());
    // Text-Color
    setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
}

CustomLNF::~CustomLNF()
{}



