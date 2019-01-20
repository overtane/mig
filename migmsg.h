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
#include <memory>

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
class Parameter;
class Group;
class WireFormat;
class MsgBuf;

typedef uint8_t enum_t;
typedef std::map<int, ::mig::Parameter&> parameter_container_t;
typedef std::unique_ptr<Message> message_ptr_t;
typedef std::unique_ptr<WireFormat> wire_format_ptr_t;
typedef std::unique_ptr<MsgBuf> msgbuf_ptr_t;
typedef std::unique_ptr<uint8_t []> storage_ptr_t; // for dynamic storage areas

typedef message_ptr_t (*MessageCreatorFunc)(void);

class blob_t {

  public:
    blob_t(storage_ptr_t& p, size_t n) : m_data(nullptr) { assign(p, n); }
    blob_t(uint8_t *p, size_t n) : m_storage(nullptr) { assign(p, n); }

    void assign(storage_ptr_t& p, size_t n) {
      m_data = p.get();
      m_storage = std::move(p);
      m_size = (m_data) ? n : 0;
    }

    void assign(const uint8_t *p, size_t n) {
      m_data = p;
      m_storage = nullptr;
      m_size = (m_data) ? n : 0;
    }

    void assign(blob_t& b) {
      if (b.m_storage != nullptr)
        assign(b.m_storage, b.m_size);
      else
        assign(b.m_data, b.m_size);
    }

    bool equals(const blob_t& b) const {
      if (m_size != b.size())
        return false;
      else if (m_data == b.data())
        return true;
      else {
        for (auto i=0; i<m_size; i++)
          if (m_data[i] != b.data()[i]) return false;
      }
      return true;
    }

    const uint8_t *data() const { return m_data; }
    size_t size() const { return m_size; }
  
  private:
    storage_ptr_t m_storage;
    const uint8_t *m_data;
    size_t m_size = 0;
};

struct void_t {};

//! Interface class for wire format message buffer
class MsgBuf {
  
  public:
    virtual ~MsgBuf() { }
   
    //! allocate/reallocate buffer. reallocation deletes the previous 
    virtual int alloc_buf(size_t n) = 0;
    //! assign new storage buffer overriding the previous 
    virtual int set_buf(storage_ptr_t& buf, size_t n) = 0;
    
    //! put one byte and advance buffer pointer by 1
    virtual int putc(uint8_t c) = 0;
    //! put array of bytes and advance buffer pointer by n
    virtual int putp(const uint8_t *p, size_t n) = 0;
    //! get one byte and advance buffer pointer by 1
    virtual uint8_t getc() = 0;
    //! get pointer to buffer data if available
    virtual uint8_t *getp(size_t) const = 0;
    //! reset buffer pointer to the start of buffer
    virtual void reset() = 0; //
    //! advance buffer pointer
    virtual size_t advance(size_t) = 0;
    //! get internal buffer size
    virtual size_t size() const = 0;

    //! hexdump buffer contents to stream
    virtual void hexdump(std::ostream&) const = 0;
};

//! Interface class for wire formatting (serialize/deserialize)
class WireFormat {

  public:
    //! instantiate wire formatter from message instance (outgoing)
    static wire_format_ptr_t factory(Message&);
    //! instantiate wire formatter from byte buffer (incoming)
    static wire_format_ptr_t factory(storage_ptr_t&, size_t);

    virtual ~WireFormat() { }

    void set_byteorder(ByteOrder w) { this->m_byteorder = w; }
    ByteOrder byteorder() const { return this->m_byteorder; }

    void set_buf(msgbuf_ptr_t& buf) { this->m_buf = std::move(buf); }
    MsgBuf *buf() const { return this->m_buf.get(); }

    void set_size(size_t size) { this->m_size = size; }
    size_t size() const { return this->m_size; }

    int id() const { return m_id; }
    void set_id(int id) { m_id = id; }

    virtual size_t wire_size(const Group&) const = 0;
    virtual size_t wire_size(const Message&) const = 0;
    virtual size_t wire_size(const Parameter&) const = 0;
 
