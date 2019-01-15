/* 
   Messaging Interface Generator

   Copyright 2019 Olli Vertanen

   Permission is hereby granted, free of charge, to any person obtaining a 
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
 
*/

#include "migmsg.h"
#include <iostream>
#include <iomanip>

namespace mig {

int WireFormat::to_wire(uint8_t value) {

  return buf()->put(value);
}

int WireFormat::to_wire(uint16_t value) {

  if (byteorder() == ByteOrder::Network) {
    value = htons(value);
    return buf()->put((uint8_t *)&value, 2);
  } else {
    // TODO implement
    return -1;
  } 
}

int WireFormat::to_wire(uint32_t value) {

  if (byteorder() == ByteOrder::Network) {
    value = htonl(value);
    return buf()->put((uint8_t *)&value, 4);
  } else {
    // TODO implement
    return -1;
  } 
}

int WireFormat::to_wire(uint64_t value) {

  union u {
    uint64_t i64;
    uint32_t i32[2];
  };

  union u v1, v2;

  if (byteorder() == ByteOrder::Network) {
    v1.i64 = value;
    v2.i32[1] = htonl(v1.i32[0]);
    v2.i32[0] = htonl(v2.i32[1]);
    return buf()->put((uint8_t *)&v2, 8);
  } else {
    // TODO implement
    return -1;
  } 
}

int WireFormat::to_wire(const std::string& value) {
    return buf()->put((uint8_t *)value.c_str(), value.size()+1);
}

int WireFormat::to_wire(const blob_t& value) {
  return buf()->put((uint8_t *)value.data(), value.size());
}

int WireFormat::to_wire(bool value) {
  return to_wire((uint8_t)value);
}

int WireFormat::to_wire(int8_t value) {
  return to_wire((uint8_t)value);
}

int WireFormat::to_wire(int16_t value) {
  return to_wire((uint16_t)value);
}

int WireFormat::to_wire(int32_t value) {
  return to_wire((uint32_t)value);
}

int WireFormat::to_wire(int64_t value) {
  return to_wire((uint64_t)value);
}

#if 0
static void dummy()
{
  char a = 0x42;

  std::cout << std::setfill('0');

  std::cout << std::hex << std::setw(2) << (int)a << ' ';
  std::cout << std::hex << std::setw(2) << (int)a << '\n';
}
#endif
}
