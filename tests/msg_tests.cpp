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

#include "gtest/gtest.h"
#include <cstdlib>

// Generated message definitions
#include "msg_tests.msg.h"

// 
// Generated code tests
//
class MessageTests : public ::testing::Test
{
  public:
    TestMessage1001 m1;
    TestMessage1002 m2;
};

TEST_F(MessageTests, Enum1) 
{
  EXPECT_EQ(static_cast<int>(TestEnum1::VALUE1), 0);
  EXPECT_EQ(static_cast<int>(TestEnum1::VALUE2), 1);
  EXPECT_EQ(static_cast<int>(TestEnum1::Count), 2);
}

TEST_F(MessageTests, Message1001) 
{
  EXPECT_EQ(m1.nparams(), 0);
  EXPECT_EQ(m1.size(), 0);
  EXPECT_EQ(m1.is_valid(), true);
  EXPECT_EQ(m1.is_set(), true);
  EXPECT_EQ(m1.id(), 0x1001);
}

TEST_F(MessageTests, Message1002) 
{
  // message's initial state
  EXPECT_EQ(m2.nparams(), 4);
  EXPECT_EQ(m2.size(), 0); // size is zero when all unset
  EXPECT_EQ(m2.is_valid(), false);
  EXPECT_EQ(m2.is_set(), false);
  EXPECT_EQ(m2.id(), 0x1002);

  // parameters' initial states
  EXPECT_EQ(m2.param1.is_optional(), false);
  EXPECT_EQ(m2.param1.id(), 0);
  EXPECT_EQ(m2.param1.is_set(), false);
  EXPECT_EQ(m2.param1.size(), 0);
  EXPECT_EQ(m2.param1.is_valid(), false);
  EXPECT_EQ(m2.param2.is_optional(), false);
  EXPECT_EQ(m2.param2.id(), 1);
  EXPECT_EQ(m2.param2.is_set(), false);
  EXPECT_EQ(m2.param2.size(), 0);
  EXPECT_EQ(m2.param2.is_valid(), false);
  EXPECT_EQ(m2.param3.is_optional(), false);
  EXPECT_EQ(m2.param3.id(), 2);
  EXPECT_EQ(m2.param3.is_set(), false);
  EXPECT_EQ(m2.param3.size(), 0);
  EXPECT_EQ(m2.param3.is_valid(), false);
  EXPECT_EQ(m2.param4.is_optional(), true);
  EXPECT_EQ(m2.param4.id(), 3);
  EXPECT_EQ(m2.param4.is_set(), false);
  EXPECT_EQ(m2.param4.size(), 0);
  EXPECT_EQ(m2.param4.is_valid(), true);

  // set void parameter
  m2.param1.set();
  EXPECT_EQ(m2.param1.is_set(), true);
  EXPECT_EQ(m2.param1.is_valid(), true);
  EXPECT_EQ(m2.param1.size(), 0);
  EXPECT_EQ(m2.size(), 0);
  EXPECT_EQ(m2.is_set(), false);
  EXPECT_EQ(m2.is_valid(), false);

  // set uint8 parameter
  m2.param2 = 42;
  EXPECT_EQ(m2.param2.is_set(), true);
  EXPECT_EQ(m2.param2.is_valid(), true);
  EXPECT_EQ(m2.param2.size(), 1);
  EXPECT_EQ(m2.param2.get(), 42);
  EXPECT_EQ(m2.size(), 1);
  EXPECT_EQ(m2.is_set(), false);
  EXPECT_EQ(m2.is_valid(), false);

  // set int16 parameter
  m2.param3.set(-12345);
  EXPECT_EQ(m2.param3.is_set(), true);
  EXPECT_EQ(m2.param3.is_valid(), true);
  EXPECT_EQ(m2.param3.size(), 2);
  EXPECT_EQ(m2.param3.get(), -12345);
  EXPECT_EQ(m2.size(), 3);
  EXPECT_EQ(m2.is_set(), true);
  EXPECT_EQ(m2.is_valid(), true); // message is valid when all required parameters are set

  // set uint32 parameter
  m2.param4 = 7654321;
  EXPECT_EQ(m2.param4.is_set(), true);
  EXPECT_EQ(m2.param4.is_valid(), true);
  EXPECT_EQ(m2.param4.size(), 4);
  EXPECT_EQ(m2.param4.get(), 7654321);
  EXPECT_EQ(m2.size(), 7);
  EXPECT_EQ(m2.is_set(), true);
  EXPECT_EQ(m2.is_valid(), true);

}



