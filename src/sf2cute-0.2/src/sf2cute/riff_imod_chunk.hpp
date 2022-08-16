/// @file
/// SoundFont "imod" RIFF chunk header
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_RIFF_IMOD_CHUNK_HPP_
#define SF2CUTE_RIFF_IMOD_CHUNK_HPP_

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <ostream>

#include <sf2cute/types.hpp>
#include <sf2cute/modulator.hpp>

#include "riff.hpp"

namespace sf2cute {

class SFInstrument;

/// The SFRIFFImodChunk class represents a SoundFont 2 "imod" chunk.
class SFRIFFImodChunk : public RIFFChunkInterface {
public:
  /// Unsigned integer type for the chunk size.
  using size_type = RIFFChunkInterface::size_type;

  /// The item size of imod chunk, in terms of bytes.
  static constexpr size_type kItemSize = 10;

  /// Constructs a new empty SFRIFFImodChunk.
  SFRIFFImodChunk();

  /// Constructs a new SFRIFFImodChunk using the specified instruments.
  /// @param instruments The instruments of the chunk.
  /// @throws std::length_error Too many instrument modulators.
  SFRIFFImodChunk(
      const std::vector<std::shared_ptr<SFInstrument>> & instruments);

  /// Constructs a new copy of specified SFRIFFImodChunk.
  /// @param origin a SFRIFFImodChunk object.
  SFRIFFImodChunk(const SFRIFFImodChunk & origin) = default;

  /// Copy-assigns a new value to the SFRIFFImodChunk, replacing its current contents.
  /// @param origin a SFRIFFImodChunk object.
  SFRIFFImodChunk & operator=(const SFRIFFImodChunk & origin) = default;

  /// Acquires the contents of specified SFRIFFImodChunk.
  /// @param origin a SFRIFFImodChunk object.
  SFRIFFImodChunk(SFRIFFImodChunk && origin) = default;

  /// Move-assigns a new value to the SFRIFFImodChunk, replacing its current contents.
  /// @param origin a SFRIFFImodChunk object.
  SFRIFFImodChunk & operator=(SFRIFFImodChunk && origin) = default;

  /// Destructs the SFRIFFImodChunk.
  virtual ~SFRIFFImodChunk() = default;

  /// @copydoc RIFFChunkInterface::name()
  virtual std::string name() const override {
    return "imod";
  }

  /// Returns the instruments of this chunk.
  /// @return the instruments of this chunk.
  const std::vector<std::shared_ptr<SFInstrument>> &
      instruments() const {
    return *instruments_;
  }

  /// Sets the instruments of this chunk.
  /// @param instruments the instruments of this chunk.
  /// @throws std::length_error Too many instrument modulators.
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
  /// Returns the number of instrument modulator items.
  /// @return the number of instrument modulator items, including the terminator item.
  /// @throws std::length_error Too many instrument modulators.
  uint16_t NumItems() const;

  /// Writes an item of imod chunk.
  /// @param out the output stream.
  /// @param source_op the source of data for the modulator.
  /// @param destination_op the destination of the modulator.
  /// @param amount the degree to which the source modulates the destination.
  /// @param amount_source_op the modulation source to be applied to the modulation amount.
  /// @param transform_op the transform type to be applied to the modulation source.
  /// @return the output stream.
  /// @throws std::ios_base::failure An I/O error occurred.
  static std::ostream & WriteItem(std::ostream & out,
      SFModulator source_op,
      SFGenerator destination_op,
      int16_t amount,
      SFModulator amount_source_op,
      SFTransform transform_op);

  /// The size of the chunk (excluding header).
  size_type size_;

  /// The instruments of the chunk.
  const std::vector<std::shared_ptr<SFInstrument>> * instruments_;
};

} // namespace sf2cute

#endif // SF2CUTE_RIFF_IMOD_CHUNK_HPP_