    virtual int to_wire(const Message&) = 0;
    virtual int to_wire(const Group&) = 0;
    virtual int to_wire(const Parameter&) = 0;

    virtual int from_wire(Message&) const = 0;
    virtual int from_wire(Group&) const = 0;

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
    virtual int from_wire(bool&) const;
    virtual int from_wire(blob_t&) const = 0;
    virtual int from_wire(std::string&) const = 0;

    virtual void dump(std::ostream&, const Message&) const = 0;
    virtual void dump(std::ostream&, const Group&, int) const = 0;

  protected:
    WireFormat() {}

  private:
    msgbuf_ptr_t m_buf; //!< Buffer area
    size_t m_size; //!< Size of wire formatted message in bytes
    int m_id; //!< Id of the message in m_buf
 
    ByteOrder m_byteorder = ByteOrder::Network;
};

//! Base class for message parameters
class Parameter {

  public:
    Parameter(int id, bool is_optional, bool is_scalar=true, Group* group=nullptr) :
        m_id(id),
        m_is_optional(is_optional),
        m_is_scalar(is_scalar),
        m_group(group) {}
    virtual ~Parameter() {}

    void set() { this->m_is_set = true; }
    bool is_optional() const { return this->m_is_optional; }
    int  id() const { return this->m_id; }
    bool is_scalar() const { return this->m_is_scalar; }
    Group* group() const { return this->m_group; }

    virtual bool is_set() const { return this->m_is_set; }
    virtual std::size_t size() const = 0;
    virtual bool is_valid() const { return this->is_set() || this->is_optional(); }
    virtual int data_to_wire(WireFormat&) const = 0;
    virtual size_t wire_size(const WireFormat& w) const { (void)w; return this->size(); };
    virtual int data_from_wire(const WireFormat&) = 0;

  private:
    const int m_id;
    const bool m_is_optional;
    const bool m_is_scalar;
    Group *m_group;
    bool m_is_set = false;
};

//! Base class for groups of parameters (messages and group parameters)
class Group {

  public:
    Group() = delete;
    virtual ~Group() {}

    int nparams() { return this->m_params.size(); } 
    const parameter_container_t& params() const { return this->m_params; }
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
    size_t wire_size(const WireFormat& w) const { return w.wire_size(*this); }
    int from_wire(const WireFormat& w) { return w.from_wire(*this); }

  protected:
    Group(const parameter_container_t& params) : m_params(params)  {}

  private:
    const parameter_container_t& m_params; // this is a reference to the actual map
};


class Message : public Group {

  public:
    //! Instantiate messages from incoming byte stream
    static message_ptr_t factory(wire_format_ptr_t&);

    Message() = delete;
    ~Message() {}

    int id() const { return this->m_id; }

    void set_wire_format(wire_format_ptr_t& wire_format)  { m_wire_format = std::move(wire_format); }
    WireFormat* wire_format() const { return m_wire_format.get(); }
    int to_wire() {
      m_wire_format = WireFormat::factory(*this);
      // TODO return value based on success
      return 0;
    }

    void dump(std::ostream& os) const {
      if (this->wire_format())
        this->wire_format()->dump(os, *this);
    }

  protected:
    Message(int id, const parameter_container_t& m_params) : 
      Group(m_params), m_id(id) {}

  private:
    const int m_id;
    wire_format_ptr_t m_wire_format = nullptr;

    static const std::map<int, MessageCreatorFunc> creators;

};

template <class T>
class ScalarParameter : public Parameter {

  public:
    ScalarParameter(int id, bool optional=false) : Parameter(id, optional) {}
    ScalarParameter& operator=(T value) { this->assign(value); return *this; } 
    bool operator==(T value) const { return m_data == value; }
    bool operator!=(T value) const { return m_data != value; }
    bool operator>(T value) const { return m_data > value; }
    bool operator>=(T value) const { return !(m_data < value); }
    bool operator<(T value) const { return m_data < value; }
    bool operator<=(T value) const { return !(m_data > value); }

