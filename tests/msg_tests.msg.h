//  msg_tests.msg.h
//  This is an automatically generated file
//  Please do not edit
//  Source:  msg_tests.msg
//  Fri Jan 11 11:09:37 2019

#ifndef _MSG_TESTS_MSG_H_
#define _MSG_TESTS_MSG_H_

#include "migmsg.h"

enum TestEnum1 : ::mig::enum_t {
  kTestEnum1VALUE1 = 0,
  kTestEnum1VALUE2 = 1,
  kTestEnum1Count = 2
};


struct TestGroup1 : ::mig::GroupBase {

  public:
    TestGroup1() : ::mig::GroupBase(allpars) {}
    ::mig::scalar_parameter<::mig::void_t> Param1{0};

  private:
    std::vector<::mig::parameter * const> allpars = {
      &Param1,
    };
};


class TestMessage1001 : public ::mig::Message {

  public:
    TestMessage1001() : ::mig::Message(0x1001, allpars) {}

  private:
    std::vector<::mig::parameter * const> allpars = {
    };
};


class TestMessage1002 : public ::mig::Message {

  public:
    TestMessage1002() : ::mig::Message(0x1002, allpars) {}
    ::mig::scalar_parameter<::mig::void_t> Param1{0};

  private:
    std::vector<::mig::parameter * const> allpars = {
      &Param1,
    };
};


class TestMessage1003 : public ::mig::Message {

  public:
    TestMessage1003() : ::mig::Message(0x1003, allpars) {}
    ::mig::scalar_parameter<::mig::void_t> Param1{0};
    ::mig::scalar_parameter<bool> Param2{1};
    ::mig::scalar_parameter<uint8_t> Param3{2};
    ::mig::composite_parameter<::mig::blob_t> Param4{3};

  private:
    std::vector<::mig::parameter * const> allpars = {
      &Param1,
      &Param2,
      &Param3,
      &Param4,
    };
};


class TestMessage1004 : public ::mig::Message {

  public:
    TestMessage1004() : ::mig::Message(0x1388, allpars) {}
    ::mig::scalar_parameter<::mig::void_t> Param1{0, ::mig::OPTIONAL};
    ::mig::scalar_parameter<bool> Param2{1, ::mig::OPTIONAL};
    ::mig::scalar_parameter<uint8_t> Param3{2, ::mig::OPTIONAL};
    ::mig::composite_parameter<::mig::blob_t> Param4{3};

  private:
    std::vector<::mig::parameter * const> allpars = {
      &Param1,
      &Param2,
      &Param3,
      &Param4,
    };
};


class TestMessage1005 : public ::mig::Message {

  public:
    TestMessage1005() : ::mig::Message(0x1389, allpars) {}
    ::mig::scalar_parameter<TestEnum1> Param1{92};
    ::mig::group_parameter<TestGroup1> Param2{12, ::mig::OPTIONAL};

  private:
    std::vector<::mig::parameter * const> allpars = {
      &Param1,
      &Param2,
    };
};


#endif // ifndef _MSG_TESTS_MSG_H_
