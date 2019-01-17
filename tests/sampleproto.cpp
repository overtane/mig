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
    
    uint8_t *alloc_buf(size_t n) override {
      if (data)
        delete[] data;
      data = new uint8_t[n];
      size = n;
      return data;
    }
    uint8_t *set_buf(uint8_t *buf, size_t n) override 
      { data = buf; size = n; return data; }
    
    int put(uint8_t c) override { 
      if (data && next < size ) {
        data[next] = c;
        next++;
        return 0;
      }
      return -1;
    }
    
    int put(const uint8_t *p, size_t n) override { 
      while (next < size && n-- && data) { // TODO optimize!!
        data[next++] = *p++;
      }
      return (n)? -1 : 0;
    }

    uint8_t getc() override {
      if (data && next < size ) {
        uint8_t c = data[next];
        next++;
        return c;
      }
      return 0xff;
    }
    
    void reset() override { this->next = 0; }
  
    uint8_t *getp() const override {
        if (this->next < this->size)
          return &this->data[this->next];
        return nullptr;
    }

    void advance(size_t n) override { this->next += n; }

    void hexdump(std::ostream& os) const override {
      os << std::setfill('0');
      for (auto i=0; i < this->size; i++)
        os << std::hex << std::setw(2) << int(this->data[i]) << ' ';
      os << '\n';
    }

  private:
    
    uint8_t *data = nullptr;
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

    void dump(std::ostream&, const Message&) const override;
    void dump(std::ostream&, const GroupBase&, int) const override;
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
  for (auto& it : msg.params())
    s += wire_size(it.second);

  std::cout << s << '\n';
  return s;
}

size_t SampleProto::wire_size(const GroupBase& group) {
  auto s = 1;
  for (auto& it : group.params())
    s += wire_size(it.second);
  return s;
}

size_t SampleProto::wire_size(const parameter& par) {

// if parameter is fixed size, wire size is derived from data type
// for variable size parameters, data size is given before data

  if (par.is_set()) {
    auto s = par_wire_overhead; // par id
    if  (!par.is_scalar() && !par.group())
      s += 2; // data size
    s += par.wire_size(*this); // data
    std::cout << par.id() << ": " << s << '\n';
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

  for (auto& it : msg.params())
    to_wire(it.second); // serialize each parameter
 
  to_wire((uint8_t)0xFF); // end of message 

  buf()->reset(); // read pointer to start of buffer

  return 0;
}

int SampleProto::to_wire(const parameter& par) {

// parameters               | par 1 | par 2 | ...
// fixed size parameter:    | par id | data
// variable size parameter: | par id | size | data

  std::cout << "par: " << par.id() << '\n';
  if (par.is_set()) {
    to_wire((uint8_t)par.id());
    if  (!par.is_scalar() && !par.group())
      to_wire((uint16_t)par.size());
    par.data_to_wire(*this);
  }
  return 0;
}

int SampleProto::to_wire(const GroupBase& group) {

// group parameter: | par id | group | 0xFF
// group          : | par 1 | par 2 | ...

  std::cout << "group" << '\n';

  for (auto& it : group.params())
    to_wire(it.second); // serialize each parameter
 
  to_wire((uint8_t)0xFF); // end of message 
  std::cout << "end group" << '\n';
  return 0;
}

#if 0
void SampleProto::hexdump(std::ostream& os, const Message& msg) const {

  (void)msg;
  uint16_t id, size; 

  buf()->reset();
  from_wire(id);
  from_wire(size);

  os << std::setfill('0');
  os << "Message: 0x" << std::hex << std::setw(4) << id;
  os << std::dec << ", length " << size << '\n';

  buf()->hexdump(os);

  dump(os, msg);
}
#endif

void SampleProto::dump(std::ostream& os, const GroupBase& group, int id) const {

  uint16_t size;

  uint8_t c;
  while ( (c = buf()->getc()) != 0xff) {
    if (group.params().count(c) > 0) {
      parameter& par = group.params().at(c);

      if (dynamic_cast<const Message*>(&group))
        // direct parameter
        os << "- Parameter: " << std::dec  << int(c) << ": ";
      else
        // group parameter
        os << "  Group " << std::dec << id << ": ";

      if (par.is_scalar()) {
        size = par.size(); 
        uint8_t *p = buf()->getp(); // TODO getp(size);
        os << std::setfill('0');
        for (auto i=0; i < size; i++, p++)
          os << std::hex << std::setw(2) << int(*p) << ' ';
        os << '\n';
        buf()->advance(size);
      } else if (par.group()) {
        os << '\n';
        dump(os, *par.group(), c);
        os << "- end group " << std::dec << int(c) << '\n';
      } else {
        from_wire(size);
        uint8_t *p = buf()->getp(); // TODO getp(size);
        for (auto i=0; i < size; i++, p++)
          os << std::hex << std::setw(2) << int(*p) << ' ';
        os << '\n';
        buf()->advance(size);
      }
    } else 
      os << "invalid\n"; 
  }
  
}

void SampleProto::dump(std::ostream& os, const Message& msg) const {

  (void)msg;
  uint16_t id, size; 

  buf()->reset();
  from_wire(id);
  from_wire(size);

  os << std::setfill('0');
  os << "Message: 0x" << std::hex << std::setw(4) << id;
  os << std::dec << ", length " << size << '\n';

  dump(os, dynamic_cast<const GroupBase&>(msg), msg.id());
}

} // namespace mig
