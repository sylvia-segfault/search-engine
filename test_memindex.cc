/*
 * Copyright ©2020 Hal Perkins.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Spring Quarter 2020 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

extern "C" {
  #include "./MemIndex.h"
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gtest/gtest.h"
extern "C" {
  #include "libhw1/LinkedList.h"
  #include "./CrawlFileTree.h"
  #include "./DocTable.h"
}
#include "./test_suite.h"

char *MakeCopy(const char* s) {
  int len = strlen(s);
  char *copy = (char*)malloc(len + 1);
  strncpy(copy, s, len);
  copy[len] = '\0';
  return copy;
}

namespace hw2 {

TEST(Test_MemIndex, Simple) {
  constexpr DocID_t kDocID1 = 1234;
  constexpr DocID_t kDocID2 = 5678;

  // We need to assign these logically-constant strings into a non-const
  // pointers because the compiler won't let me cast away the const
  // qualifier on a string literal.
  const char *kBananas = "bananas";
  const char *kPears = "pears";
  const char *kApples = "apples";
  const char *kGrapes = "grapes";

  LinkedList *ll1 = LinkedList_Allocate();
  LinkedList_Push(ll1, (LLPayload_t)100);
  LinkedList_Push(ll1, (LLPayload_t)200);

  LinkedList *ll2 = LinkedList_Allocate();
  LinkedList_Push(ll2, (LLPayload_t)300);

  LinkedList *ll3 = LinkedList_Allocate();
  LinkedList_Push(ll3, (LLPayload_t)400);
  LinkedList_Push(ll3, (LLPayload_t)500);
  LinkedList_Push(ll3, (LLPayload_t)600);

  LinkedList *ll4 = LinkedList_Allocate();
  LinkedList_Push(ll4, (LLPayload_t)700);

  LinkedList *ll5 = LinkedList_Allocate();
  LinkedList_Push(ll5, (LLPayload_t)800);

  MemIndex *idx = MemIndex_Allocate();

  // Document 1 has bananas, pears, and apples.
  MemIndex_AddPostingList(idx, MakeCopy(kBananas), kDocID1, ll1);
  MemIndex_AddPostingList(idx, MakeCopy(kPears), kDocID1, ll2);
  MemIndex_AddPostingList(idx, MakeCopy(kApples), kDocID1, ll3);

  // Document 2 only has apples and bananas.
  MemIndex_AddPostingList(idx, MakeCopy(kApples), kDocID2, ll4);
  MemIndex_AddPostingList(idx, MakeCopy(kBananas), kDocID2, ll5);

  ASSERT_EQ(3, MemIndex_NumWords(idx));

  // No results.
  char *query1[] = {const_cast<char *>(kGrapes)};
  LinkedList *results = MemIndex_Search(idx, query1, 1);
  ASSERT_EQ(0, (LinkedList *)results);

  HW2Environment::AddPoints(5);

  // One resultant document.
  char *query2[] = {const_cast<char *>(kPears)};
  results = MemIndex_Search(idx, query2, 1);
  ASSERT_EQ(1, LinkedList_NumElements(results));
  LLIterator *itr = LLIterator_Allocate(results);
  SearchResult *res;
  LLIterator_Get(itr, reinterpret_cast<LLPayload_t*>(&res));
  ASSERT_EQ(kDocID1, res->docid);
  LLIterator_Free(itr);
  LinkedList_Free(results, (LLPayloadFreeFnPtr)free);

  HW2Environment::AddPoints(5);

  // Multiple resultant documents.
  char *query3[] = {const_cast<char *>(kApples)};
  results = MemIndex_Search(idx, query3, 1);
  ASSERT_EQ(2, LinkedList_NumElements(results));
  itr = LLIterator_Allocate(results);
  LLIterator_Get(itr, reinterpret_cast<LLPayload_t*>(&res));
  ASSERT_EQ(kDocID1, res->docid);
  LLIterator_Next(itr);
  LLIterator_Get(itr, reinterpret_cast<LLPayload_t*>(&res));
  ASSERT_EQ(kDocID2, res->docid);
  LLIterator_Free(itr);
  LinkedList_Free(results, (LLPayloadFreeFnPtr)free);

  HW2Environment::AddPoints(10);

  // Multiple search terms.
  char *query4[] = {const_cast<char *>(kApples), const_cast<char *>(kBananas)};
  results = MemIndex_Search(idx, query4, 2);
  ASSERT_EQ(2, LinkedList_NumElements(results));
  itr = LLIterator_Allocate(results);
  LLIterator_Get(itr, reinterpret_cast<LLPayload_t*>(&res));
  ASSERT_EQ(kDocID1, res->docid);
  LLIterator_Next(itr);
  LLIterator_Get(itr, reinterpret_cast<LLPayload_t*>(&res));
  ASSERT_EQ(kDocID2, res->docid);
  LLIterator_Free(itr);
  LinkedList_Free(results, (LLPayloadFreeFnPtr)free);

  // Multiple search terms: testing different term order.
  char *query5[] = {const_cast<char *>(kBananas), const_cast<char *>(kApples)};
  results = MemIndex_Search(idx, query5, 2);
  ASSERT_EQ(2, LinkedList_NumElements(results));
  itr = LLIterator_Allocate(results);
  LLIterator_Get(itr, reinterpret_cast<LLPayload_t*>(&res));
  ASSERT_EQ(kDocID1, res->docid);
  LLIterator_Next(itr);
  LLIterator_Get(itr, reinterpret_cast<LLPayload_t*>(&res));
  ASSERT_EQ(kDocID2, res->docid);
  LLIterator_Free(itr);
  LinkedList_Free(results, (LLPayloadFreeFnPtr)free);

  // Multiple search terms: not all documents should be results.
  char *query6[] = {const_cast<char *>(kPears), const_cast<char *>(kBananas)};
  results = MemIndex_Search(idx, query6, 2);
  ASSERT_EQ(1, LinkedList_NumElements(results));
  itr = LLIterator_Allocate(results);
  LLIterator_Get(itr, reinterpret_cast<LLPayload_t*>(&res));
  ASSERT_EQ(kDocID1, res->docid);
  LLIterator_Free(itr);
  LinkedList_Free(results, (LLPayloadFreeFnPtr)free);

  HW2Environment::AddPoints(10);

  MemIndex_Free(idx);
}

}  // namespace hw2
