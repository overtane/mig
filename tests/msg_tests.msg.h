//  msg_tests.msg.h
//  This is an automatically generated file
//  Please do not edit
//  Source:  msg_tests.msg
//  Tue Jan 15 22:13:45 2019

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
    std::map<int, ::mig::parameter&> m_params = {
    };
};


struct TestGroup1 : ::mig::GroupBase {

  public:
    TestGroup1() : ::mig::GroupBase(m_params) {}
    ::mig::scalar_parameter<::mig::void_t> Param1{0};

  private:
    std::map<int, ::mig::parameter&> m_params = {
      {0, Param1},
    };
};


class TestMessage1002 : public ::mig::Message {

  public:
    TestMessage1002() : ::mig::Message(0x1002, m_params) {}
    ::mig::scalar_parameter<::mig::void_t> param1{0};
    ::mig::scalar_parameter<uint8_t> param2{1};
    ::mig::scalar_parameter<int16_t> param3{2};
    ::mig::scalar_parameter<uint32_t> param4{3, ::mig::OPTIONAL};

  private:
    std::map<int, ::mig::parameter&> m_params = {
      {0, param1},
      {1, param2},
      {2, param3},
      {3, param4},
    };
};


class TestMessage1003 : public ::mig::Message {

  public:
    TestMessage1003() : ::mig::Message(0x1003, m_params) {}
    ::mig::scalar_parameter<::mig::void_t> Param1{0};
    ::mig::scalar_parameter<bool> Param2{1};
    ::mig::scalar_parameter<uint8_t> Param3{2};
    ::mig::composite_parameter<::mig::blob_t> Param4{3};

  private:
    std::map<int, ::mig::parameter&> m_params = {
      {0, Param1},
      {1, Param2},
      {2, Param3},
      {3, Param4},
    };
};


class TestMessage1004 : public ::mig::Message {

  public:
    TestMessage1004() : ::mig::Message(0x1388, m_params) {}
    ::mig::scalar_parameter<::mig::void_t> Param1{0, ::mig::OPTIONAL};
    ::mig::scalar_parameter<bool> Param2{1, ::mig::OPTIONAL};
    ::mig::scalar_parameter<uint8_t> Param3{2, ::mig::OPTIONAL};
    ::mig::composite_parameter<::mig::blob_t> Param4{3};

  private:
    std::map<int, ::mig::parameter&> m_params = {
      {0, Param1},
      {1, Param2},
      {2, Param3},
      {3, Param4},
    };
};


class TestMessage1005 : public ::mig::Message {

  public:
    TestMessage1005() : ::mig::Message(0x1389, m_params) {}
    ::mig::enum_parameter<TestEnum1> Param1{92};
    ::mig::group_parameter<TestGroup1> Param2{12, ::mig::OPTIONAL};

  private:
    std::map<int, ::mig::parameter&> m_params = {
      {92, Param1},
      {12, Param2},
    };
};


#endif // ifndef _MSG_TESTS_MSG_H_
