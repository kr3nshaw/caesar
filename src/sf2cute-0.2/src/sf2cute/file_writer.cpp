/// @file
/// SoundFont 2 File writer class implementation.
///
/// @author gocha <https://github.com/gocha>

#include "file_writer.hpp"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <fstream>
#include <stdexcept>

#include <sf2cute/file.hpp>

#include "byteio.hpp"
#include "riff.hpp"
#include "riff_smpl_chunk.hpp"
#include "riff_phdr_chunk.hpp"
#include "riff_pbag_chunk.hpp"
#include "riff_pmod_chunk.hpp"
#include "riff_pgen_chunk.hpp"
#include "riff_inst_chunk.hpp"
#include "riff_ibag_chunk.hpp"
#include "riff_imod_chunk.hpp"
#include "riff_igen_chunk.hpp"
#include "riff_shdr_chunk.hpp"

namespace sf2cute {

/// Constructs a new empty SoundFontWriter.
SoundFontWriter::SoundFontWriter() :
    file_(nullptr) {
}

/// Constructs a new SoundFontWriter using specified file.
SoundFontWriter::SoundFontWriter(const SoundFont & file) :
    file_(&file) {
}

/// Writes the SoundFont to a file.
void SoundFontWriter::Write(const std::string & filename) {
  std::ofstream out;

  out.exceptions(std::ios::badbit | std::ios::failbit);
  out.open(filename, std::ios::binary);

  Write(out);
}

/// Writes the SoundFont to an output stream.
void SoundFontWriter::Write(std::ostream & out) {
  RIFF riff("sfbk");
  riff.AddChunk(MakeInfoListChunk());
  riff.AddChunk(MakeSdtaListChunk());
  riff.AddChunk(MakePdtaListChunk());
  riff.Write(out);
}

/// Writes the SoundFont to an output stream.
void SoundFontWriter::Write(std::ostream && out) {
  Write(out);
}

/// Make an INFO chunk.
std::unique_ptr<RIFFChunkInterface> SoundFontWriter::MakeInfoListChunk() {
  std::unique_ptr<RIFFListChunk> info = std::make_unique<RIFFListChunk>("INFO");

  // Mandatory chunks:

  info->AddSubchunk(MakeVersionChunk("ifil", SFVersionTag(2, 1)));

  info->AddSubchunk(MakeZSTRChunk("isng", file().sound_engine().substr(0, SoundFont::kInfoTextMaxLength)));

  info->AddSubchunk(MakeZSTRChunk("INAM", file().bank_name().substr(0, SoundFont::kInfoTextMaxLength)));

  // Optional chunks:

  if (file().has_rom_name()) {
    info->AddSubchunk(MakeZSTRChunk("irom", file().rom_name().substr(0, SoundFont::kInfoTextMaxLength)));
  }

  if (file().has_rom_version()) {
    info->AddSubchunk(MakeVersionChunk("iver", file().rom_version()));
  }

  if (file().has_creation_date()) {
    info->AddSubchunk(MakeZSTRChunk("ICRD", file().creation_date().substr(0, SoundFont::kInfoTextMaxLength)));
  }

  if (file().has_engineers()) {
    info->AddSubchunk(MakeZSTRChunk("IENG", file().engineers().substr(0, SoundFont::kInfoTextMaxLength)));
  }

  if (file().has_product()) {
    info->AddSubchunk(MakeZSTRChunk("IPRD", file().product().substr(0, SoundFont::kInfoTextMaxLength)));
  }

  if (file().has_copyright()) {
    info->AddSubchunk(MakeZSTRChunk("ICOP", file().copyright().substr(0, SoundFont::kInfoTextMaxLength)));
  }

  if (file().has_comment()) {
    info->AddSubchunk(MakeZSTRChunk("ICMT", file().comment().substr(0, SoundFont::kInfoTextMaxLength)));
  }

  if (file().has_software()) {
    info->AddSubchunk(MakeZSTRChunk("ISFT", file().software().substr(0, SoundFont::kInfoTextMaxLength)));
  }

  return std::move(info);
}

/// Make a sdta chunk.
std::unique_ptr<RIFFChunkInterface> SoundFontWriter::MakeSdtaListChunk() {
  std::unique_ptr<RIFFListChunk> sdta = std::make_unique<RIFFListChunk>("sdta");
  sdta->AddSubchunk(std::make_unique<SFRIFFSmplChunk>(file().samples()));
  return std::move(sdta);
}

/// Make a pdta chunk.
std::unique_ptr<RIFFChunkInterface> SoundFontWriter::MakePdtaListChunk() {
  // Constructs a map for indexing each instruments.
  std::unordered_map<const SFInstrument *, uint16_t> instrument_index_map;
  for (uint16_t index = 0; index < file().instruments().size(); index++) {
    instrument_index_map.insert(std::make_pair(file().instruments().at(index).get(), index));
  }

  // Constructs a map for indexing each samples.
  std::unordered_map<const SFSample *, uint16_t> sample_index_map;
  for (uint16_t index = 0; index < file().samples().size(); index++) {
    sample_index_map.insert(std::make_pair(file().samples().at(index).get(), index));
  }

  // Constructs the pdta chunk and its subchunks.
  std::unique_ptr<RIFFListChunk> pdta = std::make_unique<RIFFListChunk>("pdta");
  pdta->AddSubchunk(std::make_unique<SFRIFFPhdrChunk>(file().presets()));
  pdta->AddSubchunk(std::make_unique<SFRIFFPbagChunk>(file().presets()));
  pdta->AddSubchunk(std::make_unique<SFRIFFPmodChunk>(file().presets()));
  pdta->AddSubchunk(std::make_unique<SFRIFFPgenChunk>(file().presets(), instrument_index_map));
  pdta->AddSubchunk(std::make_unique<SFRIFFInstChunk>(file().instruments()));
  pdta->AddSubchunk(std::make_unique<SFRIFFIbagChunk>(file().instruments()));
  pdta->AddSubchunk(std::make_unique<SFRIFFImodChunk>(file().instruments()));
  pdta->AddSubchunk(std::make_unique<SFRIFFIgenChunk>(file().instruments(), sample_index_map));
  pdta->AddSubchunk(std::make_unique<SFRIFFShdrChunk>(file().samples(), sample_index_map));
  return std::move(pdta);
}

/// Make a chunk with a version number.
std::unique_ptr<RIFFChunkInterface> SoundFontWriter::MakeVersionChunk(std::string name, SFVersionTag version) {
  std::vector<char> data(4);
  WriteInt16L(&data[0], version.major_version);
  WriteInt16L(&data[2], version.minor_version);
  return std::make_unique<RIFFChunk>(std::move(name), std::move(data));
}

/// Make a chunk with a string.
std::unique_ptr<RIFFChunkInterface> SoundFontWriter::MakeZSTRChunk(std::string name, std::string data) {
  std::vector<char> zstr((data.size() + 1 + 1) & ~1);
  std::copy(data.begin(), data.end(), zstr.begin());
  std::fill(std::next(zstr.begin(), data.size()), zstr.end(), 0);
  return std::make_unique<RIFFChunk>(std::move(name), std::move(zstr));
}

} // namespace sf2cute
