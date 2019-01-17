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
#include <map>
#include <string>
#include <cstdint>
#include <cstddef>
#include <iostream>

namespace mig {

enum ParameterOpt {
  OPTIONAL = true,
  REQUIRED = false
};

enum class ByteOrder {
  LittleEndian = 0, 
  BigEndian = 1,
  Network = BigEndian,
};

class Message;
class parameter;
class GroupBase;

typedef uint8_t enum_t;
typedef std::vector<uint8_t> blob_t; 
struct void_t {};

//! Interface class for wire format message buffer
class MsgBuf {
  
  public:
    virtual ~MsgBuf() { }
   
    //! allocate/reallocate buffer. reallocation deletes the previous 
    virtual uint8_t *alloc_buf(size_t n) = 0;
    //! set buffer and override the previous 
    virtual uint8_t *set_buf(uint8_t *buf, size_t n) = 0;
    
    //! put one byte and advance buffer pointer by 1
    virtual int put(uint8_t c) = 0;
    //! put array of bytes and advance buffer pointer by n
    virtual int put(const uint8_t *p, size_t n) = 0;
    //! get one byte and advance buffer pointer by 1
    virtual uint8_t getc() = 0;
    //! reset buffer pointer to the start of buffer
    virtual void reset() = 0; //
    //! get pointer to buffer data
    virtual uint8_t *getp() const = 0;
    //! advance buffer pointer
    virtual void advance(size_t) = 0;

    //! hexdump buffer contents to stream
    virtual void hexdump(std::ostream&) const = 0;
};

//! Interface class for wire formatting (serialize/deserialize)
class WireFormat {

  public:
    virtual ~WireFormat() { if (m_buf) delete m_buf; }

    //! instantiate the actual wire formatter
    static WireFormat *factory(Message&);

    void set_byteorder(ByteOrder w) { this->m_byteorder = w; }
    ByteOrder byteorder() const { return this->m_byteorder; }

    void set_buf(MsgBuf *buf) { this->m_buf = buf; }
    MsgBuf *buf() const { return this->m_buf; }

    void set_size(size_t size) { this->m_size = size; }
    size_t size() const { return this->m_size; }

    virtual size_t wire_size(const GroupBase&) = 0;
    virtual size_t wire_size(const Message&) = 0;
    virtual size_t wire_size(const parameter&) = 0;
 
    virtual int to_wire(const Message&) = 0;
    virtual int to_wire(const GroupBase&) = 0;
    virtual int to_wire(const parameter&) = 0;

    virtual int to_wire(int8_t);
    virtual int to_wire(int16_t);
    virtual int to_wire(int32_t);
    virtual int to_wire(int64_t);
    virtual int to_wire(uint8_t);
    virtual int to_wire(uint16_t);
    virtual int to_wire(uint32_t);
    virtual int to_wire(uint64_t);
    virtual int to_wire(bool);
    virtual int to_wire(const blob_t&);
    virtual int to_wire(const std::string&);

    virtual int from_wire(int8_t&) const;
    virtual int from_wire(int16_t&) const;
    virtual int from_wire(int32_t&) const;
    virtual int from_wire(int64_t&) const;
    virtual int from_wire(uint8_t&) const;
    virtual int from_wire(uint16_t&) const;
    virtual int from_wire(uint32_t&) const;
    virtual int from_wire(uint64_t&) const;

    virtual void dump(std::ostream&, const Message&) const = 0;
    virtual void dump(std::ostream&, const GroupBase&, int) const = 0;

  private:
    MsgBuf *m_buf; //!< Buffer area
    size_t m_size; //!< Size of wire formatted message in bytes

    ByteOrder m_byteorder = ByteOrder::Network;
};

//! Base class for message parameters
class parameter {

  public:
    parameter(int id, bool is_optional, bool is_scalar=true, GroupBase* group=nullptr) :
        m_id(id),
        m_is_optional(is_optional),
        m_is_scalar(is_scalar),
        m_group(group)
    {}
    virtual ~parameter() {}

    void set() { this->m_is_set = true; }
    bool is_optional() const { return this->m_is_optional; }
    int  id() const { return this->m_id; }
    bool is_scalar() const { return this->m_is_scalar; }
    GroupBase* group() const { return this->m_group; }

    virtual bool is_set() const { return this->m_is_set; }
    virtual std::size_t size() const = 0;
    virtual bool is_valid() const { return this->is_set() || this->is_optional(); }
    virtual void data_to_wire(WireFormat&) const = 0;
    virtual size_t wire_size(WireFormat& w) const { (void)w; return this->size(); };

    GroupBase* g_group = nullptr;

  private:
    const int m_id;
    const bool m_is_optional;
    const bool m_is_scalar;
    GroupBase *m_group;
    bool m_is_set = false;
};

//! Base class for groups of parameters (messages and group parameters)
class GroupBase {

