/// @file
/// SoundFont "igen" RIFF chunk implementation
///
/// @author gocha <https://github.com/gocha>

#include "riff_igen_chunk.hpp"

#include <stdint.h>
#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <sstream>
#include <ostream>
#include <stdexcept>

#include <sf2cute/instrument.hpp>
#include <sf2cute/instrument_zone.hpp>

#include "byteio.hpp"

namespace sf2cute {

/// Constructs a new empty SFRIFFIgenChunk.
SFRIFFIgenChunk::SFRIFFIgenChunk() :
    size_(0),
    instruments_(nullptr),
    sample_index_map_() {
}

/// Constructs a new SFRIFFIgenChunk using the specified instruments.
SFRIFFIgenChunk::SFRIFFIgenChunk(
    const std::vector<std::shared_ptr<SFInstrument>> & instruments,
    std::unordered_map<const SFSample *, uint16_t> sample_index_map) :
    instruments_(&instruments),
    sample_index_map_(std::move(sample_index_map)) {
  size_ = kItemSize * NumItems();
}

/// Writes this chunk to the specified output stream.
void SFRIFFIgenChunk::Write(std::ostream & out) const {
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
        // Check the sample for the global zone.
        if (instrument->global_zone().has_sample()) {
          // Throw exception if the global zone has a link to a sample.
          throw std::invalid_argument("Global instrument zone cannot have a link to a sample.");
        }

        // Write all the generators in the global zone.
        for (const auto & generator : SortGenerators(instrument->global_zone().generators())) {
          WriteItem(out, generator->op(), generator->amount());
        }
      }

      // Instrument zones:
      for (const auto & zone : instrument->zones()) {
        // Write all the generators in the instrument zone.
        for (const auto & generator : SortGenerators(zone->generators())) {
          WriteItem(out, generator->op(), generator->amount());
        }

        // Check the sample for the zone.
        if (zone->has_sample()) {
          // Find the index number for the sample.
          const auto & sample = zone->sample();
          if (sample_index_map().count(sample.get()) != 0) {
            // Write the sampleID generator.
            GenAmountType sample_index(sample_index_map().at(sample.get()));
            WriteItem(out, SFGenerator::kSampleID, sample_index);
          }
          else {
            // Throw exception if the sample could not be found in the index map.
            throw std::out_of_range("Instrument zone points to an unknown sample.");
          }
        }
        else {
          // Throw exception if the instrument zone does not have a link to a sample.
          throw std::invalid_argument("Instrument zone must have a link to a sample.");
        }
      }
    }

    // Write the last terminator item.
    WriteItem(out, SFGenerator(0), GenAmountType(0));

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

/// Returns the number of instrument generator items.
uint16_t SFRIFFIgenChunk::NumItems() const {
  size_t num_generators = 1; // 1 = terminator
  for (const auto & instrument : instruments()) {
    // Count the generators in the global zone.
    if (instrument->has_global_zone()) {
      num_generators += instrument->global_zone().generators().size();
      if (num_generators > UINT16_MAX) {
        throw std::length_error("Too many instrument generators.");
      }
    }

    // Count the generators in the instrument zones.
    for (const auto & zone : instrument->zones()) {
      num_generators += (zone->has_sample() ? 1 : 0) + zone->generators().size();
      if (num_generators > UINT16_MAX) {
        throw std::length_error("Too many instrument generators.");
      }
    }
  }
  return static_cast<uint16_t>(num_generators);
}

/// Writes an item of igen chunk.
std::ostream & SFRIFFIgenChunk::WriteItem(std::ostream & out,
    SFGenerator op,
    GenAmountType amount) {
  // struct sfInstGenList:
  // SFGenerator sfGenOper;
  InsertInt16L(out, static_cast<uint16_t>(op));

  // GenAmountType genAmount;
  InsertInt16L(out, amount.value);

  return out;
}

/// Sort generators based on the ordering requirements of the generator chunk.
std::vector<SFGeneratorItem *> SFRIFFIgenChunk::SortGenerators(
    const std::vector<std::unique_ptr<SFGeneratorItem>> & generators) {
  // Make a copy of generator pointers.
  std::vector<SFGeneratorItem *> sorted_generators;
  sorted_generators.reserve(generators.size());
  for (const auto & generator : generators) {
    sorted_generators.push_back(generator.get());
  }

  // Sort the generator pointers.
  std::sort(sorted_generators.begin(), sorted_generators.end(),
    [](const SFGeneratorItem * x, const SFGeneratorItem * y) {
      return SFGeneratorItem::Compare(x->op(), y->op());
    });

  // Return the sorted pointers.
  return sorted_generators;
}

} // namespace sf2cute
