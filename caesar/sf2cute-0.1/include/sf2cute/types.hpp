/// @file
/// SoundFont 2 Basic Types and Structures.
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_TYPES_HPP_
#define SF2CUTE_TYPES_HPP_

#include <stdint.h>
#include <utility>
#include <tuple>
#include <string>
#include <ostream>
#include <sstream>

namespace sf2cute {

/// Values that represents both the type of sample (mono, stereo left, etc.)
/// and the whether the sample is located in RAM or ROM memory.
///
/// @remarks This enumeration represents the official SFSampleLink type.
/// @see "4.5 SoundFont 2 RIFF File Format Type Definitions".
/// In SoundFont Technical Specification 2.04.
enum class SFSampleLink : uint16_t {
  /// Mono sample
  kMonoSample = 1,
  /// Right sample
  kRightSample = 2,
  /// Left sample
  kLeftSample = 4,
  /// Linked sample
  kLinkedSample = 8,
  /// Mono sample, located in ROM
  kRomMonoSample = 0x8001,
  /// Right sample, located in ROM
  kRomRightSample = 0x8002,
  /// Left sample, located in ROM
  kRomLeftSample = 0x8004,
  /// Linked sample, located in ROM
  kRomLinkedSample = 0x8008
};

/// Values that represents the type of generator.
///
/// @remarks This enumeration represents the official SFGenerator type.
/// @see "8.1.2 Generator Enumerators Defined".
/// In SoundFont Technical Specification 2.04.
enum class SFGenerator : uint16_t {
  kStartAddrsOffset = 0,
  kEndAddrsOffset,
  kStartloopAddrsOffset,
  kEndloopAddrsOffset,
  kStartAddrsCoarseOffset,
  kModLfoToPitch,
  kVibLfoToPitch,
  kModEnvToPitch,
  kInitialFilterFc,
  kInitialFilterQ,
  kModLfoToFilterFc,
  kModEnvToFilterFc,
  kEndAddrsCoarseOffset,
  kModLfoToVolume,
  kUnused1,
  kChorusEffectsSend,
  kReverbEffectsSend,
  kPan,
  kUnused2,
  kUnused3,
  kUnused4,
  kDelayModLFO,
  kFreqModLFO,
  kDelayVibLFO,
  kFreqVibLFO,
  kDelayModEnv,
  kAttackModEnv,
  kHoldModEnv,
  kDecayModEnv,
  kSustainModEnv,
  kReleaseModEnv,
  kKeynumToModEnvHold,
  kKeynumToModEnvDecay,
  kDelayVolEnv,
  kAttackVolEnv,
  kHoldVolEnv,
  kDecayVolEnv,
  kSustainVolEnv,
  kReleaseVolEnv,
  kKeynumToVolEnvHold,
  kKeynumToVolEnvDecay,
  kInstrument,
  kReserved1,
  kKeyRange,
  kVelRange,
  kStartloopAddrsCoarseOffset,
  kKeynum,
  kVelocity,
  kInitialAttenuation,
  kReserved2,
  kEndloopAddrsCoarseOffset,
  kCoarseTune,
  kFineTune,
  kSampleID,
  kSampleModes,
  kReserved3,
  kScaleTuning,
  kExclusiveClass,
  kOverridingRootKey,
  kUnused5,
  kEndOper,
};

/// Values that represents the type of general controller.
///
/// @remarks This enumeration represents the Index field of SFModulator type,
/// while the CC field is set to 0.
/// @see "8.2.1. Source Enumerator Controller Palettes".
/// In SoundFont Technical Specification 2.04.
enum class SFGeneralController : uint8_t {
  kNoController = 0,
  kNoteOnVelocity = 2,
  kNoteOnKeyNumber = 3,
  kPolyPressure = 10,
  kChannelPressure = 13,
  kPitchWheel = 14,
  kPitchWheelSensitivity = 16,
  kLink = 127,
};

/// Values that represents the type of MIDI controller.
///
/// If you want to construct SFMidiController from a control number,
/// it can be done in `SFMidiController(number)`.
///
/// @remarks This enumeration represents the Index field of SFModulator type,
/// while the CC field is set to 1.
/// which is a kind of Modulator Source Enumerators.
/// @see General MIDI Level 2 Recommended Practice (RP024).
enum class SFMidiController : uint8_t {
  kBankSelect = 0,
  kModulationDepth,
  kController2,
  kController3,
  kController4,
  kPortamentoTime,
  kDataEntry,
  kChannelVolume,
  kController8,
  kController9,
  kPan,
  kExpression,
  kController12,
  kController13,
  kController14,
  kController15,
  kController16,
  kController17,
  kController18,
  kController19,
  kController20,
  kController21,
  kController22,
  kController23,
  kController24,
  kController25,
  kController26,
  kController27,
  kController28,
  kController29,
  kController30,
  kController31,
  kBankSelectLSB,
  kController33,
  kController34,
  kController35,
  kController36,
  kController37,
  kDataEntryLSB,
  kController39,
  kController40,
  kController41,
  kController42,
  kController43,
  kController44,
  kController45,
  kController46,
  kController47,
  kController48,
  kController49,
  kController50,
  kController51,
  kController52,
  kController53,
  kController54,
  kController55,
  kController56,
  kController57,
  kController58,
  kController59,
  kController60,
  kController61,
  kController62,
  kController63,
  kHold,
  kPortamento,
  kSostenuto,
  kSoft,
  kController68,
  kController69,
  kController70,
  kFilterResonance,
  kReleaseTime,
  kAttackTime,
  kBrightness,
  kDecayTime,
  kVibratoRate,
  kVibratoDepth,
  kVibratoDelay,
  kController79,
  kController80,
  kController81,
  kController82,
  kController83,
  kController84,
  kController85,
  kController86,
  kController87,
  kController88,
  kController89,
  kController90,
  kReverbSendLevel,
  kController92,
  kChorusSendLevel,
  kController94,
  kController95,
  kController96,
  kController97,
  kNRPNLSB,
  kNRPNMSB,
  kRPNLSB,
  kRPNMSB,
  kController102,
  kController103,
  kController104,
  kController105,
  kController106,
  kController107,
  kController108,
  kController109,
  kController110,
  kController111,
  kController112,
  kController113,
  kController114,
  kController115,
  kController116,
  kController117,
  kController118,
  kController119,
  kAllSoundOff,
  kResetAllController,
  kController122,
  kAllNotesOff,
  kOmniModeOff,
  kOmniModeOn,
  kMonoModeOn,
  kPolyModeOn,
};

/// Values that represents the type of controller palette.
///
/// @remarks This enumeration represents the CC field of SFModulator type.
/// @see "8.2 Modulator Source Enumerators".
/// In SoundFont Technical Specification 2.04.
enum class SFControllerPalette : uint8_t {
  kGeneralController = 0,
  kMidiController = 1,
};

/// Values that represents the direction of controller.
///
/// @remarks This enumeration represents the D field of SFModulator type.
/// @see "8.2 Modulator Source Enumerators".
/// In SoundFont Technical Specification 2.04.
enum class SFControllerDirection : uint8_t {
  kIncrease = 0,
  kDecrease = 1,
};

/// Values that represents the polarity of controller.
///
/// @remarks This enumeration represents the P field of SFModulator type.
/// @see "8.2 Modulator Source Enumerators".
/// In SoundFont Technical Specification 2.04.
enum class SFControllerPolarity : uint8_t {
  kUnipolar = 0,
  kBipolar = 1,
};

/// Values that represents the source type of controller.
///
/// @remarks This enumeration represents the Type field of SFModulator type.
/// @see "8.2.4 Source Types".
/// In SoundFont Technical Specification 2.04.
enum class SFControllerType : uint8_t {
  kLinear = 0,
  kConcave = 1,
  kConvex = 2,
  kSwitch = 3,
};

/// Values that represents the transform type of controller.
///
/// @remarks This enumeration represents the official SFTransform type.
/// @see "8.3 Modulator Transform Enumerators".
/// In SoundFont Technical Specification 2.04.
enum class SFTransform : uint8_t {
  kLinear = 0,
  kAbsoluteValue = 2,
};

/// Values that represents the bit flags for the sampleModes generator.
enum class SampleMode : uint16_t {
  /// Indicates a sound reproduced with no loop.
  kNoLoop = 0,
  /// Indicates a sound which loops continuously.
  kLoopContinuously,
  /// Unused but should be interpreted as indicating no loop.
  kUnusedNoLoop,
  /// Indicates a sound which loops for the duration of key depression then
  /// proceeds to play the remainder of the sample.
  kLoopEndsByKeyDepression,
};

/// The RangesType class represents a range for amount of generator.
///
/// @remarks This class represents the official rangesType type.
/// @see "4.5 SoundFont 2 RIFF File Format Type Definitions".
/// In SoundFont Technical Specification 2.04.
struct RangesType {
  /// Constructs a new zero-valued range.
  RangesType() noexcept :
      lo(0),
      hi(0) {
  }

