/*
 ==============================================================================
    Copyright (c) 2019-2023 Foleys Finest Audio - Daniel Walz
    All rights reserved.

    **BSD 3-Clause License**

    Redistribution and use in source and binary forms, with or without modification,
    are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

 ==============================================================================

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
    IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
    LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
    OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
    OF THE POSSIBILITY OF SUCH DAMAGE.
 ==============================================================================
 */

/*
  ==============================================================================
    Modifications:  static juce::String    rightTabbed   { "rightTabbed" };
    Active since: 03. August 2026
    Author: Thomas Buck
    Description: Added 'glowType': 'glowTypeNone/glowTypeLinear'
  ==============================================================================
*/

#include "foleys_MagicPlotComponent.h"

namespace foleys
{


MagicPlotComponent::MagicPlotComponent()
{
    setColour (plotColourId, juce::Colours::orange);
    setColour (plotFillColourId, juce::Colours::orange.withAlpha (0.5f));
    setColour (plotInactiveColourId, juce::Colours::orange.darker());
    setColour (plotInactiveFillColourId, juce::Colours::orange.darker().withAlpha (0.5f));

    setOpaque (false);
    setPaintingIsUnclipped (true);
}

void MagicPlotComponent::setPlotSource (MagicPlotSource* source)
{
    plotSource = source;
}

void MagicPlotComponent::setDecayFactor (float decayFactor)
{
    decay = decayFactor;
    updateGlowBufferSize();
}

void MagicPlotComponent::setGradientFromString (const juce::String& cssString, Stylesheet& stylesheet)
{
    if (cssString.isNotEmpty())
    {
        gradient = std::make_unique<GradientBackground>();
        gradient->setup(cssString, stylesheet);
    }
    else
    {
        gradient.reset();
    }
}

void MagicPlotComponent::paint (juce::Graphics& g)
{
    if (plotSource == nullptr)
        return;

    const auto lastUpdate = plotSource->getLastDataUpdate();
    if (lastUpdate > lastDataTimestamp)
    {
        plotSource->createPlotPaths (path, filledPath, getLocalBounds().toFloat(), *this);
        lastDataTimestamp = lastUpdate;
    }

    if (! glowBuffer.isNull())
        drawPlotGlowing (g);
    else
    {
        drawPlot (g);
    }
}

void MagicPlotComponent::resized()
{
    lastDataTimestamp = 0;
    updateGlowBufferSize();
}

bool MagicPlotComponent::needsUpdate() const
{
    return plotSource ? (lastDataTimestamp < plotSource->getLastDataUpdate()) : false;
}

void MagicPlotComponent::drawPlot (juce::Graphics& g)
{
    const auto active = plotSource->isActive();
    auto colour = findColour (active ? plotFillColourId : plotInactiveFillColourId);

    if (!gradient && colour.isTransparent() == false)
        g.setColour (colour);

    if (gradient)
        gradient->setupGradientFill (g, getLocalBounds().toFloat());

    if (gradient || !colour.isTransparent())
        g.fillPath (filledPath);

    colour = findColour (active ? plotColourId : plotInactiveColourId);
    if (colour.isTransparent() == false)
    {
        if (glowType == GlowType::glowTypeNone)
        {
            g.setColour (colour);
            g.strokePath (path, juce::PathStrokeType (1.0));
        }
        else if (glowType == GlowType::glowTypeLinear)
        {
            // the broad outer glow (very thick, yet extremely transparent)
            g.setColour (colour.withAlpha (0.8f));
            g.strokePath (path, juce::PathStrokeType (13.0f));

            // the broad outer glow (very thick, yet extremely transparent)
            g.setColour (colour.withAlpha (0.12f));
            g.strokePath (path, juce::PathStrokeType (7.0f));

            // the inner cone of light (medium thickness, slightly higher opacity)
            g.setColour (colour.withAlpha (0.3f));
            g.strokePath (path, juce::PathStrokeType (3.0f));

            // the original, sharp core line right at the very top
            g.setColour (colour.withAlpha (0.9f));
            g.strokePath (path, juce::PathStrokeType (1.2f));

            g.setColour (colour.brighter(0.9));
            g.strokePath (path, juce::PathStrokeType (0.7f));
        }
    }
}

void MagicPlotComponent::drawPlotGlowing (juce::Graphics& g)
{
    if (decay < 1.0f)
        glowBuffer.multiplyAllAlphas (decay);

    juce::Graphics glow (glowBuffer);
    drawPlot (glow);

    g.drawImageAt (glowBuffer, 0, 0);
}

void MagicPlotComponent::updateGlowBufferSize()
{
    const auto w = getWidth();
    const auto h = getHeight();

    if (decay > 0.0f && w > 0 && h > 0)
    {
        if (glowBuffer.getWidth() != w || glowBuffer.getHeight() != h)
            glowBuffer = juce::Image (juce::Image::ARGB, w, h, true);
    }
    else
    {
        glowBuffer = juce::Image();
    }
}


} // namespace foleys
