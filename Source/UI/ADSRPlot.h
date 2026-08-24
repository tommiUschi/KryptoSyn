//
// Created by tommibe on 13.02.26.
//
#pragma once

#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <foleys_gui_magic/foleys_gui_magic.h>

/**
 * class ADSRPlot is used to draw all ADSR-Curves in the Plots
 */

class ADSRPlot : public foleys::MagicPlotSource,
                 private juce::AudioProcessorParameter::Listener//, private juce::Timer
{
public:
    ADSRPlot()
    {
        guiState = nullptr;
        this->ADSRPlot::setActive (true);
    }
    ~ADSRPlot() override
    {
        if (attackParam)  attackParam->removeListener(this);
        if (decayParam)   decayParam->removeListener(this);
        if (sustainParam) sustainParam->removeListener(this);
        if (releaseParam) releaseParam->removeListener(this);
    }

    static const juce::Identifier typeId;

    void timerInjection() {
        if (needsUpdate){
            if (auto* broadcaster = dynamic_cast<juce::ChangeBroadcaster*>(this))
                broadcaster->sendChangeMessage();
            needsUpdate = false;
        }
    }

    void setParameters (juce::RangedAudioParameter* a,
                    juce::RangedAudioParameter* d,
                    juce::RangedAudioParameter* s,
                    juce::RangedAudioParameter* r)
    {
        attackParam  = a;
        decayParam   = d;
        sustainParam = s;
        releaseParam = r;
        attackParam->addListener(this);
        decayParam->addListener(this);
        sustainParam->addListener(this);
        releaseParam->addListener(this);
    }

    // default values at startup
    void setDefaults(float defAtt, float defDec, float defSust, float defRelease)
    {
        attackParam->setValue(defAtt); decayParam->setValue(defDec);
        sustainParam->setValue(defSust); releaseParam->setValue(defRelease);
        if (needsUpdate){
            if (auto* broadcaster = dynamic_cast<juce::ChangeBroadcaster*>(this))
                broadcaster->sendChangeMessage();
            needsUpdate = false;
        }
    }

    void setGuiState (foleys::MagicGUIState& stateToUse) { guiState = &stateToUse; }

    // preparation (called during loading/resizing):
    void prepareToPlay (double sampleRate, int samplesPerBlock) override
    {
        juce::ignoreUnused (sampleRate, samplesPerBlock);
    }

    // audio data reception (leave blank for ADSR)
    void pushSamples (const juce::AudioBuffer<float>& buffer) override
    {
        juce::ignoreUnused (buffer);
        this->resetLastDataFlag();
    }

    void setLabelIds (const juce::String& plotGain1, const juce::String& plotGain2, const juce::String& plotGain3,
            const juce::String& plotFilter1, const juce::String& plotFilter2, const juce::String& plotFilter3)
    {
        plotGain1Id = plotGain1; plotGain2Id = plotGain2; plotGain3Id = plotGain3;
        plotFilter1Id = plotFilter1; plotFilter2Id = plotFilter2; plotFilter3Id = plotFilter3;
    }

    // --- AudioProcessorParameter::Listener Methods (MUST be present) ---
    void parameterValueChanged (int parameterIndex, float newValue) override
    {
        juce::ignoreUnused (parameterIndex);
        needsUpdate = true;
        // Only proceed if we have the GUI state
        if (guiState == nullptr) return;
        // We are postponing the visual change to the message thread
        juce::MessageManager::callAsync ([this, parameterIndex, newValue]
        {
            const auto& tree = guiState->getGuiTree();
            if (needsUpdate){
                if (auto* broadcaster = dynamic_cast<juce::ChangeBroadcaster*>(this))
                    broadcaster->sendChangeMessage();
            needsUpdate = false;
        } });
    }


    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override
    {
        juce::ignoreUnused (parameterIndex, gestureIsStarting);
    }

    // The actual sign logic
    void createPlotPaths (juce::Path& path,
                            juce::Path& filledPath,
                            juce::Rectangle<float> bounds,
                            foleys::MagicPlotComponent& component) override
    {
        juce::ignoreUnused (component);
        path.clear();
        filledPath.clear();

        if (!attackParam || !decayParam || !sustainParam || !releaseParam)
            return;
        // retrieve values (0.0 to 2.0)
        const float a = attackParam->getValue();
        const float d = decayParam->getValue();
        const float s = sustainParam->getValue();
        const float r = releaseParam->getValue();

        const auto w = bounds.getWidth();
        const auto h = bounds.getHeight() - 1.0f;
        const auto b = bounds.getBottom();
        const auto l = bounds.getX();
        const auto t = bounds.getY();
        // path = path.createPathWithRoundedCorners(8);
        // start at the bottom left
        path.startNewSubPath (l, b);
        // Attack: Go up to the top
        float curX = l + (a * w * 0.25f);
        path.lineTo (curX, t);
        // Decay: Goes down to the sustain level.
        curX += (d * w * 0.25f);
        path.lineTo (curX, b - (s * h));
        // Sustain: A plateau (25% of the width)
        curX += (w * 0.25f);
        path.lineTo (curX, b - (s * h));
        // Release: back to the bottom
        curX += (r * w * 0.25f);
        path.lineTo (curX, b);
        // close path for the fill
        filledPath = path;
        filledPath.lineTo (l + w, b); // Ensure that we finish on the right
        filledPath.closeSubPath();
    }

    bool isActive() const override { return active; }
    void setActive (bool shouldBeActive) override { active = shouldBeActive; }

    juce::TimeSliceClient* getBackgroundJob() override { return nullptr; }

private:
    foleys::MagicGUIState* guiState = nullptr;

    juce::String plotGain1Id = {""};
    juce::String plotGain2Id = {""};
    juce::String plotGain3Id = {""};
    juce::String plotFilter1Id = {""};
    juce::String plotFilter2Id = {""};
    juce::String plotFilter3Id = {""};

    std::atomic<juce::int64> lastData { 0 };
    bool active = true;
    bool needsUpdate = true;
    // caching for performance (so we don't have to search every millimeter)
    juce::ValueTree plotGain1Node, plotGain2Node, plotGain3Node, plotFilter1Node, plotFilter2Node, plotFilter3Node;

    static juce::ValueTree findNodeById (juce::ValueTree tree, const juce::String& idToFind)
    {
        // check: Does the current node have the ID being searched for?
        // in foleys_gui_magic is "id" the standard-property-name
        if (tree.getProperty ("id").toString() == idToFind)
            return tree;
        // recursion: Search all children
        for (int i = 0; i < tree.getNumChildren(); ++i) {
            if (auto foundNode = findNodeById (tree.getChild (i), idToFind); foundNode.isValid())
                return foundNode; // Gefunden! Direkt zurückgeben und Suche beenden
        }
        // nothing found
        return {};
    }
    juce::RangedAudioParameter* attackParam  = nullptr;
    juce::RangedAudioParameter* decayParam   = nullptr;
    juce::RangedAudioParameter* sustainParam = nullptr;
    juce::RangedAudioParameter* releaseParam = nullptr;
    juce::RangedAudioParameter* envelopeAmParam = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ADSRPlot)
};

