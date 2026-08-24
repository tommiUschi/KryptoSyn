
#pragma once

#include <foleys_gui_magic/foleys_gui_magic.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_core/juce_core.h>
#include <functional>

/**
 * custom component to filebrowsing for the sampler page
 */

class SampleBrowserComponent : public juce::Component
{
public:
    SampleBrowserComponent()
    {
        addAndMakeVisible (categoryBox);
        addAndMakeVisible (subCategoryBox);
        addAndMakeVisible (fileBox);

#ifdef _DEBUG
        baseDir = juce::File ("/home/tommibe/Development/newAudio/KryptoSyn/SynthLabSamples/");
        categoryBox.onChange = [this] { updateSubCategories(); };
        subCategoryBox.onChange = [this] { updateFiles(); triggerInstrumentSelection(); };
#else
        // !!! IMPORTANT - Swap the comments depending on whether the project is to be executed
        // from the development environment or from the release output:

        //baseDir = juce::File ("/home/tommibe/Development/newAudio/KryptoSyn/SynthLabSamples/");
        baseDir = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
            .getParentDirectory()      // Standalone
            .getParentDirectory()      // Release
            .getChildFile("SynthLabSamples");
        if (!baseDir.exists()) {
            baseDir = juce::File ("/home/tommibe/Development/newAudio/KryptoSyn/SynthLabSamples/");
        }
        categoryBox.onChange = [this] { updateSubCategories(); };
        subCategoryBox.onChange = [this] { updateFiles(); triggerInstrumentSelection(); };
#endif
    }

    ~SampleBrowserComponent() override = default;

    // the lambda object
    std::function<void (const juce::File&)> onInstrumentSelected;
    // we call this method from the outside when everything is ready
    void initialise()
    {
        updateCategories();
    }

    // callback definition for the PGM item
    std::function<void (const juce::File&)> onFileSelected;


    void setCustomColors (juce::Colour bg, juce::Colour text, juce::Colour outline, juce::Colour arrow)
    {
        for (auto* box : { &categoryBox, &subCategoryBox})
        {
            // colors for the closed box in the UI layout
            box->setColour (juce::ComboBox::backgroundColourId, bg);
            box->setColour (juce::ComboBox::textColourId, text);
            box->setColour (juce::ComboBox::outlineColourId, outline);
            box->setColour (juce::ComboBox::arrowColourId, arrow);

            // colors for the open drop-down menu (PopupMenu)
            box->setColour (juce::PopupMenu::backgroundColourId, bg);
            box->setColour (juce::PopupMenu::textColourId, text);
            box->setColour (juce::PopupMenu::highlightedBackgroundColourId, outline);
            box->setColour (juce::PopupMenu::highlightedTextColourId, bg); // Invert text on hover
        }
        for (auto* box : { &fileBox })
        {// for the combobox on the bottom (only for displaying)
            // colors for the closed box in the UI layout
            box->setColour (juce::ComboBox::backgroundColourId, bg.brighter(0.11f));
            box->setColour (juce::ComboBox::textColourId, text.darker(0.4f));
            box->setColour (juce::ComboBox::outlineColourId, outline);
            box->setColour (juce::ComboBox::arrowColourId, arrow.darker(0.9f));

            // colors for the open drop-down menu (PopupMenu)
            box->setColour (juce::PopupMenu::backgroundColourId, bg.brighter(0.11f));
            box->setColour (juce::PopupMenu::textColourId, text.darker(0.4f));
            box->setColour (juce::PopupMenu::highlightedBackgroundColourId, outline);
            box->setColour (juce::PopupMenu::highlightedTextColourId, bg); // Invert text on hover
        }
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced (5);
        int boxHeight = 28;
        int gap = 6;

        categoryBox.setBounds (bounds.removeFromTop (boxHeight));
        bounds.removeFromTop (gap);
        subCategoryBox.setBounds (bounds.removeFromTop (boxHeight));
        bounds.removeFromTop (gap);
        fileBox.setBounds (bounds.removeFromTop (boxHeight));
    }

private:
    void updateFiles()
    {
        fileBox.clear (juce::dontSendNotification);
        auto catName = categoryBox.getText();
        auto subName = subCategoryBox.getText();
        if (catName.isEmpty() || subName.isEmpty()) return;

        auto finalDir = baseDir.getChildFile (catName).getChildFile (subName);
        int id = 1;

        // here, we are looking for the actual *.wav files (findFiles) within the instrument!
        for (const auto& entry : juce::RangedDirectoryIterator (finalDir, false, "*.wav", juce::File::findFiles))
        {
            fileBox.addItem (entry.getFile().getFileName(), id++);
        }

        if (fileBox.getNumItems() > 0)
        {
            // dontSendNotification` is sufficient here, since the grades box (Box 3)
            // does not need to trigger any further downward cascade
            fileBox.setSelectedId (1, juce::dontSendNotification);
        }
    }

    void updateCategories()
    {
        categoryBox.clear (juce::dontSendNotification);
        if (!baseDir.isDirectory()) return;

        int id = 1;
        // rangedDirectoryIterator searches the Linux file system in a safe and modern way.
        for (const auto& entry : juce::RangedDirectoryIterator (baseDir, false, "*", juce::File::findDirectories))
        {
            categoryBox.addItem (entry.getFile().getFileName(), id++);
        }

        if (categoryBox.getNumItems() > 0)
            categoryBox.setSelectedId (1);
    }

    void updateSubCategories()
    {
        subCategoryBox.clear (juce::dontSendNotification);
        auto catName = categoryBox.getText();
        if (catName.isEmpty()) return;

        auto catDir = baseDir.getChildFile (catName);
        int id = 1;
        for (const auto& entry : juce::RangedDirectoryIterator (catDir, false, "*", juce::File::findDirectories))
        {
            subCategoryBox.addItem (entry.getFile().getFileName(), id++);
        }

        if (subCategoryBox.getNumItems() > 0)
        {
            subCategoryBox.setSelectedId (1, juce::sendNotification);
        }
    }

    void triggerInstrumentSelection()
    {
        auto instrumentName = subCategoryBox.getText();
        if (instrumentName.isEmpty() || onInstrumentSelected == nullptr) return;

        auto instrumentDir = baseDir.getChildFile (categoryBox.getText())
                                    .getChildFile (instrumentName);

        if (instrumentDir.isDirectory())
        {
            // triggers the callback to the processor
            onInstrumentSelected (instrumentDir);
        }
    }
    juce::File baseDir;
    juce::ComboBox categoryBox;
    juce::ComboBox subCategoryBox;
    juce::ComboBox fileBox;
};
