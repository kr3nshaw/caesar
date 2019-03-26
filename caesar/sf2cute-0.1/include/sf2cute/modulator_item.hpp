/// @file
/// SoundFont 2 Modulator class header.
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_MODULATOR_ITEM_HPP_
#define SF2CUTE_MODULATOR_ITEM_HPP_

#include <stdint.h>

#include "types.hpp"
#include "modulator.hpp"
#include "modulator_key.hpp"

namespace sf2cute {

/// The SFModulatorItem class represents a modulator.
///
/// @remarks This class represents the official sfModList/sfInstModList type.
/// @see "7.4 The PMOD Sub-chunk". In SoundFont Technical Specification 2.04.
/// @see "7.8 The IMOD Sub-chunk". In SoundFont Technical Specification 2.04.
class SFModulatorItem {
public:
  /// Constructs a new SFModulatorItem.
  SFModulatorItem();

  /// Constructs a new SFModulatorItem using the specified controllers.
  /// @param source_op the source of data for the modulator.
  /// @param destination_op the destination of the modulator.
  /// @param amount the degree to which the source modulates the destination.
  /// @param amount_source_op the modulation source to be applied to the modulation amount.
  /// @param transform_op the transform type to be applied to the modulation source.
  SFModulatorItem(SFModulator source_op,
      SFGenerator destination_op,
      int16_t amount,
      SFModulator amount_source_op,
      SFTransform transform_op);

  /// Constructs a new copy of specified SFModulatorItem.
  /// @param origin a SFModulatorItem object.
  SFModulatorItem(const SFModulatorItem & origin) = default;

  /// Copy-assigns a new value to the SFModulatorItem, replacing its current contents.
  /// @param origin a SFModulatorItem object.
  SFModulatorItem & operator=(const SFModulatorItem & origin) = default;

  /// Acquires the contents of specified SFModulatorItem.
  /// @param origin a SFModulatorItem object.
  SFModulatorItem(SFModulatorItem && origin) = default;

  /// Move-assigns a new value to the SFModulatorItem, replacing its current contents.
  /// @param origin a SFModulatorItem object.
  SFModulatorItem & operator=(SFModulatorItem && origin) = default;

  /// Destructs the SFModulatorItem.
  ~SFModulatorItem() = default;

  /// Returns the unique key of the modulator.
  /// @return the unique key of the modulator.
  /// @remarks Using source_op(), destination_op() or amount_source_op() is recommended for an individual member access.
  /// @see source_op()
  /// @see destination_op()
  /// @see amount_source_op()
  SFModulatorKey key() const noexcept;

  /// Sets the unique key of the modulator.
  /// @param key the unique key of the modulator.
  /// @remarks Using set_source_op(SFModulator), set_destination_op(SFGenerator) or set_amount_source_op(SFModulator) is recommended for an individual member access.
  /// @see set_source_op(SFModulator)
  /// @see set_destination_op(SFGenerator)
  /// @see set_amount_source_op(SFModulator)
  void set_key(SFModulatorKey key);

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

  /// Returns the constant of modulation amount.
  /// @return the constant of modulation amount.
  int16_t amount() const noexcept;

  /// Sets the constant of modulation amount.
  /// @param amount the degree to which the source modulates the destination.
  void set_amount(int16_t amount);

  /// Returns the modulation source to be applied to the modulation amount.
  /// @return the modulation source to be applied to the modulation amount.
  SFModulator amount_source_op() const noexcept;

  /// Sets the modulation source to be applied to the modulation amount.
  /// @param amount_source_op the modulation source to be applied to the modulation amount.
  void set_amount_source_op(SFModulator amount_source_op);

  /// Returns the transform type to be applied to the modulation source.
  /// @return the transform type to be applied to the modulation source.
  SFTransform transform_op() const noexcept;

  /// Sets the transform type to be applied to the modulation source.
  /// @param transform_op the transform type to be applied to the modulation source.
  void set_transform_op(SFTransform transform_op);

private:
  /// The unique key of the modulator.
  SFModulatorKey key_;

  /// The degree to which the source modulates the destination.
  int16_t amount_;

  /// The transform type to be applied to the modulation source.
  SFTransform transform_op_;
};

} // namespace sf2cute

#endif // SF2CUTE_MODULATOR_ITEM_HPP_
