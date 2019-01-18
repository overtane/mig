//  msg_tests.msg.h
//  This is an automatically generated file
//  Please do not edit
//  Source:  msg_tests.msg
//  Fri Jan 18 10:45:01 2019

#ifndef _MSG_TESTS_MSG_H_
#define _MSG_TESTS_MSG_H_

#include "migmsg.h"

enum class TestEnum1 : ::mig::enum_t {
  VALUE1 = 0,
  VALUE2 = 1,
  Count = 2
};


class TestMessage1001 : public ::mig::Message {

  public:
    TestMessage1001() : ::mig::Message(0x1001, m_params) {}

  private:
    const std::map<int, ::mig::parameter&> m_params = {
    };
};


struct TestGroup1 : ::mig::GroupBase {

  public:
    TestGroup1() : ::mig::GroupBase(m_params) {}
    ::mig::scalar_parameter<::mig::void_t> param1{0};
    ::mig::scalar_parameter<uint32_t> param2{9};

  private:
    const std::map<int, ::mig::parameter&> m_params = {
      {0, param1},
      {9, param2},
    };
};


class TestMessage1002 : public ::mig::Message {

  public:
    TestMessage1002() : ::mig::Message(0x1002, m_params) {}
    ::mig::scalar_parameter<::mig::void_t> param1{0};
    ::mig::scalar_parameter<uint8_t> param2{1};
    ::mig::scalar_parameter<int16_t> param3{2};
    ::mig::scalar_parameter<uint32_t> param4{3, ::mig::OPTIONAL};
    ::mig::enum_parameter<TestEnum1> param5{12, ::mig::OPTIONAL};
    ::mig::scalar_parameter<bool> param6{13, ::mig::OPTIONAL};

  private:
    const std::map<int, ::mig::parameter&> m_params = {
      {0, param1},
      {1, param2},
      {2, param3},
      {3, param4},
      {12, param5},
      {13, param6},
    };
};


class TestMessage1003 : public ::mig::Message {

  public:
    TestMessage1003() : ::mig::Message(0x1003, m_params) {}
    ::mig::var_parameter<std::string> param1{2};
    ::mig::var_parameter<::mig::blob_t> param2{4};
    ::mig::group_parameter<TestGroup1> param3{6};
    ::mig::scalar_parameter<::mig::void_t> param4{3, ::mig::OPTIONAL};
    ::mig::scalar_parameter<uint8_t> param5{5};

  private:
    const std::map<int, ::mig::parameter&> m_params = {
      {2, param1},
      {4, param2},
      {6, param3},
      {3, param4},
      {5, param5},
    };
};


#endif // ifndef _MSG_TESTS_MSG_H_
