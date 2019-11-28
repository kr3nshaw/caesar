/// @file
/// SoundFont "pgen" RIFF chunk header
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_RIFF_PGEN_CHUNK_HPP_
#define SF2CUTE_RIFF_PGEN_CHUNK_HPP_

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <ostream>

#include <sf2cute/types.hpp>

#include "riff.hpp"

namespace sf2cute {

class SFPreset;
class SFInstrument;
class SFGeneratorItem;

/// The SFRIFFPgenChunk class represents a SoundFont 2 "pgen" chunk.
class SFRIFFPgenChunk : public RIFFChunkInterface {
public:
  /// Unsigned integer type for the chunk size.
  using size_type = RIFFChunkInterface::size_type;

  /// The item size of pgen chunk, in terms of bytes.
  static constexpr size_type kItemSize = 4;

  /// Constructs a new empty SFRIFFPgenChunk.
  SFRIFFPgenChunk();

  /// Constructs a new SFRIFFPgenChunk using the specified presets.
  /// @param presets the presets of the chunk.
  /// @param instrument_index_map map containing the instruments as keys and their indices as map values.
  /// @throws std::length_error Too many preset generators.
  SFRIFFPgenChunk(
      const std::vector<std::shared_ptr<SFPreset>> & presets,
      std::unordered_map<const SFInstrument *, uint16_t> instrument_index_map);

  /// Constructs a new copy of specified SFRIFFPgenChunk.
  /// @param origin a SFRIFFPgenChunk object.
  SFRIFFPgenChunk(const SFRIFFPgenChunk & origin) = default;

  /// Copy-assigns a new value to the SFRIFFPgenChunk, replacing its current contents.
  /// @param origin a SFRIFFPgenChunk object.
  SFRIFFPgenChunk & operator=(const SFRIFFPgenChunk & origin) = default;

  /// Acquires the contents of specified SFRIFFPgenChunk.
  /// @param origin a SFRIFFPgenChunk object.
  SFRIFFPgenChunk(SFRIFFPgenChunk && origin) = default;

  /// Move-assigns a new value to the SFRIFFPgenChunk, replacing its current contents.
  /// @param origin a SFRIFFPgenChunk object.
  SFRIFFPgenChunk & operator=(SFRIFFPgenChunk && origin) = default;

  /// Destructs the SFRIFFPgenChunk.
  virtual ~SFRIFFPgenChunk() = default;

  /// @copydoc RIFFChunkInterface::name()
  virtual std::string name() const override {
    return "pgen";
  }

  /// Returns the presets of this chunk.
  /// @return the presets of this chunk.
  const std::vector<std::shared_ptr<SFPreset>> &
      presets() const {
    return *presets_;
  }

  /// Sets the presets of this chunk.
  /// @param presets the presets of this chunk.
  /// @throws std::length_error Too many preset generators.
  void set_presets(
      const std::vector<std::shared_ptr<SFPreset>> & presets) {
    presets_ = &presets;
    size_ = kItemSize * NumItems();
  }

  /// Returns the map containing the instruments as keys and their indices as map values.
  /// @return the map containing the instruments as keys and their indices as map values.
  const std::unordered_map<const SFInstrument *, uint16_t> &
      instrument_index_map() const {
    return instrument_index_map_;
  }

  /// Sets the map containing the instruments as keys and their indices as map values.
  /// @param instrument_index_map the map containing the instruments as keys and their indices as map values.
  void set_instrument_index_map(
      std::unordered_map<const SFInstrument *, uint16_t> instrument_index_map) {
    instrument_index_map_ = std::move(instrument_index_map);
  }

  /// Returns the whole length of this chunk.
  /// @return the length of this chunk including a chunk header, in terms of bytes.
  virtual size_type size() const noexcept override {
    return 8 + size_;
  }

  /// Writes this chunk to the specified output stream.
  /// @param out the output stream.
  /// @throws std::invalid_argument Global preset zone has an instrument.
  /// @throws std::invalid_argument Instrument zone does not have an instrument.
  /// @throws std::length_error The chunk size exceeds the maximum.
  /// @throws std::out_of_range Preset zone points to an unknown instrument.
  /// @throws std::ios_base::failure An I/O error occurred.
  virtual void Write(std::ostream & out) const override;

private:
  /// Returns the number of preset generator items.
  /// @return the number of preset generator items, including the terminator item.
  /// @throws std::length_error Too many preset generators.
  uint16_t NumItems() const;

  /// Writes an item of pgen chunk.
  /// @param out the output stream.
  /// @param op the type of the generator.
  /// @param amount the amount of the generator.
  /// @return the output stream.
  /// @throws std::ios_base::failure An I/O error occurred.
  static std::ostream & WriteItem(std::ostream & out,
      SFGenerator op,
      GenAmountType amount);

  /// Sort generators based on the ordering requirements of the generator chunk.
  /// @param generators the generators.
  /// @return the sorted collection of the generator pointers.
  static std::vector<SFGeneratorItem *> SortGenerators(
      const std::vector<std::unique_ptr<SFGeneratorItem>> & generators);

  /// The size of the chunk (excluding header).
  size_type size_;

  /// The presets of the chunk.
  const std::vector<std::shared_ptr<SFPreset>> * presets_;

  /// The map containing the instruments as keys and their indices as map values.
  std::unordered_map<const SFInstrument *, uint16_t> instrument_index_map_;
};

} // namespace sf2cute

#endif // SF2CUTE_RIFF_PGEN_CHUNK_HPP_
