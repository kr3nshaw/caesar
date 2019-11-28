/// @file
/// SoundFont "igen" RIFF chunk header
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_RIFF_IGEN_CHUNK_HPP_
#define SF2CUTE_RIFF_IGEN_CHUNK_HPP_

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <ostream>

#include <sf2cute/types.hpp>

#include "riff.hpp"

namespace sf2cute {

class SFInstrument;
class SFSample;
class SFGeneratorItem;

/// The SFRIFFIgenChunk class represents a SoundFont 2 "igen" chunk.
class SFRIFFIgenChunk : public RIFFChunkInterface {
public:
  /// Unsigned integer type for the chunk size.
  using size_type = RIFFChunkInterface::size_type;

  /// The item size of igen chunk, in terms of bytes.
  static constexpr size_type kItemSize = 4;

  /// Constructs a new empty SFRIFFIgenChunk.
  SFRIFFIgenChunk();

  /// Constructs a new SFRIFFIgenChunk using the specified instruments.
  /// @param instruments The instruments of the chunk.
  /// @param sample_index_map the map containing the samples as keys and their indices as map values.
  /// @throws std::length_error Too many instrument generators.
  SFRIFFIgenChunk(
      const std::vector<std::shared_ptr<SFInstrument>> & instruments,
      std::unordered_map<const SFSample *, uint16_t> sample_index_map);

  /// Constructs a new copy of specified SFRIFFIgenChunk.
  /// @param origin a SFRIFFIgenChunk object.
  SFRIFFIgenChunk(const SFRIFFIgenChunk & origin) = default;

  /// Copy-assigns a new value to the SFRIFFIgenChunk, replacing its current contents.
  /// @param origin a SFRIFFIgenChunk object.
  SFRIFFIgenChunk & operator=(const SFRIFFIgenChunk & origin) = default;

  /// Acquires the contents of specified SFRIFFIgenChunk.
  /// @param origin a SFRIFFIgenChunk object.
  SFRIFFIgenChunk(SFRIFFIgenChunk && origin) = default;

  /// Move-assigns a new value to the SFRIFFIgenChunk, replacing its current contents.
  /// @param origin a SFRIFFIgenChunk object.
  SFRIFFIgenChunk & operator=(SFRIFFIgenChunk && origin) = default;

  /// Destructs the SFRIFFIgenChunk.
  virtual ~SFRIFFIgenChunk() = default;

  /// @copydoc RIFFChunkInterface::name()
  virtual std::string name() const override {
    return "igen";
  }

  /// Returns the instruments of this chunk.
  /// @return the instruments of this chunk.
  const std::vector<std::shared_ptr<SFInstrument>> &
      instruments() const {
    return *instruments_;
  }

  /// Sets the instruments of this chunk.
  /// @param instruments the instruments of this chunk.
  /// @throws std::length_error Too many instrument generators.
  void set_instruments(
      const std::vector<std::shared_ptr<SFInstrument>> & instruments) {
    instruments_ = &instruments;
    size_ = kItemSize * NumItems();
  }

  /// Returns the map containing the samples as keys and their indices as map values.
  /// @return the map containing the samples as keys and their indices as map values.
  const std::unordered_map<const SFSample *, uint16_t> &
      sample_index_map() const {
    return sample_index_map_;
  }

  /// Sets the map containing the samples as keys and their indices as map values.
  /// @param sample_index_map the map containing the samples as keys and their indices as map values.
  void set_sample_index_map(
      std::unordered_map<const SFSample *, uint16_t> sample_index_map) {
    sample_index_map_ = std::move(sample_index_map);
  }

  /// Returns the whole length of this chunk.
  /// @return the length of this chunk including a chunk header, in terms of bytes.
  virtual size_type size() const noexcept override {
    return 8 + size_;
  }

  /// Writes this chunk to the specified output stream.
  /// @param out the output stream.
  /// @throws std::invalid_argument Global instrument zone has a sample.
  /// @throws std::invalid_argument Instrument zone does not have a sample.
  /// @throws std::length_error The chunk size exceeds the maximum.
  /// @throws std::out_of_range Instrument zone points to an unknown sample.
  /// @throws std::ios_base::failure An I/O error occurred.
  virtual void Write(std::ostream & out) const override;

private:
  /// Returns the number of instrument generator items.
  /// @return the number of instrument generator items, including the terminator item.
  /// @throws std::length_error Too many instrument generators.
  uint16_t NumItems() const;

  /// Writes an item of igen chunk.
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

  /// The instruments of the chunk.
  const std::vector<std::shared_ptr<SFInstrument>> * instruments_;

  /// The map containing the samples as keys and their indices as map values.
  std::unordered_map<const SFSample *, uint16_t> sample_index_map_;
};

} // namespace sf2cute

#endif // SF2CUTE_RIFF_IGEN_CHUNK_HPP_
