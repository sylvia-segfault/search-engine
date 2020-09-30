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

#include <cstdio>     // for getline().
#include <cstdlib>    // for EXIT_SUCCESS, EXIT_FAILURE, free().
#include <cstring>    // for strlen().
#include <iostream>   // for std::cout, std::cerr, etc.
#include <sstream>    // for std::stringstream.
#include <vector>     // for std::vector.
#include <list>       // for std::list.
#include <algorithm>  // for std::transform and ::tolower.

#include "./QueryProcessor.h"

using namespace std;  // NOLINT(build/namespaces)
using namespace hw3;  // NOLINT(build/namespaces)

static void Usage(char *progname) {
  std::cerr << "Usage: " << progname << " [index files+]" << std::endl;
  exit(EXIT_FAILURE);
}

// Your job is to implement the entire filesearchshell.cc
// functionality. We're essentially giving you a blank screen to work
// with; you need to figure out an appropriate design, to decompose
// the problem into multiple functions or classes if that will help,
// to pick good interfaces to those functions/classes, and to make
// sure that you don't leak any memory.
//
// Here are the requirements for a working solution:
//
// The user must be able to run the program using a command like:
//
//   ./filesearchshell ./foo.idx ./bar/baz.idx /tmp/blah.idx [etc]
//
// i.e., to pass a set of filenames of indices as command line
// arguments. Then, your program needs to implement a loop where
// each loop iteration it:
//
//  (a) prints to the console a prompt telling the user to input the
//      next query.
//
//  (b) reads a white-space separated list of query words from
//      std::cin, converts them to lowercase, and constructs
//      a vector of c++ strings out of them.
//
//  (c) uses QueryProcessor.cc/.h's QueryProcessor class to
//      process the query against the indices and get back a set of
//      query results.  Note that you should instantiate a single
//      QueryProcessor  object for the lifetime of the program, rather
//      than  instantiating a new one for every query.
//
//  (d) print the query results to std::cout in the format shown in
//      the transcript on the hw3 web page.
//
// Also, you're required to quit out of the loop when std::cin
// experiences EOF, which a user passes by pressing "control-D"
// on the console.  As well, users should be able to type in an
// arbitrarily long query -- you shouldn't assume anything about
// a maximum line length.  Finally, when you break out of the
// loop and quit the program, you need to make sure you deallocate
// all dynamically allocated memory.  We will be running valgrind
// on your filesearchshell implementation to verify there are no
// leaks or errors.
//
// You might find the following technique useful, but you aren't
// required to use it if you have a different way of getting the
// job done.  To split a std::string into a vector of words, you
// can use a std::stringstream to get the job done and the ">>"
// operator. See, for example, "gnomed"'s post on stackoverflow for
// his example on how to do this:
//
//   http://stackoverflow.com/questions/236129/c-how-to-split-a-string
//
// (Search for "gnomed" on that page.  He uses an istringstream, but
// a stringstream gets the job done too.)
//
// Good luck, and write beautiful code!

int main(int argc, char **argv) {
  if (argc < 2) {
    Usage(argv[0]);
  }

  // STEP 1:
  // Implement filesearchshell!

  // Save the index files input by the customer
  // into a list so that we can process them.
  list<string> indexlist;
  for (int i = 1; i < argc; i++) {
    indexlist.push_back(argv[i]);
  }

  // Process the list of index files
  hw3::QueryProcessor processor(indexlist, true);

  // Keep prompting the user to input query words, process them
  // and print out the results of documents in which the query words
  // appeared and each document's rank. Keeps prompting the user to enter
  // their input until they want to stop which means that we received
  // CTRL-D from the input stream.
  while (1) {
    // store the query results
    vector<hw3::QueryProcessor::QueryResult> searchresult;
    vector<string> query;  // store query words input by the customer
    string input;  // take the customer's input query words

    cout << "Enter query:" << endl;
    getline(cin, input);  // get the line of the customer's input
    // stop everything if the customer types CTRL-D to signify exit
    if (cin.eof()) {
      break;
    }

    // Convert the customer's input query words to lower case
    transform(input.begin(), input.end(), input.begin(), ::tolower);

    // Grab each query word input by the customer and store
    // them into a vector
    stringstream ss(input);
    string word;
    while (ss >> word) {
      query.push_back(word);
    }

    // Process a vector of query words input by the customer
    searchresult = processor.ProcessQuery(query);

    // Print out the results of each matching document and its
    // corresponding rank if any is found.
    if (searchresult.size() != 0) {
      for (hw3::QueryProcessor::QueryResult qr : searchresult) {
        cout << "  " << qr.documentName << " (" << qr.rank << ")" << endl;
      }
    } else {  // show no results if we can't find any matching document
      cout << "  [no results]" << endl;
    }
  }
  return EXIT_SUCCESS;
}
