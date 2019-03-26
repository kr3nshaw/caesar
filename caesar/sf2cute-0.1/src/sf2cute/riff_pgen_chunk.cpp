/// @file
/// SoundFont "pgen" RIFF chunk implementation
///
/// @author gocha <https://github.com/gocha>

#include "riff_pgen_chunk.hpp"

#include <stdint.h>
#include <algorithm>
#include <memory>
#include <string>
#include <unordered_map>
#include <sstream>
#include <ostream>
#include <stdexcept>

#include <sf2cute/preset.hpp>
#include <sf2cute/preset_zone.hpp>

#include "byteio.hpp"

namespace sf2cute {

namespace {

constexpr auto kChunkName = "pgen";

} // namespace

/// Constructs a new empty SFRIFFPgenChunk.
SFRIFFPgenChunk::SFRIFFPgenChunk() :
    name_(kChunkName),
    size_(0),
    presets_(nullptr),
    instrument_index_map_() {
}

/// Constructs a new SFRIFFPgenChunk using the specified presets.
SFRIFFPgenChunk::SFRIFFPgenChunk(
    const std::vector<std::shared_ptr<SFPreset>> & presets,
    std::unordered_map<const SFInstrument *, uint16_t> instrument_index_map) :
    name_(kChunkName),
    presets_(&presets),
    instrument_index_map_(std::move(instrument_index_map)) {
  size_ = kItemSize * NumItems();
}

/// Returns the name of this chunk.
const std::string & SFRIFFPgenChunk::name() const {
  return name_;
}

/// Sets the name of this chunk.
void SFRIFFPgenChunk::set_name(std::string name) {
  // Throw exception if the length of chunk name is not 4.
  if (name.size() != 4) {
    std::ostringstream message_builder;
    message_builder << "Invalid RIFF chunk name \"" << name << "\".";
    throw std::invalid_argument(message_builder.str());
  }

  // Set the name.
  name_ = std::move(name);
}

/// Returns the presets of this chunk.
const std::vector<std::shared_ptr<SFPreset>> &
    SFRIFFPgenChunk::presets() const {
  return *presets_;
}

/// Sets the presets of this chunk.
void SFRIFFPgenChunk::set_presets(
    const std::vector<std::shared_ptr<SFPreset>> & presets) {
  presets_ = &presets;
  size_ = kItemSize * NumItems();
}

/// Returns the map containing the samples as keys and their keys as map values.
const std::unordered_map<const SFInstrument *, uint16_t> &
    SFRIFFPgenChunk::instrument_index_map() const {
  return instrument_index_map_;
}

/// Sets the map containing the samples as keys and their keys as map values.
void SFRIFFPgenChunk::set_instrument_index_map(
    std::unordered_map<const SFInstrument *, uint16_t> instrument_index_map) {
  instrument_index_map_ = std::move(instrument_index_map);
}

/// Returns the whole length of this chunk.
SFRIFFPgenChunk::size_type SFRIFFPgenChunk::size() const noexcept {
  return 8 + size_;
}

/// Writes this chunk to the specified output stream.
void SFRIFFPgenChunk::Write(std::ostream & out) const {
  // Save exception bits of output stream.
  const int old_exception_bits = out.exceptions();
  // Set exception bits to get output error as an exception.
  out.exceptions(std::ios::badbit | std::ios::failbit);

  try {
    // Write the chunk header.
    RIFFChunk::WriteHeader(out, name(), size_);

    // Presets:
    for (const auto & preset : presets()) {
      // Global zone:
      if (preset->has_global_zone()) {
        // Check the instrument for the global zone.
        if (preset->global_zone().has_instrument()) {
          // Throw exception if the global zone has a link to an instrument.
          throw std::invalid_argument("Global preset zone cannot have a link to an instrument.");
        }

        // Write all the generators in the global zone.
        for (const auto & generator : SortGenerators(preset->global_zone().generators())) {
          WriteItem(out, generator->op(), generator->amount());
        }
      }

      // Preset zones:
      for (const auto & zone : preset->zones()) {
        // Write all the generators in the preset zone.
        for (const auto & generator : SortGenerators(zone->generators())) {
          WriteItem(out, generator->op(), generator->amount());
        }

        // Check the sample for the zone.
        if (zone->has_instrument()) {
          // Find the index number for the instrument.
          const auto & instrument = zone->instrument();
          if (instrument_index_map().count(instrument.get()) != 0) {
            // Write the instrument generator.
            GenAmountType instrument_index(instrument_index_map().at(instrument.get()));
            WriteItem(out, SFGenerator::kInstrument, instrument_index);
          }
          else {
            // Throw exception if the instrument could not be found in the index map.
            throw std::out_of_range("Preset zone points to an unknown instrument.");
          }
        }
        else {
          // Throw exception if the preset zone does not have a link to an instrument.
          throw std::invalid_argument("Preset zone must have a link to an instrument.");
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

/// Returns the number of preset generator items.
uint16_t SFRIFFPgenChunk::NumItems() const {
  size_t num_generators = 1; // 1 = terminator
  for (const auto & preset : presets()) {
    // Count the generators in the global zone.
    if (preset->has_global_zone()) {
      num_generators += preset->global_zone().generators().size();
      if (num_generators > UINT16_MAX) {
        throw std::length_error("Too many preset generators.");
      }
    }

    // Count the generators in the preset zones.
    for (const auto & zone : preset->zones()) {
      num_generators += (zone->has_instrument() ? 1 : 0) + zone->generators().size();
      if (num_generators > UINT16_MAX) {
        throw std::length_error("Too many preset generators.");
      }
    }
  }
  return static_cast<uint16_t>(num_generators);
}

/// Writes an item of pgen chunk.
std::ostream & SFRIFFPgenChunk::WriteItem(std::ostream & out,
    SFGenerator op,
    GenAmountType amount) {
  // struct sfGenList:
  // SFGenerator sfGenOper;
  InsertInt16L(out, static_cast<uint16_t>(op));

  // GenAmountType genAmount;
  InsertInt16L(out, amount.value);

  return out;
}

/// Sort generators based on the ordering requirements of the generator chunk.
std::vector<SFGeneratorItem *> SFRIFFPgenChunk::SortGenerators(
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
