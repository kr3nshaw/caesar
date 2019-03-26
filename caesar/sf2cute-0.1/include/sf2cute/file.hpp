/// @file
/// SoundFont 2 File class header.
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_FILE_HPP_
#define SF2CUTE_FILE_HPP_

#include <memory>
#include <utility>
#include <functional>
#include <vector>
#include <unordered_map>

#include "types.hpp"

namespace sf2cute {

class SFSample;
class SFInstrumentZone;
class SFInstrument;
class SFPresetZone;
class SFPreset;
class SoundFont;

/// The SoundFont class represents a SoundFont file.
class SoundFont {
public:
  /// Maximum length of text fields of INFO chunk (excluding the terminator byte), in terms of bytes.
  static constexpr std::string::size_type kInfoTextMaxLength = 256 - 1;

  /// Constructs a new empty SoundFont.
  SoundFont();

  /// Constructs a new copy of specified SoundFont.
  /// @param origin a SoundFont object.
  SoundFont(const SoundFont & origin);

  /// Copy-assigns a new value to the SoundFont, replacing its current contents.
  /// @param origin a SoundFont object.
  SoundFont & operator=(const SoundFont & origin);

  /// Acquires the contents of specified SoundFont.
  /// @param origin a SoundFont object.
  SoundFont(SoundFont && origin) noexcept;

  /// Move-assigns a new value to the SoundFont, replacing its current contents.
  /// @param origin a SoundFont object.
  SoundFont & operator=(SoundFont && origin) noexcept;

  /// Destructs the SoundFont.
  ~SoundFont() = default;

  /// Returns the list of presets.
  /// @return the list of presets assigned to the SoundFont.
  const std::vector<std::shared_ptr<SFPreset>> & presets() const noexcept;

  /// Adds a new preset to the SoundFont.
  /// @param args the arguments for the SFPreset constructor.
  /// @return the new SFPreset object.
  /// @throws std::invalid_argument Preset has already been owned by another file.
  template<typename ... Args>
  std::shared_ptr<SFPreset> NewPreset(Args && ... args) {
    std::shared_ptr<SFPreset> preset =
      std::make_shared<SFPreset>(std::forward<Args>(args)...);
    AddPreset(preset);
    return std::move(preset);
  }

  /// Adds a preset to the SoundFont.
  /// @param preset a preset to be assigned to the SoundFont.
  /// @throws std::invalid_argument Preset has already been owned by another file.
  void AddPreset(std::shared_ptr<SFPreset> preset);

  /// Removes a preset from the SoundFont.
  /// @param position the preset to remove.
  void RemovePreset(
      std::vector<std::shared_ptr<SFPreset>>::const_iterator position);

  /// Removes presets from the SoundFont.
  /// @param first the first preset to remove.
  /// @param last the last preset to remove.
  void RemovePreset(
      std::vector<std::shared_ptr<SFPreset>>::const_iterator first,
      std::vector<std::shared_ptr<SFPreset>>::const_iterator last);

  /// Removes presets from the SoundFont.
  /// @param predicate unary predicate which returns true if the preset should be removed.
  void RemovePresetIf(
      std::function<bool(const std::shared_ptr<SFPreset> &)> predicate);

  /// Removes all of the presets.
  void ClearPresets() noexcept;

  /// Returns the list of instruments.
  /// @return the list of instruments assigned to the SoundFont.
  const std::vector<std::shared_ptr<SFInstrument>> & instruments() const noexcept;

  /// Adds a new instrument to the SoundFont.
  /// @param args the arguments for the SFInstrument constructor.
  /// @return the new SFInstrument object.
  /// @throws std::invalid_argument Instrument has already been owned by another file.
  template<typename ... Args>
  std::shared_ptr<SFInstrument> NewInstrument(Args && ... args) {
    std::shared_ptr<SFInstrument> instrument =
      std::make_shared<SFInstrument>(std::forward<Args>(args)...);
    AddInstrument(instrument);
    return std::move(instrument);
  }

  /// Adds an instrument to the SoundFont.
  /// @param instrument an instrument to be assigned to the SoundFont.
  /// @throws std::invalid_argument Instrument has already been owned by another file.
  void AddInstrument(std::shared_ptr<SFInstrument> instrument);

  /// Removes an instrument from the SoundFont.
  /// @param position the instrument to remove.
  void RemoveInstrument(
      std::vector<std::shared_ptr<SFInstrument>>::const_iterator position);

  /// Removes instruments from the SoundFont.
  /// @param first the first instrument to remove.
  /// @param last the last instrument to remove.
  void RemoveInstrument(
      std::vector<std::shared_ptr<SFInstrument>>::const_iterator first,
      std::vector<std::shared_ptr<SFInstrument>>::const_iterator last);

  /// Removes instruments from the SoundFont.
  /// @param predicate unary predicate which returns true if the instrument should be removed.
  void RemoveInstrumentIf(
      std::function<bool(const std::shared_ptr<SFInstrument> &)> predicate);

  /// Removes all of the instruments.
  void ClearInstruments() noexcept;

