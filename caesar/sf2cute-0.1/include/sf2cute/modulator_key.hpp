/// @file
/// SoundFont 2 Key members of Modulator class header.
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_MODULATOR_KEY_HPP_
#define SF2CUTE_MODULATOR_KEY_HPP_

#include <utility>

#include "types.hpp"
#include "modulator.hpp"

namespace sf2cute {

/// The SFModulatorKey class represents the unique key of a modulator.
///
/// @remarks This class represents a part of the sfModList/sfInstModList type.
/// @see "7.4 The PMOD Sub-chunk". In SoundFont Technical Specification 2.04.
/// @see "7.8 The IMOD Sub-chunk". In SoundFont Technical Specification 2.04.
class SFModulatorKey {
public:
  /// Constructs a new SFModulatorKey.
  SFModulatorKey();

  /// Constructs a new SFModulatorKey using the specified controllers.
  /// @param source_op the source of data for the modulator.
  /// @param destination_op the destination of the modulator.
  /// @param amount_source_op the modulation source to be applied to the modulation amount.
  SFModulatorKey(SFModulator source_op,
      SFGenerator destination_op,
      SFModulator amount_source_op);

  /// Constructs a new copy of specified SFModulatorKey.
  /// @param origin a SFModulatorKey object.
  SFModulatorKey(const SFModulatorKey & origin) = default;

  /// Copy-assigns a new value to the SFModulatorKey, replacing its current contents.
  /// @param origin a SFModulatorKey object.
  SFModulatorKey & operator=(const SFModulatorKey & origin) = default;

  /// Acquires the contents of specified SFModulatorKey.
  /// @param origin a SFModulatorKey object.
  SFModulatorKey(SFModulatorKey && origin) = default;

  /// Move-assigns a new value to the SFModulatorKey, replacing its current contents.
  /// @param origin a SFModulatorKey object.
  SFModulatorKey & operator=(SFModulatorKey && origin) = default;

  /// Destructs the SFModulatorKey.
  ~SFModulatorKey() = default;

  /// Returns the source of data for the modulator.
  /// @return the source of data for the modulator.
  SFModulator source_op() const noexcept;

  /// Sets the source of data for the modulator.
  /// @param source_op the source of data for the modulator.
  void set_source_op(SFModulator source_op);

  /// Returns the destination of the modulator.
  /// @return the destination of the modulator.
  SFGenerator destination_op() const noexcept;

  /// Sets the destination of the modulator.
  /// @param destination_op the destination of the modulator.
  void set_destination_op(SFGenerator destination_op);

  /// Returns the modulation source to be applied to the modulation amount.
  /// @return the modulation source to be applied to the modulation amount.
  SFModulator amount_source_op() const noexcept;

  /// Sets the modulation source to be applied to the modulation amount.
  /// @param amount_source_op the modulation source to be applied to the modulation amount.
  void set_amount_source_op(SFModulator amount_source_op);

  /// Indicates a SFModulatorKey object is "equal to" the other one.
  /// @param x the first object to be compared.
  /// @param y the second object to be compared.
  /// @return true if a SFModulatorKey object is "equal to" the other one.
  friend inline bool operator==(
      const SFModulatorKey & x,
      const SFModulatorKey & y) noexcept {
    return std::tie(x.source_op_, x.destination_op_, x.amount_source_op_) ==
      std::tie(y.source_op_, y.destination_op_, y.amount_source_op_);
  }

  /// Indicates a SFModulatorKey object is "not equal to" the other one.
  /// @param x the first object to be compared.
  /// @param y the second object to be compared.
  /// @return true if a SFModulatorKey object is "not equal to" the other one.
  friend inline bool operator!=(
      const SFModulatorKey & x,
      const SFModulatorKey & y) noexcept {
    return std::rel_ops::operator!=(x, y);
  }

  /// Indicates a SFModulatorKey object is "less than" the other one.
  /// @param x the first object to be compared.
  /// @param y the second object to be compared.
  /// @return true if a SFModulatorKey object is "less than" the other one.
  friend inline bool operator<(
      const SFModulatorKey & x,
      const SFModulatorKey & y) noexcept {
    return std::tie(x.source_op_, x.destination_op_, x.amount_source_op_) <
      std::tie(y.source_op_, y.destination_op_, y.amount_source_op_);
  }

  /// Indicates a SFModulatorKey object is "less than or equal to" the other one.
  /// @param x the first object to be compared.
  /// @param y the second object to be compared.
  /// @return true if a SFModulatorKey object is "less than or equal to" the other one.
  friend inline bool operator<=(
      const SFModulatorKey & x,
      const SFModulatorKey & y) noexcept {
    return std::rel_ops::operator<=(x, y);
  }

  /// Indicates a SFModulatorKey object is "greater than" the other one.
  /// @param x the first object to be compared.
  /// @param y the second object to be compared.
  /// @return true if a SFModulatorKey object is "greater than" the other one.
  friend inline bool operator>(
      const SFModulatorKey & x,
      const SFModulatorKey & y) noexcept {
    return std::rel_ops::operator>(x, y);
  }

  /// Indicates a SFModulatorKey object is "greater than or equal to" the other one.
  /// @param x the first object to be compared.
  /// @param y the second object to be compared.
  /// @return true if a SFModulatorKey object is "greater than or equal to" the other one.
  friend inline bool operator>=(
      const SFModulatorKey & x,
      const SFModulatorKey & y) noexcept {
    return std::rel_ops::operator>=(x, y);
  }

private:
  /// The source of data for the modulator.
  SFModulator source_op_;

  /// The destination of the modulator.
  SFGenerator destination_op_;

  /// The modulation source to be applied to the modulation amount.
  SFModulator amount_source_op_;
};

} // namespace sf2cute

#endif // SF2CUTE_MODULATOR_KEY_HPP_
