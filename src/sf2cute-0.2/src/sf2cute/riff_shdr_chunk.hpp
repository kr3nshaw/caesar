/// @file
/// SoundFont "shdr" RIFF chunk header
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_RIFF_SHDR_CHUNK_HPP_
#define SF2CUTE_RIFF_SHDR_CHUNK_HPP_

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <ostream>

#include <sf2cute/types.hpp>
#include <sf2cute/modulator.hpp>

#include "riff.hpp"

namespace sf2cute {

class SFSample;

/// The SFRIFFShdrChunk class represents a SoundFont 2 "shdr" chunk.
class SFRIFFShdrChunk : public RIFFChunkInterface {
public:
  /// Unsigned integer type for the chunk size.
  using size_type = RIFFChunkInterface::size_type;

  /// The item size of shdr chunk, in terms of bytes.
  static constexpr size_type kItemSize = 46;

  /// Constructs a new empty SFRIFFShdrChunk.
  SFRIFFShdrChunk();

  /// Constructs a new SFRIFFShdrChunk using the specified samples.
  /// @param samples The samples of the chunk.
  /// @param sample_index_map the map containing the samples as keys and their indices as map values.
  /// @throws std::length_error Too many samples.
  SFRIFFShdrChunk(const std::vector<std::shared_ptr<SFSample>> & samples,
      std::unordered_map<const SFSample *, uint16_t> sample_index_map);

  /// Constructs a new copy of specified SFRIFFShdrChunk.
  /// @param origin a SFRIFFShdrChunk object.
  SFRIFFShdrChunk(const SFRIFFShdrChunk & origin) = default;

  /// Copy-assigns a new value to the SFRIFFShdrChunk, replacing its current contents.
  /// @param origin a SFRIFFShdrChunk object.
  SFRIFFShdrChunk & operator=(const SFRIFFShdrChunk & origin) = default;

  /// Acquires the contents of specified SFRIFFShdrChunk.
  /// @param origin a SFRIFFShdrChunk object.
  SFRIFFShdrChunk(SFRIFFShdrChunk && origin) = default;

  /// Move-assigns a new value to the SFRIFFShdrChunk, replacing its current contents.
  /// @param origin a SFRIFFShdrChunk object.
  SFRIFFShdrChunk & operator=(SFRIFFShdrChunk && origin) = default;

  /// Destructs the SFRIFFShdrChunk.
  virtual ~SFRIFFShdrChunk() = default;

  /// @copydoc RIFFChunkInterface::name()
  virtual std::string name() const override {
    return "shdr";
  }

  /// Returns the samples of this chunk.
  /// @return the samples of this chunk.
  const std::vector<std::shared_ptr<SFSample>> & samples() const {
    return *samples_;
  }

  /// Sets the samples of this chunk.
  /// @param samples the samples of this chunk.
  /// @throws std::length_error Too many samples.
  void set_samples(const std::vector<std::shared_ptr<SFSample>> & samples) {
    samples_ = &samples;
    size_ = kItemSize * NumItems();
  }

  /// Returns the map containing the samples as keys and their indices as map values.
  /// @return the map containing the samples as keys and their indices as map values.
  const std::unordered_map<const SFSample *, uint16_t> & sample_index_map() const {
    return sample_index_map_;
  }

  /// Sets the map containing the samples as keys and their indices as map values.
  /// @param sample_index_map the map containing the samples as keys and their indices as map values.
  void set_sample_index_map(std::unordered_map<const SFSample *, uint16_t> sample_index_map) {
    sample_index_map_ = std::move(sample_index_map);
  }

  /// Returns the whole length of this chunk.
  /// @return the length of this chunk including a chunk header, in terms of bytes.
  virtual size_type size() const noexcept override {
    return 8 + size_;
  }

  /// Writes this chunk to the specified output stream.
  /// @param out the output stream.
  /// @throws std::length_error The chunk size exceeds the maximum.
  /// @throws std::out_of_range Sample has a link to an unknown sample.
  /// @throws std::ios_base::failure An I/O error occurred.
  virtual void Write(std::ostream & out) const override;

private:
  /// Returns the number of sample header items.
  /// @return the number of sample header items, including the terminator item.
  /// @throws std::length_error Too many samples.
  uint16_t NumItems() const;

  /// Writes an item of shdr chunk.
  /// @param out the output stream.
  /// @param name the name of sample.
  /// @param start the beginning index of the sample, in sample data points, inclusive.
  /// @param end the ending index of the sample, in sample data points, exclusive.
  /// @param start_loop the beginning index of the loop, in sample data points, inclusive.
  /// @param end_loop the ending index of the loop, in sample data points, exclusive.
  /// @param sample_rate the sample rate, in hertz.
  /// @param original_key the MIDI key number of the recorded pitch of the sample.
  /// @param correction the pitch correction that should be applied to the sample, in cents.
  /// @param link the associated right or left stereo sample. nullptr is allowed.
  /// @param type both the type of sample and the whether the sample is located in RAM or ROM memory.
  /// @return the output stream.
  /// @throws std::ios_base::failure An I/O error occurred.
  static std::ostream & WriteItem(std::ostream & out,
      const std::string & name, uint32_t start, uint32_t end,
      uint32_t start_loop, uint32_t end_loop, uint32_t sample_rate,
      uint8_t original_key, int8_t correction, uint16_t link, SFSampleLink type);

  /// The size of the chunk (excluding header).
  size_type size_;

  /// The samples of the chunk.
  const std::vector<std::shared_ptr<SFSample>> * samples_;

  /// The map containing the samples as keys and their indices as map values.
  std::unordered_map<const SFSample *, uint16_t> sample_index_map_;
};

} // namespace sf2cute

#endif // SF2CUTE_RIFF_SHDR_CHUNK_HPP_
