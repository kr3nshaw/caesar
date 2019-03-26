/// @file
/// SoundFont 2 Instrument Zone class header.
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_INSTRUMENT_ZONE_HPP_
#define SF2CUTE_INSTRUMENT_ZONE_HPP_

#include <memory>
#include <vector>

#include "zone.hpp"

namespace sf2cute {

class SFSample;
class SFGeneratorItem;
class SFModulatorItem;
class SFInstrument;
class SoundFont;

/// The SFInstrumentZone class represents an instrument zone.
///
/// @remarks This class represents the official sfInstBag type and the sampleID generator.
/// @see "7.6 The IBAG Sub-chunk". In SoundFont Technical Specification 2.04.
class SFInstrumentZone : public SFZone {
  friend class SFInstrument;

public:
  /// Constructs a new empty SFInstrumentZone.
  SFInstrumentZone();

  /// Constructs a new SFInstrumentZone with a sample.
  /// @param sample the sample to be associated with the zone.
  explicit SFInstrumentZone(std::weak_ptr<SFSample> sample);

  /// Constructs a new SFInstrumentZone with a sample, using the specified generators and modulators.
  /// @param sample the sample to be associated with the zone.
  /// @param generators a collection of generators to be assigned to the zone.
  /// @param modulators a collection of modulators to be assigned to the zone.
  SFInstrumentZone(std::weak_ptr<SFSample> sample,
      std::vector<SFGeneratorItem> generators,
      std::vector<SFModulatorItem> modulators);

  /// Constructs a new copy of specified SFInstrumentZone.
  /// @param origin a SFInstrumentZone object.
  SFInstrumentZone(const SFInstrumentZone & origin);

  /// Copy-assigns a new value to the SFInstrumentZone, replacing its current contents.
  /// @param origin a SFInstrumentZone object.
  SFInstrumentZone & operator=(const SFInstrumentZone & origin);

  /// Acquires the contents of specified SFInstrumentZone.
  /// @param origin a SFInstrumentZone object.
  SFInstrumentZone(SFInstrumentZone && origin) = default;

  /// Move-assigns a new value to the SFInstrumentZone, replacing its current contents.
  /// @param origin a SFInstrumentZone object.
  SFInstrumentZone & operator=(SFInstrumentZone && origin) = default;

  /// Destructs the SFInstrumentZone.
  virtual ~SFInstrumentZone() override = default;

  /// Returns true if the zone has an associated sample.
  /// @return true if the zone has an associated sample.
  /// @remarks This function returns false if the associated sample has been deleted.
  bool has_sample() const noexcept;

  /// Returns the associated sample.
  /// @return a pointer to the associated sample.
  /// @remarks This function returns nullptr if the associated sample has been deleted.
  std::shared_ptr<SFSample> sample() const noexcept;

  /// Sets the associated sample.
  /// @param sample a pointer to the associated sample.
  void set_sample(std::weak_ptr<SFSample> sample);

  /// Resets the associated sample.
  void reset_sample() noexcept;

  /// Returns true if the zone has a parent file.
  /// @return true if the zone has a parent file.
  bool has_parent_file() const noexcept;

  /// Returns the parent file.
  /// @return the parent file.
  SoundFont & parent_file() const noexcept;

  /// Returns true if the zone has a parent instrument.
  /// @return true if the zone has a parent instrument.
  bool has_parent_instrument() const noexcept;

  /// Returns the parent instrument.
  /// @return the parent instrument.
  SFInstrument & parent_instrument() const noexcept;

private:
  /// Sets the parent instrument.
  /// @param parent_instrument the parent instrument.
  void set_parent_instrument(
      SFInstrument & parent_instrument) noexcept;

  /// Resets the parent instrument.
  void reset_parent_instrument() noexcept;

  /// The associated sample.
  std::weak_ptr<SFSample> sample_;

  /// The parent instrument.
  SFInstrument * parent_instrument_;
};

} // namespace sf2cute

#endif // SF2CUTE_INSTRUMENT_ZONE_HPP_
