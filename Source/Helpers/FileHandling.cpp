

#include "FileHandling.h"


juce::String FileHandling::loadHelpText(HelpTextMode mode)
{
#ifdef _DEBUG
    juce::String textFile;
    if (mode == HelpTextMode::Instructions)
    {
        textFile = "Instructions.Text.txt";
    }
    if (mode == HelpTextMode::License)
    {
        textFile = "License.Text.txt";
    }
    auto resourceFile = juce::File ("/home/tommibe/Development/newAudio/KryptoSyn/Resources/"+textFile);
    if (! resourceFile.existsAsFile())
    {
        DBG ("file could not found in path: " << resourceFile.getFullPathName());
        return "Error: The file could not be loaded\n"
               "File not found on:\n" + resourceFile.getFullPathName();
    }
	return resourceFile.loadFileAsString();
#else
	return "hat man nich jeklappt, weeste?";
#endif
}


juce::File FileHandling::loadFactoryPreset(int presetNumber){

	juce::String presetFile;
	switch (presetNumber)
	{
	case 1:
		presetFile = "factory.1.preset";
	case 2:
		presetFile = "factory.2.preset";
	case 3:
		presetFile = "factory.3.preset";
	default:
		presetFile = "factory.1.preset";
	}
#ifdef _DEBUG
	auto resourceFile = juce::File ("/home/tommibe/Development/newAudio/KryptoSyn/Resources/"+presetFile);


	if (! resourceFile.existsAsFile())
	{
		DBG ("file could not found in path: " << resourceFile.getFullPathName());
		return juce::File();
	}
	return resourceFile;
#endif
	return juce::File();
}



