/// @file
/// SoundFont 2 Preset Zone class header.
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_PRESET_ZONE_HPP_
#define SF2CUTE_PRESET_ZONE_HPP_

#include <memory>
#include <vector>

#include "zone.hpp"

namespace sf2cute {

class SFGeneratorItem;
class SFModulatorItem;
class SFInstrument;
class SFPreset;
class SoundFont;

/// The SFPresetZone class represents a preset zone.
///
/// @remarks This class represents the official sfPresetBag type and the instrument generator.
/// @see "7.3 The PBAG Sub-chunk". In SoundFont Technical Specification 2.04.
class SFPresetZone : public SFZone {
  friend class SFPreset;

public:
  /// Constructs a new empty SFPresetZone.
  SFPresetZone();

  /// Constructs a new SFPresetZone with an instrument.
  /// @param instrument the instrument to be associated with the zone.
  explicit SFPresetZone(std::weak_ptr<SFInstrument> instrument);

  /// Constructs a new SFPresetZone with an instrument, using the specified generators and modulators.
  /// @param instrument the instrument to be associated with the zone.
  /// @param generators a collection of generators to be assigned to the zone.
  /// @param modulators a collection of modulators to be assigned to the zone.
  SFPresetZone(std::weak_ptr<SFInstrument> instrument,
      std::vector<SFGeneratorItem> generators,
      std::vector<SFModulatorItem> modulators);

  /// Constructs a new copy of specified SFPresetZone.
  /// @param origin a SFPresetZone object.
  SFPresetZone(const SFPresetZone & origin);

  /// Copy-assigns a new value to the SFPresetZone, replacing its current contents.
  /// @param origin a SFPresetZone object.
  SFPresetZone & operator=(const SFPresetZone & origin);

  /// Acquires the contents of specified SFPresetZone.
  /// @param origin a SFPresetZone object.
  SFPresetZone(SFPresetZone && origin) = default;

  /// Move-assigns a new value to the SFPresetZone, replacing its current contents.
  /// @param origin a SFPresetZone object.
  SFPresetZone & operator=(SFPresetZone && origin) = default;

  /// Destructs the SFPresetZone.
  virtual ~SFPresetZone() override = default;

  /// Returns true if the zone has an associated instrument.
  /// @return true if the zone has an associated instrument.
  /// @remarks This function returns false if the associated instrument has been deleted.
  bool has_instrument() const noexcept;

  /// Returns the associated instrument.
  /// @return a pointer to the associated instrument.
  /// @remarks This function returns nullptr if the associated instrument has been deleted.
  std::shared_ptr<SFInstrument> instrument() const noexcept;

  /// Sets the associated instrument.
  /// @param instrument a pointer to the associated instrument.
  void set_instrument(std::weak_ptr<SFInstrument> instrument);

  /// Resets the associated instrument.
  void reset_instrument() noexcept;

  /// Returns true if the zone has a parent file.
  /// @return true if the zone has a parent file.
  bool has_parent_file() const noexcept;

  /// Returns the parent file.
  /// @return the parent file.
  SoundFont & parent_file() const noexcept;

  /// Returns true if the zone has a parent preset.
  /// @return true if the zone has a parent preset.
  bool has_parent_preset() const noexcept;

  /// Returns the parent preset.
  /// @return the parent preset.
  SFPreset & parent_preset() const noexcept;

private:
  /// Sets the parent preset.
  /// @param parent_preset the parent preset.
  void set_parent_preset(SFPreset & parent_preset) noexcept;

  /// Resets the parent preset.
  void reset_parent_preset() noexcept;

  /// The associated instrument.
  std::weak_ptr<SFInstrument> instrument_;

  /// The parent preset.
  SFPreset * parent_preset_;
};

} // namespace sf2cute

#endif // SF2CUTE_PRESET_ZONE_HPP_
