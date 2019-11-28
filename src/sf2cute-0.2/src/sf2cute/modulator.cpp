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

} // namespace sf2cute
