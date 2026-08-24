//
// Created by tommibe on 13.02.26.
//


#pragma once

#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

/**
 * for customized rotary Slider (used in the left side-bar)
 */

class CustomLNF : public juce::LookAndFeel_V4
{
public:
    CustomLNF();

    ~CustomLNF() override;

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, const float rotaryStartAngle,
                           const float rotaryEndAngle, juce::Slider& slider) override
    {
        // calculate the basics
        auto radius = (juce::jmin (width/1.15f, height/1.15f) / 2.0f) - 5.0f;
        auto centreX = x + width  * 0.5f;
        auto centreY = y + height * 0.5f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // draw the background circle (the "housing")
        g.setColour (juce::Colours::darkgrey.darker(0.3));
        g.fillEllipse (centreX - radius, centreY - radius, radius * 2.0f, radius * 2.0f);

        // draw the glowing ring (progress)
        juce::Path backgroundArc;
        backgroundArc.addCentredArc (centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour (juce::Colours::black);
        g.strokePath (backgroundArc, juce::PathStrokeType (3.0f));

        juce::Path valueArc;
        valueArc.addCentredArc (centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
        g.setColour (juce::Colours::beige);
        g.strokePath (valueArc, juce::PathStrokeType (3.0f));
        // draw pointer/needle
        juce::Path p;
        auto pointerLength = radius * 0.8f;
        auto pointerThickness = 3.0f;
        p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));
        g.setColour (juce::Colours::beige.darker(0.24));
        g.fillPath (p);
    }

    juce::Label* createSliderTextBox (juce::Slider& slider) override
    {
        // for now, we'll let JUCE create the standard label...
        auto* lbl = LookAndFeel_V4::createSliderTextBox (slider);
        // ...and then "tune" it according to our wishes:
        lbl->setColour (juce::Label::textColourId, juce::Colours::lightgreen);
        lbl->setColour (juce::Label::backgroundColourId, juce::Colours::black.withAlpha(1.0f));
        lbl->setColour (juce::Label::outlineColourId, juce::Colours::green.withAlpha(0.8f).brighter());
        // lbl->setFont (14.0f); // Adjust font size
        // if we dont want a frame:
        //lbl->setBorderSize (juce::BorderSize<int>(0));
        return lbl;
    }

    JUCE_DECLARE_NON_COPYABLE(CustomLNF)
};






