/// @file
/// SoundFont "inst" RIFF chunk implementation
///
/// @author gocha <https://github.com/gocha>

#include "riff_inst_chunk.hpp"

#include <stdint.h>
#include <algorithm>
#include <memory>
#include <string>
#include <sstream>
#include <ostream>
#include <stdexcept>

#include <sf2cute/instrument.hpp>

#include "byteio.hpp"

namespace sf2cute {

/// Constructs a new empty SFRIFFInstChunk.
SFRIFFInstChunk::SFRIFFInstChunk() :
    size_(0),
    instruments_(nullptr) {
}

/// Constructs a new SFRIFFInstChunk using the specified instruments.
SFRIFFInstChunk::SFRIFFInstChunk(
    const std::vector<std::shared_ptr<SFInstrument>> & instruments) :
    instruments_(&instruments) {
  size_ = kItemSize * NumItems();
}

/// Writes this chunk to the specified output stream.
void SFRIFFInstChunk::Write(std::ostream & out) const {
  // Save exception bits of output stream.
  const std::ios_base::iostate old_exception_bits = out.exceptions();
  // Set exception bits to get output error as an exception.
  out.exceptions(std::ios::badbit | std::ios::failbit);

  try {
    // Write the chunk header.
    RIFFChunk::WriteHeader(out, name(), size_);

    // Instruments:
    size_t inst_bag_index = 0;
    for (const auto & instrument : instruments()) {
      // Write the instrument header.
      WriteItem(out, instrument->name(), uint16_t(inst_bag_index));

      // Count the number of instrument zones.
      size_t num_zones = 0;
      if (instrument->has_global_zone()) {
        num_zones++;
      }
      num_zones += instrument->zones().size();

      // Increment the instrument bag index.
      inst_bag_index += num_zones;
    }

    // Write the last terminator item.
    WriteItem(out, "EOI", uint16_t(inst_bag_index));

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

/// Returns the number of instrument items.
uint16_t SFRIFFInstChunk::NumItems() const {
  size_t num_instrument_items = instruments().size() + 1;
  if (num_instrument_items > UINT16_MAX) {
    throw std::length_error("Too many instruments.");
  }
  return static_cast<uint16_t>(num_instrument_items);
}

/// Writes an item of inst chunk.
std::ostream & SFRIFFInstChunk::WriteItem(std::ostream & out,
    const std::string & name,
    uint16_t inst_bag_index) {
  // struct sfInst:
  // char achInstName[20];
  std::string instrument_name(name.substr(0, SFInstrument::kMaxNameLength));
  out.write(instrument_name.data(), instrument_name.size());
  for (std::string::size_type i = 0; i < SFInstrument::kMaxNameLength + 1 - instrument_name.size(); i++) {
    out.put(0);
  }

  // uint16_t wInstBagNdx;
  InsertInt16L(out, inst_bag_index);

  return out;
}

} // namespace sf2cute
