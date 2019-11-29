/// @file
/// SoundFont "ibag" RIFF chunk implementation
///
/// @author gocha <https://github.com/gocha>

#include "riff_ibag_chunk.hpp"

#include <stdint.h>
#include <memory>
#include <string>
#include <sstream>
#include <ostream>
#include <stdexcept>

#include <sf2cute/instrument.hpp>
#include <sf2cute/instrument_zone.hpp>

#include "byteio.hpp"

namespace sf2cute {

/// Constructs a new empty SFRIFFIbagChunk.
SFRIFFIbagChunk::SFRIFFIbagChunk() :
    size_(0),
    instruments_(nullptr) {
}

/// Constructs a new SFRIFFIbagChunk using the specified instruments.
SFRIFFIbagChunk::SFRIFFIbagChunk(
    const std::vector<std::shared_ptr<SFInstrument>> & instruments) :
    instruments_(&instruments) {
  size_ = kItemSize * NumItems();
}

/// Writes this chunk to the specified output stream.
void SFRIFFIbagChunk::Write(std::ostream & out) const {
  // Save exception bits of output stream.
  const std::ios_base::iostate old_exception_bits = out.exceptions();
  // Set exception bits to get output error as an exception.
  out.exceptions(std::ios::badbit | std::ios::failbit);

  try {
    // Write the chunk header.
    RIFFChunk::WriteHeader(out, name(), size_);

    // Instruments:
    size_t generator_index = 0;
    size_t modulator_index = 0;
    for (const auto & instrument : instruments()) {
      // Global instrument zone:
      if (instrument->has_global_zone()) {
        // Write the global zone.
        WriteItem(out, uint16_t(generator_index), uint16_t(modulator_index));

        // Increment the generator index and the modulator index.
        generator_index += instrument->global_zone().generators().size();
        modulator_index += instrument->global_zone().modulators().size();
      }

      // Instrument zones:
      for (const auto & zone : instrument->zones()) {
        // Write the instrument zone.
        WriteItem(out, uint16_t(generator_index), uint16_t(modulator_index));

        // Increment the generator index and the modulator index.
        generator_index += (zone->has_sample() ? 1 : 0) + zone->generators().size();
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

/// Returns the number of instrument zone items.
uint16_t SFRIFFIbagChunk::NumItems() const {
  size_t num_zones = 1; // 1 = terminator
  for (const auto & instrument : instruments()) {
    // Count the global zone.
    if (instrument->has_global_zone()) {
      num_zones++;
    }

    // Count the instrument zones.
    num_zones += instrument->zones().size();

    // Check the range of the number of zones.
    if (num_zones > UINT16_MAX) {
      throw std::length_error("Too many instrument zones.");
    }
  }
  return static_cast<uint16_t>(num_zones);
}

/// Writes an item of ibag chunk.
std::ostream & SFRIFFIbagChunk::WriteItem(std::ostream & out,
    uint16_t generator_index,
    uint16_t modulator_index) {
  // struct sfInstBag:
  // uint16_t wInstGenNdx;
  InsertInt16L(out, generator_index);

  // uint16_t wInstModNdx;
  InsertInt16L(out, modulator_index);

  return out;
}

} // namespace sf2cute
