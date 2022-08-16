/// @file
/// SoundFont 2 File writer class header.
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_FILE_WRITER_HPP_
#define SF2CUTE_FILE_WRITER_HPP_

#include <algorithm>
#include <memory>
#include <string>
#include <ostream>

#include <sf2cute/types.hpp>
#include <sf2cute/modulator.hpp>

namespace sf2cute {

class SFSample;
class SFInstrumentZone;
class SFInstrument;
class SFPresetZone;
class SFPreset;
class SoundFont;

class RIFFChunkInterface;

/// The SoundFontWriter class represents a SoundFont writer.
class SoundFontWriter {
public:
  /// Constructs a new empty SoundFontWriter.
  SoundFontWriter();

  /// Constructs a new SoundFontWriter using specified file.
  /// @param file the input SoundFont object.
  SoundFontWriter(const SoundFont & file);

  /// Constructs a new copy of specified SoundFontWriter.
  /// @param origin a SoundFontWriter object.
  SoundFontWriter(const SoundFontWriter & origin) = default;

  /// Copy-assigns a new value to the SoundFontWriter, replacing its current contents.
  /// @param origin a SoundFontWriter object.
  SoundFontWriter & operator=(const SoundFontWriter & origin) = default;

  /// Acquires the contents of specified SoundFontWriter.
  /// @param origin a SoundFontWriter object.
  SoundFontWriter(SoundFontWriter && origin) = default;

  /// Move-assigns a new value to the SoundFontWriter, replacing its current contents.
  /// @param origin a SoundFontWriter object.
  SoundFontWriter & operator=(SoundFontWriter && origin) = default;

  /// Destructs the SoundFontWriter.
  ~SoundFontWriter() = default;

  /// Returns the input SoundFont object.
  /// @return the input SoundFont object.
  const SoundFont & file() const noexcept {
    return *file_;
  }

  /// Sets the input SoundFont object.
  /// @param file the input SoundFont object.
  void set_file(const SoundFont & file) {
    file_ = &file;
  }

  /// Writes the SoundFont to a file.
  /// @param filename the name of the file to write to.
  void Write(const std::string & filename);

  /// Writes the SoundFont to an output stream.
  /// @param out the output stream to write to.
  void Write(std::ostream & out);

  /// @copydoc SoundFontWriter::Write(std::ostream &)
  void Write(std::ostream && out);

private:
  /// Make an INFO chunk.
  /// @return the INFO chunk.
  std::unique_ptr<RIFFChunkInterface> MakeInfoListChunk();

  /// Make a sdta chunk.
  /// @return the sdta chunk.
  std::unique_ptr<RIFFChunkInterface> MakeSdtaListChunk();

  /// Make a pdta chunk.
  /// @return the pdta chunk.
  std::unique_ptr<RIFFChunkInterface> MakePdtaListChunk();

  /// Make a chunk with a version number.
  /// @param name the name of the chunk.
  /// @param version the version number.
  static std::unique_ptr<RIFFChunkInterface> MakeVersionChunk(std::string name, SFVersionTag version);

  /// Make a chunk with a string.
  /// @param name the name of the chunk.
  /// @param data the data string.
  static std::unique_ptr<RIFFChunkInterface> MakeZSTRChunk(std::string name, std::string data);

  /// The input SoundFont object.
  const SoundFont * file_;
};

} // namespace sf2cute

#endif // SF2CUTE_FILE_WRITER_HPP_
