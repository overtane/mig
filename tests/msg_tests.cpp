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
    TestMessage1003 m3;
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
  EXPECT_EQ(m1.data_size(), 0);
  EXPECT_EQ(m1.is_valid(), true);
  EXPECT_EQ(m1.is_set(), true);
  EXPECT_EQ(m1.id(), 0x1001);
}

TEST_F(MessageTests, Message1002) 
{
  // Basic API method for basic scalar parameters

  // message's initial state
  EXPECT_EQ(m2.nparams(), 6);
  EXPECT_EQ(m2.data_size(), 9);
  EXPECT_EQ(m2.is_valid(), false);
  EXPECT_EQ(m2.is_set(), false);
  EXPECT_EQ(m2.id(), 0x1002);

  // parameters' initial states
  EXPECT_EQ(m2.param1.is_optional(), false);
  EXPECT_EQ(m2.param1.id(), 0);
  EXPECT_EQ(m2.param1.is_set(), false);
  EXPECT_EQ(m2.param1.data_size(), 0);
  EXPECT_EQ(m2.param1.item_size(), 0);
  EXPECT_EQ(m2.param1.is_valid(), false);
  EXPECT_EQ(m2.param1.is_scalar(), true);
  EXPECT_EQ(m2.param2.is_optional(), false);
  EXPECT_EQ(m2.param2.id(), 1);
  EXPECT_EQ(m2.param2.is_set(), false);
  EXPECT_EQ(m2.param2.item_size(), 1);
  EXPECT_EQ(m2.param2.data_size(), 1);
  EXPECT_EQ(m2.param2.is_valid(), false);
  EXPECT_EQ(m2.param2.is_scalar(), true);
  EXPECT_EQ(m2.param3.is_optional(), false);
  EXPECT_EQ(m2.param3.id(), 2);
  EXPECT_EQ(m2.param3.is_set(), false);
  EXPECT_EQ(m2.param3.data_size(), 2);
  EXPECT_EQ(m2.param3.item_size(), 2);
  EXPECT_EQ(m2.param3.is_valid(), false);
  EXPECT_EQ(m2.param3.is_scalar(), true);
  EXPECT_EQ(m2.param4.is_optional(), true);
  EXPECT_EQ(m2.param4.id(), 3);
  EXPECT_EQ(m2.param4.is_set(), false);
  EXPECT_EQ(m2.param4.data_size(), 4);
  EXPECT_EQ(m2.param4.item_size(), 4);
  EXPECT_EQ(m2.param4.is_valid(), true);
  EXPECT_EQ(m2.param4.is_scalar(), true);
  EXPECT_EQ(m2.param5.is_optional(), true);
  EXPECT_EQ(m2.param5.id(), 12);
  EXPECT_EQ(m2.param5.is_set(), false);
  EXPECT_EQ(m2.param5.item_size(), 1);
  EXPECT_EQ(m2.param5.data_size(), 1);
  EXPECT_EQ(m2.param5.is_valid(), true);
  EXPECT_EQ(m2.param5.is_scalar(), true);
  EXPECT_EQ(m2.param6.is_optional(), true);
  EXPECT_EQ(m2.param6.id(), 13);
  EXPECT_EQ(m2.param6.is_set(), false);
  EXPECT_EQ(m2.param6.item_size(), 1);
  EXPECT_EQ(m2.param6.data_size(), 1);
  EXPECT_EQ(m2.param6.is_valid(), true);
  EXPECT_EQ(m2.param6.is_scalar(), true);

  // set void parameter
  m2.param1.set();
  EXPECT_EQ(m2.param1.is_set(), true);
  EXPECT_EQ(m2.param1.is_valid(), true);
  EXPECT_EQ(m2.param1.item_size(), 0);
  EXPECT_EQ(m2.param1.data_size(), 0);
  EXPECT_EQ(m2.data_size(), 9);
  EXPECT_EQ(m2.is_set(), false);
  EXPECT_EQ(m2.is_valid(), false);

  // set uint8 parameter
  m2.param2 = 42;
  EXPECT_EQ(m2.param2.is_set(), true);
  EXPECT_EQ(m2.param2.is_valid(), true);
  EXPECT_EQ(m2.param2.item_size(), 1);
  EXPECT_EQ(m2.param2.data_size(), 1);
  EXPECT_EQ(m2.param2.data(), 42);
  EXPECT_EQ((m2.param2==42), true);
  EXPECT_EQ(m2.data_size(), 9);
  EXPECT_EQ(m2.is_set(), false);
  EXPECT_EQ(m2.is_valid(), false);

  // set int16 parameter
  m2.param3.assign(-12345);
  EXPECT_EQ(m2.param3.is_set(), true);
  EXPECT_EQ(m2.param3.is_valid(), true);
  EXPECT_EQ(m2.param3.item_size(), 2);
  EXPECT_EQ(m2.param3.data_size(), 2);
  EXPECT_EQ(m2.param3.data(), -12345);
  EXPECT_EQ(m2.data_size(), 9);
  m2.param3 = 12; // reassign value
  EXPECT_EQ(m2.param3.data(), 12);

  EXPECT_EQ(m2.is_set(), true);
  EXPECT_EQ(m2.is_valid(), true); // message is valid when all required parameters are set

  // set uint32 parameter
  m2.param4 = 7654321;
  EXPECT_EQ(m2.param4.is_set(), true);
  EXPECT_EQ(m2.param4.is_valid(), true);
  EXPECT_EQ(m2.param4.item_size(), 4);
  EXPECT_EQ(m2.param4.data_size(), 4);
  EXPECT_EQ((m2.param4==7654321), true);
  EXPECT_EQ((m2.param4!=7654321), false);
  EXPECT_EQ((m2.param4>=7654321), true);
  EXPECT_EQ((m2.param4<=7654321), true);
  EXPECT_EQ((m2.param4>7654321), false);
  EXPECT_EQ((m2.param4<7654321), false);
  EXPECT_EQ(m2.data_size(), 9);
  EXPECT_EQ(m2.is_set(), true);
  EXPECT_EQ(m2.is_valid(), true);

  // set enum2 parameter
  m2.param5 = TestEnum1::VALUE2;
  EXPECT_EQ(m2.param5.is_set(), true);
  EXPECT_EQ(m2.param5.is_valid(), true);
  EXPECT_EQ(m2.param5.item_size(), 1);
  EXPECT_EQ(m2.param5.data_size(), 1);
  EXPECT_EQ(m2.param5.data(), TestEnum1::VALUE2);
  EXPECT_EQ((m2.param5==TestEnum1::VALUE2), true);
  EXPECT_EQ((m2.param5!=TestEnum1::VALUE2), false);
  EXPECT_EQ(m2.data_size(), 9);
  EXPECT_EQ(m2.is_set(), true);
  EXPECT_EQ(m2.is_valid(), true);
  m2.param5 = TestEnum1::VALUE1;
  EXPECT_EQ(m2.param5.data(), TestEnum1::VALUE1);

  // set bool parameter
  m2.param6 = false;
  EXPECT_EQ(m2.param6.is_set(), true);
  EXPECT_EQ(m2.param6.is_valid(), true);
  EXPECT_EQ(m2.param6.item_size(), 1);
  EXPECT_EQ(m2.param6.data_size(), 1);
  EXPECT_EQ(m2.param6.data(), false);
  EXPECT_EQ(m2.data_size(), 9);
  EXPECT_EQ(m2.is_set(), true);
  EXPECT_EQ(m2.is_valid(), true);
  m2.param6 = true;
  EXPECT_EQ(m2.param6.data(), true);

  
  
}


TEST_F(MessageTests, Message1003_blob) 
{
  // blob data tests

  EXPECT_EQ(m3.param2.is_set(), false);
  EXPECT_EQ(m3.param2.item_size(), 0);
  EXPECT_EQ(m3.param2.data_size(), 0);

  uint8_t a[] = { 1, 2, 3 };
  ::mig::blob_t b(a, 3);
  m3.param2.assign(b);
  EXPECT_EQ(m3.param2.is_set(), true);
  EXPECT_EQ(m3.param2.item_size(), 3);
  EXPECT_EQ(m3.param2.data_size(), 3);
  EXPECT_EQ(m3.param2.data().equals(b), true);
  uint8_t a2[] = { 1, 2, 3 };
  ::mig::blob_t b2(a2, 3);
  EXPECT_EQ(m3.param2.data().equals(b2), true);
  a2[2] = 4;
  EXPECT_EQ(m3.param2.data().equals(b2), false);
}
