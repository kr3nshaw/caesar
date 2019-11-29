/// @file
/// SoundFont "inst" RIFF chunk header
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_RIFF_INST_CHUNK_HPP_
#define SF2CUTE_RIFF_INST_CHUNK_HPP_

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <ostream>

#include "riff.hpp"

namespace sf2cute {

class SFInstrument;

/// The SFRIFFInstChunk class represents a SoundFont 2 "inst" chunk.
class SFRIFFInstChunk : public RIFFChunkInterface {
public:
  /// Unsigned integer type for the chunk size.
  using size_type = RIFFChunkInterface::size_type;

  /// The item size of inst chunk, in terms of bytes.
  static constexpr size_type kItemSize = 22;

  /// Constructs a new empty SFRIFFInstChunk.
  SFRIFFInstChunk();

  /// Constructs a new SFRIFFInstChunk using the specified instruments.
  /// @param instruments The instruments of the chunk.
  /// @throws std::length_error Too many instruments.
  SFRIFFInstChunk(
      const std::vector<std::shared_ptr<SFInstrument>> & instruments);

  /// Constructs a new copy of specified SFRIFFInstChunk.
  /// @param origin a SFRIFFInstChunk object.
  SFRIFFInstChunk(const SFRIFFInstChunk & origin) = default;

  /// Copy-assigns a new value to the SFRIFFInstChunk, replacing its current contents.
  /// @param origin a SFRIFFInstChunk object.
  SFRIFFInstChunk & operator=(const SFRIFFInstChunk & origin) = default;

  /// Acquires the contents of specified SFRIFFInstChunk.
  /// @param origin a SFRIFFInstChunk object.
  SFRIFFInstChunk(SFRIFFInstChunk && origin) = default;

  /// Move-assigns a new value to the SFRIFFInstChunk, replacing its current contents.
  /// @param origin a SFRIFFInstChunk object.
  SFRIFFInstChunk & operator=(SFRIFFInstChunk && origin) = default;

  /// Destructs the SFRIFFInstChunk.
  virtual ~SFRIFFInstChunk() = default;

  /// @copydoc RIFFChunkInterface::name()
  virtual std::string name() const override {
    return "inst";
  }

  /// Returns the instruments of this chunk.
  /// @return the instruments of this chunk.
  const std::vector<std::shared_ptr<SFInstrument>> &
      instruments() const {
    return *instruments_;
  }

  /// Sets the instruments of this chunk.
  /// @param instruments the instruments of this chunk.
  /// @throws std::length_error Too many instruments.
  void set_instruments(
      const std::vector<std::shared_ptr<SFInstrument>> & instruments) {
    instruments_ = &instruments;
    size_ = kItemSize * NumItems();
  }

  /// Returns the whole length of this chunk.
  /// @return the length of this chunk including a chunk header, in terms of bytes.
  virtual size_type size() const noexcept override {
    return 8 + size_;
  }

  /// Writes this chunk to the specified output stream.
  /// @param out the output stream.
  /// @throws std::length_error The chunk size exceeds the maximum.
  /// @throws std::ios_base::failure An I/O error occurred.
  virtual void Write(std::ostream & out) const override;

private:
  /// Returns the number of instrument items.
  /// @return the number of instrument items, including the terminator item.
  /// @throws std::length_error Too many instruments.
  uint16_t NumItems() const;

  /// Writes an item of inst chunk.
  /// @param out the output stream.
  /// @param name the name of instrument.
  /// @param inst_bag_index the instrument bag index starting from 0.
  /// @return the output stream.
  /// @throws std::ios_base::failure An I/O error occurred.
  static std::ostream & WriteItem(std::ostream & out,
      const std::string & name,
      uint16_t inst_bag_index);

  /// The size of the chunk (excluding header).
  size_type size_;

  /// The instruments of the chunk.
  const std::vector<std::shared_ptr<SFInstrument>> * instruments_;
};

} // namespace sf2cute

#endif // SF2CUTE_RIFF_INST_CHUNK_HPP_
