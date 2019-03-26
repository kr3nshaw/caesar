/// @file
/// SoundFont 2 Generator class implementation.
///
/// @author gocha <https://github.com/gocha>

#include <sf2cute/generator_item.hpp>

#include <array>
#include <utility>

namespace sf2cute {

/// Constructs a new SFGeneratorItem.
SFGeneratorItem::SFGeneratorItem() :
    op_(SFGenerator(0)),
    amount_(0) {
}

/// Constructs a new SFGeneratorItem using the specified properties.
SFGeneratorItem::SFGeneratorItem(SFGenerator op, GenAmountType amount) :
    op_(std::move(op)),
    amount_(std::move(amount)) {
}

/// Returns the type of the generator.
SFGenerator SFGeneratorItem::op() const noexcept {
  return op_;
}

/// Sets the type of the generator.
void SFGeneratorItem::set_op(SFGenerator op) {
  op_ = std::move(op);
}

/// Returns the amount of the generator.
GenAmountType SFGeneratorItem::amount() const noexcept {
  return amount_;
}

/// Sets the amount of the generator.
void SFGeneratorItem::set_amount(GenAmountType amount) {
  amount_ = std::move(amount);
}

/// Sets the amount of the generator in a range.
void SFGeneratorItem::set_amount(uint8_t lo, uint8_t hi) {
  amount_.range.lo = std::move(lo);
  amount_.range.hi = std::move(hi);
}

/// Sets the amount of the generator in an integer.
void SFGeneratorItem::set_amount(int16_t amount) {
  amount_.value = std::move(amount);
}

/// Sets the amount of the generator in an unsigned integer.
void SFGeneratorItem::set_amount(uint16_t amount) {
  amount_.uvalue = std::move(amount);
}

/// Indicates a SFGenerator object is "less than" the other one.
bool SFGeneratorItem::Compare(const SFGenerator & x, const SFGenerator & y) noexcept {
  std::array<SFGenerator, 2> firstElements{ SFGenerator::kKeyRange, SFGenerator::kVelRange };
  std::array<SFGenerator, 2> lastElements{ SFGenerator::kSampleID, SFGenerator::kInstrument };

  if (x == y) {
    return false;
  }

  for (const SFGenerator & first : firstElements) {
    if (x == first) {
      return true;
    }
    else if (y == first) {
      return false;
    }
  }

  for (const SFGenerator & last : lastElements) {
    if (x == last) {
      return false;
    }
    else if (y == last) {
      return true;
    }
  }

  return x < y;
}

} // namespace sf2cute
