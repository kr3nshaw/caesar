/// @file
/// RIFF (Resource Interchange File Format) library implementation
///
/// The RIFF library defines small classes for writing a RIFF file.
///
/// @author gocha <https://github.com/gocha>

#include "riff.hpp"

#include <stdint.h>
#include <memory>
#include <string>
#include <vector>
#include <ostream>
#include <sstream>
#include <stdexcept>

#include "byteio.hpp"

namespace sf2cute {

/// Constructs a new empty RIFFChunk.
RIFFChunk::RIFFChunk() :
    name_("    ") {
}

/// Constructs a new empty RIFFChunk using the specified name.
RIFFChunk::RIFFChunk(std::string name) {
  set_name(std::move(name));
}

/// Constructs a new RIFFChunk using the specified name and data.
RIFFChunk::RIFFChunk(std::string name, std::vector<char> data) {
  set_name(std::move(name));
  set_data(std::move(data));
}

/// Sets the name of this chunk.
void RIFFChunk::set_name(std::string name) {
  // Throw exception if the length of chunk name is not 4.
  if (name.size() != 4) {
    std::ostringstream message_builder;
    message_builder << "Invalid RIFF chunk name \"" << name << "\".";
    throw std::invalid_argument(message_builder.str());
  }

  // Set the name.
  name_ = std::move(name);
}

/// Writes this chunk to the specified output stream.
void RIFFChunk::Write(std::ostream & out) const {
  // Save exception bits of output stream.
  const std::ios_base::iostate old_exception_bits = out.exceptions();
  // Set exception bits to get output error as an exception.
  out.exceptions(std::ios::badbit | std::ios::failbit);

  try {
    // Write the chunk header.
    WriteHeader(out, name(), data_.size());

    // Write the chunk data.
    out.write(reinterpret_cast<const char *>(data_.data()), data_.size());

    // Write a padding byte if necessary.
    if (data_.size() % 2 != 0) {
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

/// Writes a chunk header to the specified output stream.
void RIFFChunk::WriteHeader(std::ostream & out,
    const std::string & name,
    size_type size) {
  // Throw exception if the chunk size exceeds the maximum.
  if (size > UINT32_MAX) {
    std::ostringstream message_builder;
    message_builder << "RIFF chunk \"" << name << "\" size too large.";
    throw std::length_error(message_builder.str());
  }

  // Save exception bits of output stream.
  const std::ios_base::iostate old_exception_bits = out.exceptions();
  // Set exception bits to get output error as an exception.
  out.exceptions(std::ios::badbit | std::ios::failbit);

  try {
    // Write the chunk name.
    out.write(name.data(), name.size());

    // Write the chunk size.
    InsertInt32L(out, static_cast<uint32_t>(size));
  }
  catch (const std::exception &) {
    // Recover exception bits of output stream.
    out.exceptions(old_exception_bits);

    // Rethrow the exception.
    throw;
  }
}

/// Constructs a new empty RIFFListChunk.
RIFFListChunk::RIFFListChunk() :
    name_("    ") {
}

/// Constructs a new empty RIFFListChunk using the specified list type.
RIFFListChunk::RIFFListChunk(std::string name) {
  set_name(std::move(name));
}

/// Sets the form type of this chunk.
void RIFFListChunk::set_name(std::string name) {
  // Throw exception if the length of chunk name is not 4.
  if (name.size() != 4) {
    std::ostringstream message_builder;
    message_builder << "Invalid RIFF chunk name \"" << name << "\".";
    throw std::invalid_argument(message_builder.str());
  }

  // Set the name.
  name_ = std::move(name);
}

/// Appends the specified RIFFChunkInterface to this chunk.
void RIFFListChunk::AddSubchunk(std::unique_ptr<RIFFChunkInterface> && subchunk) {
  subchunks_.push_back(std::move(subchunk));
}

/// Removes all subchunks from this chunk.
void RIFFListChunk::ClearSubchunks() {
  subchunks_.clear();
}

/// Writes this chunk to the specified output stream.
void RIFFListChunk::Write(std::ostream & out) const {
  // Write the chunk header.
  WriteHeader(out, name(), size() - 8);

  // Write each subchunks.
  for (const auto & subchunk : subchunks_) {
    subchunk->Write(out);
  }
}

/// Writes a "LIST" chunk header to the specified output stream.
void RIFFListChunk::WriteHeader(std::ostream & out,
    const std::string & name,
    size_type size) {
  // Throw exception if the chunk size exceeds the maximum.
  if (size > UINT32_MAX) {
    std::ostringstream message_builder;
    message_builder << "RIFF chunk \"" << name << "\" size too large.";
    throw std::length_error(message_builder.str());
  }

  // Save exception bits of output stream.
  const std::ios_base::iostate old_exception_bits = out.exceptions();
  // Set exception bits to get output error as an exception.
  out.exceptions(std::ios::badbit | std::ios::failbit);

  try {
    // Write the chunk ID "LIST".
    out.write("LIST", 4);

    // Write the chunk size.
    InsertInt32L(out, static_cast<uint32_t>(size));

    // Write the list type.
    out.write(name.data(), name.size());
  }
  catch (const std::exception &) {
    // Recover exception bits of output stream.
    out.exceptions(old_exception_bits);

    // Rethrow the exception.
    throw;
  }
}

/// Constructs a new empty RIFF.
RIFF::RIFF() :
    name_("    ") {
}

/// Constructs a new empty RIFF using the specified form type.
RIFF::RIFF(std::string name) {
  set_name(std::move(name));
}

/// Sets the form type of this chunk.
void RIFF::set_name(std::string name) {
  // Throw exception if the length of form type is not 4.
  if (name.size() != 4) {
    std::ostringstream message_builder;
    message_builder << "Invalid RIFF form type \"" << name << "\".";
    throw std::invalid_argument(message_builder.str());
  }

  // Set the name.
  name_ = std::move(name);
}

/// Writes this RIFF to the specified output stream.
void RIFF::Write(std::ostream & out) const {
  // Save exception bits of output stream.
  const std::ios_base::iostate old_exception_bits = out.exceptions();
  // Set exception bits to get output error as an exception.
  out.exceptions(std::ios::badbit | std::ios::failbit);

  try {
    // Write the RIFF header.
    WriteHeader(out, name(), size() - 8);

    // Write each chunks.
    for (const auto & chunk : chunks_) {
      chunk->Write(out);
    }
  }
  catch (const std::exception &) {
    // Recover exception bits of output stream.
    out.exceptions(old_exception_bits);

    // Rethrow the exception.
    throw;
  }
}

/// Writes a "RIFF" chunk header to the specified output stream.
void RIFF::WriteHeader(std::ostream & out,
    const std::string & name,
    size_type size) {
  // Throw exception if the RIFF file size exceeds the maximum.
  if (size > UINT32_MAX) {
    throw std::length_error("RIFF file size too large.");
  }

  // Save exception bits of output stream.
  const std::ios_base::iostate old_exception_bits = out.exceptions();
  // Set exception bits to get output error as an exception.
  out.exceptions(std::ios::badbit | std::ios::failbit);

  try {
    // Write the ID "RIFF".
    out.write("RIFF", 4);

    // Write the file size.
    InsertInt32L(out, static_cast<uint32_t>(size));

    // Write the form type.
    out.write(name.data(), name.size());
  }
  catch (const std::exception &) {
    // Recover exception bits of output stream.
    out.exceptions(old_exception_bits);

    // Rethrow the exception.
    throw;
  }
}

} // namespace sf2cute
