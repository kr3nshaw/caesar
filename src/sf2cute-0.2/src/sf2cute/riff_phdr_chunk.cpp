/// @file
/// SoundFont "phdr" RIFF chunk implementation
///
/// @author gocha <https://github.com/gocha>

#include "riff_phdr_chunk.hpp"

#include <stdint.h>
#include <memory>
#include <string>
#include <sstream>
#include <ostream>
#include <stdexcept>

#include <sf2cute/preset.hpp>

#include "byteio.hpp"

namespace sf2cute {

/// Constructs a new empty SFRIFFPhdrChunk.
SFRIFFPhdrChunk::SFRIFFPhdrChunk() :
    size_(0),
    presets_(nullptr) {
}

/// Constructs a new SFRIFFPhdrChunk using the specified presets.
SFRIFFPhdrChunk::SFRIFFPhdrChunk(
    const std::vector<std::shared_ptr<SFPreset>> & presets) :
    presets_(&presets) {
  size_ = kItemSize * NumItems();
}

/// Writes this chunk to the specified output stream.
void SFRIFFPhdrChunk::Write(std::ostream & out) const {
  // Save exception bits of output stream.
  const std::ios_base::iostate old_exception_bits = out.exceptions();
  // Set exception bits to get output error as an exception.
  out.exceptions(std::ios::badbit | std::ios::failbit);

  try {
    // Write the chunk header.
    RIFFChunk::WriteHeader(out, name(), size_);

    // Presets:
    size_t preset_bag_index = 0;
    for (const auto & preset : presets()) {
      // Write the preset header.
      WriteItem(out, preset->name(),
        preset->preset_number(), preset->bank(), uint16_t(preset_bag_index),
        preset->library(), preset->genre(), preset->morphology());

      // Count the number of preset zones.
      size_t num_zones = 0;
      if (preset->has_global_zone()) {
        num_zones++;
      }
      num_zones += preset->zones().size();

      // Increment the preset bag index.
      preset_bag_index += num_zones;
    }

    // Write the last terminator item.
    WriteItem(out, "EOP", 0, 0, uint16_t(preset_bag_index), 0, 0, 0);

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

/// Returns the number of preset items.
uint16_t SFRIFFPhdrChunk::NumItems() const {
  size_t num_preset_items = presets().size() + 1;
  if (num_preset_items > UINT16_MAX) {
    throw std::length_error("Too many presets.");
  }
  return static_cast<uint16_t>(num_preset_items);
}

/// Writes an item of phdr chunk.
std::ostream & SFRIFFPhdrChunk::WriteItem(std::ostream & out,
    const std::string & name,
    uint16_t preset_number,
    uint16_t bank,
    uint16_t preset_bag_index,
    uint32_t library,
    uint32_t genre,
    uint32_t morphology) {
  // struct sfPresetHeader:
  // char achPresetName[20];
  std::string preset_name(name.substr(0, SFPreset::kMaxNameLength));
  out.write(preset_name.data(), preset_name.size());
  for (std::string::size_type i = 0; i < SFPreset::kMaxNameLength + 1 - preset_name.size(); i++) {
    out.put(0);
  }

  // uint16_t wPreset;
  InsertInt16L(out, preset_number);

  // uint16_t wBank;
  InsertInt16L(out, bank);

  // uint16_t wPresetBagNdx;
  InsertInt16L(out, preset_bag_index);

  // uint32_t dwLibrary;
  InsertInt32L(out, library);

  // uint32_t dwGenre;
  InsertInt32L(out, genre);

  // uint32_t dwMorphology;
  InsertInt32L(out, morphology);

  return out;
}

} // namespace sf2cute
