/// @file
/// SoundFont 2 Modulator's Controller class implementation.
///
/// @author gocha <https://github.com/gocha>

#include <sf2cute/modulator.hpp>

#include <stdint.h>

namespace sf2cute {

/// Constructs a new SFModulator with no controller.
SFModulator::SFModulator() :
    controller_(0),
    controller_palette_(SFControllerPalette(0)),
    direction_(SFControllerDirection(0)),
    polarity_(SFControllerPolarity(0)),
    type_(SFControllerType(0)) {
}

/// Constructs a new SFModulator from a pre-encoded integer.
SFModulator::SFModulator(uint16_t value) :
    controller_(value & 0x7f),
    controller_palette_(SFControllerPalette((value >> 7) & 1)),
    direction_(SFControllerDirection((value >> 8) & 1)),
    polarity_(SFControllerPolarity((value >> 9) & 1)),
    type_(SFControllerType((value >> 10) & 0x3f)) {
}

/// Constructs a new SFModulator with a general controller.
SFModulator::SFModulator(SFGeneralController controller,
    SFControllerDirection direction,
    SFControllerPolarity polarity,
    SFControllerType type) :
    controller_(uint8_t(controller)),
    controller_palette_(SFControllerPalette::kGeneralController),
    direction_(direction),
    polarity_(polarity),
    type_(type) {
}

/// Constructs a new SFModulator with a MIDI controller.
SFModulator::SFModulator(SFMidiController controller,
    SFControllerDirection direction,
    SFControllerPolarity polarity,
    SFControllerType type) :
    controller_(uint8_t(controller)),
    controller_palette_(SFControllerPalette::kMidiController),
    direction_(direction),
    polarity_(polarity),
    type_(type) {
}

/// Returns the index of the controller.
uint8_t SFModulator::controller() const noexcept {
  return controller_;
}

/// Returns the type of this general controller.
SFGeneralController SFModulator::general_controller() const noexcept {
  return SFGeneralController(controller_);
}

/// Returns the type of this MIDI controller.
SFMidiController SFModulator::midi_controller() const noexcept {
  return SFMidiController(controller_);
}

/// Sets the index of the controller
void SFModulator::set_controller(uint8_t controller) {
  controller_ = std::move(controller);
}

/// Sets the general controller index and the type of controller palette.
void SFModulator::set_general_controller(SFGeneralController controller) {
  controller_ = std::move(static_cast<uint8_t>(controller));
  controller_palette_ = std::move(SFControllerPalette::kGeneralController);
}

/// Sets the MIDI controller index and the type of controller palette.
void SFModulator::set_midi_controller(SFMidiController controller) {
  controller_ = std::move(static_cast<uint8_t>(controller));
  controller_palette_ = std::move(SFControllerPalette::kMidiController);
}

/// Returns the type of controller palette.
SFControllerPalette SFModulator::controller_palette() const noexcept {
  return controller_palette_;
}

/// Sets the type of controller palette.
void SFModulator::set_controller_palette(SFControllerPalette controller_palette) {
  controller_palette_ = std::move(controller_palette);
}

/// Returns the direction of the controller.
SFControllerDirection SFModulator::direction() const noexcept {
  return direction_;
}

/// Sets the direction of the controller.
void SFModulator::set_direction(SFControllerDirection direction) {
  direction_ = std::move(direction);
}

/// Returns the polarity of the controller.
SFControllerPolarity SFModulator::polarity() const noexcept {
  return polarity_;
}

/// Sets the polarity of the controller.
void SFModulator::set_polarity(SFControllerPolarity polarity) {
  polarity_ = std::move(polarity);
}

/// Returns the source type of the controller.
SFControllerType SFModulator::type() const noexcept {
  return type_;
}

/// Sets the source type of the controller.
void SFModulator::set_type(SFControllerType type) {
  type_ = std::move(type);
}

/// Converts SFModulator to an integer value.
SFModulator::operator uint16_t() const noexcept {
  return (controller() & 0x7f) |
    ((static_cast<uint16_t>(controller_palette_) & 1) << 7) |
    ((static_cast<uint16_t>(direction_) & 1) << 8) |
    ((static_cast<uint16_t>(polarity_) & 1) << 9) |
    ((static_cast<uint16_t>(type_) & 0x3f) << 10);
}

} // namespace sf2cute
