/// @file
/// SoundFont "pmod" RIFF chunk implementation
///
/// @author gocha <https://github.com/gocha>

#include "riff_pmod_chunk.hpp"

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

/// Constructs a new empty SFRIFFPmodChunk.
SFRIFFPmodChunk::SFRIFFPmodChunk() :
    size_(0),
    presets_(nullptr) {
}

/// Constructs a new SFRIFFPmodChunk using the specified presets.
SFRIFFPmodChunk::SFRIFFPmodChunk(
    const std::vector<std::shared_ptr<SFPreset>> & presets) :
    presets_(&presets) {
  size_ = kItemSize * NumItems();
}

/// Writes this chunk to the specified output stream.
void SFRIFFPmodChunk::Write(std::ostream & out) const {
  // Save exception bits of output stream.
  const std::ios_base::iostate old_exception_bits = out.exceptions();
  // Set exception bits to get output error as an exception.
  out.exceptions(std::ios::badbit | std::ios::failbit);

  try {
    // Write the chunk header.
    RIFFChunk::WriteHeader(out, name(), size_);

    // Presets:
    for (const auto & preset : presets()) {
      // Global zone:
      if (preset->has_global_zone()) {
        // Write all the modulators in the global zone.
        for (const auto & modulator : preset->global_zone().modulators()) {
          WriteItem(out, modulator->source_op(), modulator->destination_op(),
            modulator->amount(), modulator->amount_source_op(), modulator->transform_op());
        }
      }

      // Preset zones:
      for (const auto & zone : preset->zones()) {
        // Write all the modulators in the preset zone.
        for (const auto & modulator : zone->modulators()) {
          WriteItem(out, modulator->source_op(), modulator->destination_op(),
            modulator->amount(), modulator->amount_source_op(), modulator->transform_op());
        }
      }
    }

    // Write the last terminator item.
    WriteItem(out, SFModulator(0), SFGenerator(0), 0, SFModulator(0), SFTransform(0));

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

/// Returns the number of preset modulator items.
uint16_t SFRIFFPmodChunk::NumItems() const {
  size_t num_modulators = 1; // 1 = terminator
  for (const auto & preset : presets()) {
    // Count the modulators in the global zone.
    if (preset->has_global_zone()) {
      num_modulators += preset->global_zone().modulators().size();
      if (num_modulators > UINT16_MAX) {
        throw std::length_error("Too many preset modulators.");
      }
    }

    // Count the modulators in the instrument zones.
    for (const auto & zone : preset->zones()) {
      num_modulators += zone->modulators().size();
      if (num_modulators > UINT16_MAX) {
        throw std::length_error("Too many preset modulators.");
      }
    }
  }
  return static_cast<uint16_t>(num_modulators);
}

/// Writes an item of pmod chunk.
std::ostream & SFRIFFPmodChunk::WriteItem(std::ostream & out,
    SFModulator source_op,
    SFGenerator destination_op,
    int16_t amount,
    SFModulator amount_source_op,
    SFTransform transform_op) {
  // struct sfModList:
  // SFModulator sfModSrcOper;
  InsertInt16L(out, uint16_t(source_op));

  // SFGenerator sfModDestOper;
  InsertInt16L(out, uint16_t(destination_op));

  // int16_t modAmount;
  InsertInt16L(out, amount);

  // SFModulator sfModAmtSrcOper;
  InsertInt16L(out, uint16_t(amount_source_op));

  // SFTransform sfModTransOper;
  InsertInt16L(out, uint16_t(transform_op));

  return out;
}

} // namespace sf2cute
