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

// This program is a search shell which prompts the user to enter
// their queries until they hit CTRL-D to signify exit. It prints
// out the results of documents in which the queried words appeared
// and each document's rank.

// Feature test macro for strtok_r (c.f., Linux Programming Interface p. 63)
#define _XOPEN_SOURCE 600
#define _READ_SIZE 128  // define the size of input buffer
#define _MAX_TOKEN 32  // define the maximum size of the user's query

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "libhw1/CSE333.h"
#include "./CrawlFileTree.h"
#include "./DocTable.h"
#include "./MemIndex.h"

//////////////////////////////////////////////////////////////////////////////
// Helper function declarations, constants, etc

// Prompt the user about inputting the correct format for the command-line
// arguments and a valid full path.
static void Usage(void);

// Process the queried words input by the customer and prints out the
// information about the queried words. Specifically, prints out the
// results of documents in which the queried words appeared and each
// document's rank. Keeps prompting the user to enter their input
// until they want to stop.
//
// Arguments:
// - dt: a doc table which contains information about documents' ID
//       and their corresponding file names.
// - mi: an in-memory inverted index table which contains all the
//       information about the queried words.
static void ProcessQueries(DocTable *dt, MemIndex *mi);

// Returns true if a string that contains queried words was returned.
// Otherwise, return false.
//
// Arguments:
// - f: An input buffer which takes the user's input
//      from the standard input.
// - retstr: an output parameter which returns a string
//           of user's queried words.
//
// Returns:
// - True: if a string of queried words was returned on success
// - False: on failure
static int GetNextLine(FILE *f, char **retstr);

//////////////////////////////////////////////////////////////////////////////
// Main
int main(int argc, char **argv) {
  if (argc != 2) {
    Usage();
  }

  // Implement searchshell!  We're giving you very few hints
  // on how to do it, so you'll need to figure out an appropriate
  // decomposition into functions as well as implementing the
  // functions.  There are several major tasks you need to build:
  //
  //  - Crawl from a directory provided by argv[1] to produce and index
  //  - Prompt the user for a query and read the query from stdin, in a loop
  //  - Split a query into words (check out strtok_r)
  //  - Process a query against the index and print out the results
  //
  // When searchshell detects end-of-file on stdin (cntrl-D from the
  // keyboard), searchshell should free all dynamically allocated
  // memory and any other allocated resources and then exit.

  DocTable *doc_table;
  MemIndex *mem_index;

  // Indexing the given directory
  printf("Indexing \'%s\'\n", argv[1]);

  // Crawls the given directory, indexing ASCII text files.
  // Prompt the user if it failed.
  if (!CrawlFileTree(argv[1], &doc_table, &mem_index)) {
    Usage();
  }

  Verify333(doc_table != NULL);
  Verify333(mem_index != NULL);

  // A helper function that processes the queried words
  ProcessQueries(doc_table, mem_index);

  // Free the doc table and inverted index table
  // once we are done with using them.
  DocTable_Free(doc_table);
  MemIndex_Free(mem_index);

  return EXIT_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////
// Helper function definitions

static void Usage(void) {
  fprintf(stderr, "Usage: ./searchshell <docroot>\n");
  fprintf(stderr,
          "where <docroot> is an absolute or relative " \
          "path to a directory to build an index under.\n");
  exit(EXIT_FAILURE);
}

static void ProcessQueries(DocTable *dt, MemIndex *mi) {
  // Keep prompting the user to input queried words, process them
  // and print out the results of documents in which the queried words
  // appeared and each  document's rank. Keeps prompting the user to enter
  // their input until they want to stop which means that we received
  // CTRL-D/NULL from the GetNextLine helper method.
  while (1) {
    printf("enter query:\n");
    // An output parameter which stores user's queried words.
    char *input;
    // Break out of the infinite while loop if they hit CTRL-D
    // to signify exit.
    if (!GetNextLine(stdin, &input)) {
      break;
    }
    char *query[_MAX_TOKEN];  // store an array of queried words
    char *token;  // get each token from the user's input
    char *left_read = input;  // point to bytes left to read
    int query_index = 0;
    // Split the user's query by space since each
    // token is a word.
    while ((token = strtok_r(left_read, " ", &left_read)) != NULL) {
      query[query_index] = token;
      query_index++;
    }

    // Get a linked list of SearchResult structs which contains
    // information about the documents that match the queried words.
    LinkedList *result_list = MemIndex_Search(mi, query, query_index);
    if (result_list != NULL) {
      // Allocate an iterator for the linked list
      LLIterator* searchresult_it = LLIterator_Allocate(result_list);
      Verify333(searchresult_it != NULL);

      // Prints out the results from each SearchResult in the list,
      // starting from two spaces at the very beginning, followed by
      // the full path of the document, and its rank.
      while (LLIterator_IsValid(searchresult_it)) {
        SearchResult *payload;
        LLIterator_Get(searchresult_it, (LLPayload_t *) &payload);
        printf("  %s (%d)\n",
              DocTable_GetDocName(dt, payload->docid), payload->rank);
        LLIterator_Next(searchresult_it);
      }

      // Free the linked list iterator once we are done.
      LLIterator_Free(searchresult_it);

    } else {  // If we can't find this word in any document
      free(input);  // free the input buffer
      continue;  // continute to prompt the user to input
    }
    // Free the linked list that contains search results
    // once we are done with it.
    LinkedList_Free(result_list, (LLPayloadFreeFnPtr) &free);
    free(input);  // free the input buffer
  }
}

static int GetNextLine(FILE *f, char **retstr) {
  // Malloc space to store the current
  // line of user's input.
  char *line = (char*) malloc(sizeof(char) * _READ_SIZE);
  char *result = fgets(line, _READ_SIZE, f);  // Get user's input

  if (result == NULL) {  // If there is no input
    free(line);  // Free the input buffer
    return 0;
  } else {
    // Find the first occurence of the new line character
    // in the string and get a returned pointer to it.
    char *ptr = strchr(line, '\n');
    // If the pointer exits, replace the new line character
    // with the null terminator.
    if (ptr != NULL) {
      *ptr = '\0';
    }

    // Turn each character of this string into
    // its lower case.
    for (int i = 0; i < strlen(line); i++) {
      line[i] = tolower(line[i]);
    }
    // Save the contents for the line of queried words
    *retstr = line;
    return 1;  // Return true since it succeeded.
  }
}
