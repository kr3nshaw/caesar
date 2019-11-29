/// @file
/// SoundFont 2 Key members of Modulator class implementation.
///
/// @author gocha <https://github.com/gocha>

#include <sf2cute/modulator_key.hpp>

#include <stdint.h>

namespace sf2cute {

/// Constructs a new SFModulatorKey.
SFModulatorKey::SFModulatorKey() :
    source_op_(SFModulator(0)),
    destination_op_(SFGenerator(0)),
    amount_source_op_(SFModulator(0)) {
}

/// Constructs a new SFModulatorKey using the specified controllers.
SFModulatorKey::SFModulatorKey(SFModulator source_op,
    SFGenerator destination_op,
    SFModulator amount_source_op) :
    source_op_(std::move(source_op)),
    destination_op_(std::move(destination_op)),
    amount_source_op_(std::move(amount_source_op)) {
}

} // namespace sf2cute
