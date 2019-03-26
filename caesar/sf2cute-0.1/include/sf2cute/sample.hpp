/// @file
/// SoundFont 2 Sample class header.
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_SAMPLE_HPP_
#define SF2CUTE_SAMPLE_HPP_

#include <stdint.h>
#include <memory>
#include <utility>
#include <vector>

#include "types.hpp"

namespace sf2cute {

class SFSample;
class SoundFont;
class SoundFontWriter;

/// The SFSample class represents a sample header and data.
///
/// @remarks This class represents the official sfSample type and
/// sample data stored in "smpl" chunk.
/// @see "6.1 Sample Data Format in the smpl Sub-chunk".
/// In SoundFont Technical Specification 2.04.
/// @see "7.10 The SHDR Sub-chunk".
/// In SoundFont Technical Specification 2.04.
class SFSample {
  friend class SoundFont;
  friend class SoundFontWriter;

public:
  /// Maximum length of sample name (excluding the terminator byte), in terms of bytes.
  static constexpr std::string::size_type kMaxNameLength = 20 - 1;

  /// The length of terminator samples, in sample data points.
  static constexpr uint32_t kTerminatorSampleLength = 46;

  /// Constructs a new empty SFSample.
  SFSample();

  /// Constructs a new empty SFSample using the specified name.
  /// @param name the name of the sample.
  explicit SFSample(std::string name);

  /// Constructs a new SFSample.
  /// @param name the name of the sample.
  /// @param data the sample data
  /// @param start_loop the beginning index of the loop, in sample data points, inclusive.
  /// @param end_loop the ending index of the loop, in sample data points, exclusive.
  /// @param sample_rate the sample rate, in hertz.
  /// @param original_key the MIDI key number of the recorded pitch of the sample.
  /// @param correction the pitch correction that should be applied to the sample, in cents.
  SFSample(std::string name,
      std::vector<int16_t> data,
      uint32_t start_loop,
      uint32_t end_loop,
      uint32_t sample_rate,
      uint8_t original_key,
      int8_t correction);

  /// Constructs a new SFSample with a sample link.
  /// @param name the name of the sample.
  /// @param data the sample data
  /// @param start_loop the beginning index of the loop, in sample data points, inclusive.
  /// @param end_loop the ending index of the loop, in sample data points, exclusive.
  /// @param sample_rate the sample rate, in hertz.
  /// @param original_key the MIDI key number of the recorded pitch of the sample.
  /// @param correction the pitch correction that should be applied to the sample, in cents.
  /// @param link the associated right or left stereo sample. nullptr is allowed.
  /// @param type both the type of sample and the whether the sample is located in RAM or ROM memory.
  SFSample(std::string name,
      std::vector<int16_t> data,
      uint32_t start_loop,
      uint32_t end_loop,
      uint32_t sample_rate,
      uint8_t original_key,
      int8_t correction,
      std::weak_ptr<SFSample> link,
      SFSampleLink type);

  /// Constructs a new copy of specified SFSample.
  /// @param origin a SFSample object.
  SFSample(const SFSample & origin);

  /// Copy-assigns a new value to the SFSample, replacing its current contents.
  /// @param origin a SFSample object.
  SFSample & operator=(const SFSample & origin);

  /// Acquires the contents of specified SFSample.
  /// @param origin a SFSample object.
  SFSample(SFSample && origin) = default;

  /// Move-assigns a new value to the SFSample, replacing its current contents.
  /// @param origin a SFSample object.
  SFSample & operator=(SFSample && origin) = default;

  /// Destructs the SFSample.
  ~SFSample() = default;

  /// Constructs a new SFSample.
  /// @param args the arguments for the SFSample constructor.
  /// @return the new SFSample object.
  template<typename ... Args>
  static std::shared_ptr<SFSample> New(Args && ... args) {
    return std::make_shared<SFSample>(std::forward<Args>(args)...);
  }

  /// Returns the name of this sample.
  /// @return the name of this sample.
  const std::string & name() const noexcept;

  /// Sets the name of this sample.
  /// @param name the name of this sample.
  void set_name(std::string name);

