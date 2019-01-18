
#include "../migmsg.h"
#include <iostream>
#include <iomanip>

/* Prototype and playground for API generation */

extern "C" {
#include <arpa/inet.h>
}

// Prototyping API
//
// g++ -Wall -std=c++14 -o proto api_proto.cpp -I.. ../migmsg.cpp sampleproto.cpp

// --
// 'Generated' definitions 

// enum TestEnum1 {
//  VALUE1 = 1,
//  VALUE2 = 99
// }

enum class TestEnum1 : mig::enum_t {
  VALUE1 = 1,
  VALUE2 = 99
};

// group TestGroup1 {
//  int16 param1 = 0 [optional],
//  int32 param2 = 1 [optional]
// }

struct TestGroup1 : ::mig::GroupBase {
    ::mig::scalar_parameter<int16_t> param1{0, ::mig::OPTIONAL};
    ::mig::scalar_parameter<int32_t> param2{1, ::mig::OPTIONAL};

    TestGroup1() : ::mig::GroupBase(m_params) {}
    virtual ~TestGroup1() {}
  private:
    std::map<int, ::mig::parameter&> m_params = {
        {0, param1},
        {1, param2}
    };
};

// message TestMessage1002 = 0x1002 {
//   int8 param1 = 0 [optional],
//   bool param2 = 1,
//   bool param3 = 2 [optional],
//   uint32 param4 = 3,
//   TestEnum1 param5 = 4,
//   TestGroup1 param6 = 5,
//   string param7,
//   blob param8
// }

class TestMessage1002 : public ::mig::Message {

  public:
    TestMessage1002(::mig::WireFormat *wire_format=nullptr) : 
      ::mig::Message(0x1002, m_params, wire_format) { std::cout<<"m_params: "<<m_params.size()<<'\n';}
    virtual ~TestMessage1002() {}

    ::mig::scalar_parameter<int8_t> param1{0, ::mig::OPTIONAL};
    ::mig::scalar_parameter<bool> param2{1, ::mig::REQUIRED};  
    ::mig::scalar_parameter<uint32_t> param3{2, ::mig::OPTIONAL};  
    ::mig::scalar_parameter<mig::void_t> param4{3};  
    ::mig::enum_parameter<TestEnum1> param5{4};  

    ::mig::group_parameter<TestGroup1> param6{5};
    ::mig::var_parameter<std::string> param7{6};
    ::mig::var_parameter<mig::blob_t> param8{7};

  private:
    const std::map<int, ::mig::parameter&> m_params = {
      {0, param1},
      {1, param2},
      {2, param3},
      {3, param4},
      {4, param5},
      {5, param6},
      {6, param7},
      {7, param8}
  };
};

void dump(std::ostream& os, const mig::Message& msg) {

  os << "Message "
     << ((msg.is_valid()) ? "is valid " : "not valid ")
     << "size "
     << std::dec << msg.size()
     << "\n";

  for (auto& it : msg.params()) {

    mig::parameter& par = it.second;

    os << "param "
       << par.id()
//            << ", value "
//            << m2.param1.get() + 0
       << ", defined " << par.is_set()
       << ", optional " << par.is_optional()
       << ", size " << par.size()
       << '\n';
  }

}

// --
// API usage

int main() {

  TestMessage1002 m2;
  
  m2.param1.set(8);
  m2.param2.set(true);
  //m2.param3.set(12345567);
  m2.param4.set();

  m2.param5 = TestEnum1::VALUE2;
  //std::cout << m2.param7.size() << '\n';

  m2.param6.data().param1.set(2);
  m2.param6.data().param2.set(1234567890);

  std::string str("Hello World");

  m2.param7.set(str);
  //std::cout << m2.param7.size() << '\n';

  uint8_t data[3] = { 1, 1, 1 };

  mig::blob_t testData; 
  testData.assign(data, 3);  

  m2.param8.set(testData);
 
  std::cout << "Param1 id "
            << m2.param1.id()
            << ", value "
            << m2.param1.get() + 0
            << ", defined " << m2.param1.is_set()
            << ", optional " << m2.param1.is_optional()
            << ", size " << m2.param1.size()
            << '\n';

  std::cout << "Param2 id "
            << m2.param2.id()
            << ", value "
            << m2.param2.get() + 0
            << ", defined " << m2.param2.is_set()
            << ", optional " << m2.param2.is_optional()
            << ", size " << m2.param2.size()
            << '\n';

  std::cout << "Param3 id "
            << m2.param3.id()
            << ", value "
            << m2.param3.get() + 0
            << ", defined " << m2.param3.is_set()
            << ", optional " << m2.param3.is_optional()
            << ", size " << m2.param3.size()
            << '\n';

  std::cout << "Param4 id "
            << m2.param4.id()
            << ", value "
            << m2.param4.get() + 0
            << ", defined " << m2.param4.is_set()
            << ", optional " << m2.param4.is_optional()
            << ", size " << m2.param4.size()
            << '\n';

  std::cout << "Param5 id "
            << m2.param5.id()
            << ", value "
            << int(m2.param5.get()) 
            << ", defined " << m2.param5.is_set()
            << ", optional " << m2.param5.is_optional()
            << ", size " << m2.param5.size()
            << '\n';

  std::cout << "Param6 id "
            << m2.param6.id()
            << ", value "
            << " group" // get returns reference. What if not defined?
            << ", defined " << m2.param6.is_set()
            << ", optional " << m2.param6.is_optional()
            << ", size " << m2.param6.size()
            << '\n';

  std::cout << "Param7 id "
            << m2.param7.id()
            << ", value "
            << m2.param7.get()
            << ", defined " << m2.param7.is_set()
            << ", optional " << m2.param7.is_optional()
            << ", size " << m2.param7.size()
            << '\n';

  std::cout << "Param8 id "
            << m2.param8.id()
            << ", value "
            << " reference" // get returns reference. What if not defined?
            << ", defined " << m2.param8.is_set()
            << ", optional " << m2.param8.is_optional()
            << ", size " << m2.param8.size()
            << '\n';

  std::cout << "Message "
            << ((m2.is_valid()) ? "is valid " : "not valid ")
            << "size "
            << m2.size()
            << "\n";


  m2.to_wire();
  m2.dump(std::cout);

  size_t n = m2.wire_format()->buf()->size();

  uint8_t *p = new uint8_t[n];
  m2.wire_format()->buf()->reset();
  memcpy(p,  m2.wire_format()->buf()->getp(n), n);

  mig::Message *m3 = mig::Message::factory(mig::WireFormat::factory(p,n));
  if (m3) m3->dump(std::cout);
  dump(std::cout, *m3);
}

mig::Message *mig::Message::factory(mig::WireFormat *w) {

  if (w) {
    mig::Message *m = nullptr;

    uint16_t id, size;

    w->buf()->reset();
    w->from_wire(id);
    w->from_wire(size);

    m = new TestMessage1002(w);

    m->from_wire(w);

    return m;

  } else {
    return nullptr;
  }

}
