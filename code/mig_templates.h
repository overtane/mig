# ifndef _MIG_TEMPLATES_H_
# define _MIG_TEMPLATES_H_

#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>

namespace mig {

extern int par_wire_overhead;
extern int msg_wire_overhead;

enum ParameterOpt {
  OPTIONAL = true,
  REQUIRED = false
};

class parameter
{
    uint16_t par_id;
    bool optional;
    bool value_is_set;


  public:
    parameter(int id, bool optional=REQUIRED) :
        par_id(id),
        optional(optional),
        value_is_set(false) 
    {}

    virtual ~parameter() {}

    bool is_set() const { return this->value_is_set; } 
    void set() { this->value_is_set = true; }
    bool is_optional() const { return this->optional; } 
    uint16_t id() { return this->par_id; }

    virtual std::size_t size() const = 0;
    virtual std::size_t wire_overhead() const = 0;
    std::size_t wire_size() const { return this->size() + this->wire_overhead(); }
    bool is_valid() { return this->is_set() || this->is_optional(); }
};


class GroupBase {
    std::vector<::mig::parameter * const>& allpars;

  public:
    GroupBase() = delete;
    GroupBase(std::vector<::mig::parameter * const>& allpars) : allpars(allpars)  {}

    int npars() { return this->allpars.size(); } 
    std::vector<::mig::parameter * const>& params() { return this->allpars; }
    virtual bool is_valid() const {
        for (auto it : this->allpars) if (!it->is_valid()) return false;
        return true;
    }
    std::size_t size() const {
        std::size_t s = 0;
        for (auto it : this->allpars) s += it->size();
        return s;
    } 
    virtual std::size_t wire_overhead() const { 
        std::size_t s = 0;
        for (auto it : this->allpars) s += it->wire_overhead();
        return s;
    } 
    std::size_t wire_size() const { return this->size() + this->wire_overhead(); }
};

class Message : public GroupBase {
    const int msg_id;

  public:
    Message(uint16_t id, std::vector<::mig::parameter * const>& allpars) : 
        GroupBase(allpars), msg_id(id)  {}

    uint16_t id() const { return this->msg_id; }

    // bool encode(std:ostream& os); // TODO: operator >>
};

typedef uint8_t enum_t;
typedef std::vector<uint8_t> blob_t; 
struct void_t {};

template <class T>
class scalar_parameter : public parameter
{
    T value = T(0);

  public:
    scalar_parameter(int id, bool optional=false) : parameter(id, optional) {}

    void set(T value) { this->value = value; this->parameter::set(); }
    T get() const { return this->value; }
    std::size_t size() const { return (this->is_set()) ? sizeof(T): 0; }
    std::size_t wire_overhead() const { return mig::par_wire_overhead; }
};

template <>
class scalar_parameter <void_t> : public parameter
{

  public:
    scalar_parameter(int id, bool optional=false) : parameter(id, optional) {}

    bool get() const { return this->is_set(); }
    std::size_t size() const { return 0; }
    std::size_t wire_overhead() const { return mig::par_wire_overhead; }
};


template <class T>
class compound_parameter : public parameter
{   
    T* data;

  public:
    compound_parameter(int id, bool optional=false) : parameter(id, optional), data(nullptr) {}
    virtual ~compound_parameter() { if (data) delete data; } 

    void set(T* data) {
        if (this->data)
            delete data;
        this->data = data;
        this->parameter::set();
    }
    T* get() { return this->data; }

    std::size_t size() const { return (this->is_set()) ? this->data->size(): 0; }
    std::size_t wire_overhead() const { return mig::par_wire_overhead; } // TODO
};


template <>
class compound_parameter <std::string>: public parameter
{   
    std::string data;

  public:
    compound_parameter(int id, bool optional=false) : parameter(id, optional) {}

    void set(std::string data) { this->data = data; this->parameter::set(); }
    std::string& get() { return this->data; }
    std::size_t size() const { return (this->is_set()) ? this->data.size()+1: 0; }

    // string parameter overhead as big as single parameter
    std::size_t wire_overhead() const { return mig::par_wire_overhead; }
};

} // end namespace mig

#endif
