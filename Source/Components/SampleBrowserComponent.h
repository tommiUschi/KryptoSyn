
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
        categoryBox.onChange    = [this] { updateSubCategories(); };
        subCategoryBox.onChange = [this] { updateFiles(); triggerInstrumentSelection(); };
        fileBox.onChange        = [this] { triggerInstrumentSelection(); }; // <-- NEU!
#else
        baseDir = juce::File::getSpecialLocation(juce::File::currentExecutableFile)
            .getParentDirectory()      // Standalone
            .getParentDirectory()      // Release
            .getChildFile("SynthLabSamples");
        if (!baseDir.exists()) {
            baseDir = juce::File ("/home/tommibe/Development/newAudio/KryptoSyn/SynthLabSamples/");
        }
        categoryBox.onChange    = [this] { updateSubCategories(); };
        subCategoryBox.onChange = [this] { updateFiles(); triggerInstrumentSelection(); };
        fileBox.onChange        = [this] { triggerInstrumentSelection(); }; // <-- NEU!
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
        for (auto* box : { &categoryBox, &subCategoryBox })
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

    /**
     * Sets the combo boxes to a specific category and instrument from an external source.
     *
     * @param categoryName name of the parent folder (e.g. “drums”)
     * @param instrumentName name of the instrument folder (e.g., "808_Kick")
     * @param triggerAudioLoad true = also triggers the reloading of the samples in the processor
     *                         false = aktualisiert nur die Anzeige in der GUI (gut für Preset-Loads)
     */
    /**
     * specifically sets the combo boxes to category, subfolder, and file
     */
    void selectFoldersByName (const juce::String& categoryName,
                          const juce::String& instrumentName,
                          const juce::String& fileName = {},
                          bool triggerAudioLoad = true)
    {
        if (categoryBox.getNumItems() == 0)
            updateCategories();

        // only re-import if the category is actually different
        bool catChanged = (categoryBox.getText() != categoryName);
        if (catChanged)
        {
            selectBoxItemByText (categoryBox, categoryName, juce::dontSendNotification);
            updateSubCategories();
        }

        // reload only if the subcategory or category has changed
        bool subChanged = (subCategoryBox.getText() != instrumentName);
        if (catChanged || subChanged)
        {
            selectBoxItemByText (subCategoryBox, instrumentName, juce::dontSendNotification);
            updateFiles();
        }

        // select file in ComboBox 3
        if (fileName.isNotEmpty())
        {
            selectBoxItemByText (fileBox, fileName, juce::dontSendNotification);
        }

        // only perform audio loading if explicitly requested
        if (triggerAudioLoad)
        {
            triggerInstrumentSelection();
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

    juce::String getCurrentCategory() const    { return categoryBox.getText(); }
    juce::String getCurrentSubCategory() const { return subCategoryBox.getText(); }
    juce::String getCurrentFile() const        { return fileBox.getText(); }

private:
    void updateFiles()
    {
        fileBox.clear (juce::dontSendNotification);
        auto catName = categoryBox.getText();
        auto subName = subCategoryBox.getText();
        if (catName.isEmpty() || subName.isEmpty()) return;

        auto finalDir = baseDir.getChildFile (catName).getChildFile (subName);
        int id = 1;

        // search for SFZ files (isRecursive = false for fast I/O)
        juce::Array<juce::File> sfzFiles;
        for (const auto& entry : juce::RangedDirectoryIterator (finalDir, false, "*.sfz", juce::File::findFiles))
        {
            sfzFiles.add (entry.getFile());
        }

        if (!sfzFiles.isEmpty())
        {
            for (const auto& sfz : sfzFiles)
                fileBox.addItem (sfz.getFileName(), id++);
        }
        else
        {
            // fallback for old WAV structure (isRecursive = false)
            for (const auto& entry : juce::RangedDirectoryIterator (finalDir, false, "*.wav", juce::File::findFiles))
                fileBox.addItem (entry.getFile().getFileName(), id++);
        }

        if (fileBox.getNumItems() > 0)
        {
            fileBox.setSelectedId (1, juce::dontSendNotification);
        }
    }

    void updateCategories()
    {
        categoryBox.clear (juce::dontSendNotification);
        if (!baseDir.isDirectory()) return;

        int id = 1;
        for (const auto& entry : juce::RangedDirectoryIterator (baseDir, false, "*", juce::File::findDirectories))
        {
            categoryBox.addItem (entry.getFile().getFileName(), id++);
        }

        if (categoryBox.getNumItems() > 0)
            categoryBox.setSelectedId (1, juce::dontSendNotification);
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
            auto dirName = entry.getFile().getFileName();

            // IMPORTANT: Ignore technical SFZ folders and hidden folders!
            if (dirName.equalsIgnoreCase ("samples") ||
                dirName.equalsIgnoreCase ("include") ||
                dirName.startsWith ("."))
            {
                continue;
            }

            subCategoryBox.addItem (dirName, id++);
        }

        if (subCategoryBox.getNumItems() > 0)
        {
            subCategoryBox.setSelectedId (1, juce::dontSendNotification);
        }
    }

    bool selectBoxItemByText (juce::ComboBox& box,
                              const juce::String& textToFind,
                              juce::NotificationType notification)
    {
        for (int i = 0; i < box.getNumItems(); ++i)
        {
            if (box.getItemText (i) == textToFind)
            {
                box.setSelectedId (box.getItemId (i), notification);
                return true;
            }
        }
        return false;
    }

    void triggerInstrumentSelection()
    {
        auto catName  = categoryBox.getText();
        auto subName  = subCategoryBox.getText();
        auto fileName = fileBox.getText();

        if (catName.isEmpty() || subName.isEmpty() || fileName.isEmpty() || onInstrumentSelected == nullptr)
            return;

        auto subDir = baseDir.getChildFile (catName).getChildFile (subName);

        // if an .sfz file is selected in fileBox:
        if (fileName.endsWithIgnoreCase (".sfz"))
        {
            auto sfzFile = subDir.getChildFile (fileName);
            if (sfzFile.existsAsFile())
            {
                onInstrumentSelected (sfzFile);
                return;
            }

            // case-insensitive fallback search on Linux
            for (const auto& entry : juce::RangedDirectoryIterator (subDir, true, "*.sfz", juce::File::findFiles))
            {
                if (entry.getFile().getFileName().equalsIgnoreCase (fileName))
                {
                    onInstrumentSelected (entry.getFile());
                    return;
                }
            }
        }

        // if it is an old WAV folder:
        if (subDir.isDirectory())
        {
            onInstrumentSelected (subDir);
        }
    }
    juce::File baseDir;
    juce::ComboBox categoryBox;
    juce::ComboBox subCategoryBox;
    juce::ComboBox fileBox;
};
