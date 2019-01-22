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
typedef std::map<int, Parameter&> parameter_container_t;
typedef std::unique_ptr<Message> message_ptr_t;
typedef std::unique_ptr<WireFormat> wire_format_ptr_t;
typedef std::unique_ptr<MsgBuf> msgbuf_ptr_t;
typedef std::unique_ptr<uint8_t []> storage_ptr_t; // for dynamic storage areas

typedef message_ptr_t (*MessageCreatorFunc)(void);

struct void_t {};

class blob_t {

  public:
    blob_t(storage_ptr_t& p, size_t n) { assign(p, n); }
    blob_t(const uint8_t *p, size_t n) { assign(p, n); }
    blob_t(blob_t& b) { copy(b.m_data, b.m_size); }
    blob_t(blob_t&& b) noexcept { move(b); }

    void assign(storage_ptr_t& p, size_t n) { // move m_storage to new owner
      m_storage = std::move(p);
      m_data = m_storage.get();
      m_size = (m_data) ? n : 0;
    }

    void assign(const uint8_t *p, size_t n) { // grab not owned pointer p
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

    void move(blob_t& b) { //! move blob to new owner
      assign(b.m_storage, b.m_size); 
      b.m_data = nullptr;
      b.m_storage = nullptr;
      b.m_size = 0;
    }

    void copy(const uint8_t *p, size_t n) {
      m_storage = std::make_unique<uint8_t []>(n);
      memcpy(m_storage.get(), p, n);
      m_data = m_storage.get();
      m_size = n;
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
    storage_ptr_t m_storage = nullptr;
    const uint8_t *m_data = nullptr;
    size_t m_size = 0;
};

class string_t {

  public:
    string_t(const char *p) { copy(p); } // copy a c-string
    string_t(const char *p, size_t n) { assign(p, n); } // no copy, just assign
    string_t(const std::string& str) { copy(str); } // copy cpp-string
    string_t(const string_t& str) { copy(str); } // copy mig-string
    string_t(string_t&& str) noexcept { move(str); } // move constructor

    friend std::ostream& operator<<(std::ostream&, const string_t&);

    void assign(const char *p, size_t n) { // assign non-managed area without copy
      m_data = p;
      m_storage = nullptr;
      m_size = (m_data) ? n : 0;
      // TODO check null terminator
    }

    void assign(string_t& str) {
      if (str.m_storage != nullptr) {
        m_storage = std::move(str.m_storage);
        m_data = (const char *)m_storage.get();
        m_size = (m_data) ? str.m_size : 0;
      } else {
        assign(str.m_data, str.m_size);
      }
    }

    void move(string_t& str) {
      assign(str);
      str.m_storage = nullptr;
      str.m_data = nullptr;
      str.m_size = 0;
    }

    void copy(const string_t& str) {
      m_storage = std::make_unique<uint8_t []>(str.m_size);
      memcpy((char *)m_storage.get(), str.m_data, str.m_size); 
      m_data = (const char *)m_storage.get();
      m_size = str.m_size;
    }

    void copy(const std::string& str) {
      auto size = str.size();
      m_storage = std::make_unique<uint8_t []>(size+1);
      memcpy((char *)m_storage.get(), str.c_str(), size);
      m_storage[size] = '\0';
      m_data = (const char *)m_storage.get();
      m_size = size+1;
    }

    void copy(const char *p) {
      auto length = [](const char *p){ auto i=0; while (p[i]!='\0') i++; return i; };
      m_storage = std::make_unique<uint8_t []>(length(p)+1);
      memcpy((char *)m_storage.get(), p, length(p));
      m_storage[length(p)] = '\0';
      m_data = (const char *)m_storage.get();
      m_size = length(p)+1;
    }

    bool equals(const string_t& s) const {
      if (m_size != s.size())
        return false;
      else if (m_data == s.data())
        return true;
      else {
        for (auto i=0; i<m_size; i++)
          if (m_data[i] != s.data()[i]) return false;
      }
      return true;
    }

    const char *data() const { return m_data; }
    size_t size() const { return m_size; }
    size_t length() const {
      for (int i=0; i<m_size; i++)
        if (m_data[i] == '\0')
          return i;
      return m_size-1; 
    }
  
  private:
    storage_ptr_t m_storage = nullptr;
    const char *m_data = nullptr;
    size_t m_size = 0;
};

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
    virtual int advance(int) = 0;
    //! revers buffer pointer
    virtual int reverse(int) = 0;
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
    virtual int to_wire(const string_t&);
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
    virtual int from_wire(string_t&) const = 0;
    virtual int from_wire(std::string&) const = 0;

    virtual void dump(std::ostream&, const Message&) const = 0;
    virtual void dump(std::ostream&, const Group&, int) const = 0;
    virtual void dump(std::ostream&, const Parameter&) const = 0;

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
    Parameter() = delete;
    Parameter(int id, bool optional=false, bool repeated=false) :
        m_id(id),
        m_optional(optional),
        m_repeated(repeated),
        m_is_set(false) {}
    Parameter(const Parameter& p) :
        m_id(p.m_id),
        m_optional(p.m_optional),
        m_repeated(p.m_repeated),
        m_is_set(p.m_is_set) {}
    virtual ~Parameter() {}

    void set() { this->m_is_set = true; }
    bool is_optional() const { return this->m_optional; }
    bool is_repeated() const { return this->m_repeated; }
    int  id() const { return this->m_id; }

    virtual bool is_scalar() const = 0;
    virtual const Group* group() const = 0;
    virtual int nrepeats() const { return 1; }
    virtual bool is_set() const { return this->m_is_set; }
    virtual std::size_t item_size() const = 0;
    virtual std::size_t data_size() const { return nrepeats() * item_size(); }
    virtual bool is_valid() const { return this->is_set() || this->is_optional(); }
    virtual int data_to_wire(WireFormat&, int i=0) const = 0;
    virtual int data_from_wire(const WireFormat&) = 0;

  private:
    const int m_id;
    const bool m_optional;
    const bool m_repeated;
    bool m_is_set;
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
    std::size_t data_size() const {
        std::size_t s = 0;
        for (auto& it : this->m_params) s += it.second.data_size();
        return s;
    } 
    bool is_set() const { return this->is_valid(); } // group is set if it is valid

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
    ScalarParameter(const ScalarParameter& p) : Parameter(p), m_data(p.m_data) {}
    ScalarParameter& operator=(const T& value) { this->assign(value); return *this; } 
    bool operator==(const T& value) const { return m_data == value; }
    bool operator!=(const T& value) const { return m_data != value; }
    bool operator>(const T& value) const { return m_data > value; }
    bool operator>=(const T& value) const { return !(m_data < value); }
    bool operator<(const T& value) const { return m_data < value; }
    bool operator<=(const T& value) const { return !(m_data > value); }

    void assign(const T& value) { this->m_data = value; this->Parameter::set(); }
    const T& data() const { return this->m_data; }
    size_t item_size() const override { return sizeof(T); }
    const Group* group() const override { return nullptr; }
    bool is_scalar() const override { return true; }
    //bool is_repeated() const override { return false; }

    int data_to_wire(WireFormat& w, int) const override { return w.to_wire(m_data); }
    int data_from_wire(const WireFormat& w) override { 
      Parameter::set(); 
      return w.from_wire(m_data); 
    }

  private:
    T m_data = T(0);
};


template <class T>
class ScalarArray : public Parameter {

