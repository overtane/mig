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
    msgbuf(storage_ptr_t& p, size_t n) { set_buf(p, n); }
    ~msgbuf() {}
    
    int alloc_buf(size_t n) override {
      m_data = std::make_unique<uint8_t []>(n);
      m_size = n;
      m_next = 0;
      return 0;
    }
    int set_buf(storage_ptr_t& p, size_t n) override 
      { m_data = std::move(p); m_size = n; m_next=0; return 0; }
    
    int putc(uint8_t c) override {
      if (m_data.get() && m_next < m_size ) {
        m_data.get()[m_next] = c;
        m_next++;
        return 0;
      }
      return -1;
    }
    
    int putp(const uint8_t *p, size_t n) override { 
      while (m_next < m_size && n-- && m_data.get()) { 
        // TODO this is bytewise assignment - OPTIMIZE!!!
        m_data.get()[m_next] = *p;
        m_next++;
        p++;
      }
      return (n)? -1 : 0; // everything copied?
    }

    uint8_t getc() override {
      if (m_data.get() && m_next < m_size ) {
        uint8_t c = m_data.get()[m_next];
        m_next++;
        return c;
      }
      return 0xff;
    }
    
    uint8_t *getp(size_t n) const override {
        if (m_next + n - 1 < m_size)
          return &m_data.get()[m_next];
        return nullptr;
    }

    int advance(int n) override { 
        if (m_next + n - 1 <  m_size) 
          m_next += n;
        else
          m_next = m_size;
        return m_next;
    }

    int reverse(int n) override { 
        if (m_next - n > 0)
          m_next -= n;
        else
          m_next = 0; 
        return m_next;
    }

    void reset() override { m_next = 0; }
  
    size_t size() const override { return m_size; }

    void hexdump(std::ostream& os) const override {
      os << std::setfill('0');
      for (auto i=0; i < m_size; i++)
        os << std::hex << std::setw(2) << int(m_data.get()[i]) << ' ';
      os << '\n';
    }

  private:
    
    storage_ptr_t m_data = nullptr;
    size_t m_size = 0;
    int m_next = 0;
};



class SampleProto : public WireFormat {

  public:
    SampleProto(Message& msg);
    SampleProto(storage_ptr_t& buf, size_t n);
    ~SampleProto() {}    

    static const int par_wire_overhead = 1;
    static const int msg_wire_overhead = 5;

    size_t wire_size(const Group&) const override;
    size_t wire_size(const Message&) const override;
    size_t wire_size(const Parameter&) const override;
 
    int to_wire(const Message&) override;
    int to_wire(const Group&) override;
    int to_wire(const Parameter&) override;
 
    int from_wire(Message&) const override;
    int from_wire(Group&) const override;
    int from_wire(blob_t&) const override;
    int from_wire(string_t&) const override;
    int from_wire(std::string&) const override;

    using WireFormat::to_wire;
    using WireFormat::from_wire;

    void dump(std::ostream&, const Message&) const override;
    void dump(std::ostream&, const Group&, int) const override;
    void dump(std::ostream&, const Parameter&) const override;
};


SampleProto::SampleProto(Message& msg) {
  
  auto size = wire_size(msg);
  msgbuf_ptr_t buf = std::make_unique<msgbuf>(size);

  set_buf(buf);
  set_size(size);
  
  to_wire(msg);
}

SampleProto::SampleProto(storage_ptr_t& p, size_t n) {

  msgbuf_ptr_t buf = std::make_unique<msgbuf>(p, n);
  set_buf(buf);
  set_size(n);
  
  uint16_t id;
  from_wire(id);
  set_id(id);

  uint16_t msg_size;
  from_wire(msg_size);
  if (msg_size < n)
    set_size(msg_size);
}


wire_format_ptr_t WireFormat::factory(Message& msg) {
  wire_format_ptr_t w = std::make_unique<SampleProto>(msg);
  return w;
}

wire_format_ptr_t WireFormat::factory(storage_ptr_t& p, size_t n) {
  wire_format_ptr_t w = std::make_unique<SampleProto>(p, n);
  return w;
}

size_t SampleProto::wire_size(const Message& msg) const {
  auto s = msg_wire_overhead;
  for (auto& it : msg.params())
    s += wire_size(it.second);
  return s;
}

size_t SampleProto::wire_size(const Group& group) const {
  auto s = 1; // end mark
  for (auto& it : group.params())
    s += wire_size(it.second);
  return s;
}

size_t SampleProto::wire_size(const Parameter& par) const {

// if parameter is fixed size, wire size is derived from data type
// for variable size parameters, data size is given before data

size_t s = 0;

  // TODO any parameter type can be repeated 
  // repeated group etc
  // calculate size accordingly
  if (par.is_repeated()) {
    s = par.nrepeats() * par_wire_overhead;
    s += par.nrepeats() * par.size();
    std::cout << "par " << par.id() << " wire size " << s << '\n';
  } else if (par.group()) {
    s =  par_wire_overhead + wire_size(*par.group());
    std::cout << "par " << par.id() << " wire size " << s << '\n';
  } else if (par.is_set()) {
    s = par_wire_overhead; // par id
    if  (!par.is_scalar())
      s += 2; // data size
    s += par.size(); // data
    std::cout << "par " << par.id() << " wire size " << s << '\n';
  } else {
    std::cout << "par " << par.id() << " wire size 0\n";
  }
  return s;
}

