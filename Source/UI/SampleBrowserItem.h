//
// Created by tommibe on 11.05.26.
//

#pragma once

#include <foleys_gui_magic/foleys_gui_magic.h>
#include "../Components/SampleBrowserComponent.h" // Pfad ggf. anpassen

/**
 * class SampleBrowserItem is needed for SampleBrowser component on the sampler page
 */
class SampleBrowserItem : public foleys::GuiItem
{
public:
    FOLEYS_DECLARE_GUI_FACTORY (SampleBrowserItem)

    SampleBrowserItem (foleys::MagicGUIBuilder& builder, const juce::ValueTree& node);

    void update() override;
    juce::Component* getWrappedComponent() override;

private:
    SampleBrowserComponent browserComponent;
};
