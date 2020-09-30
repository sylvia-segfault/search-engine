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
  #include "./CrawlFileTree.h"
}

#include "gtest/gtest.h"
extern "C" {
  #include "./DocTable.h"
  #include "./MemIndex.h"
}
#include "./test_suite.h"

namespace hw2 {

TEST(Test_CrawlFileTree, ReadsFromDisk) {
  bool res;
  DocTable *dt;
  MemIndex *idx;

  // Test that it detects a valid directory.
  res = CrawlFileTree(const_cast<char *>("./test_tree/bash-4.2/support"),
                      &dt,
                      &idx);
  ASSERT_EQ(true, res);
  HW2Environment::AddPoints(10);
  DocTable_Free(dt);
  MemIndex_Free(idx);
  HW2Environment::AddPoints(10);

  // Test that it detects a non-existant directory.
  res = CrawlFileTree(const_cast<char *>("./nonexistent/"), &dt, &idx);
  ASSERT_EQ(false, res);
  HW2Environment::AddPoints(10);

  // Test that it rejects files (instead of directories).
  res = CrawlFileTree(const_cast<char *>("./test_suite.c"), &dt, &idx);
  ASSERT_EQ(false, res);
  HW2Environment::AddPoints(10);
}

TEST(Test_CrawlFileTree, ChecksArgs) {
  DocTable *dt;
  MemIndex *idx;

  // Test that CrawlFileTree can be called with any combination of NULLs
  ASSERT_EQ(false, CrawlFileTree(NULL, &dt, &idx));
  ASSERT_EQ(false, CrawlFileTree(const_cast<char *>(""), &dt, &idx));
  ASSERT_EQ(false,
            CrawlFileTree(const_cast<char *>("./test_tree"), NULL, &idx));
  ASSERT_EQ(false,
            CrawlFileTree(const_cast<char *>("./test_tree"), &dt, NULL));
}

TEST(Test_CrawlFileTree, Integration) {
  int res;
  DocTable *dt;
  MemIndex *idx;

  LinkedList *llres;
  LLIterator *lit;
  int i;

  const char *q1[] = {"equations"};
  const char *q2[] = {"report", "normal"};
  const char *q3[] = {"report", "suggestions", "normal"};
  const char *q4[] = {"report", "normal", "foobarbaz"};

  // Crawl the test tree.
  res = CrawlFileTree(const_cast<char *>("./test_tree/bash-4.2/support"),
                      &dt,
                      &idx);
  ASSERT_EQ(1, res);
  ASSERT_EQ(3852, MemIndex_NumWords(idx));

  // Process query 1, check results.
  llres = MemIndex_Search(idx, const_cast<char **>(q1), 1);
  ASSERT_NE((LinkedList *) NULL, llres);
  ASSERT_EQ(LinkedList_NumElements(llres), 2);
  lit = LLIterator_Allocate(llres);
  for (i = 0; i < LinkedList_NumElements(llres); i++) {
    SearchResult *res;

    LLIterator_Get(lit, reinterpret_cast<LLPayload_t*>(&res));
    if (i == 0) {
      char *docname = DocTable_GetDocName(dt, res->docid);
      ASSERT_EQ(
         strcmp(docname,
                "./test_tree/bash-4.2/support/texi2html"), 0);
    } else if (i == 1) {
      char *docname = DocTable_GetDocName(dt, res->docid);
      ASSERT_EQ(
         strcmp(docname,
                "./test_tree/bash-4.2/support/man2html.c"), 0);
    }
    LLIterator_Next(lit);
  }
  LLIterator_Free(lit);
  LinkedList_Free(llres, reinterpret_cast<LLPayloadFreeFnPtr>(&free));
  HW2Environment::AddPoints(20);

  // Process query 2, check results.
  llres = MemIndex_Search(idx, const_cast<char **>(q2), 2);
  ASSERT_NE((LinkedList *) NULL, llres);
  ASSERT_EQ(LinkedList_NumElements(llres), 2);
  lit = LLIterator_Allocate(llres);
  for (i = 0; i < LinkedList_NumElements(llres); i++) {
    SearchResult *res;

    LLIterator_Get(lit, reinterpret_cast<LLPayload_t*>(&res));
    if (i == 0) {
      char *docname = DocTable_GetDocName(dt, res->docid);
      ASSERT_EQ(
         strcmp(docname,
                "./test_tree/bash-4.2/support/texi2html"), 0);
    } else if (i == 1) {
      char *docname = DocTable_GetDocName(dt, res->docid);
      ASSERT_EQ(
         strcmp(docname,
                "./test_tree/bash-4.2/support/man2html.c"), 0);
    }
    LLIterator_Next(lit);
  }
  LLIterator_Free(lit);
  LinkedList_Free(llres, reinterpret_cast<LLPayloadFreeFnPtr>(&free));
  HW2Environment::AddPoints(20);

  // Process query 3, check results.
  llres = MemIndex_Search(idx, const_cast<char **>(q3), 3);
  ASSERT_NE((LinkedList *) NULL, llres);
  ASSERT_EQ(LinkedList_NumElements(llres), 1);
  lit = LLIterator_Allocate(llres);
  for (i = 0; i < LinkedList_NumElements(llres); i++) {
    SearchResult *res;

    LLIterator_Get(lit, reinterpret_cast<LLPayload_t*>(&res));
    if (i == 0) {
      char *docname = DocTable_GetDocName(dt, res->docid);
      ASSERT_EQ(
         strcmp(docname,
                "./test_tree/bash-4.2/support/texi2html"), 0);
    }
    LLIterator_Next(lit);
  }
  LLIterator_Free(lit);
  LinkedList_Free(llres, reinterpret_cast<LLPayloadFreeFnPtr>(&free));
  HW2Environment::AddPoints(20);

  // Process query 4, check results.
  llres = MemIndex_Search(idx, const_cast<char **>(q4), 3);
  ASSERT_EQ((LinkedList *) NULL, llres);
  HW2Environment::AddPoints(20);

  // Free up everything.
  DocTable_Free(dt);
  MemIndex_Free(idx);
  HW2Environment::AddPoints(10);
}

}  // namespace hw2

