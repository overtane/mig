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

// 
// Sample protocol implementation
//
// - see README.md for further information 
//

#include "migmsg.h"
#include <iostream>
#include <iomanip>

namespace mig {

class msgbuf : public MsgBuf {
  
  public:
    msgbuf(size_t n) { alloc_buf(n); }
    ~msgbuf() { if (data) delete[] data; }
    
    uint8_t *alloc_buf(size_t n) {
      if (data)
        delete[] data;
      data = new uint8_t[n];
      size = n;
      return data;
    }
    uint8_t *set_buf(uint8_t *buf, size_t n) { data = buf; size = n; return data; }
    
    int put(uint8_t c) { 
      if (data && next < size ) {
        data[next] = c;
        next++;
        return 0;
      }
      return -1;
    }
    
    int put(const uint8_t *p, size_t n) { 
      while (next < size && n-- && data) { // TODO optimize!!
        data[next++] = *p++;
      }
      return (n)? -1 : 0;
    }

    uint8_t get() {
      if (data && next < size ) {
        uint8_t c = data[next];
        next++;
        return c;
      }
      return -1;
    }
    
    void reset() { next = 0; }
  
  private:
    
    unsigned char *data = nullptr;
    size_t size = 0;
    int next = 0;
};

class SampleProto : public WireFormat {

  public:
    SampleProto(Message& msg);
    ~SampleProto() {}    

    static const int par_wire_overhead = 1;
    static const int msg_wire_overhead = 5;

    size_t wire_size(const GroupBase&) override;
    size_t wire_size(const Message&) override;
    size_t wire_size(const parameter&) override;
 
    int to_wire(const Message&) override;
    int to_wire(const GroupBase&) override;
    int to_wire(const parameter&) override;
 
    using WireFormat::to_wire;
};


SampleProto::SampleProto(Message& msg) {
  
  size_t size = wire_size(msg);
  msgbuf *buf = new msgbuf(size);

  set_buf(buf);
  set_size(size);
  
  to_wire(msg);
}

WireFormat* WireFormat::factory(Message& msg) {
  WireFormat *w = new SampleProto(msg);
  return w;
}

size_t SampleProto::wire_size(const Message& msg) {
  auto s = msg_wire_overhead;
  for (auto it : msg.params())
    s += wire_size(*it);
  return s;
}

size_t SampleProto::wire_size(const GroupBase& group) {
  auto s = 0;
  for (auto it : group.params())
    s += wire_size(*it);
  return s;
}

size_t SampleProto::wire_size(const parameter& par) {

// if parameter is fixed size, wire size is derived from data type
// for variable size parameters, data size is given before data

  if (par.is_set()) {
    auto s = par_wire_overhead;
    if  (!par.is_fixed_size() && !par.is_group())
      s++;
    s = par.wire_size(*this);
    return s;
  }
  return 0;
}

int SampleProto::to_wire(const Message& msg) {

  std::cout << "msg: " << std::hex << msg.id() << '\n';
 
// Message: | header | parameters | 0xFF
// Header:  | Msg id | Msg size |

  to_wire((uint16_t)msg.id());
  to_wire((uint16_t)this->size()); // wire format size 

  for (auto it : msg.params())
    to_wire(*it); // serialize each parameter
 
  to_wire((uint8_t)0x03); // ETX

  return 0;
}

int SampleProto::to_wire(const parameter& par) {

// parameters               | par 1 | par 2 | ...
// fixed size parameter:    | par id | data
// variable size parameter: | par id | size | data

  std::cout << "par: " << par.id() << '\n';
  if (par.is_set()) {
    to_wire((uint8_t)par.id());
    if (!par.is_fixed_size()) 
      to_wire((uint16_t)par.size());
    par.data_to_wire(*this);
  }
  return 0;
}

int SampleProto::to_wire(const GroupBase& group) {

// group parameter: | par id | group | 0xFF
// group          : | par 1 | par 2 | ...

  std::cout << "group" << '\n';

  for (auto it : group.params())
    to_wire(*it); // serialize each parameter
 
  std::cout << "end group" << '\n';
  return 0;
}

}
