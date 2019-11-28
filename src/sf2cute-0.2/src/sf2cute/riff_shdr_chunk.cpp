/// @file
/// SoundFont "shdr" RIFF chunk implementation
///
/// @author gocha <https://github.com/gocha>

#include "riff_shdr_chunk.hpp"

#include <stdint.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <sstream>
#include <ostream>
#include <stdexcept>

#include <sf2cute/sample.hpp>

#include "byteio.hpp"

namespace sf2cute {

/// Constructs a new empty SFRIFFShdrChunk.
SFRIFFShdrChunk::SFRIFFShdrChunk() :
    size_(0),
    samples_(nullptr),
    sample_index_map_() {
}

/// Constructs a new SFRIFFShdrChunk using the specified samples.
SFRIFFShdrChunk::SFRIFFShdrChunk(const std::vector<std::shared_ptr<SFSample>> & samples,
      std::unordered_map<const SFSample *, uint16_t> sample_index_map) :
    samples_(&samples),
    sample_index_map_(std::move(sample_index_map)) {
  size_ = kItemSize * NumItems();
}

/// Writes this chunk to the specified output stream.
void SFRIFFShdrChunk::Write(std::ostream & out) const {
  // Save exception bits of output stream.
  const std::ios_base::iostate old_exception_bits = out.exceptions();
  // Set exception bits to get output error as an exception.
  out.exceptions(std::ios::badbit | std::ios::failbit);

  try {
    // Write the chunk header.
    RIFFChunk::WriteHeader(out, name(), size_);

    // Sample headers:
    size_t start_sample = 0;
    for (const auto & sample : samples()) {
      // Find the linked sample.
      uint16_t link_index = 0;
      if (sample->has_link()) {
        const auto & link = sample->link();
        if (sample_index_map().count(link.get()) != 0) {
          link_index = sample_index_map().at(link.get());
        }
        else {
          throw std::out_of_range("Sample has a link to an unknown sample.");
        }
      }

      // Calculate the sample indices.
      size_t end_sample = start_sample + sample->data().size();
      size_t start_loop = start_sample + sample->start_loop();
      size_t end_loop = start_sample + sample->end_loop();

      // Check the range of indices.
      if (start_sample > UINT32_MAX || end_sample > UINT32_MAX ||
          start_loop > UINT32_MAX || end_loop > UINT32_MAX) {
        throw std::length_error("Too many sample datapoints.");
      }

      // Write the sample header.
      WriteItem(out,
        sample->name(),
        uint32_t(start_sample),
        uint32_t(end_sample),
        uint32_t(start_loop),
        uint32_t(end_loop),
        sample->sample_rate(),
        sample->original_key(),
        sample->correction(),
        link_index,
        sample->type());

      // Calculate the next sample index.
      start_sample += sample->data().size() + SFSample::kTerminatorSampleLength;
    }

    // Write the last terminator item.
    WriteItem(out, "EOS", 0, 0, 0, 0, 0, 0, 0, 0, SFSampleLink(0));

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

/// Returns the number of sample header items.
uint16_t SFRIFFShdrChunk::NumItems() const {
  auto num_sample_items = samples().size() + 1;
  if (num_sample_items > UINT16_MAX) {
    throw std::length_error("Too many samples.");
  }
  return static_cast<uint16_t>(num_sample_items);
}

/// Writes an item of shdr chunk.
std::ostream & SFRIFFShdrChunk::WriteItem(std::ostream & out,
    const std::string & name, uint32_t start, uint32_t end,
    uint32_t start_loop, uint32_t end_loop, uint32_t sample_rate,
    uint8_t original_key, int8_t correction, uint16_t link, SFSampleLink type) {
  // struct sfSample:
  // char achSampleName[20];
  std::string sample_name(name.substr(0, SFSample::kMaxNameLength));
  out.write(sample_name.data(), sample_name.size());
  for (std::string::size_type i = 0; i < SFSample::kMaxNameLength + 1 - sample_name.size(); i++) {
    out.put(0);
  }

  // uint32_t dwStart;
  InsertInt32L(out, start);

  // uint32_t dwEnd;
  InsertInt32L(out, end);

  // uint32_t dwStartloop;
  InsertInt32L(out, start_loop);

  // uint32_t dwEndloop;
  InsertInt32L(out, end_loop);

  // uint32_t dwSampleRate;
  InsertInt32L(out, sample_rate);

  // uint8_t byOriginalKey;
  InsertInt8(out, original_key);

  // int8_t chCorrection;
  InsertInt8(out, correction);

  // uint16_t wSampleLink;
  InsertInt16L(out, link);

  // SFSampleLink sfSampleType;
  InsertInt16L(out, uint16_t(type));

  return out;
}

} // namespace sf2cute
