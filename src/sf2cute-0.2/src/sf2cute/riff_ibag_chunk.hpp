/// @file
/// SoundFont "ibag" RIFF chunk header
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_RIFF_IBAG_CHUNK_HPP_
#define SF2CUTE_RIFF_IBAG_CHUNK_HPP_

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <ostream>

#include "riff.hpp"

namespace sf2cute {

class SFInstrument;

/// The SFRIFFIbagChunk class represents a SoundFont 2 "ibag" chunk.
class SFRIFFIbagChunk : public RIFFChunkInterface {
public:
  /// Unsigned integer type for the chunk size.
  using size_type = RIFFChunkInterface::size_type;

  /// The item size of ibag chunk, in terms of bytes.
  static constexpr size_type kItemSize = 4;

  /// Constructs a new empty SFRIFFIbagChunk.
  SFRIFFIbagChunk();

  /// Constructs a new SFRIFFIbagChunk using the specified instruments.
  /// @param instruments The instruments of the chunk.
  /// @throws std::length_error Too many instrument zones.
  SFRIFFIbagChunk(
      const std::vector<std::shared_ptr<SFInstrument>> & instruments);

  /// Constructs a new copy of specified SFRIFFIbagChunk.
  /// @param origin a SFRIFFIbagChunk object.
  SFRIFFIbagChunk(const SFRIFFIbagChunk & origin) = default;

  /// Copy-assigns a new value to the SFRIFFIbagChunk, replacing its current contents.
  /// @param origin a SFRIFFIbagChunk object.
  SFRIFFIbagChunk & operator=(const SFRIFFIbagChunk & origin) = default;

  /// Acquires the contents of specified SFRIFFIbagChunk.
  /// @param origin a SFRIFFIbagChunk object.
  SFRIFFIbagChunk(SFRIFFIbagChunk && origin) = default;

  /// Move-assigns a new value to the SFRIFFIbagChunk, replacing its current contents.
  /// @param origin a SFRIFFIbagChunk object.
  SFRIFFIbagChunk & operator=(SFRIFFIbagChunk && origin) = default;

  /// Destructs the SFRIFFIbagChunk.
  virtual ~SFRIFFIbagChunk() = default;

  /// @copydoc RIFFChunkInterface::name()
  virtual std::string name() const override {
    return "ibag";
  }

  /// Returns the instruments of this chunk.
  /// @return the instruments of this chunk.
  const std::vector<std::shared_ptr<SFInstrument>> &
      instruments() const {
    return *instruments_;
  }

  /// Sets the instruments of this chunk.
  /// @param instruments the instruments of this chunk.
  /// @throws std::length_error Too many instrument zones.
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
  /// Returns the number of instrument zone items.
  /// @return the number of instrument zone items, including the terminator item.
  /// @throws std::length_error Too many instrument zones.
  uint16_t NumItems() const;

  /// Writes an item of ibag chunk.
  /// @param out the output stream.
  /// @param generator_index the generator index starting from 0.
  /// @param modulator_index the modulator index starting from 0.
  /// @return the output stream.
  /// @throws std::ios_base::failure An I/O error occurred.
  static std::ostream & WriteItem(std::ostream & out,
      uint16_t generator_index,
      uint16_t modulator_index);

  /// The size of the chunk (excluding header).
  size_type size_;

  /// The instruments of the chunk.
  const std::vector<std::shared_ptr<SFInstrument>> * instruments_;
};

} // namespace sf2cute

#endif // SF2CUTE_RIFF_IBAG_CHUNK_HPP_
