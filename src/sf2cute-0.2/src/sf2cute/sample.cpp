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

} // namespace sf2cute
