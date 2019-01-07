# ifndef _MIG_TEMPLATES_H_
# define _MIG_TEMPLATES_H_

#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>

namespace mig {

extern int par_wire_overhead;

enum ParameterOpt {
  OPTIONAL = true,
  REQUIRED = false
};

class Message {
    const uint16_t msg_id;

  public:
    Message(uint16_t id) : msg_id(id) {}
    uint16_t id() const { return this->msg_id; }
    virtual bool is_valid() const = 0;
    virtual std::size_t size() const = 0;
    virtual std::size_t wire_overhead() const = 0;
    std::size_t wire_size() { return this->size() + this->wire_overhead(); }
    // bool encode(std:ostream& os); // TODO: operator >>
};

typedef uint8_t enum_t;
typedef std::vector<uint8_t> blob_t; 
struct void_t {};

class parameter
{
    uint16_t par_id;
    bool optional;
    bool exists;

  public:
    parameter(int id, bool optional=false) :
        par_id(id),
        optional(optional),
        exists(false) 
    {}

    bool is_set() const { return this->exists; } 
    void set() { this->exists = true; }
    bool is_optional() const { return this->optional; } 
    uint16_t id() { return this->par_id; }

    virtual std::size_t size() const = 0;
    virtual std::size_t wire_overhead() const = 0;
    std::size_t wire_size() const { return this->size() + this->wire_overhead(); }
};

template <class T>
class simple_parameter : public parameter
{
    T value = T(0);

  public:
    simple_parameter(int id, bool optional=false) : parameter(id, optional) {}

    void set(T value) { this->value = value; this->parameter::set(); }
    T get() const { return this->value; }
    std::size_t size() const { return (this->is_set()) ? sizeof(T): 0; }
    std::size_t wire_overhead() const { return mig::par_wire_overhead; }
};

template <>
class simple_parameter <void_t> : public parameter
{

  public:
    simple_parameter(int id, bool optional=false) : parameter(id, optional) {}

    bool get() const { return this->is_set(); }
    std::size_t size() const { return 0; }
    std::size_t wire_overhead() const { return mig::par_wire_overhead; }
};


template <class T>
class complex_parameter : public parameter
{   
    T data;

  public:
    complex_parameter(int id, bool optional=false) : parameter(id, optional) {}

    // TODO OPT would it be more efficient to use reference here and in member
    // variable.
    void set(T data) { this->data = data; this->parameter::set(); }
    T& get() { return this->data; }


    std::size_t size() const { return (this->is_set()) ? this->data.size(): 0; }

    // TODO
    std::size_t wire_overhead() const { return mig::par_wire_overhead; }
};


template <>
class complex_parameter <std::string>: public parameter
{   
    std::string data;
&
  public:
    complex_parameter(int id, bool optional=false) : parameter(id, optional) {}

    void set(std::string data) { this->data = data; this->parameter::set(); }
    std::string& get() { return this->data; }
    std::size_t size() const { return (this->is_set()) ? this->data.size()+1: 0; }

    // string parameter overhead as big as single parameter
    std::size_t wire_overhead() const { return mig::par_wire_overhead; }
};

} // end namespace mig

#endif
