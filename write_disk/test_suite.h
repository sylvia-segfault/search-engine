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

#ifndef HW3_TEST_SUITE_H_
#define HW3_TEST_SUITE_H_

#include "gtest/gtest.h"

class HW3Environment : public ::testing::Environment {
 public:
  static void AddPoints(int points);

  // These are run once for the entire test environment:
  virtual void SetUp();
  virtual void TearDown();

 private:
  static constexpr int HW3_MAXPOINTS = 290;
  static int points_;
};


#endif  // HW3_TEST_SUITE_H_
