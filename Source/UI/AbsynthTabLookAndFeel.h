//
// Created by tommibe on 13.02.26.
//


#pragma once

#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace AbsynthColors
{
	// cool color: (0, 193, 161)
	
    // main-background
    const juce::Colour background   = juce::Colour::fromRGB (30, 40, 35);
    const juce::Colour panel        = juce::Colour::fromRGB (37, 51, 44);
    const juce::Colour sidebar      = juce::Colour::fromRGB (44, 61, 53);
    const juce::Colour header      = juce::Colour::fromRGB (34, 48, 41);

    // layer-depth
    const juce::Colour InnerShadow  = juce::Colour::fromRGBA (0, 0, 0, 102);
    const juce::Colour OuterGlow  = juce::Colour::fromRGBA (110, 187, 154, 64);
    const juce::Colour DividerLines  = juce::Colour::fromRGBA (0, 0, 0, 51);

    // Secondary Accent
    const juce::Colour headAlternativeModulationer = juce::Colour::fromRGB (95, 168, 160);
    const juce::Colour highLight = juce::Colour::fromRGB (123, 198, 204);

    // primary Accent
    const juce::Colour accent       = juce::Colour::fromRGB (110, 187, 154);
    const juce::Colour accentHover  = juce::Colour::fromRGB (142, 211, 181);
    const juce::Colour accentDim    = juce::Colour::fromRGB (74, 127, 107);

    // text-Colors
    const juce::Colour textMain     = juce::Colour::fromRGB (210, 230, 218);
    const juce::Colour textSubtle   = juce::Colour::fromRGB (159, 184, 172);
    const juce::Colour textDisabled   = juce::Colour::fromRGB (108, 128, 118);

    // butons/Tabs
    const juce::Colour normalButton     = juce::Colour::fromRGB (47, 64, 55);
    const juce::Colour hoverButton      = juce::Colour::fromRGB (62, 85, 73);
    const juce::Colour activeButton     = juce::Colour::fromRGB (110, 187, 154);
    const juce::Colour pressedButton    = juce::Colour::fromRGB (85, 127, 107);

}


// not yet used/working
class AbsynthScrollbarLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawScrollbarButton(juce::Graphics& g, juce::ScrollBar& scrollbar,
                            int width, int height, int buttonDirection,
                            bool isScrollbarVertical, bool isMouseOverButton,
                            bool isButtonDown) override
    {
        auto area = scrollbar.getLocalBounds().toFloat().reduced (1.0f);

        const juce::Colour base      = juce::Colour::fromRGB (30, 163, 141);   // dunkles Grün-Grau
        const juce::Colour hover     = juce::Colour::fromRGB (45, 186, 163);

        juce::Colour fill;

        if (isButtonDown )
            fill = hover.brighter (0.15f);
        else if (isMouseOverButton)
            fill = hover;
        else
            fill = base;

        const juce::ColourGradient grad (
            fill.brighter (0.15f), area.getTopLeft(),
            fill.darker   (0.25f), area.getBottomLeft(),
            false
        );
        g.setGradientFill (grad);
        g.fillRoundedRectangle (area, 2.0f);

        scrollbar.setColour(juce::ScrollBar::backgroundColourId, base);

    }

};

#pragma region GENERA_VIEW_DESIGN
class AbsynthViewLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawPropertyComponentBackground(juce::Graphics&, int width, int height,
                juce::PropertyComponent&) override
    {
        juce::PropertyComponent* prop;
        juce::ImageCache* img;
    }


};
#pragma endregion GENERAL_VIEW_DESIGN

#pragma region TAB_DESIGN // for the main tabs
class AbsynthTabLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawTabButton (juce::TabBarButton& button,
                        juce::Graphics& g,
                        bool isMouseOver,
                        bool isMouseDown) override
    {
        auto area = button.getLocalBounds().toFloat().reduced (2.0f);

        const bool isActive = button.isFrontTab();

        // basic colors
        const juce::Colour base      = juce::Colour::fromRGB (30, 56, 42);   // dunkles Grün-Grau
        const juce::Colour hover     = juce::Colour::fromRGB (45, 60, 50);
        const juce::Colour active    = juce::Colour::fromRGB (70, 110, 90);   // leichtes "Glow-Grün"
        const juce::Colour textCol   = juce::Colour::fromRGB (200, 220, 210);

        // define background-colors
        juce::Colour fill;

        if (isActive)
            fill = active;
        else if (isMouseDown)
            fill = hover.brighter (0.4f);
        else if (isMouseOver)
            fill = hover;
        else
            fill = base;

        // small gradient
        const juce::ColourGradient grad (
            fill.brighter (0.15f), area.getTopLeft(),
            fill.darker   (0.25f), area.getBottomLeft(),
            false
        );

        g.setGradientFill (grad);
        g.fillRoundedRectangle (area, 6.0f);

        // ✨ subtile glow on aktive tab
        if (isActive)
        {
            juce::Colour glow = active.brighter (0.6f).withAlpha (0.25f);
            g.setColour (glow);
            g.drawRoundedRectangle (area.expanded (1.5f), 7.0f, 2.0f);
        }

        // very subtile outline
        g.setColour (juce::Colours::lightgreen);//::black.withAlpha (0.4f));
        g.drawRoundedRectangle (area, 7.0f, 1.2f);

        // text
        g.setColour (textCol.withAlpha (isActive ? 1.0f : 0.75f));

        g.setFont (juce::Font(14.0f, juce::Font::plain));

        g.drawText (button.getButtonText(),
                    area,
                    juce::Justification::centred);
    }
};
#pragma endregion TAB_DESIGN

