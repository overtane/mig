
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

struct TestGroup1 : ::mig::Group {
    ::mig::ScalarParameter<int16_t> param1{0, ::mig::OPTIONAL};
    ::mig::ScalarParameter<int32_t> param2{1, ::mig::OPTIONAL};

    TestGroup1() : ::mig::Group(m_params) {}
    virtual ~TestGroup1() {}
  private:
    const ::mig::parameter_container_t  m_params = {
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
    TestMessage1002() : ::mig::Message(0x1002, m_params) { }
    virtual ~TestMessage1002() {}
    static ::mig::message_ptr_t create() { return std::make_unique<TestMessage1002>(); }

    ::mig::ScalarParameter<int8_t> param1{8, ::mig::OPTIONAL};
    ::mig::ScalarParameter<bool> param2{1, ::mig::REQUIRED};
    ::mig::ScalarParameter<uint32_t> param3{2, ::mig::OPTIONAL};
    ::mig::ScalarParameter<mig::void_t> param4{3};
    ::mig::EnumParameter<TestEnum1> param5{4};

    ::mig::GroupParameter<TestGroup1> param6{5};
    ::mig::VarParameter<std::string> param7{6};
    ::mig::VarParameter<mig::blob_t> param8{0};

  private:
    const ::mig::parameter_container_t m_params = {
      {1, param2},
      {2, param3},
      {3, param4},
      {4, param5},
      {5, param6},
      {6, param7},
      {0, param8},
      {8, param1}
  };
};

const std::map<int, mig::MessageCreatorFunc> mig::Message::creators {
  {0x1002, TestMessage1002::create}
};


void dump(std::ostream& os, const mig::Message& msg) {

  os << "Message "
     << "0x" << std::hex << msg.id()
     << ((msg.is_valid()) ? " is valid " : "not valid ")
     << "size "
     << std::dec << msg.size()
     << "\n";

  for (auto& it : msg.params()) {

    mig::Parameter& par = it.second;

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
  
  m2.param1.assign(8);
  m2.param2.assign(true);
  //m2.param3.set(12345567);
  m2.param4.set();

  m2.param5 = TestEnum1::VALUE2;
  //std::cout << m2.param7.size() << '\n';

  m2.param6.data().param1.assign(2);
  m2.param6.data().param2.assign(1234567890);

  std::string str("Hello World");

  m2.param7.assign(str);
  //std::cout << m2.param7.size() << '\n';

  uint8_t data[3] = { 1, 1, 1 };

  mig::blob_t testData(data, 3); 

  m2.param8.assign(testData);
 
  std::cout << "Param1 id "
            << m2.param1.id()
            << ", value "
            << m2.param1.data()
            << ", defined " << m2.param1.is_set()
            << ", optional " << m2.param1.is_optional()
            << ", size " << m2.param1.size()
            << '\n';

  std::cout << "Param2 id "
            << m2.param2.id()
            << ", value "
            << m2.param2.data() + 0
            << ", defined " << m2.param2.is_set()
            << ", optional " << m2.param2.is_optional()
            << ", size " << m2.param2.size()
            << '\n';

  std::cout << "Param3 id "
            << m2.param3.id()
            << ", value "
            << m2.param3.data() + 0
            << ", defined " << m2.param3.is_set()
            << ", optional " << m2.param3.is_optional()
            << ", size " << m2.param3.size()
            << '\n';

  std::cout << "Param4 id "
            << m2.param4.id()
            << ", value "
            << " none "  
            << ", defined " << m2.param4.is_set()
            << ", optional " << m2.param4.is_optional()
            << ", size " << m2.param4.size()
            << '\n';

  std::cout << "Param5 id "
            << m2.param5.id()
            << ", value "
            << int(m2.param5.data()) 
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
            << m2.param7.data()
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

  auto n = m2.wire_format()->buf()->size();
  auto p = std::make_unique<uint8_t[]>(n);
  m2.wire_format()->buf()->reset();
  memcpy(p.get(),  m2.wire_format()->buf()->getp(n), n);

  auto w = mig::WireFormat::factory(p,n);

  auto m3 = mig::Message::factory(w);
  if (m3.get())
    m3->dump(std::cout);
  dump(std::cout, *m3);
}


