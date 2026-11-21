# Nappe

**Reverb + tremolo for DJs and producers: VST3 plugin (Windows) and standalone app.**

C++17 · JUCE 8 · CMake · GitHub Actions

[![Build](https://github.com/hantsaa/Nappe/actions/workflows/build-windows.yml/badge.svg)](https://github.com/hantsaa/Nappe/actions/workflows/build-windows.yml)

> **(FR)** Nappe est un effet audio pour DJ et producteurs : une réverbe large et aérienne qui reste au-dessus du mix, un bruit blanc injecté dans la réverbe, un trémolo synchronisable sur le tempo du projet, un coupe-bas et un mode maintenu ou bascule.
>
> Le moteur audio est indépendant de JUCE et testé automatiquement.

_Ce projet est une implémentation originale, sans lien avec un fabricant. Il n'ouvre aucune connexion réseau._

## Features

- **Algorithmic reverb**: a 12-line feedback delay network with Householder mixing, early reflections and input diffusion.
- **Low-end control inside the loop**: the bass decays faster than the mids, so the tail does not build up mud.
- **Texture noise**: white noise injected into the reverb, high-passed at 800 Hz, then shaped by LOW CUT and the tremolo.
- **Tremolo** from 0.2 to 40 Hz, or locked to the host tempo (1/2 to 1/32, triplets included).
- **Tap tempo**, WET or MIX output, hold or toggle engagement.

## Status

| Part                      | Status                                                                                        |
| ------------------------- | --------------------------------------------------------------------------------------------- |
| DSP engine                | Engine tests (CTest) run in CI on Linux, then Windows                                         |
| VST3 build (Windows)      | Built by GitHub Actions (`windows-2022`)                                                      |
| Standalone app (Windows)  | Tested by the author on Windows                                                               |
| FL Studio (VST3, Windows) | Tested by the author                                                                          |
| Code signing              | Not signed yet; a signature request to SignPath Foundation is planned (see `CODE_SIGNING.md`) |
| Sound                     | Measured on the engine model, not compared with hardware                                      |

## Installation (Windows)

1. From the **Releases** page, download `Nappe-Windows.zip` and `SHA256SUMS.txt`.
2. Check the file. In PowerShell, in the download folder:
   ```
   Get-FileHash .\Nappe-Windows.zip -Algorithm SHA256
   ```
   The hash must match the line in `SHA256SUMS.txt`. Do not continue otherwise.
3. Unzip the archive.
4. Copy the `Nappe.vst3` folder to `C:\Program Files\Common Files\VST3\`. Windows asks for administrator rights.
5. In your DAW, rescan the plugins. In FL Studio: _Options → Manage plugins → Find installed plugins_.

The standalone app is `Nappe.exe`. It mutes its audio input by default to avoid feedback. Enable the input in _Options → Audio/MIDI Settings_ to process a signal. Without the Steinberg ASIO SDK at build time, it uses Windows Audio (WASAPI).

If Windows blocks a file after download, right-click it, choose _Properties_, then tick _Unblock_, once the hash is verified.

## Security

- The plugin and the standalone app open no network connection and collect no data.
- Each release publishes SHA-256 checksums (`SHA256SUMS.txt`).
- The release binaries are not signed yet. Until the signature is active, Windows can show _Windows protected your PC_ or _Unknown publisher_. This is expected for an unsigned program: check the hash first, then choose _More info → Run anyway_.
- To report a vulnerability, see `SECURITY.md`.

## Controls

| Control     | Function                                                                                                                                                                      |
| ----------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| LEVEL       | Reverb output level. At maximum, the reverb stays below the dry signal in the mids.                                                                                           |
| SIZE        | Room size and diffusion density.                                                                                                                                              |
| DECAY       | Nominal reverb time, from 0.5 s to 20 s. Measured times are shorter, especially in the bass (see Measurements).                                                               |
| BRIGHT      | High-frequency damping inside the loop, and a high shelf at the output.                                                                                                       |
| NOISE       | White noise sent into the reverb.                                                                                                                                             |
| DEPTH       | Tremolo depth, 0 to 100 %.                                                                                                                                                    |
| RATE        | Tremolo rate, 0.2 to 40 Hz, or the division in SYNC mode. In SYNC mode the host tempo is followed; without host tempo, 120 BPM is used and the display shows _NO HOST TEMPO_. |
| LOW CUT     | High-pass filter from 20 Hz to 20 kHz (default 100 Hz). Lower it for more bass in the reverb.                                                                                 |
| WET / MIX   | WET outputs the processed signal only. MIX adds the dry signal.                                                                                                               |
| ENGAGE      | Turns the effect on. In _hold_ mode it works only while pressed; in _toggle_ mode each press switches it.                                                                     |
| TAP         | Tap tempo: average of the last four taps. Disabled in SYNC mode.                                                                                                              |
| FREE / SYNC | Free rate in Hz, or locked to the host tempo.                                                                                                                                 |

## Measurements

The repository includes an analysis script (`test/analyze.py`, which needs numpy and scipy). On the engine model, with DECAY at 0.72 (about two thirds of the range), the 63 Hz tail lasts 3.1 s against 5.9 s at 1 kHz. At maximum DECAY, the figures are 4.8 s against 12.9 s. An earlier version held the bass about as long as the mids (10 s at 63 Hz), and on a club test loop its reverb sat 2 dB above the dry signal at 63 Hz. With LOW CUT at 20 Hz, the reverb at 63 Hz sits about 5 dB under the dry signal; at the default 100 Hz, about 17 dB under. These are measurements of the model, not comparisons with hardware.

## Tests

The CTest suite checks:

- the MOM gate: silence when off, decaying tail, no ramp at the first block;
- stability under a random sweep of parameters;
- the output limiter;
- protection against NaN and infinite input values;
- delay line interpolation, the soft limiter and the -3 dB point of the high-pass filter;
- the decay time, which must not depend on the sample rate (about 6 % spread between 44.1 and 96 kHz, threshold 10 %) and must increase with DECAY.

## Build from source

Windows (Visual Studio 2022 with _Desktop development with C++_, CMake 3.22 or later):

```
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
ctest --test-dir build -C Release
```

The outputs are `build/Nappe_artefacts/Release/VST3/Nappe.vst3` and `build/Nappe_artefacts/Release/Standalone/Nappe.exe`.

Engine tests only, without JUCE (any C++17 compiler):

```
g++ -O2 -std=c++17 -Wall -Wextra -pedantic -o test_dsp test/test_dsp.cpp
./test_dsp
```

## Repository layout

| Path                                  | Content                                                                     |
| ------------------------------------- | --------------------------------------------------------------------------- |
| `Source/NappeDSP.h`                   | Audio engine, pure C++17, no JUCE dependency                                |
| `Source/PluginProcessor.*`            | Parameters, audio processing, host tempo, state                             |
| `Source/PluginEditor.*`               | User interface, drawn in code                                               |
| `test/`                               | Engine tests and measurement analysis                                       |
| `.github/workflows/build-windows.yml` | CI: tests, Windows build, checksums, signing request, releases on `v*` tags |
| `SECURITY.md`, `CODE_SIGNING.md`      | Security policy and code signing policy                                     |

## Limitations

- The sound has not been compared with a hardware unit.
- No external spring-tank send and return.
- The decay time varies by about 6 % between 44.1 and 96 kHz, mostly in the high frequencies.
- The release binaries are not signed yet.

## Design

Nappe is an original implementation. Its engine and interface were written for this project, and it does not reuse code, firmware, documentation or artwork from any hardware manufacturer.

## Licence

Copyright (C) 2026 hantsaa. Released under the GNU General Public License v3.0 or later (see `LICENSE`). The project uses JUCE, which is available under the GPLv3 or a commercial licence.
