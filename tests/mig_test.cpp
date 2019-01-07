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

extern "C" {
#include "mig.c"
}

class HashTableTest : public ::testing::Test
{
  protected:
    void SetUp() override {
      hash_table_init(ht1);
      hash_table_init(ht2);
    }

  hash_table ht1;
  hash_table ht2;

  hash_key hk1;
  hash_key hk2;
};


TEST_F(HashTableTest, IsEmptyAndInitialized) {
  EXPECT_EQ(hash_table_size(ht1), 0);
  EXPECT_EQ(hash_table_size(ht2), 0);

  int n1=0, n2=0;
  for (int i=0; i<HASH_TABLE_SIZE; i++) {
    if (ht1[i] != 0)
      n1++;
    if (ht2[i] != 0)
      n2++;
  }

  EXPECT_EQ(n1, 0);
  EXPECT_EQ(n2, 0);
}


TEST_F(HashTableTest, HashFromIdTest) {
  hk1.id = 0;
  EXPECT_EQ(id2hash(&hk1), 0);
  hk1.id = 5;
  EXPECT_EQ(id2hash(&hk1), 5);
  hk1.id = HASH_TABLE_SIZE-1;
  EXPECT_EQ(id2hash(&hk1), HASH_TABLE_SIZE-1);
  hk1.id = HASH_TABLE_SIZE;
  EXPECT_EQ(id2hash(&hk1), 0);
  hk1.id = HASH_TABLE_SIZE+5;
  EXPECT_EQ(id2hash(&hk1), 5);
  hk1.id = 10*HASH_TABLE_SIZE+5;
  EXPECT_EQ(id2hash(&hk1), 5);
}

TEST_F(HashTableTest, HashFromNameTest) {
  hk2.name = "";
  EXPECT_EQ(name2hash(&hk2), 0);
  hk2.name = "A";
  EXPECT_EQ(name2hash(&hk2), 'A' % HASH_TABLE_SIZE);
  hk2.name = "AB";
  EXPECT_EQ(name2hash(&hk2), ('A'+'B')%HASH_TABLE_SIZE);
}

TEST_F(HashTableTest, IdCmpTest) {
  hk1.id = 3;
  hk2.id = 3;
  EXPECT_EQ(idcmp(&hk1,&hk2), 0);
  hk1.id = 3;
  hk2.id = 5;
  EXPECT_LT(idcmp(&hk1,&hk2), 0);
  hk1.id = 5;
  hk2.id = 3;
  EXPECT_GT(idcmp(&hk1,&hk2), 0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