  public:
    ScalarArray(int id, bool optional=false) : Parameter(id, optional, true) {}

    T& operator[](int i) { 
      if (m_data.size() == i) // a light hack: indexing end pos pushes new item
        m_data.push_back(0);  // TODO fix this
      return m_data[i]; }

    void assign(int i, const T value) {
      if (i < m_data.size())
        this->m_data[i] = value;
    }
    const T& data(int i) const { return this->m_data[i]; }
    std::vector<T>& data() { return this->m_data; }
    size_t item_size() const override { return sizeof(T); }
    const Group* group() const override { return nullptr; }
    bool is_scalar() const override { return true; }
    bool is_set() const override { return this->m_data.size() > 0; }
    int nrepeats() const override { return m_data.size(); }

    int data_to_wire(WireFormat& w, int i) const override { 
      if (i < m_data.size())
          return w.to_wire(m_data[i]);
      return -1;   
    }
    int data_from_wire(const WireFormat& w) override { 
      T data;
      auto ret = w.from_wire(data); 
      if (ret  == 0) 
        m_data.push_back(data);
      return ret; 
    }

  private:
    std::vector<T> m_data;
};


template <>
class ScalarParameter <void_t> : public Parameter {

  public:
    ScalarParameter(int id, bool optional=false) : Parameter(id, optional) {}

