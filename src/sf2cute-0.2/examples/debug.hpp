/// @file
/// Prints SoundFont 2 object.

#ifndef SF2CUTE_EXAMPLES_DEBUG_HPP_
#define SF2CUTE_EXAMPLES_DEBUG_HPP_

#include <cstdio>

#include <memory>
#include <vector>

#include <sf2cute.hpp>

namespace {

/// Prints internal informations of a SoundFont object.
/// @param sf2 the SoundFont object.
void PrintSoundFont(const sf2cute::SoundFont & sf2) {
  //using namespace sf2cute;

  // Child elements:
  printf("File %p\n", &sf2);
  printf("  Presets: %zu\n", sf2.presets().size());
  printf("  Instruments: %zu\n", sf2.instruments().size());
  printf("  Samples: %zu\n", sf2.samples().size());
  printf("\n");

  // INFO fields:
  printf("Sound Engine: %s\n", sf2.sound_engine().c_str());
  printf("Bank Name: %s\n", sf2.bank_name().c_str());
  printf("ROM Name: %s\n", sf2.has_rom_name() ? sf2.rom_name().c_str() : "(none)");
  printf("ROM Version: %s\n", sf2.has_rom_version() ? sf2.rom_version().to_string().c_str() : "(none)");
  printf("Creation Date: %s\n", sf2.has_creation_date() ? sf2.creation_date().c_str() : "(none)");
  printf("Sound Engineers: %s\n", sf2.has_engineers() ? sf2.engineers().c_str() : "(none)");
  printf("Product: %s\n", sf2.has_product() ? sf2.product().c_str() : "(none)");
  printf("Copyright: %s\n", sf2.has_copyright() ? sf2.copyright().c_str() : "(none)");
  printf("Comment: %s\n", sf2.has_comment() ? sf2.comment().c_str() : "(none)");
  printf("Software: %s\n", sf2.has_software() ? sf2.software().c_str() : "(none)");
  printf("\n");

  // Presets:
  for (auto && preset : sf2.presets()) {
    printf("Preset [%d:%d] %p \"%s\" (parent file: %p)\n",
      preset->bank(),
      preset->preset_number(),
      preset.get(),
      preset->name().c_str(),
      preset->has_parent_file() ? &preset->parent_file() : nullptr);

    // Global preset zone:
    if (preset->has_global_zone()) {
      printf("  -> Global Preset Zone %p\n", &preset->global_zone());

      // Generators:
      for (const auto & generator : preset->global_zone().generators()) {
        printf("    -> Generator %p\n", &generator);
      }

      // Modulators:
      for (const auto & modulator : preset->global_zone().modulators()) {
        printf("    -> Modulator %p\n", &modulator);
      }
    }

    // Preset zones:
    for (const auto & preset_zone : preset->zones()) {
      printf("  -> Preset Zone %p\n", &preset_zone);

      // Instrument:
      if (preset_zone->has_instrument()) {
        auto instrument = preset_zone->instrument();
        printf("    -> Instrument %p \"%s\" (parent file: %p)\n",
          instrument.get(),
          instrument->name().c_str(),
          instrument->has_parent_file() ? &instrument->parent_file() : nullptr);
      }

      // Generators:
      for (const auto & generator : preset_zone->generators()) {
        printf("    -> Generator %p\n", &generator);
      }

      // Modulators:
      for (const auto & modulator : preset_zone->modulators()) {
        printf("    -> Modulator %p\n", &modulator);
      }
    }
  }
  printf("\n");

  // Instruments:
  for (auto && instrument : sf2.instruments()) {
    printf("Instrument %p \"%s\" (parent file: %p)\n",
      instrument.get(),
      instrument->name().c_str(),
      instrument->has_parent_file() ? &instrument->parent_file() : nullptr);

    // Global instrument zone:
    if (instrument->has_global_zone()) {
      printf("  -> Global Instrument Zone %p (parent instrument %p)\n",
        &instrument->global_zone(),
        instrument->global_zone().has_parent_instrument() ? &instrument->global_zone().parent_instrument() : nullptr);

      // Generators:
      for (const auto & generator : instrument->global_zone().generators()) {
        printf("  -> Generator %p\n", &generator);
      }

      // Modulators:
      for (const auto & modulator : instrument->global_zone().modulators()) {
        printf("  -> Modulator %p\n", &modulator);
      }
    }

    // Instrument zones:
    for (auto && instrument_zone : instrument->zones()) {
      printf("  -> Instrument Zone %p\n", &instrument_zone);

      // Sample:
      if (instrument_zone->has_sample()) {
        auto sample = instrument_zone->sample();
        printf("    -> Sample %p \"%s\" (parent file: %p)\n",
          sample.get(),
          sample->name().c_str(),
          sample->has_parent_file() ? &sample->parent_file() : nullptr);

        // Generators:
        for (const auto & generator : instrument_zone->generators()) {
          printf("      -> Generator %p\n", &generator);
        }

        // Modulators:
        for (const auto & modulator : instrument_zone->modulators()) {
          printf("      -> Modulator %p\n", &modulator);
        }
      }
    }
  }
  printf("\n");

  // Samples:
  for (auto && sample : sf2.samples()) {
    printf("Sample %p \"%s\" (link %p, parent file: %p)\n",
      sample.get(),
      sample->name().c_str(),
      sample->has_link() ? sample->link().get() : nullptr,
      sample->has_parent_file() ? &sample->parent_file() : nullptr);
  }
  printf("\n");
}

} // namespace

#endif // SF2CUTE_EXAMPLES_DEBUG_HPP_
