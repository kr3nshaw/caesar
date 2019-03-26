/// @file
/// SoundFont 2 Modulator's Controller class header.
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_MODULATOR_HPP_
#define SF2CUTE_MODULATOR_HPP_

#include <stdint.h>
#include <utility>

#include "types.hpp"

namespace sf2cute {

class SFModulator;

/// The SFModulator class represents a controller in a modulator.
///
/// @remarks This class represents the official sfModulator type.
/// @see "8.2 Modulator Source Enumerators".
/// In SoundFont Technical Specification 2.04.
class SFModulator {
public:
  /// Constructs a new SFModulator with no controller.
  SFModulator();

  /// Constructs a new SFModulator from a pre-encoded integer.
  /// @param value the value that represents a SFModulator
  explicit SFModulator(uint16_t value);

  /// Constructs a new SFModulator with a general controller.
  /// @param controller the type of general controller.
  /// @param direction the direction of the controller.
  /// @param polarity the polarity of the controller.
  /// @param type the source type of the controller.
  SFModulator(SFGeneralController controller,
      SFControllerDirection direction,
      SFControllerPolarity polarity,
      SFControllerType type);

  /// Constructs a new SFModulator with a MIDI controller.
  /// @param controller the type of MIDI controller.
  /// @param direction the direction of the controller.
  /// @param polarity the polarity of the controller.
  /// @param type the source type of the controller.
  SFModulator(SFMidiController controller,
      SFControllerDirection direction,
      SFControllerPolarity polarity,
      SFControllerType type);

  /// Constructs a new copy of specified SFModulator.
  /// @param origin a SFModulator object.
  SFModulator(const SFModulator & origin) = default;

  /// Copy-assigns a new value to the SFModulator, replacing its current contents.
  /// @param origin a SFModulator object.
  SFModulator & operator=(const SFModulator & origin) = default;

  /// Acquires the contents of specified SFModulator.
  /// @param origin a SFModulator object.
  SFModulator(SFModulator && origin) = default;

  /// Move-assigns a new value to the SFModulator, replacing its current contents.
  /// @param origin a SFModulator object.
  SFModulator & operator=(SFModulator && origin) = default;

  /// Destructs the SFModulator.
  ~SFModulator() = default;

  /// Returns the index of the controller.
  /// @return the index of the controller
  /// @remarks Using general_controller() or midi_controller() is recommended if you want to process a specific controller type.
  /// @see general_controller()
  /// @see midi_controller()
  /// @see controller_palette()
  uint8_t controller() const noexcept;

  /// Returns the type of this general controller.
  /// @return the type of this general controller.
  /// @remarks Verify the type of controller palette before getting the value.
  /// @see controller_palette()
  SFGeneralController general_controller() const noexcept;

  /// Returns the type of this MIDI controller.
  /// @return the type of this MIDI controller.
  /// @remarks Verify the type of controller palette before getting the value.
  /// @see controller_palette()
  SFMidiController midi_controller() const noexcept;

  /// Sets the index of the controller
  /// @param controller the index of the controller.
  /// @remarks Using set_general_controller(SFGeneralController) or set_midi_controller(SFMidiController) is recommended if you want to process a specific controller type.
  /// @see set_general_controller(SFGeneralController)
  /// @see set_midi_controller(SFMidiController)
  void set_controller(uint8_t controller);

  /// Sets the general controller index and the type of controller palette.
  /// @param controller the type of general controller.
  void set_general_controller(SFGeneralController controller);

  /// Sets the MIDI controller index and the type of controller palette.
  /// @param controller the type of MIDI controller.
  void set_midi_controller(SFMidiController controller);

  /// Returns the type of controller palette.
  /// @return the type of controller palette.
  SFControllerPalette controller_palette() const noexcept;

  /// Sets the type of controller palette.
  /// @param controller_palette the type of controller palette.
  /// @remarks Using set_general_controller(SFGeneralController) or set_midi_controller(SFMidiController) is recommended.
  /// @see set_general_controller(SFGeneralController)
  /// @see set_midi_controller(SFGeneralController)
  void set_controller_palette(SFControllerPalette controller_palette);

