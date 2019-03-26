/// @file
/// SoundFont 2 Instrument class implementation.
///
/// @author gocha <https://github.com/gocha>

#include <sf2cute/instrument.hpp>

#include <algorithm>
#include <memory>
#include <utility>
#include <functional>
#include <string>
#include <vector>

#include <sf2cute/instrument_zone.hpp>
#include <sf2cute/file.hpp>

namespace sf2cute {

/// Constructs a new empty instrument.
SFInstrument::SFInstrument() :
    parent_file_(nullptr) {
}

/// Constructs a new empty SFInstrument using the specified name.
SFInstrument::SFInstrument(std::string name) :
    name_(std::move(name)),
    parent_file_(nullptr) {
}

/// Constructs a new SFInstrument using the specified name and zones.
SFInstrument::SFInstrument(std::string name,
    std::vector<SFInstrumentZone> zones) :
    name_(std::move(name)),
    zones_(),
    global_zone_(nullptr),
    parent_file_(nullptr) {
  // Set instrument zones.
  zones_.reserve(zones.size());
  for (auto && zone : zones) {
    zones_.push_back(std::make_unique<SFInstrumentZone>(std::move(zone)));
  }

  // Repair references.
  SetBackwardReferences();
}

/// Constructs a new SFInstrument using the specified name, zones and global zone.
SFInstrument::SFInstrument(std::string name,
    std::vector<SFInstrumentZone> zones,
    SFInstrumentZone global_zone) :
    name_(std::move(name)),
    zones_(),
    global_zone_(std::make_unique<SFInstrumentZone>(std::move(global_zone))),
    parent_file_(nullptr) {
  // Set instrument zones.
  zones_.reserve(zones.size());
  for (auto && zone : zones) {
    zones_.push_back(std::make_unique<SFInstrumentZone>(std::move(zone)));
  }

  // Repair references.
  SetBackwardReferences();
}

/// Constructs a new copy of specified SFInstrument.
SFInstrument::SFInstrument(const SFInstrument & origin) :
    name_(origin.name_),
    zones_(),
    global_zone_(nullptr),
    parent_file_(nullptr) {
  // Copy global zone.
  if (origin.has_global_zone()) {
    global_zone_ = std::make_unique<SFInstrumentZone>(origin.global_zone());
  }

  // Copy instrument zones.
  zones_.reserve(origin.zones().size());
  for (const auto & zone : origin.zones()) {
    zones_.push_back(std::make_unique<SFInstrumentZone>(*zone));
  }

  // Repair references.
  SetBackwardReferences();
}

/// Copy-assigns a new value to the SFInstrument, replacing its current contents.
SFInstrument & SFInstrument::operator=(const SFInstrument & origin) {
  // Copy global zone.
  global_zone_ = nullptr;
  if (origin.has_global_zone()) {
    global_zone_ = std::make_unique<SFInstrumentZone>(origin.global_zone());
  }

  // Copy instrument zones.
  zones_.clear();
  zones_.reserve(origin.zones().size());
  for (const auto & zone : origin.zones()) {
    zones_.push_back(std::make_unique<SFInstrumentZone>(*zone));
  }

  // Copy other fields.
  name_ = origin.name_;
  parent_file_ = nullptr;

  // Repair references.
  SetBackwardReferences();

  return *this;
}

/// Acquires the contents of specified SFInstrument.
SFInstrument::SFInstrument(SFInstrument && origin) noexcept :
    name_(std::move(origin.name_)),
    zones_(std::move(origin.zones_)),
    global_zone_(std::move(origin.global_zone_)),
    parent_file_(nullptr) {
  SetBackwardReferences();
}

/// Move-assigns a new value to the SFInstrument, replacing its current contents.
SFInstrument & SFInstrument::operator=(SFInstrument && origin) noexcept {
  // Copy fields.
  name_ = std::move(origin.name_);
  zones_ = std::move(origin.zones_);
  global_zone_ = std::move(origin.global_zone_);
  parent_file_ = nullptr;

  // Repair references.
  SetBackwardReferences();

  return *this;
}

/// Returns the name of this instrument.
const std::string & SFInstrument::name() const noexcept {
  return name_;
}

/// Sets the name of this instrument.
void SFInstrument::set_name(std::string name) {
  name_ = std::move(name);
}

/// Returns the list of instrument zones.
const std::vector<std::unique_ptr<SFInstrumentZone>> & SFInstrument::zones() const noexcept {
  return zones_;
}

/// Adds an instrument zone to the instrument.
void SFInstrument::AddZone(SFInstrumentZone zone) {
  // Check the parent instrument of the zone.
  if (zone.has_parent_instrument()) {
    if (&zone.parent_instrument() == this) {
      // If the zone is already be owned by this instrument, do nothing.
      return;
    }
    else {
      // Otherwise raise an error. A zone cannot be shared by two parents.
      throw std::invalid_argument("Instrument zone has already been owned by another instrument.");
    }
  }

  // Set this instrument to the parent instrument of the zone.
  zone.set_parent_instrument(*this);

  // If the zone has an orphan sample, add it to the parent file.
  if (has_parent_file()) {
    if (zone.has_sample()) {
      parent_file().AddSample(zone.sample());
    }
  }

  // Add the zone to the list.
  zones_.push_back(std::make_unique<SFInstrumentZone>(std::move(zone)));
}

/// Removes an instrument zone from the instrument.
void SFInstrument::RemoveZone(
    std::vector<std::unique_ptr<SFInstrumentZone>>::const_iterator position) {
  zones_.erase(position);
}

/// Removes instrument zones from the instrument.
void SFInstrument::RemoveZone(
    std::vector<std::unique_ptr<SFInstrumentZone>>::const_iterator first,
    std::vector<std::unique_ptr<SFInstrumentZone>>::const_iterator last) {
  zones_.erase(first, last);
}

/// Removes instrument zones from the instrument.
void SFInstrument::RemoveZoneIf(
    std::function<bool(const std::unique_ptr<SFInstrumentZone> &)> predicate) {
  zones_.erase(std::remove_if(zones_.begin(), zones_.end(),
    [&predicate](const std::unique_ptr<SFInstrumentZone> & zone) -> bool {
    if (predicate(zone)) {
      return true;
    }
    else {
      return false;
    }
  }), zones_.end());
}

/// Removes all of the instrument zones.
void SFInstrument::ClearZones() noexcept {
  zones_.clear();
}

/// Returns true if the instrument has a global zone.
bool SFInstrument::has_global_zone() const noexcept {
  return static_cast<bool>(global_zone_);
}

/// Returns the global zone.
SFInstrumentZone & SFInstrument::global_zone() const noexcept {
  return *global_zone_;
}

/// Sets the global zone.
void SFInstrument::set_global_zone(SFInstrumentZone global_zone) {
  // Check the parent instrument of the zone.
  if (global_zone.has_parent_instrument()) {
    if (&global_zone.parent_instrument() == this) {
      // If the zone is already be owned by this instrument, do nothing.
      return;
    }
    else {
      // Otherwise raise an error. A zone cannot be shared by two parents.
      throw std::invalid_argument("Instrument zone has already been owned by another instrument.");
    }
  }

  // Set this instrument to the parent instrument of the zone.
  global_zone.set_parent_instrument(*this);

  // Set the global zone to this instrument.
  global_zone_ = std::make_unique<SFInstrumentZone>(std::move(global_zone));
}

/// Resets the global zone.
void SFInstrument::reset_global_zone() noexcept {
  global_zone_ = nullptr;
}

/// Returns true if the instrument has a parent file.
bool SFInstrument::has_parent_file() const noexcept {
  return parent_file_ != nullptr;
}

/// Returns the parent file.
SoundFont & SFInstrument::parent_file() const noexcept {
  return *parent_file_;
}

/// Sets the parent file.
void SFInstrument::set_parent_file(SoundFont & parent_file) noexcept {
  parent_file_ = &parent_file;
}

/// Resets the parent file.
void SFInstrument::reset_parent_file() noexcept {
  parent_file_ = nullptr;
}

/// Sets backward references of every children elements.
void SFInstrument::SetBackwardReferences() noexcept {
  // Update the instrument zones.
  for (const auto & zone : zones_) {
    zone->set_parent_instrument(*this);
  }

  // Update the global instrument zone.
  if (has_global_zone()) {
    global_zone_->set_parent_instrument(*this);
  }
}

} // namespace sf2cute