int SampleProto::to_wire(const Message& msg) {

  std::cout << "msg: " << std::hex << msg.id() << '\n';
 
// Message: | header | parameters | 0xFF
// Header:  | Msg id | Msg size |

  to_wire((uint16_t)msg.id());
  to_wire((uint16_t)size()); // wire format size 

  for  (auto& it : msg.params()) {
    to_wire(it.second); // serialize each parameter
    std::cout << "end " << it.second.id() << '\n';
  }
 
  std::cout << "msg: " << std::hex << msg.id() << '\n';
  to_wire((uint8_t)0xFF); // end of message 

  std::cout << "msg: " << std::hex << msg.id() << '\n';
  buf()->reset(); // read pointer to start of buffer

  return 0;
}

int SampleProto::to_wire(const Parameter& par) {

// parameters               | par 1 | par 2 | ...
// fixed size parameter:    | par id | data
// variable size parameter: | par id | size | data

  std::cout << "par: " << par.id() << '\n';
  if (par.is_set())
    for (auto i=0; i < par.nrepeats(); i++ ) {
      to_wire((uint8_t)par.id());
      if  (!par.is_scalar() && !par.group())
        to_wire((uint16_t)par.size());
      par.data_to_wire(*this,i);
    }
  return 0;
}

int SampleProto::to_wire(const Group& group) {

// group parameter: | par id | group | 0xFF
// group          : | par 1 | par 2 | ...

  std::cout << "group" << '\n';

  for (auto& it : group.params())
    to_wire(it.second); // serialize each parameter
 
  to_wire((uint8_t)0xFF); // end of message 
  return 0;
}

int SampleProto::from_wire(Message& msg) const {

  buf()->reset();
  buf()->advance(4);
  return from_wire((Group&)msg);
}

int SampleProto::from_wire(Group& group) const {

  std::cout << "group\n"; 
  int ret = 0;
  uint8_t c;

  while ( (c = buf()->getc()) != 0xff) {

    std::cout << "trying parameter " << std::dec << int(c) << "\n"; 
    if (group.params().count(int(c)) > 0) { // valid param id
      std::cout << "parsing parameter " << std::dec << int(c) << "\n"; 
      Parameter& par = group.params().at(c);
      ret -= par.data_from_wire(*this);

    } else 

      ret -= 1; // non-valid param id
  }

  std::cout << "group done" << "\n"; 
  return ret;
}

int SampleProto::from_wire(blob_t& data) const {
  uint16_t n;
  from_wire(n);
  uint8_t *p = buf()->getp(n);
  data.assign(p, n); // assign message buffer sub-area
  buf()->advance(n);
  return 0;
}

int SampleProto::from_wire(string_t& data) const {
  uint16_t n;
  from_wire(n);
  const char *p = (char *)buf()->getp(n);
  data.assign(p, n); // assign message buffer sub-area
  buf()->advance(n);
  return 0;
}


int SampleProto::from_wire(std::string& data) const {
  uint16_t n;
  from_wire(n);
  const char *p = (const char *)buf()->getp(n);
  // TODO this makes a copy
  // if we want to reuse buffer area for data, string class has to be replaced
  // use mig::string_t
  data.assign(p, n-1);
  buf()->advance(n);
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

void SampleProto::dump(std::ostream& os, const Parameter& par) const {

  if (par.is_scalar()) {
    auto size = par.size(); 
    uint8_t *p = buf()->getp(size);
    os << std::setfill('0');
    for (auto i=0; i < size; i++, p++)
      os << std::hex << std::setw(2) << int(*p) << ' ';
    os << '\n';
    buf()->advance(size);

  } else if (par.group()) {
    os << '\n';
    dump(os, *par.group(), par.id());

  } else { // variable length parameter
    uint16_t size = 0;
    from_wire(size);
    uint8_t *p = buf()->getp(size);
    for (auto i=0; i < size; i++, p++)
      os << std::hex << std::setw(2) << int(*p) << ' ';
    os << '\n';
    buf()->advance(size);
  }
}

void SampleProto::dump(std::ostream& os, const Group& group, int id) const {

  uint8_t c;
  while ( (c = buf()->getc()) != 0xff) {

    if (dynamic_cast<const Message*>(&group))
      // direct parameter
      os << "- param " << std::dec  << int(c) << ": ";
    else
      // group parameter
      os << "  group " << std::dec << id << '/' << int(c) << ": ";

    if (group.params().count(c) > 0) {
      Parameter& par = group.params().at(c);
      dump(os, par);
    } else {
      os << "invalid id " << std::dec << c << '\n';
    }
  }
}

void SampleProto::dump(std::ostream& os, const Message& msg) const {

  (void)msg;
  uint16_t id, size; 

  os << "dump\n";

  buf()->reset();
  from_wire(id);
  from_wire(size);

  os << std::setfill('0');
  os << "Message: 0x" << std::hex << std::setw(4) << id;
  os << std::dec << ", length " << size << '(' << msg.size() << ")\n";

  dump(os, dynamic_cast<const Group&>(msg), 0);
}


} // namespace mig
