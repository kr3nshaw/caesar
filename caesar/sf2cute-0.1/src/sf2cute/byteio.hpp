/// @file
/// Byte I/O templates.
///
/// @author gocha <https://github.com/gocha>

#ifndef SF2CUTE_BYTEIO_HPP_
#define SF2CUTE_BYTEIO_HPP_

#include <stdint.h>

namespace sf2cute {

/// Writes an 8-bit integer.
/// @param out the output iterator.
/// @param value the number to be written.
/// @return the output iterator that points to the next element of the written data.
/// @tparam OutputIterator an Iterator that can write to the pointed-to element.
template <typename OutputIterator>
OutputIterator WriteInt8(OutputIterator out, uint8_t value) {
  static_assert(sizeof(*out) == 1, "Element size of OutputIterator must be 1.");

  *out = static_cast<char>(value);
  out = std::next(out, 1);

  return out;
}

/// Writes a 16-bit integer in little-endian order.
/// @param out the output iterator.
/// @param value the number to be written.
/// @return the output iterator that points to the next element of the written data.
/// @tparam OutputIterator an Iterator that can write to the pointed-to element.
template <typename OutputIterator>
OutputIterator WriteInt16L(OutputIterator out, uint16_t value) {
  static_assert(sizeof(*out) == 1, "Element size of OutputIterator must be 1.");

  *out = value & 0xff;
  out = std::next(out, 1);
  *out = static_cast<char>((value >> 8) & 0xff);
  out = std::next(out, 1);

  return out;
}

/// Writes a 32-bit integer in little-endian order.
/// @param out the output iterator.
/// @param value the number to be written.
/// @return the output iterator that points to the next element of the written data.
/// @tparam OutputIterator an Iterator that can write to the pointed-to element.
template <typename OutputIterator>
OutputIterator WriteInt32L(OutputIterator out, uint32_t value) {
  static_assert(sizeof(*out) == 1, "Element size of OutputIterator must be 1.");

  *out = static_cast<char>(value & 0xff);
  out = std::next(out, 1);
  *out = static_cast<char>((value >> 8) & 0xff);
  out = std::next(out, 1);
  *out = static_cast<char>((value >> 16) & 0xff);
  out = std::next(out, 1);
  *out = static_cast<char>((value >> 24) & 0xff);
  out = std::next(out, 1);

  return out;
}

/// Writes an 8-bit integer.
/// @param out the output destination object.
/// @param value the number to be written.
/// @tparam Insertable an object that implements an insertion operator for char value.
template <typename Insertable>
void InsertInt8(Insertable & out, uint8_t value) {
  out << static_cast<char>(value);
}

/// Writes a 16-bit integer in little-endian order.
/// @param out the output destination object.
/// @param value the number to be written.
/// @tparam Insertable an object that implements an insertion operator for char value.
template <typename Insertable>
void InsertInt16L(Insertable & out, uint16_t value) {
  out << static_cast<char>(value & 0xff);
  out << static_cast<char>((value >> 8) & 0xff);
}

/// Writes a 32-bit integer in little-endian order.
/// @param out the output destination object.
/// @param value the number to be written.
/// @tparam Insertable an object that implements an insertion operator for char value.
template <typename Insertable>
void InsertInt32L(Insertable & out, uint32_t value) {
  out << static_cast<char>(value & 0xff);
  out << static_cast<char>((value >> 8) & 0xff);
  out << static_cast<char>((value >> 16) & 0xff);
  out << static_cast<char>((value >> 24) & 0xff);
}

} // namespace sf2cute

#endif // SF2CUTE_BYTEIO_HPP_
