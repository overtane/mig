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

//extern int par_wire_overhead;
//extern int msg_wire_overhead;

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
        value_is_set(false) {} 
    virtual ~parameter() {}

    bool is_set() const { return this->value_is_set; } 
    void set() { this->value_is_set = true; }
    bool is_optional() const { return this->optional; } 
    uint16_t id() { return this->par_id; }

    // size should be amount of data bytes to read/write to wire
    // when parameter is not set, size equals always zero 
    virtual std::size_t size() const = 0;
    //virtual std::size_t wire_overhead() const = 0;
    //std::size_t wire_size() const { return this->size() + this->wire_overhead(); }
    bool is_valid() { return this->is_set() || this->is_optional(); }
};


class GroupBase {
    std::vector<::mig::parameter * const>& allpars;

  public:
    GroupBase() = delete;
    GroupBase(std::vector<::mig::parameter * const>& allpars) : allpars(allpars)  {}
    virtual ~GroupBase() {}

    int npars() { return this->allpars.size(); } 
    std::vector<::mig::parameter * const>& params() { return this->allpars; }
    bool is_valid() const {
        for (auto it : this->allpars) if (!it->is_valid()) return false;
        return true;
    }
    std::size_t size() const {
        std::size_t s = 0;
        for (auto it : this->allpars) s += it->size();
        return s;
    } 
    //virtual std::size_t wire_overhead() const { 
    //    std::size_t s = 0;
    //    for (auto it : this->allpars) s += it->wire_overhead();
    //    return s;
    //} 
    bool is_set() const { return this->is_valid(); } // group is set if it is valid
    //std::size_t wire_size() const { return this->size() + this->wire_overhead(); }
};

class Message : public GroupBase {
    const int msg_id;

  public:
    Message(uint16_t id, std::vector<::mig::parameter * const>& allpars) : 
        GroupBase(allpars), msg_id(id)  {}
    virtual ~Message() {}

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
    scalar_parameter& operator=(T value) { this->set(value); return *this; } 

    void set(T value) { this->value = value; this->parameter::set(); }
    T get() const { return this->value; }
    std::size_t size() const { return (this->is_set()) ? sizeof(T): 0; }
    //std::size_t wire_overhead() const { return mig::par_wire_overhead; }
};

template <>
class scalar_parameter <void_t> : public parameter
{

  public:
    scalar_parameter(int id, bool optional=false) : parameter(id, optional) {}

    bool get() const { return this->is_set(); }
    std::size_t size() const { return 0; }
    // std::size_t wire_overhead() const { return mig::par_wire_overhead; }
};

template <class T>
class group_parameter : public parameter
{   

  public:
    T group;

    group_parameter(int id, bool optional=false) : parameter(id, optional) {}
    virtual ~group_parameter() {} 

    // set(T group) this could be copy operation 
    T& get() { return this->group; }

    std::size_t is_valid() const { return (this->group.is_valid() || this->is_optional); }
    std::size_t is_set() const { return (this->group.is_set()); }
    std::size_t size() const { return this->group.size(); }
    // std::size_t wire_overhead() const { return mig::par_wire_overhead; } // TODO
};



template <class T>
class composite_parameter : public parameter
{   
    T* data = nullptr;

  public:
    composite_parameter(int id, bool optional=false) : parameter(id, optional)  {}
    virtual ~composite_parameter() { if (data) delete data; } 

    void set(T* data) {
        if (this->data) delete data;
        this->data = data;
        this->parameter::set();
    }
    T* get() { return this->data; }

    std::size_t size() const { return (this->is_set()) ? this->data->size(): 0; }
    // std::size_t wire_overhead() const { return mig::par_wire_overhead; } // TODO
};

template <>
class composite_parameter <std::string>: public parameter
{   
    std::string data;

  public:
    composite_parameter(int id, bool optional=false) : parameter(id, optional) {}
    virtual ~composite_parameter() { } 

    void set(std::string data) { this->data = data; this->parameter::set(); }
    std::string& get() { return this->data; }
    std::size_t size() const { return (this->is_set()) ? this->data.size()+1: 0; }

    // string parameter overhead as big as single parameter
    // std::size_t wire_overhead() const { return mig::par_wire_overhead; }
};

} // end namespace mig


// TODO repeated parameters

#endif // ifndef _MIGMSG_H_
