# Open B3: an open source tonewheel organ and rotary speaker emulator

![](Documentation/Images/openB3_screenshot.png)

## Summary
### [1. What is Open B3?](#what-is-open-b3)
### [2. How do I use it?](#how-do-i-use-it)
### [3. Building](#building)
### [4. Current state and road map](#current-state-and-road-map)
### [5. Can I contribute to this project?](#can-i-contribute-to-this-project)
### [6. Are you crazy?](#are-you-crazy)


## What is Open B3?
Open B3 is the younger sibling of [setBfree](https://github.com/pantherb/setBfree), which is
> a MIDI-controlled, software synthesizer designed to imitate the sound and properties of the electromechanical organs and sound modification devices that brought world-wide fame to the names and products of Laurens Hammond and Don Leslie.  
*-- from https://github.com/pantherb/setBfree*

Similarly to setBree, Open B3 is powered by the [Beatrix](https://github.com/pantherb/setBfree/tree/master/src) synthesis engine.  
It comes in two forms:
1. A C++ JUCE plugin that is based on [BeatrixCPP](./BeatrixCPP). It can be found in the [OpenB3JUCE](./OpenB3JUCE) folder, and this is what most people will be looking for.
2. A simple C++ wrapper around the Beatrix engine, which is written in C. This is done to make the usage of Beatrix more straightforward. It can be found in the [BeatrixCPP](./BeatrixCPP) folder.

#### Why do we need Open B3, if there's already setBfree?
I decided to create Open B3 in order to overcome two major limitations of setBfree:
- the setBfree standalone app relies on [JACK](https://jackaudio.org/)
- the setBfree plugin is available in LV2 format only

People outside the GNU/Linux world rarely venture into installing JACK or using a DAW that supports LV2 plugins (except Reaper). Usually, they have no idea what these two things are. This severely limits the potential user base of setBfree, and it's a shame! With Open B3, I would like to give Beatrix and setBfree the audience they deserve, since a JUCE plugin can be compiled into:
- all sorts of plugin formats (VST2, VST3, AAX, AU, AUv3, Unity, and in the near future, even [LV2](https://github.com/juce-framework/JUCE/issues/123))
- standalone desktop apps that make use of the native audio system of each OS (ALSA, Windows Audio, CoreAudio, ...)
- and even Android/iOS apps!

## How do I use it?
You can download the latest release for your OS from [here](https://github.com/michele-perrone/OpenB3/releases). Currently, only the standalone apps for Linux x64, Windows x64 and macOS M1 are provided. For the moment, if you want to run Open B3 on other architectures or as a plugin, you have to build it yourself (see [building](#building)).

## Building
To build Open B3, you will need CMake (version 3.15 or above), a C++ compiler (C++17 or above), and the latest version of the [JUCE](https://github.com/juce-framework/JUCE) library. Once you clone/download this repository, you need to clone/download a copy of JUCE inside the `OpenB3JUCE/` folder. You end up with this directory structure:

```
OpenB3
├── BeatrixCPP/
├   ├── Source/
├   └── CMakeLists.txt
└── OpenB3JUCE/
    ├── JUCE/
    ├── Source/
    └── CMakeLists.txt
```
* Windows/macOS/Linux: if you use a CMake-based IDE like CLion or QtCreator, use it to open `CMakeLists.txt` and proceed building from there.
* Windows only: download and install the latest [CMake installer](https://cmake.org/download/), [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/) IDE, and the [Windows SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/). Open CMake, point it to the source folder (OpenPianoJUCE or OpenPianoCore), and generate a VS project to be opened in Visual Studio.
* macOS only: download and install XCode from the App Store and the [CMake installer](https://cmake.org/download/). Use CMake to generate an XCode project.
* Linux/macOS and other UNIX/UNIX-based/UNIX-like operating systems: use the command line :wink:

## Current state and road map
Open B3 works, but the C++ wrapper doesn't expose all Beatrix's functionality yet. Here's a short list of what needs to be done (it may change frequently):
* [x] ~~Fix sample rate setting~~ - Fixed [HERE](https://github.com/michele-perrone/OpenB3/commit/47b80a901304214211a673c8e0176784a26215e3), but maybe `SampleRateD` shouldn't be global
* [x] ~~Expose the essential sound parameters (vibrato, vibrato&chorus, percussion, overdrive, rotary speaker speed, reverb, volume)~~ - Done [HERE](https://github.com/michele-perrone/OpenB3/commit/b5e72418c8d0bb947b01c2f1d86eecbff7fc5fa6)
* [x] ~~Design and implement a fully resizable prototype GUI in JUCE~~ - Done [HERE](https://github.com/michele-perrone/OpenB3/commit/4a248f4adbd07582b7bc59ee0874367c0725cb1e)
* [ ] Expose all remaining sound parameters (e.g. keyclick, tonewheel model, tuning, rotary speaker speed tuning, ...) and allow their tweaking in the GUI
* [ ] Expose/implement programs and program changes
* [ ] Expose/implement configuration file loading and saving
* [ ] General GUI improvements and cosmetic work

## Can I contribute to this project?
Once the project reaches a certain usability level, contributions will be welcome. It would be really nice if improvements upon the sound synthesis engine were also upstreamed into setBfree.

## Are you crazy?
Yes