  public:
    GroupBase() = delete;
    GroupBase(std::map<int, ::mig::parameter&>& params) : m_params(params)  {}
    virtual ~GroupBase() {}

    int nparams() { return this->m_params.size(); } 
    std::map<int, ::mig::parameter&>& params() const { return this->m_params; }
    bool is_valid() const {
        for (auto& it : this->m_params) if (!it.second.is_valid()) return false;
        return true;
    }
    std::size_t size() const {
        std::size_t s = 0;
        for (auto& it : this->m_params) s += it.second.size();
        return s;
    } 
    bool is_set() const { return this->is_valid(); } // group is set if it is valid
    size_t wire_size(WireFormat& w) const { return w.wire_size(*this); }

  private:
    std::map<int, ::mig::parameter&>& m_params; // this is a reference to the actual mapr
};


class Message : public GroupBase {

  public:
    static Message* factory();

    Message(int id, std::map<int, ::mig::parameter&>& m_params) : 
        GroupBase(m_params), m_id(id) {}
    ~Message() { if (m_wire_format) delete m_wire_format; }

    int id() const { return this->m_id; }

    const WireFormat *to_wire() {
      if (m_wire_format)
        delete m_wire_format;
      m_wire_format = WireFormat::factory(*this);
      return m_wire_format;
    }
    WireFormat* wire_format() const { return m_wire_format; }

    void dump(std::ostream& os) const {
      if (this->wire_format())
        this->wire_format()->dump(os, *this);
    }
  private:
    const int m_id;
    WireFormat *m_wire_format = nullptr;
};

template <class T>
class scalar_parameter : public parameter {

  public:
    scalar_parameter(int id, bool optional=false) : parameter(id, optional) {}
    scalar_parameter& operator=(T value) { this->set(value); return *this; } 

    void set(T value) { this->m_data = value; this->parameter::set(); }
    T get() const { return this->m_data; }

    std::size_t size() const override { return sizeof(T); }
    void data_to_wire(WireFormat& w) const override { w.to_wire(m_data); }

  private:
    T m_data = T(0);
};

template <>
class scalar_parameter <void_t> : public parameter {

  public:
    scalar_parameter(int id, bool optional=false) : parameter(id, optional) {}

    bool get() const { return this->is_set(); }

    std::size_t size() const override { return 0; }
    void data_to_wire(WireFormat& w) const override { (void)w; } // parameter has no value
};

template <class T>
class enum_parameter : public parameter {

  public:
    enum_parameter(int id, bool optional=false) : parameter(id, optional) {}
    enum_parameter& operator=(T value) { this->set(value); return *this; } 

    void set(T value) { this->m_data = value; this->parameter::set(); }
    T get() const { return this->m_data; }

    std::size_t size() const override { return sizeof(mig::enum_t); }
    void data_to_wire(WireFormat& w) const override { w.to_wire((enum_t)m_data); }

  private:
    T m_data = T(0);
};

template <class T>
class group_parameter : public parameter {   

  public:
    group_parameter(int id, bool optional=false) : 
      parameter(id, optional, false, (GroupBase*) &m_data) {} 
    virtual ~group_parameter() {} 

    // set(T group) // TODO this could be copy operation 
    T& data() { return this->m_data; }

    bool is_set() const override { return this->m_data.is_set(); }
    std::size_t size() const override { return this->m_data.size(); }
    bool is_valid() const override { return (this->m_data.is_valid() || this->is_optional()); }
    size_t wire_size(WireFormat& w) const override { return this->m_data.wire_size(w); }
    void data_to_wire(WireFormat& w) const override { w.to_wire((const GroupBase&)(m_data)); }

  private:
    T m_data;

};

template <class T>
class var_parameter : public parameter {

  public:
    var_parameter(int id, bool optional=false) : parameter(id, optional, false, nullptr) {}
    virtual ~var_parameter() { if (m_data) delete m_data; } 

    void set(T* data) {
        if (this->m_data)
          delete m_data;
        this->m_data = data;
        this->parameter::set();
    }
    T* get() { return this->m_data; }

    std::size_t size() const override { return this->m_data->size(); }
    void data_to_wire(WireFormat& w) const override { w.to_wire(*m_data); }

  private:
    T* m_data = nullptr;

};

template <>
class var_parameter <std::string>: public parameter
{   

  public:
    var_parameter(int id, bool optional=false) : parameter(id, optional, false, nullptr) {}
    virtual ~var_parameter() {} 

    void set(std::string data) { this->m_data = data; this->parameter::set(); }
    std::string& get() { return this->m_data; }

    std::size_t size() const override { return this->m_data.size()+1; }
    void data_to_wire(WireFormat& w) const override { w.to_wire(m_data); }

  private:
    std::string m_data;
};



// TODO repeated parameters

} // end namespace mig

#endif // ifndef _MIGMSG_H_