    void assign(T value) { this->m_data = value; this->Parameter::set(); }
    const T& data() const { return this->m_data; }
    std::size_t size() const override { return sizeof(T); }

    int data_to_wire(WireFormat& w) const override { return w.to_wire(m_data); }
    int data_from_wire(const WireFormat& w) override { 
      Parameter::set(); 
      return w.from_wire(m_data); 
    }

  private:
    T m_data = T(0);
};

template <>
class ScalarParameter <void_t> : public Parameter {

  public:
    ScalarParameter(int id, bool optional=false) : Parameter(id, optional) {}

    std::size_t size() const override { return 0; }

    int data_to_wire(WireFormat& w) const override { (void)w; return 0; }
    int data_from_wire(const WireFormat& w) override { (void)w; Parameter::set(); return 0; }
};

template <class T>
class EnumParameter : public Parameter {

  public:
    EnumParameter(int id, bool optional=false) : Parameter(id, optional) {}
    EnumParameter& operator=(T value) { this->assign(value); return *this; } 
    bool operator==(T value) const { return m_data == value; }
    bool operator!=(T value) const { return m_data != value; }

    void assign(T value) { this->m_data = value; this->Parameter::set(); }
    const T& data() const { return this->m_data; }
    std::size_t size() const override { return sizeof(mig::enum_t); }

    int data_to_wire(WireFormat& w) const override { return w.to_wire((enum_t)m_data); }
    int data_from_wire(const WireFormat& w) override {
      uint8_t x;
      w.from_wire(x);
      m_data = static_cast<T>(x);
      Parameter::set();
      return 0;
    }

  private:
    T m_data = T(0);
};

template <class T>
class GroupParameter : public Parameter {   

  public:
    GroupParameter(int id, bool optional=false) : 
      Parameter(id, optional, false, (Group*) &m_data) {} 
    virtual ~GroupParameter() {} 

    //void assign(const T& group) // TODO this could be a deep copy operation 
    T& data() { return this->m_data; } // non-const return so that group params may be accessed
    std::size_t size() const override { return this->m_data.size(); }

    bool is_set() const override { return this->m_data.is_set(); }
    bool is_valid() const override { return (this->m_data.is_valid() || this->is_optional()); }

    //size_t wire_size(const WireFormat& w) const override { return this->m_data.wire_size(w); }
    int data_to_wire(WireFormat& w) const override { return w.to_wire((const Group&)(m_data)); }
    int data_from_wire(const WireFormat& w) override { return w.from_wire((Group&)(m_data)); }

  private:
    T m_data;

};

template <class T>
class VarParameter : public Parameter {

  public:
    VarParameter(int id, bool optional=false) : Parameter(id, optional, false, nullptr) {}
    virtual ~VarParameter() { } 

    void assign(T& data) {
        m_data.assign(data);
        this->Parameter::set();
    }
    const T& data() { return this->m_data; }
    std::size_t size() const override { return this->m_data.size(); }

    int data_to_wire(WireFormat& w) const override { return w.to_wire(m_data); }
    int data_from_wire(const WireFormat& w) override { 
      Parameter::set();
      return w.from_wire(m_data); 
    }

  private:
    T m_data = T(nullptr, 0);

};

template <>
class VarParameter <std::string>: public Parameter
{   

  public:
    VarParameter(int id, bool optional=false) : Parameter(id, optional, false, nullptr) {}
    virtual ~VarParameter() {} 

    void assign(const std::string& data) { this->m_data = data; this->Parameter::set(); }
    std::string& data() { return this->m_data; }

    std::size_t size() const override { return this->m_data.size()+1; }
    int data_to_wire(WireFormat& w) const override { return w.to_wire(m_data); }
    int data_from_wire(const WireFormat& w) override {
      Parameter::set(); 
      return w.from_wire(m_data);
    }

  private:
    std::string m_data;
};

// TODO repeated parameters

} // end namespace mig

#endif // ifndef _MIGMSG_H_
