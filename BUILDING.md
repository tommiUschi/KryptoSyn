# Building KryptoSyn from Source

KryptoSyn depends on the **SynthLab SDK** (and its sample library), which is
**not included** in this repository due to its license terms — see
[`THIRD-PARTY-NOTICES.md`](./THIRD-PARTY-NOTICES.md) for details. You need to
obtain it yourself before you can build the project.

## Prerequisites

- CMake (version X.XX or later)
- A C++ toolchain (GCC/Clang on Linux, MSVC on Windows, Xcode on macOS)
- JUCE and `foleys_gui_magic` — already vendored in this repository, no
  separate download required
- Your own copy of the **SynthLab SDK**, obtained directly from
  https://www.willpirkle.com under Tritone Systems' own license terms

## Step 1: Obtain the SynthLab SDK

1. Get the SynthLab SDK (and, if you want the built-in sample-based voices,
   `SynthLabSamples.zip`) from https://www.willpirkle.com under your own
   license agreement with Tritone Systems, Inc.
2. Unpack it so that the SDK source files end up under:

   ```
   SDKs/SynthLab/
   ├── source/
   ├── includes/
   └── ...
   ```

   This folder sits at the project root, alongside `Source/` and `JUCE/`.
   (This mirrors the folder layout SynthLab ships with — no reorganizing
   needed, just place the unpacked SDK folder here.)

3. If using the sample-based oscillator/voice types, unpack
   `SynthLabSamples.zip` to:

   ```
   SynthLabSamples/
   ```

   Also at the project root, alongside `SDKs/`, `Source/` and `JUCE/`.

`SDKs/SynthLab/` and `SynthLabSamples/` are listed in `.gitignore`, so
nothing from them will accidentally be committed if you're contributing
changes back.

## Step 2: Configure and build

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

If CMake reports missing SynthLab headers or sources, double-check that the
folder from Step 1 matches the path expected in `CMakeLists.txt`
(search for `SynthLab` in that file if unsure).

## Step 3: Output

Built plugin formats (VST3 / Standalone) will be located under:

```
build/KryptoSyn_artefacts/Release/
```

## Troubleshooting

- **CMake can't find SynthLab sources** — verify `SDKs/SynthLab/` sits
  directly at the project root (same level as `Source/` and `JUCE/`); check
  `CMakeLists.txt` for the exact path variable used if you renamed anything.
- **Missing sample-based voices at runtime** — confirm `SynthLabSamples.zip`
  was unpacked into a `SynthLabSamples/` folder at the project root.

---

## Tested Environment

KryptoSyn was built and tested under the following configuration. It should
also be buildable on Windows and macOS with appropriate adjustments to your
IDE settings, `CMakeLists.txt`, and OS-level dependencies (e.g. installing
the equivalent libraries/frameworks for that platform), but only the setup
below has been verified.

**System**

| | |
|---|---|
| Distributor | Ubuntu |
| Description | Ubuntu 24.04.4 LTS |
| Release | 24.04 (Noble Numbat) |
| Window Manager | Unity7 |
| IDE | CLion |

**Hardware**

| | |
|---|---|
| System | MacBookPro15,2 |
| Board/Bus | Mac-827FB448E656EC26 |
| Memory | 8 GiB RAM |
| Audio Driver | ALSA |

**Required Linux packages**

```bash
sudo apt install libasound2 libfreetype6 libfontconfig1 libx11-6 libxext6 \
                  libxcursor1 libxinerama1 libxrandr2 libxcomposite1 \
                  libgl1-mesa-dri
```

Also make sure `glibc` and `libstdc++` are reasonably up to date.

**Optional: OpenGL support**

To enable OpenGL rendering, set `FOLEYS_ENABLE_OPEN_GL_CONTEXT` to `1` in
`CMakeLists.txt`, and additionally install:

```bash
sudo apt install libgl1-mesa-glx libegl1 libglu1-mesa
```
In `PluginAudioProcessor.h`, there is a member variable that must be set to either `standalone`
or `VST3` in the constructor, depending on the type of target plugin.
