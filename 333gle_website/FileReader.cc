/*
 * Copyright ©2020 Hal Perkins.  All rights reserved.  Permission is
 * hereby granted to the students registered for University of Washington
 * CSE 333 for use solely during Spring Quarter 2020 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <stdio.h>
#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <memory>

extern "C" {
  #include "libhw2/FileParser.h"
}

#include "./HttpUtils.h"
#include "./FileReader.h"

using std::string;

namespace hw4 {

bool FileReader::ReadFile(string *str) {
  string fullfile = basedir_ + "/" + fname_;

  // Read the file into memory, and store the file contents in the
  // output parameter "str."  Be careful to handle binary data
  // correctly; i.e., you probably want to use the two-argument
  // constructor to std::string (the one that includes a length as a
  // second argument).
  //
  // You might find ::ReadFileToString() from HW2 useful
  // here.  Be careful, though; remember that it uses malloc to
  // allocate memory, so you'll need to use free() to free up that
  // memory.  Alternatively, you can use a unique_ptr with a malloc/free
  // deleter to automatically manage this for you; see the comment in
  // HttpUtils.h above the MallocDeleter class for details.

  // STEP 1:
  // Before we read the file, check if its path is safe.
  if (!IsPathSafe(basedir_, fullfile))
    return false;

  // Get the file content by storing them into a string.
  // If we failed to read the file, return false.
  int file_size;
  char *f_content = ReadFileToString(fullfile.c_str(), &file_size);
  if (f_content == NULL)
    return false;

  // Wrap the file content pointer by a unique pointer
  // so that it frees the mallocated space that was
  // used to store the content of the file automatically.
  std::unique_ptr<char, MallocDeleter<char>> smart_content(f_content);
  // Save the result into a return parameter.
  *str = std::string(smart_content.get(), file_size);

  return true;
}

}  // namespace hw4
