# ifndef _MIG_TEMPLATES_H_
# define _MIG_TEMPLATES_H_

#include <cstdint>
#include <cstddef>

namespace mig {

enum ParameterOpt {
  OPTIONAL = true,
  REQUIRED = false
};

class Message {
    const uint16_t msg_id;

  public:
    Message(uint16_t id) : msg_id(id) {}
    uint16_t id() const { return this->msg_id; }
    virtual bool is_valid() = 0;
    virtual std::size_t size() = 0;
    // bool encode(std:ostream& os); // TODO: operator >>
};

typedef uint8_t enum_t;
typedef struct {}  void_t;

template <class T>
class simple_parameter
{
    uint16_t par_id; /* TODO should be const, needs copy assignment op */
    bool optional;   /* TODO should be const */
    bool exists;
    T value = T(0);

  public:
    simple_parameter(int id, bool optional=false) : 
        par_id(id),
        optional(optional),
        exists(false) 
    {}

    bool is_set() const { return this->exists; } 
    bool is_optional() const { return this->optional; } 
    void set(T value) { this->value = value; this->exists = true; }
    T get() const { return this->value; }
    std::size_t size() const { return (this->is_set()) ? sizeof(T): 0; }
    uint16_t id() { return this->par_id; }
};

template <>
class simple_parameter <void_t>
{
    uint16_t par_id;
    bool optional;
    bool exists;

  public:
    simple_parameter(int id, bool optional=false) : 
        par_id(id),
        optional(optional),
        exists(false) 
    {}

    bool is_set() const { return this->exists; } 
    bool is_optional() const { return this->optional; } 
    void set() { this->exists = true; }
    bool get() const { return this->is_set(); }
    std::size_t size() const { return 0; }
    uint16_t id() { return this->par_id; }
};


template <class T>
class group_parameter
{   
    uint16_t par_id;
    bool optional;
    bool exists;
    T data;

  public:
    group_parameter(int id, bool optional=false) :
        par_id(id),
        optional(optional),
        exists(false)
    {}

    bool is_set() const { return this->exists; } 
    bool is_optional() const { return this->optional; } 
    void set(T data) { this->data = data; this->exists = true; }
    T& get() { return this->data; } // What is not defined yet?
    std::size_t size() const { return (this->is_set()) ? this->data.size(): 0; }
    uint16_t id() { return this->par_id; }
};

} // end namespace mig

#endif