    std::size_t item_size() const override { return 0; }
    const Group* group() const override { return nullptr; }
    bool is_scalar() const override { return true; }

    int data_to_wire(WireFormat& w, int) const override { (void)w; return 0; }
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
    std::size_t item_size() const override { return sizeof(mig::enum_t); }
    const Group* group() const override { return nullptr; }
    bool is_scalar() const override { return true; }

    int data_to_wire(WireFormat& w, int) const override { return w.to_wire((enum_t)m_data); }
    int data_from_wire(const WireFormat& w) override {
      uint8_t x;
      auto ret = w.from_wire(x);
      if (ret == 0) {
        m_data = static_cast<T>(x);
        Parameter::set();
      }
      return ret;
    }

  private:
    T m_data = T(0);
};

template <class T>
class GroupParameter : public Parameter {   

  public:
    GroupParameter(int id, bool optional=false) : Parameter(id, optional) {} 
    virtual ~GroupParameter() {} 

    //void assign(const T& group) // TODO this could be a deep copy operation 
    T& data() { return this->m_data; } // non-const return so that group params may be accessed
    std::size_t item_size() const override { return this->m_data.data_size(); }
    const Group* group() const override { return (Group*)&m_data; }
    bool is_scalar() const override { return false; }

    bool is_set() const override { return this->m_data.is_set(); }
    bool is_valid() const override { return (this->m_data.is_valid() || this->is_optional()); }

    int data_to_wire(WireFormat& w, int) const override { return w.to_wire((const Group&)(m_data)); }
    int data_from_wire(const WireFormat& w) override { return w.from_wire((Group&)(m_data)); }

  private:
    T m_data;

};

template <class T>
class VarParameter : public Parameter {

  public:
    VarParameter(int id, bool optional=false) : Parameter(id, optional) {}
    virtual ~VarParameter() {}

    void assign(T& data) {
        m_data.assign(data);
        this->Parameter::set();
    }
    const T& data() { return this->m_data; }
    std::size_t item_size() const override { return this->m_data.size(); }
    const Group* group() const override { return nullptr; }
    bool is_scalar() const override { return false; }

    int data_to_wire(WireFormat& w, int) const override { return w.to_wire(m_data); }
    int data_from_wire(const WireFormat& w) override { 
      Parameter::set();
      return w.from_wire(m_data); 
    }

  private:
    T m_data = { nullptr, 0 };

};

/* Note: obsolete */
template <>
class VarParameter <std::string>: public Parameter
{   

  public:
    VarParameter(int id, bool optional=false) : Parameter(id, optional) {}
    virtual ~VarParameter() {} 

    void assign(const std::string& data) { this->m_data = data; this->Parameter::set(); }
    std::string& data() { return this->m_data; }
    const Group* group() const override { return nullptr; }
    bool is_scalar() const override { return false; }

    std::size_t item_size() const override { return this->m_data.size()+1; }
    int data_to_wire(WireFormat& w, int) const override { return w.to_wire(m_data); }
    int data_from_wire(const WireFormat& w) override {
      Parameter::set(); 
      return w.from_wire(m_data);
    }

  private:
    std::string m_data;
};

} // end namespace mig

#endif // ifndef _MIGMSG_H_
