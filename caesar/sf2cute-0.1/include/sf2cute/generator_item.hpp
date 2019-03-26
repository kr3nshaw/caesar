/// @file
/// SoundFont 2 Generator class header.
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_GENERATOR_ITEM_HPP_
#define SF2CUTE_GENERATOR_ITEM_HPP_

#include <stdint.h>

#include "types.hpp"

namespace sf2cute {

/// The SFGeneratorItem class represents a generator.
///
/// @remarks This class represents the official sfGenList/sfInstGenList type.
/// @see "7.5 The PGEN Sub-chunk". In SoundFont Technical Specification 2.04.
/// @see "7.9 The IGEN Sub-chunk". In SoundFont Technical Specification 2.04.
class SFGeneratorItem {
public:
  /// Constructs a new SFGeneratorItem.
  SFGeneratorItem();

  /// Constructs a new SFGeneratorItem using the specified properties.
  /// @param op the type of the generator.
  /// @param amount the value to be assigned to the generator.
  SFGeneratorItem(SFGenerator op, GenAmountType amount);

  /// Constructs a new copy of specified SFGeneratorItem.
  /// @param origin a SFGeneratorItem object.
  SFGeneratorItem(const SFGeneratorItem & origin) = default;

  /// Copy-assigns a new value to the SFGeneratorItem, replacing its current contents.
  /// @param origin a SFGeneratorItem object.
  SFGeneratorItem & operator=(const SFGeneratorItem & origin) = default;

  /// Acquires the contents of specified SFGeneratorItem.
  /// @param origin a SFGeneratorItem object.
  SFGeneratorItem(SFGeneratorItem && origin) = default;

  /// Move-assigns a new value to the SFGeneratorItem, replacing its current contents.
  /// @param origin a SFGeneratorItem object.
  SFGeneratorItem & operator=(SFGeneratorItem && origin) = default;

  /// Destructs the SFGeneratorItem.
  ~SFGeneratorItem() = default;

  /// Returns the type of the generator.
  /// @return the type of the generator.
  SFGenerator op() const noexcept;

  /// Sets the type of the generator.
  /// @param op the type of the generator.
  void set_op(SFGenerator op);

  /// Returns the amount of the generator.
  /// @return the value represents the amount of the generator.
  GenAmountType amount() const noexcept;

  /// Sets the amount of the generator.
  /// @param amount the value to be assigned to the generator.
  void set_amount(GenAmountType amount);

  /// Sets the amount of the generator in a range.
  /// @param lo the low end of range.
  /// @param hi the high end of range.
  void set_amount(uint8_t lo, uint8_t hi);

  /// Sets the amount of the generator in an integer.
  /// @param amount the value to be assigned to the generator.
  void set_amount(int16_t amount);

  /// Sets the amount of the generator in an unsigned integer.
  /// @param amount the value to be assigned to the generator.
  void set_amount(uint16_t amount);

  /// Indicates a SFGenerator object is "less than" the other one.
  /// @param x the first SFGenerator to be compared.
  /// @param y the second SFGenerator to be compared.
  /// @return true if a SFGenerator object is "less than" the other one.
  ///
  /// @remarks This function compares two SFGenerator enums based on the ordering requirement of a generator chunk.
  static bool Compare(const SFGenerator & x, const SFGenerator & y) noexcept;

private:
  /// The type of the generator.
  SFGenerator op_;

  /// The amount of the generator.
  GenAmountType amount_;
};

} // namespace sf2cute

#endif // SF2CUTE_GENERATOR_ITEM_HPP_
