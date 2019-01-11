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

extern "C" {
#include "mig.c"
}

// 
// HASH TABLE TESTS
//

class HashTableTest : public ::testing::Test
{
  protected:
    void SetUp() override {
      ht1 = hash_table_new(name2hash, namecmp);
      ht2 = hash_table_new(id2hash, idcmp);
    }

    void TearDown() override {
      hash_table_delete(ht1);
      hash_table_delete(ht2);
    }
 

  struct hash_table *ht1;
  struct hash_table *ht2;

  union hash_key hk1;
  union hash_key hk2;
};


TEST_F(HashTableTest, IsEmptyAndInitialized) {
  EXPECT_EQ(hash_table_size(ht1), 0);
  EXPECT_EQ(hash_table_size(ht2), 0);

  EXPECT_EQ(ht1->nnodes, 0);
  EXPECT_EQ(ht2->nnodes, 0);

  EXPECT_EQ((long)ht1->hash, (long)name2hash);
  EXPECT_EQ((long)ht1->comp, (long)namecmp);
  EXPECT_EQ((long)ht2->hash, (long)id2hash);
  EXPECT_EQ((long)ht2->comp, (long)idcmp);

  int n1=0, n2=0;
  for (int i=0; i<HASH_TABLE_SIZE; i++) {
    if (ht1->table[i] != 0)
      n1++;
    if (ht2->table[i] != 0)
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

TEST_F(HashTableTest, IdTableTest)
{
  // add sequential keys and check result
  for (int i=0; i<3560; i++) {
    hk2.id = i;
    hash_table_add(ht2, &hk2, NULL);
    EXPECT_EQ(hash_table_size(ht2), i+1);
    EXPECT_EQ(ht2->nnodes, i+1);
    EXPECT_NE(ht2->table[i%HASH_TABLE_SIZE], nullptr);
  }
  for (int i=3559; i>-1; i--) {
    hk2.id = i;
    struct hash_node *node = hash_table_search(ht2, &hk2);
    EXPECT_NE(node, nullptr);
    if (node)
      EXPECT_EQ(node->key.id, i);
  }


  // add random keys and check results
  std::srand(std::time(nullptr));
  union hash_key hk[256];
  for (int i=0; i<256; i++) {
    int r = std::rand() % 24378;
    hk[i].id = r;
    hash_table_add(ht2, &hk[i], NULL);
  }
  for (int i=0; i<256; i++) {
    struct hash_node *node = hash_table_search(ht2, &hk[i]);
    EXPECT_EQ(ht2->nnodes, hash_table_size(ht2));
    EXPECT_NE(node, nullptr);
    if (node)
      EXPECT_EQ(node->key.id, hk[i].id);
  }
  

  // check that sequential keys are still accessible
  for (int i=3559; i>-1; i--) {
    hk2.id = i;
    struct hash_node *node = hash_table_search(ht2, &hk2);
    EXPECT_NE(node, nullptr);
    if (node)
      EXPECT_EQ(node->key.id, i);
  }


  // check non-existent keys
  {
    union hash_key hk;
    hk.id = 24380;
    struct hash_node *node = hash_table_search(ht2, &hk);
    EXPECT_EQ(node, nullptr);
    hk.id = 99999;
    node = hash_table_search(ht2, &hk);
    EXPECT_EQ(node, nullptr);
  }

}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
