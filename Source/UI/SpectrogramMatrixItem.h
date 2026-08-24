//
// Created by tommibe on 12.05.26.
//

#pragma once
#include <foleys_gui_magic/foleys_gui_magic.h>
#include "../Components/SpectrogramCanvas.h"

/**
 * this class handles the pixel for the reverse -and outputspectrograms
 */
class SpectrogramMatrixItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (SpectrogramMatrixItem)

    SpectrogramMatrixItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node);

    void update() override;
    juce::Component* getWrappedComponent() override;

private:
    SpectrogramCanvas canvasComponent;
};

class OutputSpectrogramItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (OutputSpectrogramItem)

    OutputSpectrogramItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node);

    void update() override {}
    juce::Component* getWrappedComponent() override;

private:
    OutputSpectrogramCanvas canvasOutputComponent;
};
