/// @file
/// SoundFont "pbag" RIFF chunk header
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_RIFF_PBAG_CHUNK_HPP_
#define SF2CUTE_RIFF_PBAG_CHUNK_HPP_

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <ostream>

#include "riff.hpp"

namespace sf2cute {

class SFPreset;

/// The SFRIFFPbagChunk class represents a SoundFont 2 "pbag" chunk.
class SFRIFFPbagChunk : public RIFFChunkInterface {
public:
  /// Unsigned integer type for the chunk size.
  using size_type = RIFFChunkInterface::size_type;

  /// The item size of pbag chunk, in terms of bytes.
  static constexpr size_type kItemSize = 4;

  /// Constructs a new empty SFRIFFPbagChunk.
  SFRIFFPbagChunk();

  /// Constructs a new SFRIFFPbagChunk using the specified presets.
  /// @param presets The presets of the chunk.
  /// @throws std::length_error Too many preset zones.
  SFRIFFPbagChunk(
      const std::vector<std::shared_ptr<SFPreset>> & presets);

  /// Constructs a new copy of specified SFRIFFPbagChunk.
  /// @param origin a SFRIFFPbagChunk object.
  SFRIFFPbagChunk(const SFRIFFPbagChunk & origin) = default;

  /// Copy-assigns a new value to the SFRIFFPbagChunk, replacing its current contents.
  /// @param origin a SFRIFFPbagChunk object.
  SFRIFFPbagChunk & operator=(const SFRIFFPbagChunk & origin) = default;

  /// Acquires the contents of specified SFRIFFPbagChunk.
  /// @param origin a SFRIFFPbagChunk object.
  SFRIFFPbagChunk(SFRIFFPbagChunk && origin) = default;

  /// Move-assigns a new value to the SFRIFFPbagChunk, replacing its current contents.
  /// @param origin a SFRIFFPbagChunk object.
  SFRIFFPbagChunk & operator=(SFRIFFPbagChunk && origin) = default;

  /// Destructs the SFRIFFPbagChunk.
  virtual ~SFRIFFPbagChunk() = default;

  /// @copydoc RIFFChunkInterface::name()
  virtual std::string name() const override {
    return "pbag";
  }

  /// Returns the presets of this chunk.
  /// @return the presets of this chunk.
  const std::vector<std::shared_ptr<SFPreset>> &
      presets() const {
    return *presets_;
  }

  /// Sets the presets of this chunk.
  /// @param presets the presets of this chunk.
  /// @throws std::length_error Too many preset zones.
  void set_presets(
      const std::vector<std::shared_ptr<SFPreset>> & presets) {
    presets_ = &presets;
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
  /// Returns the number of preset zone items.
  /// @return the number of preset zone items, including the terminator item.
  /// @throws std::length_error Too many preset zones.
  uint16_t NumItems() const;

  /// Writes an item of pbag chunk.
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

  /// The presets of the chunk.
  const std::vector<std::shared_ptr<SFPreset>> * presets_;
};

} // namespace sf2cute

#endif // SF2CUTE_RIFF_PBAG_CHUNK_HPP_
