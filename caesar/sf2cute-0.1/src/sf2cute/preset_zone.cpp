/// @file
/// SoundFont 2 Preset Zone class implementation.
///
/// @author gocha <https://github.com/gocha>

#include <sf2cute/preset_zone.hpp>

#include <sf2cute/generator_item.hpp>
#include <sf2cute/preset.hpp>
#include <sf2cute/file.hpp>

namespace sf2cute {

/// Constructs a new empty SFPresetZone.
SFPresetZone::SFPresetZone() :
  parent_preset_(nullptr) {
}

/// Constructs a new SFPresetZone with an instrument.
SFPresetZone::SFPresetZone(std::weak_ptr<SFInstrument> instrument) :
    instrument_(std::move(instrument)),
    parent_preset_(nullptr) {
}

/// Constructs a new SFPresetZone with an instrument, using the specified generators and modulators.
SFPresetZone::SFPresetZone(std::weak_ptr<SFInstrument> instrument,
    std::vector<SFGeneratorItem> generators,
    std::vector<SFModulatorItem> modulators) :
    SFZone(std::move(generators), std::move(modulators)),
    instrument_(std::move(instrument)),
    parent_preset_(nullptr) {
}

/// Constructs a new copy of specified SFPresetZone.
SFPresetZone::SFPresetZone(const SFPresetZone & origin) :
    SFZone(origin),
    instrument_(origin.instrument_),
    parent_preset_(nullptr) {
}

/// Copy-assigns a new value to the SFPresetZone, replacing its current contents.
SFPresetZone & SFPresetZone::operator=(const SFPresetZone & origin) {
  *static_cast<SFZone *>(this) = origin;
  instrument_ = origin.instrument_;
  parent_preset_ = nullptr;
  return *this;
}

/// Returns true if the zone has an associated instrument.
bool SFPresetZone::has_instrument() const noexcept {
  return !instrument_.expired();
}

/// Returns the associated instrument.
std::shared_ptr<SFInstrument> SFPresetZone::instrument() const noexcept {
  return instrument_.lock();
}

/// Sets the associated instrument.
void SFPresetZone::set_instrument(std::weak_ptr<SFInstrument> instrument) {
  if (has_parent_file() && !instrument.expired()) {
    parent_file().AddInstrument(instrument.lock());
  }
  instrument_ = std::move(instrument);
}

/// Resets the associated instrument.
void SFPresetZone::reset_instrument() noexcept {
  instrument_.reset();
}

/// Returns true if the zone has a parent file.
bool SFPresetZone::has_parent_file() const noexcept {
  return has_parent_preset() && parent_preset_->has_parent_file();
}

/// Returns the parent file.
SoundFont & SFPresetZone::parent_file() const noexcept {
  return parent_preset_->parent_file();
}

/// Returns true if the zone has a parent preset.
bool SFPresetZone::has_parent_preset() const noexcept {
  return parent_preset_ != nullptr;
}

/// Returns the parent preset.
SFPreset & SFPresetZone::parent_preset() const noexcept {
  return *parent_preset_;
}

/// Sets the parent preset.
void SFPresetZone::set_parent_preset(SFPreset & parent_preset) noexcept {
  parent_preset_ = &parent_preset;
}

/// Resets the parent preset.
void SFPresetZone::reset_parent_preset() noexcept {
  parent_preset_ = nullptr;
}

} // namespace sf2cute
