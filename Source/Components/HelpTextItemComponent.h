
#pragma once

#include <foleys_gui_magic/foleys_gui_magic.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_core/juce_core.h>
#include <functional>

/**
 * for display text of the tutorial/license window
 */

class HelpTextItemComponent : public juce::Component
{
public:
    HelpTextItemComponent()
    {
        addAndMakeVisible (textEditor);
    }

    ~HelpTextItemComponent() override = default;

    // we call this method from the outside when everything is ready
    void initialise()
    {
        // the desired configuration for the tutorial
        juce::LookAndFeel_V4* lookAndFeel = new juce::LookAndFeel_V4();
        textEditor.setMultiLine (true);
        textEditor.setReadOnly (true);
        textEditor.setTitle("Tutorial");
        textEditor.setBorder(juce::BorderSize<int>(0));
        textEditor.setJustification(juce::Justification::topLeft);
        textEditor.setLookAndFeel(lookAndFeel);
        textEditor.setScrollbarsShown (false);
        textEditor.setCaretVisible (false);
        textEditor.setText(" ");
    }

    void setText(juce::String text)
    {
        textString = text;
        textEditor.setText (textString);
    }


    void setTextEditCustomColors (juce::Colour bg, juce::Colour colour)
    {
        textColor = colour;
        backgroundColour = bg;
        textEditor.setColour (juce::TextEditor::backgroundColourId, bg);
        textEditor.setColour (juce::TextEditor::textColourId, textColor);
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced (5);
        int boxHeight = 9800;
        int gap = 6;
        textEditor.setBounds (bounds.removeFromTop (boxHeight));
        textEditor.setBorder(juce::BorderSize<int>(0));
        bounds.removeFromTop (gap);
    }

private:
    juce::TextEditor textEditor;
    juce::Colour textColor;
    juce::Colour backgroundColour;
    juce::String textString;
};
