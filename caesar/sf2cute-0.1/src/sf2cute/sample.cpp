/// @file
/// SoundFont 2 Sample class implementation.
///
/// @author gocha <https://github.com/gocha>

#include <sf2cute/sample.hpp>

#include <stdint.h>
#include <memory>
#include <algorithm>
#include <string>
#include <vector>

namespace sf2cute {

/// Constructs a new empty SFSample.
SFSample::SFSample() :
    start_loop_(0),
    end_loop_(0),
    sample_rate_(0),
    original_key_(0),
    correction_(0),
    link_(),
    type_(SFSampleLink::kMonoSample),
    parent_file_(nullptr) {
}

/// Constructs a new empty SFSample using the specified name.
SFSample::SFSample(std::string name) :
    name_(std::move(name)),
    start_loop_(0),
    end_loop_(0),
    sample_rate_(0),
    original_key_(0),
    correction_(0),
    link_(),
    type_(SFSampleLink::kMonoSample),
    parent_file_(nullptr) {
}

/// Constructs a new SFSample.
SFSample::SFSample(std::string name,
    std::vector<int16_t> data,
    uint32_t start_loop,
    uint32_t end_loop,
    uint32_t sample_rate,
    uint8_t original_key,
    int8_t correction) :
    name_(std::move(name)),
    data_(std::move(data)),
    start_loop_(std::move(start_loop)),
    end_loop_(std::move(end_loop)),
    sample_rate_(std::move(sample_rate)),
    original_key_(std::move(original_key)),
    correction_(std::move(correction)),
    link_(),
    type_(SFSampleLink::kMonoSample),
    parent_file_(nullptr) {
}

/// Constructs a new SFSample with a sample link.
SFSample::SFSample(std::string name,
    std::vector<int16_t> data,
    uint32_t start_loop,
    uint32_t end_loop,
    uint32_t sample_rate,
    uint8_t original_key,
    int8_t correction,
    std::weak_ptr<SFSample> link,
    SFSampleLink type) :
    name_(std::move(name)),
    data_(std::move(data)),
    start_loop_(std::move(start_loop)),
    end_loop_(std::move(end_loop)),
    sample_rate_(std::move(sample_rate)),
    original_key_(std::move(original_key)),
    correction_(std::move(correction)),
    link_(std::move(link)),
    type_(std::move(type)),
    parent_file_(nullptr) {
}

/// Constructs a new copy of specified SFSample.
SFSample::SFSample(const SFSample & origin) :
    name_(origin.name_),
    data_(origin.data_),
    start_loop_(origin.start_loop_),
    end_loop_(origin.end_loop_),
    sample_rate_(origin.sample_rate_),
    original_key_(origin.original_key_),
    correction_(origin.correction_),
    link_(origin.link_),
    type_(origin.type_),
    parent_file_(nullptr) {
}

/// Copy-assigns a new value to the SFSample, replacing its current contents.
SFSample & SFSample::operator=(const SFSample & origin) {
  name_ = origin.name_;
  data_ = origin.data_;
  start_loop_ = origin.start_loop_;
  end_loop_ = origin.end_loop_;
  sample_rate_ = origin.sample_rate_;
  original_key_ = origin.original_key_;
  correction_ = origin.correction_;
  link_ = origin.link_;
  type_ = origin.type_;
  parent_file_ = nullptr;
  return *this;
}

/// Returns the name of this sample.
const std::string & SFSample::name() const noexcept {
  return name_;
}

/// Sets the name of this sample.
void SFSample::set_name(std::string name) {
  name_ = std::move(name);
}

/// Returns the starting point of the loop of this sample.
uint32_t SFSample::start_loop() const noexcept {
  return start_loop_;
}

/// Sets the starting point of the loop of this sample.
void SFSample::set_start_loop(uint32_t start_loop) {
  start_loop_ = std::move(start_loop);
}

/// Returns the ending point of the loop of this sample.
uint32_t SFSample::end_loop() const noexcept {
  return end_loop_;
}

/// Sets the ending point of the loop of this sample.
void SFSample::set_end_loop(uint32_t end_loop) {
  end_loop_ = std::move(end_loop);
}

/// Returns the sample rate.
uint32_t SFSample::sample_rate() const noexcept {
  return sample_rate_;
}

/// Sets the sample rate.
void SFSample::set_sample_rate(uint32_t sample_rate) {
  sample_rate_ = std::move(sample_rate);
}

/// Returns the original MIDI key number of this sample.
uint8_t SFSample::original_key() const noexcept {
  return original_key_;
}

/// Sets the original MIDI key number of this sample.
void SFSample::set_original_key(uint8_t original_key) {
  original_key_ = std::move(original_key);
}

/// Returns the pitch correction.
int8_t SFSample::correction() const noexcept {
  return correction_;
}

/// Sets the pitch correction.
void SFSample::set_correction(int8_t correction) {
  correction_ = std::move(correction);
}

/// Returns a pointer to the associated sample.
std::shared_ptr<SFSample> SFSample::link() const noexcept {
  return !link_.expired() ? link_.lock() : nullptr;
}

/// Returns true if this sample has an associated sample.
bool SFSample::has_link() const noexcept {
  return !link_.expired();
}

/// Sets the associated right or left stereo sample.
void SFSample::set_link(std::weak_ptr<SFSample> link) {
  link_ = std::move(link);
}

/// Resets the associated right or left stereo sample.
void SFSample::reset_link() noexcept {
  link_.reset();
}

/// Returns both the type of sample and the whether the sample is located in RAM or ROM memory.
SFSampleLink SFSample::type() const noexcept {
  return type_;
}

/// Sets both the type of sample and the whether the sample is located in RAM or ROM memory.
void SFSample::set_type(SFSampleLink type) {
  type_ = std::move(type);
}

/// Returns the sample data.
const std::vector<int16_t> & SFSample::data() const noexcept {
  return data_;
}

/// Returns true if this sample has a parent file.
bool SFSample::has_parent_file() const noexcept {
  return parent_file_ != nullptr;
}

/// Returns the parent file.
SoundFont & SFSample::parent_file() const noexcept {
  return *parent_file_;
}

/// Sets the parent file.
void SFSample::set_parent_file(SoundFont & parent_file) noexcept {
  parent_file_ = &parent_file;
}

/// Resets the parent file.
void SFSample::reset_parent_file() noexcept {
  parent_file_ = nullptr;
}

} // namespace sf2cute
