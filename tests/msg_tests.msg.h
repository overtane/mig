//  msg_tests.msg.h
//
//  This is an automatically generated file
//
//  --------------------
//  PLEASE, DO NOT EDIT!
//  --------------------
//
//  Source:  msg_tests.msg
//  Sun Jan 20 11:22:16 2019

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
    static ::mig::message_ptr_t create() { return std::make_unique<TestMessage1001>(); }

  private:
    const ::mig::parameter_container_t m_params = {
    };
};

struct TestGroup1 : ::mig::Group {

  public:
    TestGroup1() : ::mig::Group(m_params) {}

    ::mig::ScalarParameter<::mig::void_t> param1{0};
    ::mig::ScalarParameter<uint32_t> param2{9};

  private:
    const ::mig::parameter_container_t m_params = {
      {0, param1},
      {9, param2},
    };
};


class TestMessage1002 : public ::mig::Message {

  public:
    TestMessage1002() : ::mig::Message(0x1002, m_params) {}
    static ::mig::message_ptr_t create() { return std::make_unique<TestMessage1002>(); }

    ::mig::ScalarParameter<::mig::void_t> param1{0};
    ::mig::ScalarParameter<uint8_t> param2{1};
    ::mig::ScalarParameter<int16_t> param3{2};
    ::mig::ScalarParameter<uint32_t> param4{3, ::mig::OPTIONAL};
    ::mig::EnumParameter<TestEnum1> param5{12, ::mig::OPTIONAL};
    ::mig::ScalarParameter<bool> param6{13, ::mig::OPTIONAL};

  private:
    const ::mig::parameter_container_t m_params = {
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
    static ::mig::message_ptr_t create() { return std::make_unique<TestMessage1003>(); }

    ::mig::VarParameter<::mig::blob_t> param2{4};
    ::mig::VarParameter<std::string> param1{2};
    ::mig::GroupParameter<TestGroup1> param3{6};
    ::mig::ScalarParameter<::mig::void_t> param4{3, ::mig::OPTIONAL};
    ::mig::ScalarParameter<uint8_t> param5{5};

  private:
    const ::mig::parameter_container_t m_params = {
      {4, param2},
      {2, param1},
      {6, param3},
      {3, param4},
      {5, param5},
    };
};


const std::map<int, mig::MessageCreatorFunc> mig::Message::creators {
  { 0x1001, TestMessage1001::create },
  { 0x1002, TestMessage1002::create },
  { 0x1003, TestMessage1003::create },
  };

#endif // ifndef _MSG_TESTS_MSG_H_