  /// Constructs a new range using the specified range.
  /// @param lo the low end of range.
  /// @param hi the high end of range.
  RangesType(uint8_t lo, uint8_t hi) noexcept :
      lo(std::move(lo)),
      hi(std::move(hi)) {
  }

  /// The low end of range.
  uint8_t lo;

  /// The high end of range.
  uint8_t hi;
};

/// The GenAmountType class represents an amount of generator.
///
/// @remarks This class represents the official genAmountType type.
/// @see "4.5 SoundFont 2 RIFF File Format Type Definitions".
/// In SoundFont Technical Specification 2.04.
union GenAmountType {
  /// Constructs a new zero-valued GenAmountType.
  GenAmountType() noexcept :
      value(0) {
  }

  /// Constructs a new GenAmountType using the specified range.
  /// @param range the range.
  GenAmountType(RangesType range) noexcept :
      range(std::move(range)) {
  }

  /// Constructs a new GenAmountType using the specified integer value.
  /// @param value the amount.
  GenAmountType(int16_t value) noexcept :
      value(std::move(value)) {
  }

  /// Range of the amount.
  RangesType range;

  /// Integer value of the amount.
  int16_t value;

  /// Unsigned integer value of the amount.
  uint16_t uvalue;
};

struct SFVersionTag;

/// The SFVersionTag class represents a version tag.
///
/// @remarks This class represents the official sfVersionTag type.
/// @see "5.1 The ifil Sub-chunk".
/// In SoundFont Technical Specification 2.04.
struct SFVersionTag {
  /// Constructs a new zero-valued SFVersionTag.
  SFVersionTag() noexcept :
      major(0),
      minor(0) {
  };

