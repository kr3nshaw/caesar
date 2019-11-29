/// @file
/// Writes SoundFont 2 file using SF2cute.

#include <algorithm>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include <sf2cute.hpp>

//#include "debug.hpp"

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

  // Print the tree for debugging.
#ifdef SF2CUTE_EXAMPLES_DEBUG_HPP_
  PrintSoundFont(sf2);
#endif

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
