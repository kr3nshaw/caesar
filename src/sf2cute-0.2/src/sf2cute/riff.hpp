/// @file
/// RIFF (Resource Interchange File Format) library header
///
/// The RIFF library defines small classes for writing a RIFF file.
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_RIFF_HPP_
#define SF2CUTE_RIFF_HPP_

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <ostream>

namespace sf2cute {

/// The RIFFChunkInterface class represents an interface of RIFF chunk.
class RIFFChunkInterface {
public:
  /// Unsigned integer type for the chunk size.
  using size_type = std::vector<char>::size_type;

  /// Destructs the RIFFChunkInterface.
  virtual ~RIFFChunkInterface() = default;

  /// Returns the name of this chunk.
  /// @return the name of this chunk.
  virtual std::string name() const = 0;

  /// Returns the whole length of this chunk.
  /// @return the length of this chunk including a chunk header, in terms of bytes.
  virtual size_type size() const noexcept = 0;

  /// Writes this chunk to the specified output stream.
  /// @param out the output stream.
  /// @throws std::length_error The chunk size exceeds the maximum.
  /// @throws std::ios_base::failure An I/O error occurred.
  virtual void Write(std::ostream & out) const = 0;
};

/// The RIFFChunk class represents a RIFF chunk.
class RIFFChunk : public RIFFChunkInterface {
public:
  /// Unsigned integer type for the chunk size.
  using size_type = RIFFChunkInterface::size_type;

  /// Constructs a new empty RIFFChunk.
  RIFFChunk();

  /// Constructs a new empty RIFFChunk using the specified name.
  /// @param name The name of the chunk (FourCC).
  /// @throws std::invalid_argument The given name is not a valid FourCC.
  explicit RIFFChunk(std::string name);

  /// Constructs a new RIFFChunk using the specified name and data.
  /// @param name The name of the chunk (FourCC).
  /// @param data The data of the chunk.
  /// @throws std::invalid_argument The given name is not a valid FourCC.
  RIFFChunk(std::string name, std::vector<char> data);

  /// Constructs a new copy of specified RIFFChunk.
  /// @param origin a RIFFChunk object.
  RIFFChunk(const RIFFChunk & origin) = default;

  /// Copy-assigns a new value to the RIFFChunk, replacing its current contents.
  /// @param origin a RIFFChunk object.
  RIFFChunk & operator=(const RIFFChunk & origin) = default;

  /// Acquires the contents of specified RIFFChunk.
  /// @param origin a RIFFChunk object.
  RIFFChunk(RIFFChunk && origin) = default;

  /// Move-assigns a new value to the RIFFChunk, replacing its current contents.
  /// @param origin a RIFFChunk object.
  RIFFChunk & operator=(RIFFChunk && origin) = default;

  /// Destructs the RIFFChunk.
  virtual ~RIFFChunk() = default;

  /// @copydoc RIFFChunkInterface::name()
  virtual std::string name() const override {
    return name_;
  }

  /// Sets the name of this chunk.
  /// @param name the name of this chunk (FourCC).
  /// @throws std::invalid_argument The given name is not a valid FourCC.
  void set_name(std::string name);

  /// Returns the data of this chunk.
  /// @return the data of this chunk.
  const std::vector<char> & data() const {
    return data_;
  }

  /// Sets the data of this chunk.
  /// @param data the data of this chunk.
  void set_data(std::vector<char> data) {
    data_ = std::move(data);
  }

  /// Returns the whole length of this chunk.
  /// @return the length of this chunk including a chunk header, in terms of bytes.
  virtual size_type size() const noexcept override {
    size_type chunk_size = data_.size();
    if (chunk_size % 2 != 0) {
      chunk_size++;
    }
    return 8 + chunk_size;
  }

  /// Writes this chunk to the specified output stream.
  /// @param out the output stream.
  /// @throws std::length_error The chunk size exceeds the maximum.
  /// @throws std::ios_base::failure An I/O error occurred.
  virtual void Write(std::ostream & out) const override;

  /// Writes a chunk header to the specified output stream.
  /// @param out the output stream.
  /// @param name the name of the chunk (FourCC).
  /// @param size the length of the chunk data, in terms of bytes.
  /// @throws std::length_error The chunk size exceeds the maximum.
  /// @throws std::ios_base::failure An I/O error occurred.
  static void WriteHeader(std::ostream & out,
      const std::string & name,
      size_type size);

private:
  /// The name of the chunk.
  std::string name_;

  /// The data of the chunk.
  std::vector<char> data_;
};

/// The RIFFListChunk class represents a RIFF "LIST" chunk that can have one or more RIFFChunk objects.
class RIFFListChunk : public RIFFChunkInterface {
public:
  /// Unsigned integer type for the chunk size.
  using size_type = RIFFChunkInterface::size_type;

  /// Constructs a new empty RIFFListChunk.
  RIFFListChunk();

  /// Constructs a new empty RIFFListChunk using the specified list type.
  /// @param name The list type of the chunk (FourCC).
  /// @throws std::invalid_argument The given name is not a valid FourCC.
  explicit RIFFListChunk(std::string name);

  /// Constructs a new copy of specified RIFFListChunk.
  /// @param origin a RIFFListChunk object.
  RIFFListChunk(const RIFFListChunk & origin) = default;

  /// Copy-assigns a new value to the RIFFListChunk, replacing its current contents.
  /// @param origin a RIFFListChunk object.
  RIFFListChunk & operator=(const RIFFListChunk & origin) = default;

  /// Acquires the contents of specified RIFFListChunk.
  /// @param origin a RIFFListChunk object.
  RIFFListChunk(RIFFListChunk && origin) = default;

