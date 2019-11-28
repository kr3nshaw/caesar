SF2cute: C++ Library for SoundFont 2
====================================
[![Travis Build Status](https://travis-ci.org/gocha/sf2cute.svg?branch=master)](https://travis-ci.org/gocha/sf2cute) [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/elt3wbk82y8natuy/branch/master?svg=true)](https://ci.appveyor.com/project/gocha/sf2cute/branch/master)

SF2cute is a C++ library for writing SoundFont 2.

Downloading
-----------

You can download a source code package of the latest release
from [Releases](https://github.com/gocha/sf2cute/releases/latest) page.

Visual Studio users can install the precompiled binaries from the [SF2cute NuGet Gallery](https://www.nuget.org/packages/sf2cute).

[SF2cute Documentation](http://gocha.github.io/sf2cute/) is available on the GitHub Pages.
You can also build the documentation by yourself using [Doxygen](http://www.doxygen.org).

Compiling
---------

First of all, install the following prerequisite softwares.

* [CMake](https://cmake.org/)
* Modern C++ compiler (requires C++14 features)
    - Clang 3.6 or later
    - GCC (G++) 5 or later
    - [Visual Studio Community Edition](https://www.visualstudio.com/products/visual-studio-community-vs) 2015 or later (Windows only)

The following command will generate a Makefile for your system. You can use `cmake-gui` if you want to use a GUI and configure the options.

``` bash
cmake .
```

Then compile the library using the generated project file.

``` bash
make
```

SoundFont file writing example
------------------------------

Here is an example of SoundFont file writing.

``` cpp
#include <algorithm>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include <sf2cute.hpp>

using namespace sf2cute;

/// Makes a pulse wave of n/8 duty cycle.
/// @param n the duty cycle n/8.
/// @return the pulse wave sample datapoints.
std::vector<int16_t> MakePulseVector(int n) {
  std::vector<int16_t> data(200);
  size_t width = data.size() * n / 8;
  std::fill(data.begin(), std::next(data.begin(), width), 0x4000);
  std::fill(std::next(data.begin(), width), data.end(), 0);
  return data;
}

/// Writes SoundFont 2 file using SF2cute.
/// @param argc Number of arguments. Not used.
/// @param argv Argument vector. Not used.
/// @return 0 if the SoundFont file is successfully written.
int main(int argc, char * argv[]) {
  SoundFont sf2;

  // Set metadata.
  sf2.set_sound_engine("EMU8000");
  sf2.set_bank_name("Chipsound");
  sf2.set_rom_name("ROM");

  // Construct sample datapoints.
  std::vector<int16_t> data_50 = MakePulseVector(4);

  // Add a sample.
  std::shared_ptr<SFSample> sample_50 = sf2.NewSample(
    "Square",                 // name
    data_50,                  // sample data
    0,                        // start loop
    uint32_t(data_50.size()), // end loop
    44100,                    // sample rate
    57,                       // root key
    0);                       // microtuning

  // Make an instrument zone.
  SFInstrumentZone instrument_zone(sample_50,
    std::vector<SFGeneratorItem>{
      //SFGeneratorItem(SFGenerator::kKeyRange, RangesType(0, 127)),
      //SFGeneratorItem(SFGenerator::kVelRange, RangesType(0, 127)),
      SFGeneratorItem(SFGenerator::kSampleModes, uint16_t(SampleMode::kLoopContinuously)),
    },
    std::vector<SFModulatorItem>{});
  // Add more generators or modulators if necessary.
  instrument_zone.SetGenerator(SFGeneratorItem(SFGenerator::kReverbEffectsSend, 618));
  instrument_zone.SetModulator(SFModulatorItem(
      SFModulator(SFGeneralController::kNoteOnVelocity,
      SFControllerDirection::kDecrease, SFControllerPolarity::kUnipolar,
      SFControllerType::kConcave),
    SFGenerator::kInitialAttenuation,
    960,
    SFModulator(0),
    SFTransform::kLinear));

  // Add an instrument.
  std::shared_ptr<SFInstrument> instrument_50 = sf2.NewInstrument(
    sample_50->name(),
    std::vector<SFInstrumentZone>{
      std::move(instrument_zone)
    });

  // Add a preset.
  std::shared_ptr<SFPreset> preset_50 = sf2.NewPreset(
    instrument_50->name(), 0, 0,
    std::vector<SFPresetZone>{
      SFPresetZone(instrument_50)
    });

  // Write SoundFont file.
  try {
    std::ofstream ofs("output.sf2", std::ios::binary);
    sf2.Write(ofs);
    return 0;
  }
  catch (const std::fstream::failure & e) {
    // File output error.
    std::cerr << e.what() << std::endl;
    return 1;
  }
  catch (const std::exception & e) {
    // Other errors.
    // For example: Too many samples.
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
```
