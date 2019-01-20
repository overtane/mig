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

  return buf()->putc(value);
}

int WireFormat::to_wire(uint16_t value) {

  //std::cout << "- uint16_t\n";

  if (byteorder() == ByteOrder::Network) {
    value = htons(value);
    return buf()->putp((uint8_t *)&value, 2);
  } else {
    // TODO implement
    return -1;
  } 
}

int WireFormat::to_wire(uint32_t value) {

  if (byteorder() == ByteOrder::Network) {
    value = htonl(value);
    return buf()->putp((uint8_t *)&value, 4);
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
    return buf()->putp((uint8_t *)&v2, 8);
  } else {
    // TODO implement
    return -1;
  } 
}

int WireFormat::to_wire(const std::string& value) {
    return buf()->putp((uint8_t *)value.c_str(), value.size()+1);
}

int WireFormat::to_wire(const blob_t& value) {
  return buf()->putp((uint8_t *)value.data(), value.size());
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

// TODO limit checks

int WireFormat::from_wire(int8_t& data) const {
  data = (int8_t)buf()->getc();
  return 0;
}

int WireFormat::from_wire(int16_t& data) const {
  data = (int16_t)ntohs(*(uint16_t*)(buf()->getp(2)));
  buf()->advance(2);
  return 0;
}

int WireFormat::from_wire(int32_t& data) const {
  data = (int32_t)ntohl(*(uint32_t*)(buf()->getp(4)));
  buf()->advance(4);
  return 0;
}

int WireFormat::from_wire(int64_t& data) const {
  data = 0; // TODO;
  buf()->advance(8);
  return 0;
}

int WireFormat::from_wire(uint8_t& data) const {
  data = buf()->getc();
  return 0;
}

int WireFormat::from_wire(uint16_t& data) const {
  data = ntohs(*(uint16_t*)(buf()->getp(2)));
  buf()->advance(2);
  return 0;
}

int WireFormat::from_wire(uint32_t& data) const {
  data = ntohs(*(uint32_t*)(buf()->getp(4)));
  buf()->advance(4);
  return 0;
}

int WireFormat::from_wire(uint64_t& data) const {
  data = 0; // TODO
  buf()->advance(8);
  return 0;
}

int WireFormat::from_wire(bool& data) const {
  data = buf()->getc();
  return 0;
}

message_ptr_t Message::factory(wire_format_ptr_t& w) {

  if (w.get()) {
    const auto& it = Message::creators.find(w->id());
    if (it != Message::creators.end()) {
      MessageCreatorFunc f = it->second;
      auto m = f();
      if (m.get()) {
        m->set_wire_format(w); // proto object now owned by message 
        m->wire_format()->from_wire(*m);
        return m;
      }
    }
  }

  return nullptr;
}

}
