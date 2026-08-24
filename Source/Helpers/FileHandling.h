//
// Created by tommibe on 21.04.26.
//

#pragma once

#include <juce_core/juce_core.h>


/**
 * @brief Purely static utility class for Tutorial-/License-Window and factory
 * preset file operations.
 */
enum HelpTextMode
{
    Instructions,
    License
};

class FileHandling
{
public:
    // explicitly disallow instantiation
    FileHandling() = delete;
    ~FileHandling() = delete;
    FileHandling(const FileHandling&) = delete;
    FileHandling& operator=(const FileHandling&) = delete;


    static juce::String loadHelpText(HelpTextMode mode);
	static juce::File loadFactoryPreset(int presetNumber);

    // Static juce::String variable with C++17/C++20 inline initialization
    inline static juce::String xmlPartTut = R"xml(
    <View id="windowTut" display="contents" pos-width="100%" pos-height="100%" pos-x="0%" pos-y="0%"
          caption-color="FFFFA01F" background-color="FF264049" border="1.7" radius="8"
          border-color="0057A793" flex-direction="column" flex-justify-content="start"
          flex-wrap="wrap" caption="Tutorial" caption-size="24">

        <View pos-height="89.5%" display="flexbox" pos-width="98%" pos-y="1%" pos-x="1%"
              scroll-mode="scroll-vertical" background-color="FF36666c" border="1.7"
              radius="8" border-color="0057A793" flex-direction="column"
              flex-justify-content="start" flex-wrap="wrap" margin="2" padding="1">

            <View pos-width="95%" display="contents" pos-height="750%" max-height="3650" min-height="3600"
                  pos-x="2.5%" pos-y="1%" background-color="00000000" border-color="00ffffff">

                <HelpTextItem id="contLic" text="@TEXT@" border="2" border-color="00000000" radius="6"
                              background-color="FFdfd9c3" colour="FF111111" margin="3" padding="2"
                              pos-width="95%" pos-height="44.15%" pos-x="2.5%" pos-y="0.8%" max-height="1650"/>
                <View background-image="Instructions_png" image-placement="centred"
                    display="contents" pos-width="95%" pos-height="55.5%" pos-y="44.4%"
                    pos-x="2.5%" margin="1" padding="0.5" border="1" radius="5" border-color="00000000"
                    background-color="00000000" min-width="600" min-height="1650"
                    max-height="2000" max-width="600"/>
            </View>
        </View>

        <View pos-width="89.9%" display="contents" pos-height="9.2%" margin="1" padding="0"
              pos-x="1.3%" pos-y="91.0%" background-color="FF2C5855" border="0.7"
              border-color="FF5BABA6" radius="5">

            <TextButton margin="1" toggleable="true" padding="0" border="1.2" radius="3"
                        border-color="FF96F000" background-color="FF1A4541" caption-placement="centred"
                        max-width="164" max-height="70" flex-align-self="center" text="Cancel"
                        button-color="FF1A433F" button-on-color="FF1A6B70"
                        button-off-text="FFE2E1B8" button-on-text="FFFFA52B"
                        onClick="@CLOSE@"
                        lookAndFeel="LookAndFeel_V3" min-width="100" pos-x="79.52%" pos-y="6.8%"
                        pos-width="15%" pos-height="81%" min-height="50"/>
        </View>
    </View>
    )xml";


    inline static juce::String xmlPartLic = R"xml(
    <View id="windowTut" display="contents" pos-width="100%" pos-height="100%" pos-x="0%" pos-y="0%"
          caption-color="FFFFA01F" background-color="FF264049" border="1.7" radius="8"
          border-color="0057A793" flex-direction="column" flex-justify-content="start"
          flex-wrap="wrap" caption="License" caption-size="24">

        <View pos-height="89%" display="flexbox" pos-width="98%" pos-y="1%" pos-x="1%"
              scroll-mode="scroll-vertical" background-color="FF36666c" border="1.7"
              radius="8" border-color="0057A793" flex-direction="column"
              flex-justify-content="start" flex-wrap="wrap" margin="2" padding="1">

            <View pos-width="95%" display="contents" pos-height="1360%" max-height="9800" min-height="9800"
                  pos-x="2.5%" pos-y="1%" background-color="00000000" border-color="00ffffff">

                <HelpTextItem id="contLic" text="@TEXT@" border="2" border-color="00000000" radius="6"
                              background-color="FFdfd9c3" colour="FF111111" margin="3" padding="2" min-height="7000"
                              pos-width="95%" pos-height="100%" pos-x="2.5%" pos-y="0.8%" max-height="9800" max-width="600"/>
            </View>
        </View>

        <View pos-width="97.5%" display="contents" pos-height="10%" margin="1" padding="0"
              pos-x="1.25%" pos-y="90.2%" background-color="FF2C5855" border="0.7"
              border-color="FF5BABA6" radius="5">

            <TextButton margin="1" toggleable="true" padding="0" border="1.2" radius="3"
                        border-color="FF96F000" background-color="FF1A4541" caption-placement="centred"
                        max-width="164" max-height="70" flex-align-self="center" text="Cancel"
                        button-color="FF1A433F" button-on-color="FF1A6B70"
                        button-off-text="FFE2E1B8" button-on-text="FFFFA52B"
                        onClick="@CLOSE@"
                        lookAndFeel="LookAndFeel_V3" min-width="100" pos-x="79.52%" pos-y="6.8%"
                        pos-width="15%" pos-height="84%" min-height="50"/>
        </View>
    </View>
    )xml";

};
