/// @file
/// SoundFont "smpl" RIFF chunk header
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_RIFF_SMPL_CHUNK_HPP_
#define SF2CUTE_RIFF_SMPL_CHUNK_HPP_

#include <memory>
#include <string>
#include <vector>
#include <ostream>

#include "riff.hpp"

namespace sf2cute {

class SFSample;

/// The SFRIFFSmplChunk class represents a SoundFont 2 "smpl" chunk.
class SFRIFFSmplChunk : public RIFFChunkInterface {
public:
  /// Unsigned integer type for the chunk size.
  using size_type = RIFFChunkInterface::size_type;

  /// Constructs a new empty SFRIFFSmplChunk.
  SFRIFFSmplChunk();

  /// Constructs a new SFRIFFSmplChunk using the specified samples.
  /// @param samples The samples of the chunk.
  /// @throws std::length_error The sample pool size exceeds the maximum.
  SFRIFFSmplChunk(const std::vector<std::shared_ptr<SFSample>> & samples);

  /// Constructs a new copy of specified SFRIFFSmplChunk.
  /// @param origin a SFRIFFSmplChunk object.
  SFRIFFSmplChunk(const SFRIFFSmplChunk & origin) = default;

  /// Copy-assigns a new value to the SFRIFFSmplChunk, replacing its current contents.
  /// @param origin a SFRIFFSmplChunk object.
  SFRIFFSmplChunk & operator=(const SFRIFFSmplChunk & origin) = default;

  /// Acquires the contents of specified SFRIFFSmplChunk.
  /// @param origin a SFRIFFSmplChunk object.
  SFRIFFSmplChunk(SFRIFFSmplChunk && origin) = default;

  /// Move-assigns a new value to the SFRIFFSmplChunk, replacing its current contents.
  /// @param origin a SFRIFFSmplChunk object.
  SFRIFFSmplChunk & operator=(SFRIFFSmplChunk && origin) = default;

  /// Destructs the SFRIFFSmplChunk.
  virtual ~SFRIFFSmplChunk() = default;

  /// @copydoc RIFFChunkInterface::name()
  virtual std::string name() const override {
    return "smpl";
  }

  /// Returns the samples of this chunk.
  /// @return the samples of this chunk.
  const std::vector<std::shared_ptr<SFSample>> & samples() const {
    return *samples_;
  }

  /// Sets the samples of this chunk.
  /// @param samples the samples of this chunk.
  /// @throws std::length_error The sample pool size exceeds the maximum.
  void set_samples(const std::vector<std::shared_ptr<SFSample>> & samples) {
    samples_ = &samples;
    size_ = GetSamplePoolSize();
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
  /// Returns the total sample pool size.
  /// @return the total sample pool size.
  /// @throws std::length_error The sample pool size exceeds the maximum.
  size_type GetSamplePoolSize() const;

  /// The size of the chunk (excluding header).
  size_type size_;

  /// The samples of the chunk.
  const std::vector<std::shared_ptr<SFSample>> * samples_;
};

} // namespace sf2cute

#endif // SF2CUTE_RIFF_SMPL_CHUNK_HPP_