  /// Move-assigns a new value to the RIFFListChunk, replacing its current contents.
  /// @param origin a RIFFListChunk object.
  RIFFListChunk & operator=(RIFFListChunk && origin) = default;

  /// Destructs the RIFFListChunk.
  virtual ~RIFFListChunk() = default;

  /// @copydoc RIFFChunkInterface::name()
  /// Returns the name of this chunk.
  /// @return the name of this chunk.
  virtual std::string name() const override {
    return name_;
  }

  /// Sets the list type of this chunk.
  /// @param name the list type of this chunk (FourCC).
  /// @throws std::invalid_argument The given name is not a valid FourCC.
  void set_name(std::string name);

  /// Returns the subchunks of this chunk.
  /// @return a collection of pointers to each RIFFChunkInterface objects.
  const std::vector<std::unique_ptr<RIFFChunkInterface>> & subchunks() const {
    return subchunks_;
  }

  /// Appends the specified RIFFChunkInterface to this chunk.
  /// @param subchunk a pointer to RIFFChunkInterface object.
  void AddSubchunk(std::unique_ptr<RIFFChunkInterface> && subchunk);

  /// Removes all subchunks from this chunk.
  void ClearSubchunks();

  /// Returns the whole length of this chunk.
  /// @return the length of this chunk including a chunk header, in terms of bytes.
  virtual size_type size() const noexcept override {
    size_type chunk_size = 0;
    for (const auto & subchunk : subchunks_) {
      chunk_size += subchunk->size();
    }
    return 12 + chunk_size;
  }

  /// Writes this chunk to the specified output stream.
  /// @param out the output stream.
  /// @throws std::length_error The chunk size exceeds the maximum.
  /// @throws std::ios_base::failure An I/O error occurred.
  virtual void Write(std::ostream & out) const override;

  /// Writes a "LIST" chunk header to the specified output stream.
  /// @param out the output stream.
  /// @param name the list type of the chunk (FourCC).
  /// @param size the length of the chunk data, in terms of bytes.
  /// @throws std::length_error The chunk size exceeds the maximum.
  /// @throws std::ios_base::failure An I/O error occurred.
  static void WriteHeader(std::ostream & out,
      const std::string & name,
      size_type size);

private:
  /// The name of the chunk.
  std::string name_;

  /// A collection of pointers to each RIFFChunkInterface objects.
  std::vector<std::unique_ptr<RIFFChunkInterface>> subchunks_;
};

/// The RIFF class represents a RIFF file.
class RIFF {
public:
  /// Unsigned integer type for the chunk size.
  using size_type = RIFFChunkInterface::size_type;

  /// Constructs a new empty RIFF.
  RIFF();

  /// Constructs a new empty RIFF using the specified form type.
  /// @param name The form type of the chunk (FourCC).
  /// @throws std::invalid_argument The given name is not a valid FourCC.
  explicit RIFF(std::string name);

  /// Constructs a new copy of specified RIFF.
  /// @param origin a RIFF object.
  RIFF(const RIFF & origin) = default;

  /// Copy-assigns a new value to the RIFF, replacing its current contents.
  /// @param origin a RIFF object.
  RIFF & operator=(const RIFF & origin) = default;

  /// Acquires the contents of specified RIFF.
  /// @param origin a RIFF object.
  RIFF(RIFF && origin) = default;

  /// Move-assigns a new value to the RIFF, replacing its current contents.
  /// @param origin a RIFF object.
  RIFF & operator=(RIFF && origin) = default;

  /// Destructs the RIFF.
  virtual ~RIFF() = default;

  /// Returns the form type of this chunk.
  /// @return the form type of this chunk.
  std::string name() const {
    return name_;
  }

  /// Sets the form type of this chunk.
  /// @param name the form type of this chunk (FourCC).
  /// @throws std::invalid_argument The given name is not a valid FourCC.
  void set_name(std::string name);

  /// Returns the chunks of this RIFF.
  /// @return a collection of pointers to each RIFFChunkInterface objects.
  const std::vector<std::unique_ptr<RIFFChunkInterface>> & chunks() const {
    return chunks_;
  }

  /// Appends the specified RIFFChunkInterface to this RIFF.
  /// @param chunk a pointer to RIFFChunkInterface object.
  void AddChunk(std::unique_ptr<RIFFChunkInterface> && chunk) {
    chunks_.push_back(std::move(chunk));
  }

  /// Removes all chunks from this RIFF.
  void ClearChunks() {
    chunks_.clear();
  }

  /// Returns the whole length of this RIFF.
  /// @return the length of this RIFF including a chunk header, in terms of bytes.
  size_type size() const noexcept {
    size_type chunk_size = 0;
    for (const auto & chunk : chunks_) {
      chunk_size += chunk->size();
    }
    return 12 + chunk_size;
  }

  /// Writes this RIFF to the specified output stream.
  /// @param out the output stream.
  /// @throws std::length_error The chunk size exceeds the maximum.
  /// @throws std::ios_base::failure An I/O error occurred.
  void Write(std::ostream & out) const;

  /// Writes a "RIFF" chunk header to the specified output stream.
  /// @param out the output stream.
  /// @param name the form type of the chunk (FourCC).
  /// @param size the length of the chunk data, in terms of bytes.
  /// @throws std::length_error The chunk size exceeds the maximum.
  /// @throws std::ios_base::failure An I/O error occurred.
  static void WriteHeader(std::ostream & out,
      const std::string & name,
      size_type size);

private:
  /// The form type of the chunk.
  std::string name_;

  /// A collection of pointers to each RIFFChunkInterface objects.
  std::vector<std::unique_ptr<RIFFChunkInterface>> chunks_;
};

} // namespace sf2cute

#endif // SF2CUTE_RIFF_HPP_
