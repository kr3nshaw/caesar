/// @file
/// SoundFont 2 Instrument class header.
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_INSTRUMENT_HPP_
#define SF2CUTE_INSTRUMENT_HPP_

#include <memory>
#include <utility>
#include <functional>
#include <string>
#include <vector>

namespace sf2cute {

class SFInstrumentZone;
class SoundFont;

/// The SFInstrument class represents an instrument.
///
/// @remarks This class represents the official sfInst type.
/// @see "7.6 The INST Sub-chunk".
/// In SoundFont Technical Specification 2.04.
class SFInstrument {
  friend class SFInstrumentZone;
  friend class SoundFont;

public:
  /// Maximum length of instrument name (excluding the terminator byte), in terms of bytes.
  static constexpr std::string::size_type kMaxNameLength = 20 - 1;

  /// Constructs a new empty instrument.
  SFInstrument();

  /// Constructs a new empty SFInstrument using the specified name.
  /// @param name the name of the instrument.
  explicit SFInstrument(std::string name);

  /// Constructs a new SFInstrument using the specified name and zones.
  /// @param name the name of the instrument.
  /// @param zones a collection of instrument zones to be assigned to the instrument.
  SFInstrument(std::string name,
      std::vector<SFInstrumentZone> zones);

  /// Constructs a new SFInstrument using the specified name, zones and global zone.
  /// @param name the name of the instrument.
  /// @param zones a collection of instrument zones to be assigned to the instrument.
  /// @param global_zone a global zone to be assigned to the instrument.
  SFInstrument(std::string name,
      std::vector<SFInstrumentZone> zones,
      SFInstrumentZone global_zone);

  /// Constructs a new copy of specified SFInstrument.
  /// @param origin a SFInstrument object.
  SFInstrument(const SFInstrument & origin);

  /// Copy-assigns a new value to the SFInstrument, replacing its current contents.
  /// @param origin a SFInstrument object.
  SFInstrument & operator=(const SFInstrument & origin);

  /// Acquires the contents of specified SFInstrument.
  /// @param origin a SFInstrument object.
  SFInstrument(SFInstrument && origin) noexcept;

  /// Move-assigns a new value to the SFInstrument, replacing its current contents.
  /// @param origin a SFInstrument object.
  SFInstrument & operator=(SFInstrument && origin) noexcept;

  /// Destructs the SFInstrument.
  ~SFInstrument() = default;

  /// Constructs a new SFInstrument.
  /// @param args the arguments for the SFInstrument constructor.
  /// @return the new SFInstrument object.
  template<typename ... Args>
  static std::shared_ptr<SFInstrument> New(Args && ... args) {
    return std::make_shared<SFInstrument>(std::forward<Args>(args)...);
  }

  /// Returns the name of this instrument.
  /// @return the name of this instrument.
  const std::string & name() const noexcept;

  /// Sets the name of this instrument.
  /// @param name the name of this instrument.
  void set_name(std::string name);

  /// Returns the list of instrument zones.
  /// @return the list of instrument zones assigned to the instruemnt.
  const std::vector<std::unique_ptr<SFInstrumentZone>> & zones() const noexcept;

  /// Adds an instrument zone to the instrument.
  /// @param zone an instrument zone to be assigned to the instrument.
  /// @throws std::invalid_argument Instrument zone has already been owned by another instrument.
  void AddZone(SFInstrumentZone zone);

  /// Removes an instrument zone from the instrument.
  /// @param position the instrument zone to remove.
  void RemoveZone(
      std::vector<std::unique_ptr<SFInstrumentZone>>::const_iterator position);

  /// Removes instrument zones from the instrument.
  /// @param first the first instrument zone to remove.
  /// @param last the last instrument zone to remove.
  void RemoveZone(
      std::vector<std::unique_ptr<SFInstrumentZone>>::const_iterator first,
      std::vector<std::unique_ptr<SFInstrumentZone>>::const_iterator last);

  /// Removes instrument zones from the instrument.
  /// @param predicate unary predicate which returns true if the instrument zone should be removed.
  void RemoveZoneIf(
      std::function<bool(const std::unique_ptr<SFInstrumentZone> &)> predicate);

  /// Removes all of the instrument zones.
  void ClearZones() noexcept;

  /// Returns true if the instrument has a global zone.
  /// @return true if the instrument has a global zone.
  bool has_global_zone() const noexcept;

  /// Returns the global zone.
  /// @return the global zone.
  SFInstrumentZone & global_zone() const noexcept;

  /// Sets the global zone.
  /// @param global_zone the global zone.
  /// @throws std::invalid_argument Instrument zone has already been owned by another instrument.
  void set_global_zone(SFInstrumentZone global_zone);

  /// Resets the global zone.
  void reset_global_zone() noexcept;

  /// Returns true if the instrument has a parent file.
  /// @return true if the instrument has a parent file.
  bool has_parent_file() const noexcept;

  /// Returns the parent file.
  /// @return the parent file.
  SoundFont & parent_file() const noexcept;

private:
  /// Sets the parent file.
  /// @param parent_file the parent file.
  void set_parent_file(SoundFont & parent_file) noexcept;

  /// Resets the parent file.
  void reset_parent_file() noexcept;

  /// Sets backward references of every children elements.
  void SetBackwardReferences() noexcept;

  /// The name of instrument.
  std::string name_;

  /// The list of instrument zones.
  std::vector<std::unique_ptr<SFInstrumentZone>> zones_;

  /// The global zone.
  std::unique_ptr<SFInstrumentZone> global_zone_;

  /// The parent file.
  SoundFont * parent_file_;
};

} // namespace sf2cute

#endif // SF2CUTE_INSTRUMENT_HPP_
