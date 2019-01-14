
#include "../migmsg.h"
#include <iostream>
#include <iomanip>

extern "C" {
#include <arpa/inet.h>
}

// Prototyping API
//
// g++ -Wall -std=c++14 -o proto api_proto.cpp

// --
// 'Generated' definitions 

enum TestEnum1 : ::mig::enum_t {
  VALUE1 = 1,
  VALUE2 = 99
};

struct TestGroup1 : ::mig::GroupBase {
    ::mig::scalar_parameter<std::int16_t> param1{0, ::mig::OPTIONAL};
    ::mig::scalar_parameter<std::int32_t> param2{1, ::mig::OPTIONAL};

    TestGroup1() : ::mig::GroupBase(allpars) {}
    virtual ~TestGroup1() {}
  private:
    std::vector<::mig::parameter * const> allpars = { &param1, &param2 };

};

class TestMessage1002 : public ::mig::Message {

    std::vector<::mig::parameter * const> allpars = {
      &param1, &param2, &param3, &param4, &param5, &param6, &param7, &param8 };

  public:
    TestMessage1002() : ::mig::Message(0x1002, allpars) {}
    virtual ~TestMessage1002() {}

    ::mig::scalar_parameter<std::int8_t> param1{0, ::mig::OPTIONAL};
    ::mig::scalar_parameter<bool> param2{1, ::mig::REQUIRED};  
    ::mig::scalar_parameter<std::uint32_t> param3{2, ::mig::OPTIONAL};  
    ::mig::scalar_parameter<mig::void_t> param4{3};  
    ::mig::scalar_parameter<TestEnum1> param5{4};  

    ::mig::group_parameter<TestGroup1> param6{5};
    ::mig::composite_parameter<std::string> param7{6};
    ::mig::composite_parameter<mig::blob_t> param8{7};
};

// --
// API usage

int main() {

  TestMessage1002 m2;
  
  m2.param1.set(8);
  m2.param2.set(true);
  //m2.param3.set(12345567);
  m2.param4.set();

  m2.param5.set(VALUE2);
  //std::cout << m2.param7.size() << '\n';

  m2.param6.m_group.param1.set(2);
  m2.param6.m_group.param2.set(1234567890);

  std::string str("Hello World");

  m2.param7.set(str);
  //std::cout << m2.param7.size() << '\n';

  mig::blob_t *testData = new mig::blob_t(3); 

  (*testData)[0] = 1;
  (*testData)[1] = 1;
  (*testData)[2] = 1;

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
            << m2.param5.get() + 0
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
}


