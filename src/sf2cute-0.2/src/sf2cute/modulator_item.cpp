/// @file
/// SoundFont 2 Modulator class implementation.
///
/// @author gocha <https://github.com/gocha>

#include <sf2cute/modulator_item.hpp>

namespace sf2cute {

/// Constructs a new SFModulatorItem.
SFModulatorItem::SFModulatorItem() :
    key_(SFModulator(0), SFGenerator(0), SFModulator(0)),
    amount_(0),
    transform_op_(SFTransform(0)) {
}

/// Constructs a new SFModulatorItem using the specified controllers.
SFModulatorItem::SFModulatorItem(SFModulator source_op,
    SFGenerator destination_op,
    int16_t amount,
    SFModulator amount_source_op,
    SFTransform transform_op) :
    key_(std::move(source_op), std::move(destination_op), std::move(amount_source_op)),
    amount_(amount),
    transform_op_(transform_op) {
}

} // namespace sf2cute