  /// Returns the direction of the controller.
  /// @return the direction of the controller.
  SFControllerDirection direction() const noexcept;

  /// Sets the direction of the controller.
  /// @param direction the direction of the controller.
  void set_direction(SFControllerDirection direction);

  /// Returns the polarity of the controller.
  /// @return the polarity of the controller.
  SFControllerPolarity polarity() const noexcept;

  /// Sets the polarity of the controller.
  /// @param polarity the polarity of the controller.
  void set_polarity(SFControllerPolarity polarity);

  /// Returns the source type of the controller.
  /// @return the source type of the controller.
  SFControllerType type() const noexcept;

  /// Sets the source type of the controller.
  /// @param type the source type of the controller.
  void set_type(SFControllerType type);

  /// Converts SFModulator to an integer value.
  operator uint16_t() const noexcept;

  /// Indicates a SFModulator object is "equal to" the other one.
  /// @param x the first object to be compared.
  /// @param y the second object to be compared.
  /// @return true if a SFModulator object is "equal to" the other one.
  friend inline bool operator==(const SFModulator & x, const SFModulator & y) noexcept {
    return uint16_t(x) == uint16_t(y);
  }

  /// Indicates a SFModulator object is "not equal to" the other one.
  /// @param x the first object to be compared.
  /// @param y the second object to be compared.
  /// @return true if a SFModulator object is "not equal to" the other one.
  friend inline bool operator!=(const SFModulator & x, const SFModulator & y) noexcept {
    return std::rel_ops::operator!=(x, y);
  }

  /// Indicates a SFModulator object is "less than" the other one.
  /// @param x the first object to be compared.
  /// @param y the second object to be compared.
  /// @return true if a SFModulator object is "less than" the other one.
  friend inline bool operator<(const SFModulator & x, const SFModulator & y) noexcept {
    return uint16_t(x) < uint16_t(y);
  }

  /// Indicates a SFModulator object is "less than or equal to" the other one.
  /// @param x the first object to be compared.
  /// @param y the second object to be compared.
  /// @return true if a SFModulator object is "less than or equal to" the other one.
  friend inline bool operator<=(const SFModulator & x, const SFModulator & y) noexcept {
    return std::rel_ops::operator<=(x, y);
  }

  /// Indicates a SFModulator object is "greater than" the other one.
  /// @param x the first object to be compared.
  /// @param y the second object to be compared.
  /// @return true if a SFModulator object is "greater than" the other one.
  friend inline bool operator>(const SFModulator & x, const SFModulator & y) noexcept {
    return std::rel_ops::operator>(x, y);
  }

  /// Indicates a SFModulator object is "greater than or equal to" the other one.
  /// @param x the first object to be compared.
  /// @param y the second object to be compared.
  /// @return true if a SFModulator object is "greater than or equal to" the other one.
  friend inline bool operator>=(const SFModulator & x, const SFModulator & y) noexcept {
    return std::rel_ops::operator>=(x, y);
  }

private:
  /// The index of the controller.
  uint8_t controller_;

  /// The type of controller palette.
  SFControllerPalette controller_palette_;

  /// The direction of the controller.
  SFControllerDirection direction_;

  /// The polarity of the controller.
  SFControllerPolarity polarity_;

  /// The source type of the controller.
  SFControllerType type_;
};

} // namespace sf2cute

namespace std
{
  /// The hash template for the sf2cute::SFModulator class.
  template <>
  struct hash<sf2cute::SFModulator>
  {
    /// Calculates the hash of the argument.
    /// @param key the object to be hashed.
    /// @return the hash value.
    std::size_t operator()(sf2cute::SFModulator const & key) const noexcept {
      return std::hash<uint16_t>()(uint16_t(key));
    }
  };
} // namespace std

#endif // SF2CUTE_MODULATOR_HPP_
