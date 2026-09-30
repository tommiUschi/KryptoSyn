//
// Created by tommibe on 21.04.26.
//

#pragma once

#include <juce_core/juce_core.h>


class SfzFileFilter
{
public:
    SfzFileFilter() = default;

    /** Lädt Ignore-Muster aus einer Textdatei */
    bool loadIgnoreListFromFile (const juce::File& ignoreListFile)
    {
        if (!ignoreListFile.existsAsFile())
            return false;

        juce::StringArray lines;
        ignoreListFile.readLines (lines);
        parseIgnoreRules (lines);
        return true;
    }

    /** Lädt Ignore-Muster direkt aus einem String (z.B. aus JUCE BinaryData) */
    void loadIgnoreListFromMemory (const juce::String& textContent)
    {
        juce::StringArray lines;
        lines.addLines (textContent);
        parseIgnoreRules (lines);
    }

    /** Manuelles Hinzufügen einer Regel */
    void addRule (const juce::String& pattern)
    {
        auto trimmed = pattern.trim();
        if (trimmed.isNotEmpty() && !trimmed.startsWithChar ('#'))
            ignorePatterns.addIfNotAlreadyThere (trimmed);
    }

    /** Prüft, ob eine SFZ-Datei ignoriert werden soll */
    bool shouldIgnore (const juce::File& sfzFile, const juce::File& rootFolder = {}) const
    {
        const juce::String fileName = sfzFile.getFileName();
        
        // Relative Pfade berechnen, falls Wurzelordner übergeben wurde (z.B. "includes/header.sfz")
        juce::String relativePath = rootFolder.exists() 
            ? sfzFile.getRelativePathFrom (rootFolder).replaceCharacter ('\\', '/')
            : fileName;

        // 1. Musterabgleich über die Ignore-Liste
        for (const auto& pattern : ignorePatterns)
        {
            // Prüfen gegen den reinen Dateinamen (z.B. "*.inc.sfz")
            if (fileName.matchesWildcard (pattern, true /* ignoreCase */))
                return true;

            // Prüfen gegen den relativen Pfad (z.B. "/includes/*")
            if (relativePath.matchesWildcard (pattern, true /* ignoreCase */))
                return true;
        }

        // 2. Inhaltliche Heuristik (Optional, aber sehr effektiv):
        // Hat die Datei überhaupt abspielbare Abschnitte ()?
        if (!containsPlayableRegions (sfzFile))
            return true;

        return false;
    }

private:
    juce::StringArray ignorePatterns;

    void parseIgnoreRules (const juce::StringArray& lines)
    {
        for (auto line : lines)
        {
            line = line.trim();
            // Leerzeilen und Kommentare überspringen
            if (line.isEmpty() || line.startsWithChar ('#'))
                continue;

            ignorePatterns.addIfNotAlreadyThere (line);
        }
    }

    /**
     * Liest die ersten KB der Datei, um zu prüfen, ob ein -Header existiert.
     * Reines Include-Material hat oft nur ,  oder #define.
     */
    static bool containsPlayableRegions (const juce::File& file)
    {
        // Wir lesen nur die ersten 8 KB – reicht für SFZ-Header völlig aus
        //constexpr size_t maxBytesToRead = 8192;
        
        juce::FileInputStream stream (file);
        if (!stream.openedOk())
            return false;

        juce::String headText = stream.readString ();
        
        // SFZ ist case-insensitive, daher in Lowercase umwandeln zum Suchen
        return headText.toLowerCase().contains ("");
    }
};
