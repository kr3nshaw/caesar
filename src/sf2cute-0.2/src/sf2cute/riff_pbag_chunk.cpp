/// @file
/// SoundFont "pbag" RIFF chunk implementation
///
/// @author gocha <https://github.com/gocha>

#include "riff_pbag_chunk.hpp"

#include <stdint.h>
#include <memory>
#include <string>
#include <sstream>
#include <ostream>
#include <stdexcept>

#include <sf2cute/preset.hpp>
#include <sf2cute/preset_zone.hpp>

#include "byteio.hpp"

namespace sf2cute {

/// Constructs a new empty SFRIFFPbagChunk.
SFRIFFPbagChunk::SFRIFFPbagChunk() :
    size_(0),
    presets_(nullptr) {
}

/// Constructs a new SFRIFFPbagChunk using the specified presets.
SFRIFFPbagChunk::SFRIFFPbagChunk(
    const std::vector<std::shared_ptr<SFPreset>> & presets) :
    presets_(&presets) {
  size_ = kItemSize * NumItems();
}

/// Writes this chunk to the specified output stream.
void SFRIFFPbagChunk::Write(std::ostream & out) const {
  // Save exception bits of output stream.
  const std::ios_base::iostate old_exception_bits = out.exceptions();
  // Set exception bits to get output error as an exception.
  out.exceptions(std::ios::badbit | std::ios::failbit);

  try {
    // Write the chunk header.
    RIFFChunk::WriteHeader(out, name(), size_);

    // Presets:
    size_t generator_index = 0;
    size_t modulator_index = 0;
    for (const auto & preset : presets()) {
      // Global preset zone:
      if (preset->has_global_zone()) {
        // Write the global zone.
        WriteItem(out, uint16_t(generator_index), uint16_t(modulator_index));

        // Increment the generator index and the modulator index.
        generator_index += preset->global_zone().generators().size();
        modulator_index += preset->global_zone().modulators().size();
      }

      // Preset zones:
      for (const auto & zone : preset->zones()) {
        // Write the preset zone.
        WriteItem(out, uint16_t(generator_index), uint16_t(modulator_index));

        // Increment the generator index and the modulator index.
        generator_index += (zone->has_instrument() ? 1 : 0) + zone->generators().size();
        modulator_index += zone->modulators().size();
      }
    }

    // Write the last terminator item.
    WriteItem(out, uint16_t(generator_index), uint16_t(modulator_index));

    // Write a padding byte if necessary.
    if (size_ % 2 != 0) {
      InsertInt8(out, 0);
    }
  }
  catch (const std::exception &) {
    // Recover exception bits of output stream.
    out.exceptions(old_exception_bits);

    // Rethrow the exception.
    throw;
  }
}

/// Returns the number of preset zone items.
uint16_t SFRIFFPbagChunk::NumItems() const {
  size_t num_zones = 1; // 1 = terminator
  for (const auto & preset : presets()) {
    // Count the global zone.
    if (preset->has_global_zone()) {
      num_zones++;
    }

    // Count the preset zones.
    num_zones += preset->zones().size();

    // Check the range of the number of zones.
    if (num_zones > UINT16_MAX) {
      throw std::length_error("Too many preset zones.");
    }
  }
  return static_cast<uint16_t>(num_zones);
}

/// Writes an item of pbag chunk.
std::ostream & SFRIFFPbagChunk::WriteItem(std::ostream & out,
    uint16_t generator_index,
    uint16_t modulator_index) {
  // struct sfPresetBag:
  // uint16_t wGenNdx;
  InsertInt16L(out, generator_index);

  // uint16_t wModNdx;
  InsertInt16L(out, modulator_index);

  return out;
}

} // namespace sf2cute
