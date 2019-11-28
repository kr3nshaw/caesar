/// @file
/// SoundFont "imod" RIFF chunk implementation
///
/// @author gocha <https://github.com/gocha>

#include "riff_imod_chunk.hpp"

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

/// Constructs a new empty SFRIFFImodChunk.
SFRIFFImodChunk::SFRIFFImodChunk() :
    size_(0),
    instruments_(nullptr) {
}

/// Constructs a new SFRIFFImodChunk using the specified instruments.
SFRIFFImodChunk::SFRIFFImodChunk(
    const std::vector<std::shared_ptr<SFInstrument>> & instruments) :
    instruments_(&instruments) {
  size_ = kItemSize * NumItems();
}

/// Writes this chunk to the specified output stream.
void SFRIFFImodChunk::Write(std::ostream & out) const {
  // Save exception bits of output stream.
  const std::ios_base::iostate old_exception_bits = out.exceptions();
  // Set exception bits to get output error as an exception.
  out.exceptions(std::ios::badbit | std::ios::failbit);

  try {
    // Write the chunk header.
    RIFFChunk::WriteHeader(out, name(), size_);

    // Instruments:
    for (const auto & instrument : instruments()) {
      // Global zone:
      if (instrument->has_global_zone()) {
        // Write all the modulators in the global zone.
        for (const auto & modulator : instrument->global_zone().modulators()) {
          WriteItem(out, modulator->source_op(), modulator->destination_op(),
            modulator->amount(), modulator->amount_source_op(), modulator->transform_op());
        }
      }

      // Instrument zones:
      for (const auto & zone : instrument->zones()) {
        // Write all the modulators in the instrument zone.
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

/// Returns the number of instrument modulator items.
uint16_t SFRIFFImodChunk::NumItems() const {
  size_t num_modulators = 1; // 1 = terminator
  for (const auto & instrument : instruments()) {
    // Count the modulators in the global zone.
    if (instrument->has_global_zone()) {
      num_modulators += instrument->global_zone().modulators().size();
      if (num_modulators > UINT16_MAX) {
        throw std::length_error("Too many instrument modulators.");
      }
    }

    // Count the modulators in the instrument zones.
    for (const auto & zone : instrument->zones()) {
      num_modulators += zone->modulators().size();
      if (num_modulators > UINT16_MAX) {
        throw std::length_error("Too many instrument modulators.");
      }
    }
  }
  return static_cast<uint16_t>(num_modulators);
}

/// Writes an item of imod chunk.
std::ostream & SFRIFFImodChunk::WriteItem(std::ostream & out,
    SFModulator source_op,
    SFGenerator destination_op,
    int16_t amount,
    SFModulator amount_source_op,
    SFTransform transform_op) {
  // struct sfInstModList:
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
