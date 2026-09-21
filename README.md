# KryptoSyn

KryptoSyn is among other things a wavetable-based software synthesizer, available as a
VST3/Standalone plugin, built with `JUCE` and the `SynthLab engine`, and `foleys_gui_magic`.

[KryptoSyn Screenshot1](docs/page1.png)

[KryptoSyn Screenshot2](docs/page2.png)

[KryptoSyn Screenshot3](docs/page3.png)

[KryptoSyn Screenshot4](docs/page4.png)

[KryptoSyn Screenshot5](docs/page5.png)

[KryptoSyn Screenshot6](docs/page6.png)

[KryptoSyn Screenshot7](docs/page7.png)

## Sampler

As an example, I have included a sample folder licensed under 'Creative Commons Sampling+ 1.0' 
—located at 'SynthLabSamples'—which has NOTHING to do with the SynthLab samples.

Within the main sample folder 'SynthLabSamples', the 'SynthLab structure' works,
as does the 'sfz structure' of the 'CC Sampling+ 1.0' packs.

The 'CC Sampling+ 1.0' license sometimes involves specific structures,
such as those found in the 'G-Town Church Sampling Projects' package.

Regarding this, please note the following:

[KryptoSyn Screenshot7](docs/sampleTut.png)

## Features

You can find a good description here: [instructions](/Resources/Instructions.Text.txt)

## Status

The plugin is the first version. Version: 1.0.0

## Building

KryptoSyn already ships with JUCE and `foleys_gui_magic` vendored — no
separate download is required for those. The **SynthLab SDK** (including
the optional sample library) is subject to its own license from Tritone
Systems, Inc. and is therefore **not** included in this repository; you
need to obtain it yourself from https://www.willpirkle.com.

JUCE version: `8.0.12-4-g501c07674e`
foleys_gui_magic version: `1.3.9`
I cannot guarantee compatibility with newer versions of `JUCE`
if the included version is not used.
The 'foleys_gui_magic' vendor package must be used, as I
have made a few minor adjustments to the code:
`foleys_MagicPlotComponent.h`, `foleys_MagicPlotComponent.cpp`
`foleys_PropertiesEditor.cpp`, `foleys_StringDefinitions.h`
`foleys_Container.h`, `foleys_Container.h`


Short summary of prerequisites:

- CMake ≥ 3.22
- A C++20-capable compiler (tested on Ubuntu 24.04 / GCC)
- Your own copy of the SynthLab SDK (see above)

When building, you need to manually choose between the `VST3` and
`Standalone` target (see `PluginAudioProcessor.h`).

➡️ **For the full step-by-step guide** (SDK placement, CMake
configuration, required Linux packages, troubleshooting), see
[`BUILDING.md`](./BUILDING.md).

## License

This project is licensed under the GPLv3 License, see LICENSE. For third-party
dependencies (JUCE, foleys_gui_magic, SynthLab SDK) and their respective
license terms, see [`THIRD-PARTY-NOTICES.md`](./THIRD-PARTY-NOTICES.md).

## Contributing

Contributions are welcome — please open an issue
before major changes. 
