/// @file
/// SoundFont 2 File class implementation.
///
/// @author gocha <https://github.com/gocha>

#include <sf2cute/file.hpp>

#include <iterator>
#include <algorithm>
#include <stdexcept>
#include <unordered_map>
#include <fstream>

#include <sf2cute/sample.hpp>
#include <sf2cute/generator_item.hpp>
#include <sf2cute/modulator_item.hpp>
#include <sf2cute/instrument_zone.hpp>
#include <sf2cute/instrument.hpp>
#include <sf2cute/preset_zone.hpp>
#include <sf2cute/preset.hpp>

#include "file_writer.hpp"

namespace sf2cute {

/// Constructs a new empty SoundFont.
SoundFont::SoundFont() :
    sound_engine_(kDefaultTargetSoundEngine),
    bank_name_(kDefaultBankName),
    has_rom_version_(false) {
}

/// Constructs a new copy of specified SoundFont.
SoundFont::SoundFont(const SoundFont & origin) :
    presets_(),
    instruments_(),
    samples_(),
    sound_engine_(origin.sound_engine_),
    bank_name_(origin.bank_name_),
    rom_name_(origin.rom_name_),
    rom_version_(origin.rom_version_),
    has_rom_version_(origin.has_rom_version_),
    creation_date_(origin.creation_date_),
    engineers_(origin.engineers_),
    product_(origin.product_),
    copyright_(origin.copyright_),
    comment_(origin.comment_),
    software_(origin.software_) {
  // Copy presets.
  presets_.reserve(origin.presets().size());
  for (const auto & preset : origin.presets()) {
    presets_.push_back(std::make_shared<SFPreset>(*preset));
  }

  // Copy instruments.
  instruments_.reserve(origin.instruments().size());
  for (const auto & instrument : origin.instruments()) {
    instruments_.push_back(std::make_shared<SFInstrument>(*instrument));
  }

  // Copy samples.
  samples_.reserve(origin.samples().size());
  for (const auto & sample : origin.samples()) {
    samples_.push_back(std::make_shared<SFSample>(*sample));
  }

  // Repair references.
  SetBackwardReferences();
  RepairReferences(origin);
}

/// Copy-assigns a new value to the SoundFont, replacing its current contents.
SoundFont & SoundFont::operator=(const SoundFont & origin) {
  // Copy presets.
  presets_.clear();
  presets_.reserve(origin.presets().size());
  for (const auto & preset : origin.presets()) {
    presets_.push_back(std::make_shared<SFPreset>(*preset));
  }

  // Copy instruments.
  instruments_.clear();
  instruments_.reserve(origin.instruments().size());
  for (const auto & instrument : origin.instruments()) {
    instruments_.push_back(std::make_shared<SFInstrument>(*instrument));
  }

  // Copy samples.
  samples_.clear();
  samples_.reserve(origin.samples().size());
  for (const auto & sample : origin.samples()) {
    samples_.push_back(std::make_shared<SFSample>(*sample));
  }

  // Copy other fields.
  sound_engine_ = origin.sound_engine_;
  bank_name_ = origin.bank_name_;
  rom_name_ = origin.rom_name_;
  rom_version_ = origin.rom_version_;
  has_rom_version_ = origin.has_rom_version_;
  creation_date_ = origin.creation_date_;
  engineers_ = origin.engineers_;
  product_ = origin.product_;
  copyright_ = origin.copyright_;
  comment_ = origin.comment_;
  software_ = origin.software_;

  // Repair references.
  SetBackwardReferences();
  RepairReferences(origin);

  return *this;
}

/// Acquires the contents of specified SoundFont.
SoundFont::SoundFont(SoundFont && origin) noexcept :
    presets_(std::move(origin.presets_)),
    instruments_(std::move(origin.instruments_)),
    samples_(std::move(origin.samples_)),
    sound_engine_(std::move(origin.sound_engine_)),
    bank_name_(std::move(origin.bank_name_)),
    rom_name_(std::move(origin.rom_name_)),
    rom_version_(std::move(origin.rom_version_)),
    has_rom_version_(std::move(origin.has_rom_version_)),
    creation_date_(std::move(origin.creation_date_)),
    engineers_(std::move(origin.engineers_)),
    product_(std::move(origin.product_)),
    copyright_(std::move(origin.copyright_)),
    comment_(std::move(origin.comment_)),
    software_(std::move(origin.software_)) {
  // Repair references.
  SetBackwardReferences();
}

/// Move-assigns a new value to the SoundFont, replacing its current contents.
SoundFont & SoundFont::operator=(SoundFont && origin) noexcept {
  // Copy fields.
  presets_ = std::move(origin.presets_);
  instruments_ = std::move(origin.instruments_);
  samples_ = std::move(origin.samples_);
  sound_engine_ = std::move(origin.sound_engine_);
  bank_name_ = std::move(origin.bank_name_);
  rom_name_ = std::move(origin.rom_name_);
  rom_version_ = std::move(origin.rom_version_);
  has_rom_version_ = std::move(origin.has_rom_version_);
  creation_date_ = std::move(origin.creation_date_);
  engineers_ = std::move(origin.engineers_);
  product_ = std::move(origin.product_);
  copyright_ = std::move(origin.copyright_);
  comment_ = std::move(origin.comment_);
  software_ = std::move(origin.software_);

  // Repair references.
  SetBackwardReferences();

  return *this;
}

/// Adds a preset to the SoundFont.
void SoundFont::AddPreset(std::shared_ptr<SFPreset> preset) {
  // Do nothing if nullptr specified.
  if (!preset) {
    return;
  }

  // Check the parent file of the preset.
  if (preset->has_parent_file()) {
    if (&preset->parent_file() == this) {
      // If the preset is already be owned by this file, do nothing.
      return;
    }
    else {
      // Otherwise raise an error. A preset cannot be shared by two files.
      throw std::invalid_argument("Preset has already been owned by another file.");
    }
  }

  // Set this file to the parent file of the preset.
  preset->set_parent_file(*this);

  // Add the preset to the list.
  presets_.push_back(preset);

  // If the preset has orphan instruments, add them to the file.
  for (auto && preset_zone : preset->zones()) {
    if (preset_zone->has_instrument()) {
      AddInstrument(preset_zone->instrument());
    }
  }
}

/// Removes a preset from the SoundFont.
void SoundFont::RemovePreset(
    std::vector<std::shared_ptr<SFPreset>>::const_iterator position) {
  const std::shared_ptr<SFPreset> & preset = *position;
  preset->reset_parent_file();
  presets_.erase(position);
}

/// Removes presets from the SoundFont.
void SoundFont::RemovePreset(
    std::vector<std::shared_ptr<SFPreset>>::const_iterator first,
  std::vector<std::shared_ptr<SFPreset>>::const_iterator last) {
  for (auto position = first; position != last; ++position) {
    const auto & preset = *position;
    preset->reset_parent_file();
  }
  presets_.erase(first, last);
}

/// Removes presets from the SoundFont.
void SoundFont::RemovePresetIf(
    std::function<bool(const std::shared_ptr<SFPreset> &)> predicate) {
  presets_.erase(std::remove_if(presets_.begin(), presets_.end(),
    [&predicate](const std::shared_ptr<SFPreset> & preset) -> bool {
      if (predicate(preset)) {
        preset->reset_parent_file();
        return true;
      }
      else {
        return false;
      }
    }), presets_.end());
}

/// Removes all of the presets.
void SoundFont::ClearPresets() noexcept {
  for (const auto & preset : presets_) {
    preset->reset_parent_file();
  }
  presets_.clear();
}

/// Adds an instrument to the SoundFont.
void SoundFont::AddInstrument(std::shared_ptr<SFInstrument> instrument) {
  // Do nothing if nullptr specified.
  if (!instrument) {
    return;
  }

  // Check the parent file of the instrument.
  if (instrument->has_parent_file()) {
    if (&instrument->parent_file() == this) {
      // If the instrument is already be owned by this file, do nothing.
      return;
    }
    else {
      // Otherwise raise an error. An instrument cannot be shared by two files.
      throw std::invalid_argument("Instrument has already been owned by another file.");
    }
  }

  // Set this file to the parent file of the instrument.
  instrument->set_parent_file(*this);

  // Add the instrument to the list.
  instruments_.push_back(instrument);

  // If the instrument has orphan samples, add them to the file.
  for (auto && instrument_zone : instrument->zones()) {
    if (instrument_zone->has_sample()) {
      AddSample(instrument_zone->sample());
    }
  }
}

/// Removes an instrument from the SoundFont.
void SoundFont::RemoveInstrument(
    std::vector<std::shared_ptr<SFInstrument>>::const_iterator position) {
  const std::shared_ptr<SFInstrument> & instrument = *position;
  instrument->reset_parent_file();
  instruments_.erase(position);
}

/// Removes an instrument from the SoundFont.
void SoundFont::RemoveInstrument(
    std::vector<std::shared_ptr<SFInstrument>>::const_iterator first,
    std::vector<std::shared_ptr<SFInstrument>>::const_iterator last) {
  for (auto position = first; position != last; ++position) {
    const auto & instrument = *position;
    instrument->reset_parent_file();
  }
  instruments_.erase(first, last);
}

/// Removes an instrument from the SoundFont.
void SoundFont::RemoveInstrumentIf(
    std::function<bool(const std::shared_ptr<SFInstrument> &)> predicate) {
  instruments_.erase(std::remove_if(instruments_.begin(), instruments_.end(),
    [&predicate](const std::shared_ptr<SFInstrument> & instrument) -> bool {
      if (predicate(instrument)) {
        instrument->reset_parent_file();
        return true;
      }
      else {
        return false;
      }
    }), instruments_.end());
}

/// Removes all of the instruments.
void SoundFont::ClearInstruments() noexcept {
  for (const auto & instrument : instruments_) {
    instrument->reset_parent_file();
  }
  instruments_.clear();
}

/// Adds a sample to the SoundFont.
void SoundFont::AddSample(std::shared_ptr<SFSample> sample) {
  // Do nothing if nullptr specified.
  if (!sample) {
    return;
  }

  // Check the parent file of the sample.
  if (sample->has_parent_file()) {
    if (&sample->parent_file() == this) {
      // If the sample is already be owned by this file, do nothing.
      return;
    }
    else {
      // Otherwise raise an error. A sample cannot be shared by two files.
      throw std::invalid_argument("Sample has already been owned by another file.");
    }
  }

  // Set this file to the parent file of the sample.
  sample->set_parent_file(*this);

  // Add the sample to the list.
  samples_.push_back(sample);
}

/// Removes a sample from the SoundFont.
void SoundFont::RemoveSample(
    std::vector<std::shared_ptr<SFSample>>::const_iterator position) {
  const std::shared_ptr<SFSample> & sample = *position;
  sample->reset_parent_file();
  samples_.erase(position);
}

/// Removes a sample from the SoundFont.
void SoundFont::RemoveSample(
    std::vector<std::shared_ptr<SFSample>>::const_iterator first,
  std::vector<std::shared_ptr<SFSample>>::const_iterator last) {
  for (auto position = first; position != last; ++position) {
    const auto & sample = *position;
    sample->reset_parent_file();
  }
  samples_.erase(first, last);
}

/// Removes a sample from the SoundFont.
void SoundFont::RemoveSampleIf(
    std::function<bool(const std::shared_ptr<SFSample> &)> predicate) {
  samples_.erase(std::remove_if(samples_.begin(), samples_.end(),
    [&predicate](const std::shared_ptr<SFSample> & sample) -> bool {
      if (predicate(sample)) {
        sample->reset_parent_file();
        return true;
      }
      else {
        return false;
      }
    }), samples_.end());
}

/// Removes all of the samples.
void SoundFont::ClearSamples() noexcept {
  for (const auto & sample : samples_) {
    sample->reset_parent_file();
  }
  samples_.clear();
}

/// Writes the SoundFont to a file.
void SoundFont::Write(const std::string & filename) {
  SoundFontWriter writer(*this);
  writer.Write(filename);
}

/// Writes the SoundFont to an output stream.
void SoundFont::Write(std::ostream & out) {
  SoundFontWriter writer(*this);
  writer.Write(out);
}

/// Writes the SoundFont to an output stream.
void SoundFont::Write(std::ostream && out) {
  Write(out);
}

/// Sets backward references of every children elements.
void SoundFont::SetBackwardReferences() noexcept {
  // Set backward reference from presets to the file.
  for (const auto & preset : presets_) {
    preset->set_parent_file(*this);
  }

  // Set backward reference from instruments to the file.
  for (const auto & instrument : instruments_) {
    instrument->set_parent_file(*this);
  }

  // Set backward reference from samples to the file.
  for (const auto & sample : samples_) {
    sample->set_parent_file(*this);
  }
}

/// Repairs references in the copied children elements.
void SoundFont::RepairReferences(const SoundFont & origin) {
  // Construct a map from the original instrument to the copied instrument.
  std::unordered_map<std::shared_ptr<SFInstrument>, std::shared_ptr<SFInstrument>> instrument_map;
  std::transform(origin.instruments_.begin(), origin.instruments_.end(), instruments_.begin(),
    std::inserter(instrument_map, instrument_map.end()),
    std::make_pair<const std::shared_ptr<SFInstrument> &, const std::shared_ptr<SFInstrument> &>);

  // Construct a map from the original sample to the copied sample.
  std::unordered_map<std::shared_ptr<SFSample>, std::shared_ptr<SFSample>> sample_map;
  std::transform(origin.samples_.begin(), origin.samples_.end(), samples_.begin(),
    std::inserter(sample_map, sample_map.end()),
    std::make_pair<const std::shared_ptr<SFSample> &, const std::shared_ptr<SFSample> &>);

  // Repair presets.
  for (const auto & preset : presets_) {
    RepairPresetReference(*preset, instrument_map);
  }

  // Repair instruments.
  for (const auto & instrument : instruments_) {
    RepairInstrumentReference(*instrument, sample_map);
  }

  // Repair samples.
  for (const auto & sample : samples_) {
    RepairSampleReference(*sample, sample_map);
  }
}

/// Corrects the instrument pointer in a copied preset.
void SoundFont::RepairPresetReference(
    SFPreset & preset,
    const std::unordered_map<std::shared_ptr<SFInstrument>,
    std::shared_ptr<SFInstrument>> & instrument_map) {
  // Repair global zone.
  if (preset.has_global_zone()) {
    RepairPresetZoneReference(preset.global_zone(), instrument_map);
  }

  // Repair preset zones.
  for (const auto & preset_zone : preset.zones()) {
    RepairPresetZoneReference(*preset_zone, instrument_map);
  }
}

/// Corrects the instrument pointer in a copied preset zone.
void SoundFont::RepairPresetZoneReference(
    SFPresetZone & preset_zone,
    const std::unordered_map<std::shared_ptr<SFInstrument>,
    std::shared_ptr<SFInstrument>> & instrument_map) {
  // Repair instrument reference.
  if (preset_zone.has_instrument()) {
    const auto & instrument = preset_zone.instrument();
    if (instrument_map.count(instrument) != 0) {
      preset_zone.set_instrument(instrument_map.at(instrument));
    }
    else {
      preset_zone.reset_instrument();
    }
  }
}

/// Corrects the sample pointer in a copied instrument.
void SoundFont::RepairInstrumentReference(
    SFInstrument & instrument,
    const std::unordered_map<std::shared_ptr<SFSample>,
    std::shared_ptr<SFSample>> & sample_map) {
  // Repair global zone.
  if (instrument.has_global_zone()) {
    RepairInstrumentZoneReference(instrument.global_zone(), sample_map);
  }

  // Repair instrument zones.
  for (const auto & instrument_zone : instrument.zones()) {
    RepairInstrumentZoneReference(*instrument_zone, sample_map);
  }
}

/// Corrects the sample pointer in a copied instrument zone.
void SoundFont::RepairInstrumentZoneReference(
    SFInstrumentZone & instrument_zone,
    const std::unordered_map<std::shared_ptr<SFSample>,
    std::shared_ptr<SFSample>> & sample_map) {
  // Repair sample reference.
  if (instrument_zone.has_sample()) {
    const auto & sample = instrument_zone.sample();
    if (sample_map.count(sample) != 0) {
      instrument_zone.set_sample(sample_map.at(sample));
    }
    else {
      instrument_zone.reset_sample();
    }
  }
}

/// Corrects the sample link in a copied sample.
void SoundFont::RepairSampleReference(
    SFSample & sample,
    const std::unordered_map<std::shared_ptr<SFSample>,
    std::shared_ptr<SFSample>> & sample_map) {
  // Repair sample link.
  if (sample.has_link()) {
    const auto link = sample.link();
    if (sample_map.count(link) != 0) {
      sample.set_link(sample_map.at(link));
    }
    else {
      sample.reset_link();
    }
  }
}

} // namespace sf2cute