  /// Returns the list of samples.
  /// @return the list of samples assigned to the SoundFont.
  const std::vector<std::shared_ptr<SFSample>> & samples() const noexcept;

  /// Adds a new sample to the SoundFont.
  /// @param args the arguments for the SFSample constructor.
  /// @return the new SFSample object.
  /// @throws std::invalid_argument Sample has already been owned by another file.
  template<typename ... Args>
  std::shared_ptr<SFSample> NewSample(Args && ... args) {
    std::shared_ptr<SFSample> sample =
      std::make_shared<SFSample>(std::forward<Args>(args)...);
    AddSample(sample);
    return std::move(sample);
  }

  /// Adds a sample to the SoundFont.
  /// @param sample a sample to be assigned to the SoundFont.
  /// @throws std::invalid_argument Sample has already been owned by another file.
  void AddSample(std::shared_ptr<SFSample> sample);

  /// Removes a sample from the SoundFont.
  /// @param position the sample to remove.
  void RemoveSample(
      std::vector<std::shared_ptr<SFSample>>::const_iterator position);

  /// Removes samples from the SoundFont.
  /// @param first the first sample to remove.
  /// @param last the last sample to remove.
  void RemoveSample(
      std::vector<std::shared_ptr<SFSample>>::const_iterator first,
      std::vector<std::shared_ptr<SFSample>>::const_iterator last);

  /// Removes samples from the SoundFont.
  /// @param predicate unary predicate which returns true if the sample should be removed.
  void RemoveSampleIf(
      std::function<bool(const std::shared_ptr<SFSample> &)> predicate);

  /// Removes all of the samples.
  void ClearSamples() noexcept;

  /// Returns the target sound engine.
  /// @return the target sound engine name.
  const std::string & sound_engine() const noexcept;

  /// Sets the target sound engine.
  /// @param sound_engine the target sound engine name.
  void set_sound_engine(std::string sound_engine);

  /// Returns the SoundFont bank name.
  /// @return the SoundFont bank name.
  const std::string & bank_name() const noexcept;

  /// Sets the SoundFont bank name.
  /// @param bank_name the SoundFont bank name.
  void set_bank_name(std::string bank_name);

  /// Returns true if the SoundFont has a Sound ROM name.
  /// @return true if the SoundFont has a Sound ROM name.
  bool has_rom_name() const noexcept;

  /// Returns the Sound ROM name.
  /// @return the Sound ROM name.
  const std::string & rom_name() const noexcept;

  /// Sets the Sound ROM name.
  /// @param rom_name the Sound ROM name.
  void set_rom_name(std::string rom_name);

  /// Resets the Sound ROM name.
  void reset_rom_name() noexcept;

  /// Returns true if the SoundFont has a Sound ROM version.
  /// @return true if the SoundFont has a Sound ROM version.
  bool has_rom_version() const noexcept;

  /// Returns the Sound ROM version.
  /// @return the Sound ROM version.
  SFVersionTag rom_version() const noexcept;

  /// Sets the Sound ROM version.
  /// @param rom_version the Sound ROM version.
  void set_rom_version(SFVersionTag rom_version);

  /// Resets the Sound ROM version.
  void reset_rom_version() noexcept;

  /// Returns true if the SoundFont has a date of creation of the bank.
  /// @return true if the SoundFont has a date of creation of the bank.
  bool has_creation_date() const noexcept;

  /// Returns the date of creation of the bank.
  /// @return the date of creation of the bank.
  const std::string & creation_date() const noexcept;

  /// Sets the date of creation of the bank.
  /// @param creation_date the date of creation of the bank.
  void set_creation_date(std::string creation_date);

  /// Resets the date of creation of the bank.
  void reset_creation_date() noexcept;

  /// Returns true if the SoundFont has the sound designers and engineers information for the bank.
  /// @return true if the SoundFont has the sound designers and engineers information for the bank.
  bool has_engineers() const noexcept;

  /// Returns the sound designers and engineers for the bank.
  /// @return the sound designers and engineers for the bank.
  const std::string & engineers() const noexcept;

  /// Sets the sound designers and engineers for the bank.
  /// @param engineers the sound designers and engineers for the bank.
  void set_engineers(std::string engineers);

  /// Resets the sound designers and engineers for the bank.
  void reset_engineers() noexcept;

  /// Returns true if the SoundFont has a product name for which the bank was intended.
  /// @return true if the SoundFont has a product name for which the bank was intended.
  bool has_product() const noexcept;

  /// Returns the product name for which the bank was intended.
  /// @return the product name for which the bank was intended.
  const std::string & product() const noexcept;

  /// Sets the product name for which the bank was intended.
  /// @param product the product name for which the bank was intended.
  void set_product(std::string product);

  /// Resets the product name for which the bank was intended.
  void reset_product() noexcept;

  /// Returns true if the SoundFont has any copyright message.
  /// @return true if the SoundFont has any copyright message.
  bool has_copyright() const noexcept;

  /// Returns the copyright message.
  /// @return the copyright message.
  const std::string & copyright() const noexcept;

  /// Sets the copyright message.
  /// @param copyright the copyright message.
  void set_copyright(std::string copyright);

