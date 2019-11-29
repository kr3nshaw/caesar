/// @file
/// SoundFont "smpl" RIFF chunk implementation
///
/// @author gocha <https://github.com/gocha>

#include "riff_smpl_chunk.hpp"

#include <stdint.h>
#include <memory>
#include <string>
#include <ostream>
#include <stdexcept>

#include <sf2cute/sample.hpp>

#include "byteio.hpp"

namespace sf2cute {

/// Constructs a new empty SFRIFFSmplChunk.
SFRIFFSmplChunk::SFRIFFSmplChunk() :
    size_(0),
    samples_(nullptr) {
}

/// Constructs a new SFRIFFSmplChunk using the specified samples.
SFRIFFSmplChunk::SFRIFFSmplChunk(
    const std::vector<std::shared_ptr<SFSample>> & samples) :
    samples_(&samples) {
  size_ = GetSamplePoolSize();
}

/// Writes this chunk to the specified output stream.
void SFRIFFSmplChunk::Write(std::ostream & out) const {
  // Save exception bits of output stream.
  const std::ios_base::iostate old_exception_bits = out.exceptions();
  // Set exception bits to get output error as an exception.
  out.exceptions(std::ios::badbit | std::ios::failbit);

  try {
    // Write the chunk header.
    RIFFChunk::WriteHeader(out, name(), size_);

    // Write the chunk data.
    for (const auto & sample : samples()) {
      // Write the samples.
      for (int16_t value : sample->data()) {
        InsertInt16L(out, value);
      }

      // Write terminator samples.
      for (uint32_t index = 0; index < SFSample::kTerminatorSampleLength; index++) {
        InsertInt16L(out, 0);
      }
    }

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

/// Returns the total sample pool size.
SFRIFFSmplChunk::size_type SFRIFFSmplChunk::GetSamplePoolSize() const {
  SFRIFFSmplChunk::size_type size = 0;
  for (const auto & sample : samples()) {
    size += sizeof(int16_t) *
        (sample->data().size() + SFSample::kTerminatorSampleLength);
    if (size > UINT32_MAX) {
      throw std::length_error("The sample pool size exceeds the maximum.");
    }
  }
  return size;
}

} // namespace sf2cute