  /// Returns the starting point of the loop of this sample.
  /// @return the beginning index of the loop, in sample data points, inclusive.
  uint32_t start_loop() const noexcept;

  /// Sets the starting point of the loop of this sample.
  /// @param start_loop the beginning index of the loop, in sample data points, inclusive.
  void set_start_loop(uint32_t start_loop);

  /// Returns the ending point of the loop of this sample.
  /// @return the ending index of the loop, in sample data points, exclusive.
  uint32_t end_loop() const noexcept;

  /// Sets the ending point of the loop of this sample.
  /// @param end_loop the ending index of the loop, in sample data points, exclusive.
  void set_end_loop(uint32_t end_loop);

  /// Returns the sample rate.
  /// @return the sample rate, in hertz.
  uint32_t sample_rate() const noexcept;

  /// Sets the sample rate.
  /// @param sample_rate the sample rate, in hertz.
  void set_sample_rate(uint32_t sample_rate);

  /// Returns the original MIDI key number of this sample.
  /// @return the MIDI key number of the recorded pitch of the sample.
  uint8_t original_key() const noexcept;

  /// Sets the original MIDI key number of this sample.
  /// @param original_key the MIDI key number of the recorded pitch of the sample.
  void set_original_key(uint8_t original_key);

  /// Returns the pitch correction.
  /// @return the pitch correction that should be applied to the sample, in cents.
  int8_t correction() const noexcept;

  /// Sets the pitch correction.
  /// @param correction the pitch correction that should be applied to the sample, in cents.
  void set_correction(int8_t correction);

  /// Returns the associated right or left stereo sample.
  /// @return a pointer to the associated right or left stereo sample.
  /// @remarks This function returns nullptr if the associated sample has been deleted.
  std::shared_ptr<SFSample> link() const noexcept;

  /// Returns true if this sample has an associated sample.
  /// @return true if this sample has an associated sample.
  /// @remarks This function returns false if the associated sample has been deleted.
  bool has_link() const noexcept;

  /// Sets the associated right or left stereo sample.
  /// @param link a pointer to the associated right or left stereo sample.
  void set_link(std::weak_ptr<SFSample> link);

  /// Resets the associated right or left stereo sample.
  void reset_link() noexcept;

  /// Returns both the type of sample and the whether the sample is located in RAM or ROM memory.
  /// @return both the type of sample and the whether the sample is located in RAM or ROM memory.
  SFSampleLink type() const noexcept;

  /// Sets both the type of sample and the whether the sample is located in RAM or ROM memory.
  /// @param type both the type of sample and the whether the sample is located in RAM or ROM memory.
  void set_type(SFSampleLink type);

  /// Returns the sample data.
  /// @return the sample data.
  const std::vector<int16_t> & data() const noexcept;

  /// Returns true if this sample has a parent file.
  /// @return true if this sample has a parent file.
  bool has_parent_file() const noexcept;

  /// Returns the parent file.
  /// @return a reference to the parent file.
  SoundFont & parent_file() const noexcept;

private:
  /// Sets the parent file.
  /// @param parent_file the parent file.
  void set_parent_file(SoundFont & parent_file) noexcept;

  /// Resets the parent file.
  void reset_parent_file() noexcept;

  /// The name of sample.
  std::string name_;

  /// The starting point of the loop, inclusive.
  uint32_t start_loop_;

  /// The ending point of the loop, exclusive.
  uint32_t end_loop_;

  /// The sample rate, in hertz.
  uint32_t sample_rate_;

  /// The MIDI key number of the recorded pitch.
  uint8_t original_key_;

  /// The pitch correction, in cents.
  int8_t correction_;

  /// The associated right or left stereo sample.
  std::weak_ptr<SFSample> link_;

  /// Both the type of sample and the whether the sample is located in RAM or ROM memory.
  SFSampleLink type_;

  /// The sample data.
  std::vector<int16_t> data_;

  /// The parent file.
  SoundFont * parent_file_;
};

} // namespace sf2cute

#endif // SF2CUTE_SAMPLE_HPP_
