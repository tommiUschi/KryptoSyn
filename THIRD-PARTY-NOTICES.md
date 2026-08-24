# Third-Party Notices

KryptoSyn is licensed under the **GNU General Public License v3.0 (GPLv3)**.
See [`LICENSE`](./LICENSE) for the full license text.

This project depends on the following third-party components. Their
respective licenses apply to those components independently of the
GPLv3 license of Absynth's own source code.

---

## JUCE

- **Website:** https://juce.com
- **License:** GPLv3 (JUCE's open-source tier)
- **Location in this repo:** vendored copy at the project root *(confirm this is acceptable
  for your JUCE version/edition before publishing; JUCE's GPL tier is designed
  for exactly this use case, but license terms have changed across versions —
  re-check the version you vendored)*
- **Modifications:** [none / list any local changes here]

GPLv3 and GPLv3 are compatible by definition, so no additional action is
required beyond retaining JUCE's license/copyright files as shipped.

---

## foleys_gui_magic

- **Author:** Daniel Walz — Foleys Finest Audio
- **Website:** https://foleysfinest.com
- **License:** BSD 3-Clause (project-wide since v1.4.0, July 2023)
- **Location in this repo:** vendored copy at the project root
- **Modifications:** [list any local changes here, e.g. custom LookAndFeel
  subclasses, ScaledPluginEditor, etc.]

BSD-3-Clause is compatible with GPLv3. The original copyright notice and
disclaimer are retained in the source files as shipped by Foleys Finest Audio;
do not remove them. The Foleys Finest Audio name is not used to endorse or
promote this project.

```
Copyright (c) 2019-2023, Foleys Finest Audio - Daniel Walz
All rights reserved.
(BSD 3-Clause — see libs/foleys_gui_magic/LICENSE.md for full text)
```

---

## SynthLab SDK

- **Author:** Will Pirkle — Tritone Systems, Inc.
- **Website:** https://www.willpirkle.com
- **License:** Proprietary — SynthLab SDK License Agreement (not OSI-approved,
  not GPL-compatible)
- **Location in this repo:** **Not included.** The SynthLab SDK source is
  *not* vendored or committed to this repository. It must be obtained
  separately by anyone building this project, directly from
  https://www.willpirkle.com, under their own license agreement with
  Tritone Systems, Inc.
- **Build setup:** See [`BUILDING.md`](./BUILDING.md) for instructions on
  where to place your own copy of the SynthLab SDK so it is picked up by
  the CMake build (`SDKs/SynthLab/` — already listed in `.gitignore`).
- **Modifications:** none (unmodified SDK usage)
- **Sample content (`SynthLabSamples.zip`):** Covered by the same SynthLab
  SDK License Agreement — there is no separate, standalone license for the
  sample library. The samples are additionally subject to specific
  copyright notices regarding the origin/provenance of the individual audio
  files; see the notices shipped alongside `SynthLabSamples.zip` for
  attribution details on individual sample sources.
- **Location in this repo:** **Not included**, same as the SDK itself — the
  samples must be obtained separately alongside the SynthLab SDK (see
  `BUILDING.md`).

> Rationale: The SynthLab SDK license restricts redistribution of the SDK
> itself, even unmodified — including its bundled sample content —
> "integrated in any framework application, on any medium including the
> Internet," without written permission from Tritone Systems. To keep
> KryptoSyn's own GPLv3 distribution unambiguous, both the SynthLab SDK and
> its sample library are treated as required external dependencies rather
> than vendored/bundled content.

---

## Summary Table

| Component                      | License          | Vendored in repo? | GPLv3-compatible? |
|--------------------------------|------------------|--------------------|--------------------|
| JUCE                           | GPLv3            | Yes                | Yes                |
| foleys_gui_magic               | BSD 3-Clause     | Yes                | Yes                |
| SynthLab SDK (incl. samples)   | Proprietary      | **No** (external)  | No — excluded      |
