/// @file
/// SoundFont 2 Instrument Zone class implementation.
///
/// @author gocha <https://github.com/gocha>

#include <sf2cute/generator_item.hpp>
#include <sf2cute/instrument_zone.hpp>
#include <sf2cute/instrument.hpp>
#include <sf2cute/file.hpp>

namespace sf2cute {

/// Constructs a new empty SFInstrumentZone.
SFInstrumentZone::SFInstrumentZone() :
    parent_instrument_(nullptr) {
}

/// Constructs a new SFInstrumentZone with a sample.
SFInstrumentZone::SFInstrumentZone(std::weak_ptr<SFSample> sample) :
    sample_(std::move(sample)),
    parent_instrument_(nullptr) {
}

/// Constructs a new SFInstrumentZone with a sample, using the specified generators and modulators.
SFInstrumentZone::SFInstrumentZone(std::weak_ptr<SFSample> sample,
    std::vector<SFGeneratorItem> generators,
    std::vector<SFModulatorItem> modulators) :
    SFZone(std::move(generators), std::move(modulators)),
    sample_(std::move(sample)),
    parent_instrument_(nullptr) {
}

/// Constructs a new copy of specified SFInstrumentZone.
SFInstrumentZone::SFInstrumentZone(const SFInstrumentZone & origin) :
    SFZone(origin),
    sample_(origin.sample_),
    parent_instrument_(nullptr) {
}

/// Copy-assigns a new value to the SFInstrumentZone, replacing its current contents.
SFInstrumentZone & SFInstrumentZone::operator=(const SFInstrumentZone & origin) {
  *static_cast<SFZone *>(this) = origin;
  sample_ = origin.sample_;
  parent_instrument_ = nullptr;
  return *this;
}

/// Sets the associated sample.
void SFInstrumentZone::set_sample(std::weak_ptr<SFSample> sample) {
  if (has_parent_file() && !sample.expired()) {
    parent_file().AddSample(sample.lock());
  }
  sample_ = std::move(sample);
}

/// Returns true if the zone has a parent file.
bool SFInstrumentZone::has_parent_file() const noexcept {
  return has_parent_instrument() && parent_instrument_->has_parent_file();
}

/// Returns the parent file.
SoundFont & SFInstrumentZone::parent_file() const noexcept {
  return parent_instrument_->parent_file();
}

/// Returns the parent instrument.
SFInstrument & SFInstrumentZone::parent_instrument() const noexcept {
  return *parent_instrument_;
}

/// Sets the parent instrument.
void SFInstrumentZone::set_parent_instrument(
    SFInstrument & parent_instrument) noexcept {
  parent_instrument_ = &parent_instrument;
}

} // namespace sf2cute
