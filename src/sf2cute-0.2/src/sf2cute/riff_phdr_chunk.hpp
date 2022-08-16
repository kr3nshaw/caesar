/// @file
/// SoundFont "phdr" RIFF chunk header
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_RIFF_PHDR_CHUNK_HPP_
#define SF2CUTE_RIFF_PHDR_CHUNK_HPP_

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <ostream>

#include "riff.hpp"

namespace sf2cute {

class SFPreset;

/// The SFRIFFPhdrChunk class represents a SoundFont 2 "phdr" chunk.
class SFRIFFPhdrChunk : public RIFFChunkInterface {
public:
  /// Unsigned integer type for the chunk size.
  using size_type = RIFFChunkInterface::size_type;

  /// The item size of phdr chunk, in terms of bytes.
  static constexpr size_type kItemSize = 38;

  /// Constructs a new empty SFRIFFPhdrChunk.
  SFRIFFPhdrChunk();

  /// Constructs a new SFRIFFPhdrChunk using the specified presets.
  /// @param presets The presets of the chunk.
  /// @throws std::length_error Too many presets.
  SFRIFFPhdrChunk(
      const std::vector<std::shared_ptr<SFPreset>> & presets);

  /// Constructs a new copy of specified SFRIFFPhdrChunk.
  /// @param origin a SFRIFFPhdrChunk object.
  SFRIFFPhdrChunk(const SFRIFFPhdrChunk & origin) = default;

  /// Copy-assigns a new value to the SFRIFFPhdrChunk, replacing its current contents.
  /// @param origin a SFRIFFPhdrChunk object.
  SFRIFFPhdrChunk & operator=(const SFRIFFPhdrChunk & origin) = default;

  /// Acquires the contents of specified SFRIFFPhdrChunk.
  /// @param origin a SFRIFFPhdrChunk object.
  SFRIFFPhdrChunk(SFRIFFPhdrChunk && origin) = default;

  /// Move-assigns a new value to the SFRIFFPhdrChunk, replacing its current contents.
  /// @param origin a SFRIFFPhdrChunk object.
  SFRIFFPhdrChunk & operator=(SFRIFFPhdrChunk && origin) = default;

  /// Destructs the SFRIFFPhdrChunk.
  virtual ~SFRIFFPhdrChunk() = default;

  /// @copydoc RIFFChunkInterface::name()
  virtual std::string name() const override {
    return "phdr";
  }

  /// Returns the presets of this chunk.
  /// @return the presets of this chunk.
  const std::vector<std::shared_ptr<SFPreset>> &
      presets() const {
    return *presets_;
  }

  /// Sets the presets of this chunk.
  /// @param presets the presets of this chunk.
  /// @throws std::length_error Too many presets.
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
  /// Returns the number of preset items.
  /// @return the number of preset items, including the terminator item.
  /// @throws std::length_error Too many presets.
  uint16_t NumItems() const;

  /// Writes an item of phdr chunk.
  /// @param out the output stream.
  /// @param name the name of preset.
  /// @param preset_number the preset number.
  /// @param bank the bank number.
  /// @param preset_bag_index the preset bag index starting from 0.
  /// @param library the library.
  /// @param genre the genre.
  /// @param morphology the morphology.
  /// @return the output stream.
  /// @throws std::ios_base::failure An I/O error occurred.
  static std::ostream & WriteItem(std::ostream & out,
      const std::string & name,
      uint16_t preset_number,
      uint16_t bank,
      uint16_t preset_bag_index,
      uint32_t library,
      uint32_t genre,
      uint32_t morphology);

  /// The name of the chunk.
  std::string name_;

  /// The size of the chunk (excluding header).
  size_type size_;

  /// The presets of the chunk.
  const std::vector<std::shared_ptr<SFPreset>> * presets_;
};

} // namespace sf2cute

#endif // SF2CUTE_RIFF_PHDR_CHUNK_HPP_
