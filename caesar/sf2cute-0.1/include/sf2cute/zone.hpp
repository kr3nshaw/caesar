/// @file
/// SoundFont 2 Zone class header.
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_ZONE_HPP_
#define SF2CUTE_ZONE_HPP_

#include <memory>
#include <functional>
#include <vector>

#include "types.hpp"
#include "generator_item.hpp"
#include "modulator_key.hpp"
#include "modulator_item.hpp"

namespace sf2cute {

/// The SFZone class represents a zone.
///
/// @remarks This class represents the official sfPresetBag/sfInstBag type.
/// @see "7.3 The PBAG Sub-chunk". In SoundFont Technical Specification 2.04.
/// @see "7.7 The IBAG Sub-chunk". In SoundFont Technical Specification 2.04.
class SFZone {
public:
  /// Constructs a new empty SFZone.
  SFZone();

  /// Constructs a new SFZone using the specified generators and modulators.
  /// @param generators a collection of generators to be assigned to the zone.
  /// @param modulators a collection of modulators to be assigned to the zone.
  SFZone(
      std::vector<SFGeneratorItem> generators,
      std::vector<SFModulatorItem> modulators);

  /// Constructs a new copy of specified SFZone.
  /// @param origin a SFZone object.
  SFZone(const SFZone & origin);

  /// Copy-assigns a new value to the SFZone, replacing its current contents.
  /// @param origin a SFZone object.
  SFZone & operator=(const SFZone & origin);

  /// Acquires the contents of specified SFZone.
  /// @param origin a SFZone object.
  SFZone(SFZone && origin) = default;

  /// Move-assigns a new value to the SFZone, replacing its current contents.
  /// @param origin a SFZone object.
  SFZone & operator=(SFZone && origin) = default;

  /// Destructs the SFZone.
  virtual ~SFZone() = default;

  /// Returns the list of generators.
  /// @return the list of generators assigned to the zone.
  const std::vector<std::unique_ptr<SFGeneratorItem>> & generators() const noexcept;

  /// Sets a generator to the zone.
  /// @param generator a generator to be assigned to the zone.
  /// @remarks An existing generator which has the same key will be overwritten.
  void SetGenerator(SFGeneratorItem generator);

  /// Finds the generator which is the specified type.
  /// @return the position of the found generator or std::end(generators()) if no such generator is found.
  const std::vector<std::unique_ptr<SFGeneratorItem>>::const_iterator
      FindGenerator(SFGenerator op) const;

  /// Removes a generator from the zone.
  /// @param position the generator to remove.
  void RemoveGenerator(
      std::vector<std::unique_ptr<SFGeneratorItem>>::const_iterator position);

  /// Removes generators from the zone.
  /// @param first the first generator to remove.
  /// @param last the last generator to remove.
  void RemoveGenerator(
      std::vector<std::unique_ptr<SFGeneratorItem>>::const_iterator first,
      std::vector<std::unique_ptr<SFGeneratorItem>>::const_iterator last);

  /// Removes generators from the zone.
  /// @param predicate unary predicate which returns true if the generator should be removed.
  void RemoveGeneratorIf(
      std::function<bool(const std::unique_ptr<SFGeneratorItem> &)> predicate);

  /// Removes all of the generators.
  void ClearGenerators() noexcept;

  /// Returns the list of modulators.
  /// @return the list of modulators assigned to the zone.
  const std::vector<std::unique_ptr<SFModulatorItem>> & modulators() const noexcept;

  /// Sets a modulator to the zone.
  /// @param modulator a modulator to be assigned to the zone.
  /// @remarks An existing modulator which has the same key will be overwritten.
  void SetModulator(SFModulatorItem modulator);

  /// Finds the modulator which is the specified type.
  /// @return the position of the found modulator or std::end(modulators()) if no such modulator is found.
  const std::vector<std::unique_ptr<SFModulatorItem>>::const_iterator
      FindModulator(SFModulatorKey key) const;

  /// Removes a modulator from the zone.
  /// @param position the modulator to remove.
  void RemoveModulator(
      std::vector<std::unique_ptr<SFModulatorItem>>::const_iterator position);

  /// Removes modulators from the zone.
  /// @param first the first modulator to remove.
  /// @param last the last modulator to remove.
  void RemoveModulator(
      std::vector<std::unique_ptr<SFModulatorItem>>::const_iterator first,
      std::vector<std::unique_ptr<SFModulatorItem>>::const_iterator last);

  /// Removes modulators from the zone.
  /// @param predicate unary predicate which returns true if the modulator should be removed.
  void RemoveModulatorIf(
      std::function<bool(const std::unique_ptr<SFModulatorItem> &)> predicate);

  /// Removes all of the modulators.
  void ClearModulators() noexcept;

protected:
  /// The list of generators.
  std::vector<std::unique_ptr<SFGeneratorItem>> generators_;

  /// The list of modulators.
  std::vector<std::unique_ptr<SFModulatorItem>> modulators_;
};

} // namespace sf2cute

#endif // SF2CUTE_ZONE_HPP_