class AbsynthTabLookAndFeel_V2 : public juce::LookAndFeel_V4
{
public:
    void drawTabButton (juce::TabBarButton& button,
                        juce::Graphics& g,
                        const bool isMouseOver,
                        const bool isMouseDown) override
    {
        auto area = button.getLocalBounds().toFloat().reduced (1.8f);

        const bool isActive = button.isFrontTab();

        const juce::Colour base      = juce::Colour::fromRGB (30, 56, 42);   // dunkles Grün-Grau
        const juce::Colour hover     = juce::Colour::fromRGB (13.3, 37.5, 33);
        const juce::Colour active    = juce::Colour::fromRGB (28, 95, 64);   // leichtes "Glow-Grün"
        const juce::Colour textCol   = juce::Colour::fromRGB (216, 255, 195);

        const juce::Colour borderCol = juce::Colour::fromRGB (182, 120, 0);

        juce::Colour fill;

        if (isActive)
            fill = active;
        else if (isMouseDown)
            fill = hover.brighter (0.4f);
        else if (isMouseOver)
            fill = hover;
        else
            fill = base;

        const juce::ColourGradient grad (
            fill.brighter (0.15f), area.getTopLeft(),
            fill.darker   (0.55f), area.getBottomLeft(),
            false
        );

        g.setGradientFill (grad);
        g.fillRoundedRectangle (area, 3.4f);

        if (isActive)
        {
            juce::Colour glow = active.brighter (0.6f).withAlpha (0.28f);
            g.setColour (glow);
            g.drawRoundedRectangle (area.expanded (1.5f), 5.0f, 0.80f);
        }

        g.setColour (borderCol);
        g.drawRoundedRectangle (area, 3.4f, 0.54f);

        g.setColour (textCol.withAlpha (isActive ? 1.0f : 0.85f));

        g.setFont (juce::Font(13.0f, juce::Font::plain));

        g.drawText (button.getButtonText(),
                    area,
                    juce::Justification::centred);
    }
};

#pragma region TITLE_DESIGN
class AbsynthTitelLookAndFeel : public juce::LookAndFeel_V4
{
public:
    AbsynthTitelLookAndFeel()
    {
        // sets the background color of JUCE's own UI window components
        setColour (juce::ResizableWindow::backgroundColourId, juce::Colour::fromRGB (25, 30, 28));

        // sets the default text color for window titles (if JUCE draws the title bar)
        setColour (juce::DocumentWindow::textColourId, juce::Colour::fromRGB (57, 93, 93));
    }
    void drawDocumentWindowTitleBar (juce::DocumentWindow& window, juce::Graphics& g,
                                int w, int h, int titleSpaceX, int titleSpaceW,
                                const juce::Image* icon, bool drawTitleTextOnLeft) override
    {
        // color of the title bar background
        g.setColour (juce::Colour::fromRGB (57, 93, 93));
        g.fillAll();

        // set font size and style
        g.setFont (juce::Font (h * 0.6f, juce::Font::plain));

        // font color
        g.setColour (juce::Colour::fromRGB (83, 255, 234));

        // draw the window name ("Synthesizer")
        g.drawText (window.getName(), 10, 0, w - 20, h,
                    juce::Justification::centredLeft, true);
    }
};
#pragma endregion TITLE_DESIGN

#pragma region GENERAL_DESIGN
class AbsynthMainLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawTabButton (juce::TabBarButton& button,
                        juce::Graphics& g,
                        bool isMouseOver,
                        bool isMouseDown) override
    {
        //ToDo: new design is possible
    }
};
#pragma endregion GENERAL_DESIGN



