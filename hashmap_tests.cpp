#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <random>

#include "hashmap.h"

using namespace std;
using namespace testing;

namespace {

TEST(HashMapCore, DefaultConstructorStartsEmpty) {
  HashMap<int,int> hm;
  EXPECT_TRUE(hm.empty());
  EXPECT_EQ(hm.size(), 0u);
}

TEST(HashMapCore, SizeTracksDistinctAndDuplicateInserts) {
  HashMap<int,int> hm;
  hm.insert(1, 10);
  hm.insert(2, 20);
  hm.insert(3, 30);

  EXPECT_FALSE(hm.empty());
  EXPECT_EQ(hm.size(), 3u);

  hm.insert(1, 99);
  EXPECT_EQ(hm.size(), 3u);
  EXPECT_EQ(hm.at(1), 10);
}

TEST(HashMapCore, ContainsReflectsWhetherKeyIsPresent) {
  HashMap<int,int> hm;
  hm.insert(5, 50);
  hm.insert(7, 70);

  EXPECT_TRUE(hm.contains(5));
  EXPECT_TRUE(hm.contains(7));
  EXPECT_FALSE(hm.contains(8));
}

TEST(HashMapCore, AtReturnsValueAndThrowsForMissingKey) {
  HashMap<int,int> hm;
  hm.insert(1, 10);
  hm.insert(2, 20);

  EXPECT_EQ(hm.at(1), 10);
  EXPECT_EQ(hm.at(2), 20);
  EXPECT_THROW(hm.at(3), out_of_range);
}

TEST(HashMapCore, CollisionChainsKeepAllValues) {
  HashMap<int,int> hm;

  hm.insert(1, 100);
  hm.insert(11, 200);
  hm.insert(21, 300);

  EXPECT_TRUE(hm.contains(1));
  EXPECT_TRUE(hm.contains(11));
  EXPECT_TRUE(hm.contains(21));

  EXPECT_EQ(hm.at(1), 100);
  EXPECT_EQ(hm.at(11), 200);
  EXPECT_EQ(hm.at(21), 300);
}

TEST(HashMapCore, ClearEmptiesMapAndResetsSize) {
  HashMap<int,int> hm;
  hm.insert(3, 30);
  hm.insert(13, 130);
  hm.insert(23, 230);

  EXPECT_FALSE(hm.empty());
  EXPECT_EQ(hm.size(), 3u);

  hm.clear();

  EXPECT_TRUE(hm.empty());
  EXPECT_EQ(hm.size(), 0u);
  EXPECT_FALSE(hm.contains(3));
  EXPECT_FALSE(hm.contains(13));
  EXPECT_FALSE(hm.contains(23));
  EXPECT_THROW(hm.at(3), out_of_range);
}

TEST(HashMapCore, EraseRemovesKeyAndReturnsValue) {
  HashMap<int,int> hm;

  hm.insert(1, 10);
  hm.insert(11, 110);
  hm.insert(21, 210);

  EXPECT_EQ(hm.size(), 3u);

  int removed = hm.erase(11);
  EXPECT_EQ(removed, 110);
  EXPECT_EQ(hm.size(), 2u);

  EXPECT_FALSE(hm.contains(11));
  EXPECT_THROW(hm.at(11), out_of_range);

  EXPECT_TRUE(hm.contains(1));
  EXPECT_TRUE(hm.contains(21));
  EXPECT_EQ(hm.at(1), 10);
  EXPECT_EQ(hm.at(21), 210);
}

TEST(HashMapCore, EraseThrowsOnMissingKey) {
  HashMap<int,int> hm;
  hm.insert(4, 40);

  EXPECT_THROW(hm.erase(5), out_of_range);
  EXPECT_TRUE(hm.contains(4));
  EXPECT_EQ(hm.size(), 1u);
}

TEST(HashMapCore, CopyConstructorCopiesEmptyAndNonEmptyMaps) {
  HashMap<int,int> empty;
  HashMap<int,int> copy_empty(empty);
  EXPECT_TRUE(copy_empty.empty());
  EXPECT_EQ(copy_empty.size(), 0u);

  HashMap<int,int> hm;
  hm.insert(1, 10);
  hm.insert(11, 110);
  hm.insert(2, 20);

  HashMap<int,int> copy(hm);

  EXPECT_EQ(copy.size(), hm.size());
  EXPECT_TRUE(copy.contains(1));
  EXPECT_TRUE(copy.contains(11));
  EXPECT_TRUE(copy.contains(2));
  EXPECT_EQ(copy.at(1), 10);
  EXPECT_EQ(copy.at(11), 110);
  EXPECT_EQ(copy.at(2), 20);

  copy.insert(3, 30);
  EXPECT_TRUE(copy.contains(3));
  EXPECT_FALSE(hm.contains(3));
}

TEST(HashMapCore, AssignmentOperatorCopiesAndHandlesEmptyAndSelf) {
  HashMap<int,int> source;
  source.insert(1, 10);
  source.insert(11, 110);
  source.insert(2, 20);

  HashMap<int,int> target;
  target.insert(100, 1000);

  target = source;

  EXPECT_EQ(target.size(), source.size());
  EXPECT_TRUE(target.contains(1));
  EXPECT_TRUE(target.contains(11));
  EXPECT_TRUE(target.contains(2));
  EXPECT_EQ(target.at(1), 10);
  EXPECT_EQ(target.at(11), 110);
  EXPECT_EQ(target.at(2), 20);

  source.clear();
  EXPECT_TRUE(source.empty());
  EXPECT_FALSE(target.empty());
  EXPECT_EQ(target.size(), 3u);

  HashMap<int,int> empty;
  target = empty;
  EXPECT_TRUE(target.empty());
  EXPECT_EQ(target.size(), 0u);

  empty.insert(5, 50);
  HashMap<int,int> *alias = &empty;
  *alias = empty;
  EXPECT_TRUE(empty.contains(5));
  EXPECT_EQ(empty.size(), 1u);
}

TEST(HashMapAugmented, OperatorEqualsTrueForSameMappings) {
  HashMap<string,int> a;
  HashMap<string,int> b;

  a.insert("x", 1);
  a.insert("y", 2);
  a.insert("z", 3);

  b.insert("z", 3);
  b.insert("x", 1);
  b.insert("y", 2);

  EXPECT_TRUE(a == b);
  EXPECT_TRUE(b == a);
}

TEST(HashMapAugmented, OperatorEqualsDetectsDifferentValuesForSameKeys) {
  HashMap<int,int> a;
  HashMap<int,int> b;

  a.insert(1, 10);
  a.insert(2, 20);

  b.insert(1, 10);
  b.insert(2, 999);

  EXPECT_FALSE(a == b);
  EXPECT_FALSE(b == a);
}

TEST(HashMapAugmented, OperatorEqualsDetectsDifferentSizes) {
  HashMap<int,int> smaller;
  HashMap<int,int> larger;

  smaller.insert(1, 10);
  smaller.insert(2, 20);

  larger.insert(1, 10);
  larger.insert(2, 20);
  larger.insert(3, 30);

  EXPECT_FALSE(smaller == larger);
  EXPECT_FALSE(larger == smaller);
}

TEST(HashMapAugmented, OperatorEqualsIgnoresDifferentCapacities) {
  HashMap<int,int> default_cap;
  HashMap<int,int> small_cap(5);

  default_cap.insert(1, 10);
  default_cap.insert(2, 20);

  small_cap.insert(2, 20);
  small_cap.insert(1, 10);

  EXPECT_TRUE(default_cap == small_cap);
  EXPECT_TRUE(small_cap == default_cap);
}

TEST(HashMapAugmented, BeginNextOnEmptyMapReturnsFalse) {
  HashMap<int,int> hm;

  hm.begin();
  int key = -1;
  int value = -1;

  bool has_next = hm.next(key, value);
  EXPECT_FALSE(has_next);
}

TEST(HashMapAugmented, NextVisitsAllElementsWithCollisionsAndStops) {
  HashMap<int,int> hm;

  hm.insert(1, 100);
  hm.insert(11, 110);
  hm.insert(21, 120);

  hm.insert(2, 200);

  hm.begin();

  vector<pair<int,int> > seen;
  int key = 0;
  int value = 0;

  while (hm.next(key, value)) {
    seen.push_back(make_pair(key, value));
  }
  EXPECT_FALSE(hm.next(key, value));

  EXPECT_EQ(seen.size(), hm.size());

  vector<pair<int,int> > expected;
  expected.push_back(make_pair(1, 100));
  expected.push_back(make_pair(11, 110));
  expected.push_back(make_pair(21, 120));
  expected.push_back(make_pair(2, 200));

  EXPECT_THAT(seen, UnorderedElementsAreArray(expected));
}

}