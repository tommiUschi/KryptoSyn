
#pragma once

#include <foleys_gui_magic/foleys_gui_magic.h>

/** we need it für reverse spectrogram, to draw with the mouse*/

class CustomMouseComponent : public juce::Component
{
public:
    CustomMouseComponent()
    {
        // we register globally on the desktop
        // this also allows us to catch drags that were started outside!
        juce::Desktop::getInstance().addGlobalMouseListener (this);
    }

    ~CustomMouseComponent() override
    {
        // important for cleanup to avoid dangling pointers
        juce::Desktop::getInstance().removeGlobalMouseListener (this);
    }

    // this function pointer is triggered when the event occurs (true) or is exited (false)
    std::function<void(bool)> onStateChanged;

    // these two methods are now called globally for every mouse movement within the plugin
    void mouseMove (const juce::MouseEvent& e) override { evaluateMouse (e); }
    void mouseDrag (const juce::MouseEvent& e) override { evaluateMouse (e); }

    void paint (juce::Graphics& g) override
    {
        // Visual feedback: Cyan when active, gray when inactive
        g.fillAll (isHoveredAndPressed ? juce::Colours::darkcyan : juce::Colours::darkgrey);
        g.setColour (juce::Colours::white);
        g.drawText (buttonText, getLocalBounds(), juce::Justification::centred);
    }

    void setButtonText (const juce::String& text) { buttonText = text; repaint(); }

private:
    void evaluateMouse (const juce::MouseEvent& e)
    {
        // We convert the global mouse coordinates into the local pixel coordinates of *this* component
        auto localEvent = e.getEventRelativeTo (this);
        
        // is the mouse within our button boundaries?
        bool inside = getLocalBounds().contains (localEvent.getPosition());
        
        // is the left mouse button pressed?
        bool leftDown = e.mods.isLeftButtonDown();

        // the final target event
        bool currentState = (inside && leftDown);

        // we only trigger the logic if the state changes (saves CPU)
        if (currentState != isHoveredAndPressed)
        {
            isHoveredAndPressed = currentState;
            repaint();

            if (onStateChanged != nullptr)
                onStateChanged (isHoveredAndPressed);
        }
    }

    juce::String buttonText;
    bool isHoveredAndPressed = false;
};
