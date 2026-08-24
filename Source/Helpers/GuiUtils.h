//
// Created by tommibe on 17.07.26.
//

#pragma once

#include <utility>
#include <vector>
#include <juce_audio_processors/juce_audio_processors.h>

namespace GuiUtils
{
    /**
     * creates a JUCE NormalisableRange based on a list of control points (0.0..1.0 -> target value)
     * (it's for the linear-vertical master slider with logarithmic display)
    */
    juce::NormalisableRange<float> createControlPointRange(
        const std::vector<std::pair<float, float>>& controlPoints,
        float stepSize = 0.1f
    );
}
