# ifndef _MIGMSG_H_
# define _MIGMSG_H_

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

#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>

namespace mig {

enum ParameterOpt {
  OPTIONAL = true,
  REQUIRED = false
};

enum WireByteOrder {
  BYTE_ORDER_LITTLE_ENDIAN = 0,
  BYTE_ORDER_BIG_ENDIAN = 1,
  BYTE_ORDER_NETWORK = BYTE_ORDER_BIG_ENDIAN,
};

class Message;
class parameter;
class GroupBase;


class MsgBuf {
  
  public:
    virtual ~MsgBuf() { }
    
    virtual uint8_t *alloc_buf(size_t n) = 0;
    virtual uint8_t *set_buf(uint8_t *buf, size_t n) = 0;
    
    virtual int put(uint8_t c) = 0; 
    virtual uint8_t get() = 0;
    virtual void reset() = 0;    
};

class Wired {

  public:
    virtual ~Wired() { if (m_buf) delete m_buf; }

    static Wired *make_wired(Message&); // factory method;

    void set_byteorder(WireByteOrder w) { this->m_byteorder = w; } 
    WireByteOrder byteorder() { return this->m_byteorder; } 

    void set_buf(MsgBuf *buf) { this->m_buf = buf; }
    MsgBuf *buf() { return this->m_buf; }

    void set_size(size_t size) { this->m_size = size; }
    size_t size() { return this->m_size; } 

    virtual size_t wire_size(const GroupBase&) = 0;
    virtual size_t wire_size(const Message&) = 0;
    virtual size_t wire_size(const parameter&) = 0;
 
    virtual int to_wire(const Message&) = 0;
    virtual int to_wire(const GroupBase&) = 0;
    virtual int to_wire(const parameter&) = 0;

    virtual int to_wire(uint16_t) = 0;
    virtual int to_wire(const unsigned char *, size_t) = 0;
    virtual int to_wire(const char *, size_t) = 0;

  private:
    MsgBuf *m_buf;
    size_t m_size;

    WireByteOrder m_byteorder = BYTE_ORDER_NETWORK;
};

class parameter {

  public:
    parameter(int id, bool is_optional, bool is_group ) :
        m_id(id),
        m_is_optional(is_optional),
        m_is_group(is_group) {}
    virtual ~parameter() {}

    void set() { this->m_is_set = true; }
    bool is_optional() const { return this->m_is_optional; } 
    int  id() const { return this->m_id; }
    bool is_group() const { return this->m_is_group; }

    virtual bool is_set() const { return this->m_is_set; } 
    // size should be amount of data bytes to read/write to wire
    // when parameter is not set, size equals always zero 
    virtual std::size_t size() const = 0;
    virtual bool is_valid() const { return this->is_set() || this->is_optional(); }
    virtual void to_wire(Wired&) const = 0;
    virtual size_t wire_size(Wired& w) const { (void)w; return this->size(); };

  private:
    const int m_id;
    const bool m_is_optional;
    const bool m_is_group;
    bool m_is_set = false;
};


class GroupBase {

  public:
    GroupBase() = delete;
    GroupBase(std::vector<::mig::parameter * const>& params) : m_params(params)  {}
    virtual ~GroupBase() {}

    int nparams() { return this->m_params.size(); } 
    std::vector<::mig::parameter * const>& params() const { return this->m_params; }
    bool is_valid() const {
        for (auto it : this->m_params) if (!it->is_valid()) return false;
        return true;
    }
    std::size_t size() const {
        std::size_t s = 0;
        for (auto it : this->m_params) s += it->size();
        return s;
    } 
    bool is_set() const { return this->is_valid(); } // group is set if it is valid
    size_t wire_size(Wired& w) const {
        auto s = 0;
        for (auto it : this->m_params) s += w.wire_size(*it);
        return s;;
    }

  private:
    std::vector<::mig::parameter * const>& m_params; // this is a reference to actual vector
};


class Message : public GroupBase {

  public:
    Message(int id, std::vector<::mig::parameter * const>& m_params) : 
        GroupBase(m_params), msg_id(id) {}
    ~Message() {if (w) delete w; }

    int id() const { return this->msg_id; }

    const Wired *to_wire() {
      if (w)
        delete w;
      w = Wired::make_wired(*this);
      return w;
    }
    Wired* get_wired() { return w; }
 
    const Message& me = *this;

  private:
    const int msg_id;
    Wired *w = nullptr;
};

typedef uint8_t enum_t;
typedef std::vector<uint8_t> blob_t; 
struct void_t {};

template <class T>
class scalar_parameter : public parameter {

  public:
    scalar_parameter(int id, bool optional=false) : parameter(id, optional, false) {}
    scalar_parameter& operator=(T value) { this->set(value); return *this; } 

    void set(T value) { this->value = value; this->parameter::set(); }
    T get() const { return this->value; }

    std::size_t size() const override { return (this->is_set()) ? sizeof(T): 0; }
    void to_wire(Wired& w) const override { w.to_wire(value); }

  private:
    T value = T(0);
};

template <>
class scalar_parameter <void_t> : public parameter {

  public:
    scalar_parameter(int id, bool optional=false) : parameter(id, optional, false) {}

    bool get() const { return this->is_set(); }

    std::size_t size() const override { return 0; }
    void to_wire(Wired& w) const override { } // parameter has no value
};

template <class T>
class group_parameter : public parameter {   

  public:
    T group;

    group_parameter(int id, bool optional=false) : parameter(id, optional, true) {}
    virtual ~group_parameter() {} 

    // set(T group) this could be copy operation 
    T& get() { return this->group; }

    bool is_set() const override { return this->group.is_set(); }
    std::size_t size() const override { return this->group.size(); }
    bool is_valid() const override { return (this->group.is_valid() || this->is_optional()); }
    void to_wire(Wired& w) const override { w.to_wire(dynamic_cast<const GroupBase&>(group)); }
    size_t wire_size(Wired& w) const override { return this->group.wire_size(w); }
};

template <class T>
class composite_parameter : public parameter {

  public:
    composite_parameter(int id, bool optional=false) : parameter(id, optional, false) {}
    virtual ~composite_parameter() { if (data) delete data; } 

    void set(T* data) {
        if (this->data) delete data;
        this->data = data;
        this->parameter::set();
    }
    T* get() { return this->data; }

    std::size_t size() const override { return (this->is_set()) ? this->data->size(): 0; }
    void to_wire(Wired& w) const override { w.to_wire((const unsigned char *)data, this->size()); }

  private:
    T* data = nullptr;

};

template <>
class composite_parameter <std::string>: public parameter
{   

  public:
    composite_parameter(int id, bool optional=false) : parameter(id, optional, false) {}
    virtual ~composite_parameter() {} 

    void set(std::string data) { this->data = data; this->parameter::set(); }
    std::string& get() { return this->data; }

    std::size_t size() const override { return (this->is_set()) ? this->data.size()+1: 0; }
    void to_wire(Wired& w) const override { w.to_wire(data.c_str(), this->size()); }

  private:
    std::string data;
};

} // end namespace mig


// TODO repeated parameters

#endif // ifndef _MIGMSG_H_