  /// Resets the copyright message.
  void reset_copyright() noexcept;

  /// Returns true if the SoundFont has any comments on the bank.
  /// @return true if the SoundFont has any comments on the bank.
  bool has_comment() const noexcept;

  /// Returns the comments on the bank.
  /// @return the comments on the bank.
  const std::string & comment() const noexcept;

  /// Sets the comments on the bank.
  /// @param comment the comments on the bank.
  void set_comment(std::string comment);

  /// Resets the comments on the bank.
  void reset_comment() noexcept;

  /// Returns true if the SoundFont the information of SoundFont tools used to create and alter the bank.
  /// @return true if the SoundFont the information of SoundFont tools used to create and alter the bank.
  bool has_software() const noexcept;

  /// Returns the SoundFont tools used to create and alter the bank.
  /// @return the SoundFont tools used to create and alter the bank.
  const std::string & software() const noexcept;

  /// Sets the SoundFont tools used to create and alter the bank.
  /// @param software the SoundFont tools used to create and alter the bank.
  void set_software(std::string software);

  /// Resets the SoundFont tools used to create and alter the bank.
  void reset_software() noexcept;

  /// Writes the SoundFont to a file.
  /// @param filename the name of the file to write to.
  /// @throws std::logic_error The SoundFont has a structural error.
  /// @throws std::ios_base::failure An I/O error occurred.
  void Write(const std::string & filename);

  /// Writes the SoundFont to an output stream.
  /// @param out the output stream to write to.
  void Write(std::ostream & out);

  /// @copydoc SoundFont::Write(std::ostream &)
  void Write(std::ostream && out);

private:
  /// The default value of the target sound engine.
  static constexpr auto kDefaultTargetSoundEngine = "EMU8000";

  /// The default value of the SoundFont bank name.
  static constexpr auto kDefaultBankName = "Unnamed";

  /// Sets backward references of every children elements.
  void SetBackwardReferences() noexcept;

  /// Repairs references in the copied children elements.
  /// @param origin a SoundFont object used to construct this SoundFont object.
  void RepairReferences(const SoundFont & origin);

  /// Corrects the instrument pointer in a copied preset.
  /// @param preset the preset to be corrected.
  /// @param instrument_map a map containing original instruments as keys and copied instruments as map values.
  static void RepairPresetReference(
      SFPreset & preset,
      const std::unordered_map<std::shared_ptr<SFInstrument>,
      std::shared_ptr<SFInstrument>> & instrument_map);

  /// Corrects the instrument pointer in a copied preset zone.
  /// @param preset_zone the preset zone to be corrected.
  /// @param instrument_map a map containing original instruments as keys and copied instruments as map values.
  static void RepairPresetZoneReference(
      SFPresetZone & preset_zone,
      const std::unordered_map<std::shared_ptr<SFInstrument>,
      std::shared_ptr<SFInstrument>> & instrument_map);

  /// Corrects the sample pointer in a copied instrument.
  /// @param instrument the instrument to be corrected.
  /// @param sample_map a map containing original samples as keys and copied samples as map values.
  static void RepairInstrumentReference(
      SFInstrument & instrument,
      const std::unordered_map<std::shared_ptr<SFSample>,
      std::shared_ptr<SFSample>> & sample_map);

  /// Corrects the sample pointer in a copied instrument zone.
  /// @param instrument_zone the instrument zone to be corrected.
  /// @param sample_map a map containing original samples as keys and copied samples as map values.
  static void RepairInstrumentZoneReference(
      SFInstrumentZone & instrument_zone,
      const std::unordered_map<std::shared_ptr<SFSample>,
      std::shared_ptr<SFSample>> & sample_map);

  /// Corrects the sample link in a copied sample.
  /// @param sample the sample to be corrected.
  /// @param sample_map a map containing original samples as keys and copied samples as map values.
  static void RepairSampleReference(
      SFSample & sample,
      const std::unordered_map<std::shared_ptr<SFSample>,
      std::shared_ptr<SFSample>> & sample_map);

  /// The list of presets.
  std::vector<std::shared_ptr<SFPreset>> presets_;

  /// The list of instruments.
  std::vector<std::shared_ptr<SFInstrument>> instruments_;

  /// The list of samples.
  std::vector<std::shared_ptr<SFSample>> samples_;

  /// The target sound engine name.
  std::string sound_engine_;

  /// The SoundFont bank name.
  std::string bank_name_;

  /// The Sound ROM name.
  std::string rom_name_;

  /// The Sound ROM version.
  SFVersionTag rom_version_;

  /// True if the SoundFont has a Sound ROM version.
  bool has_rom_version_;

  /// The date of creation of the bank.
  std::string creation_date_;

  /// The sound designers and engineers for the bank.
  std::string engineers_;

  /// The product name for which the bank was intended.
  std::string product_;

  /// The copyright message.
  std::string copyright_;

  /// The comments on the bank.
  std::string comment_;

  /// The SoundFont tools used to create and alter the bank.
  std::string software_;
};

} // namespace sf2cute

#endif // SF2CUTE_FILE_HPP_
