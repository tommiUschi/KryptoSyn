#include "GuiUtils.h"
#include <cmath>

namespace GuiUtils
{
    juce::NormalisableRange<float> createControlPointRange(
        const std::vector<std::pair<float, float>>& controlPoints,
        float stepSize)
    {
        // safety: If the vector is empty, return default range
        if (controlPoints.empty())
            return juce::NormalisableRange<float>(0.0f, 1.0f);
        const float rangeStartVal = controlPoints.front().second;
        const float rangeEndVal   = controlPoints.back().second;
        // conversion: 0.0..1.0 -> value (e.g. dB)
        auto convertFrom0To1 = [controlPoints](float, float, float norm) {
            if (norm <= 0.0f) return controlPoints.front().second;
            if (norm >= 1.0f) return controlPoints.back().second;

            for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
                if (norm >= controlPoints[i].first && norm <= controlPoints[i + 1].first) {
                    float t = (norm - controlPoints[i].first) / (controlPoints[i + 1].first - controlPoints[i].first);
                    return controlPoints[i].second + t * (controlPoints[i + 1].second - controlPoints[i].second);
                }
            }
            return controlPoints.front().second;
        };
        //conversion: Value (e.g. dB) -> 0.0..1.0
        auto convertTo0To1 = [controlPoints](float, float, float val) {
            if (val <= controlPoints.front().second) return 0.0f;
            if (val >= controlPoints.back().second) return 1.0f;

            for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
                if (val >= controlPoints[i].second && val <= controlPoints[i + 1].second) {
                    float t = (val - controlPoints[i].second) / (controlPoints[i + 1].second - controlPoints[i].second);
                    return controlPoints[i].first + t * (controlPoints[i + 1].first - controlPoints[i].first);
                }
            }
            return 0.0f;
        };
        // snapping on step size
        auto snapToStep = [stepSize](float rangeStart, float rangeEnd, float val) {
            if (stepSize <= 0.0f) return val;
            float snapped = rangeStart + std::round((val - rangeStart) / stepSize) * stepSize;
            return juce::jlimit(rangeStart, rangeEnd, snapped);
        };
        return juce::NormalisableRange<float>(
            rangeStartVal,
            rangeEndVal,
            convertFrom0To1,
            convertTo0To1,
            snapToStep
        );
    }
}