  /// Constructs a new SFVersionTag using the specified version numbers.
  /// @param major the major version number.
  /// @param minor the minor version number.
  SFVersionTag(uint16_t major, uint16_t minor) noexcept :
      major(std::move(major)),
      minor(std::move(minor)) {
  };

  /// Indicates a SFVersionTag object is "equal to" the other one.
  /// @param x the first object to be compared.
  /// @param y the second object to be compared.
  /// @return true if a SFVersionTag object is "equal to" the other one.
  friend inline bool operator==(
      const SFVersionTag & x,
      const SFVersionTag & y) noexcept {
    return std::tie(x.major, x.minor) == std::tie(y.major, y.minor);
  }

  /// Indicates a SFVersionTag object is "not equal to" the other one.
  /// @param x the first object to be compared.
  /// @param y the second object to be compared.
  /// @return true if a SFVersionTag object is "not equal to" the other one.
  friend inline bool operator!=(
      const SFVersionTag & x,
      const SFVersionTag & y) noexcept {
    return std::rel_ops::operator!=(x, y);
  }

  /// Indicates a SFVersionTag object is "less than" the other one.
  /// @param x the first object to be compared.
  /// @param y the second object to be compared.
  /// @return true if a SFVersionTag object is "less than" the other one.
  friend inline bool operator<(
      const SFVersionTag & x,
      const SFVersionTag & y) noexcept {
    return std::tie(x.major, x.minor) < std::tie(y.major, y.minor);
  }

  /// Indicates a SFVersionTag object is "less than or equal to" the other one.
  /// @param x the first object to be compared.
  /// @param y the second object to be compared.
  /// @return true if a SFVersionTag object is "less than or equal to" the other one.
  friend inline bool operator<=(
      const SFVersionTag & x,
      const SFVersionTag & y) noexcept {
    return std::rel_ops::operator<=(x, y);
  }

  /// Indicates a SFVersionTag object is "greater than" the other one.
  /// @param x the first object to be compared.
  /// @param y the second object to be compared.
  /// @return true if a SFVersionTag object is "greater than" the other one.
  friend inline bool operator>(
      const SFVersionTag & x,
      const SFVersionTag & y) noexcept {
    return std::rel_ops::operator>(x, y);
  }

  /// Indicates a SFVersionTag object is "greater than or equal to" the other one.
  /// @param x the first object to be compared.
  /// @param y the second object to be compared.
  /// @return true if a SFVersionTag object is "greater than or equal to" the other one.
  friend inline bool operator>=(
      const SFVersionTag & x,
      const SFVersionTag & y) noexcept {
    return std::rel_ops::operator>=(x, y);
  }

  /// Outputs the version number to the specified output stream.
  /// @param out the output stream.
  /// @param version the version tag.
  friend inline std::ostream & operator<<(
      std::ostream & out,
      const SFVersionTag & version) {
    return out << version.to_string();
  }

  /// Returns a string represents the version.
  /// @return a string representation of the version.
  inline std::string to_string() const {
    std::ostringstream string_builder;
    string_builder << major << "." << minor;
    return string_builder.str();
  }

  /// Major version number.
  uint16_t major;

  /// Minor version number.
  uint16_t minor;
};

} // namespace sf2cute

namespace std
{
  /// The hash template for the sf2cute::SFVersionTag class.
  template <>
  struct hash<sf2cute::SFVersionTag>
  {
    /// Calculates the hash of the argument.
    /// @param key the object to be hashed.
    /// @return the hash value.
    std::size_t operator()(sf2cute::SFVersionTag const & key) const noexcept {
      return std::hash<uint32_t>()((static_cast<uint32_t>(key.major) << 16) + key.minor);
    }
  };
} // namespace std

#endif // SF2CUTE_TYPES_HPP_
