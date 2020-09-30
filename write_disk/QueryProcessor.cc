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

#include "./QueryProcessor.h"

#include <iostream>
#include <algorithm>

extern "C" {
  #include "./libhw1/CSE333.h"
}

using std::list;
using std::sort;
using std::string;
using std::vector;

namespace hw3 {

QueryProcessor::QueryProcessor(const list<string> &indexlist, bool validate) {
  // Stash away a copy of the index list.
  indexlist_ = indexlist;
  arraylen_ = indexlist_.size();
  Verify333(arraylen_ > 0);

  // Create the arrays of DocTableReader*'s. and IndexTableReader*'s.
  dtr_array_ = new DocTableReader *[arraylen_];
  itr_array_ = new IndexTableReader *[arraylen_];

  // Populate the arrays with heap-allocated DocTableReader and
  // IndexTableReader object instances.
  list<string>::const_iterator idx_iterator = indexlist_.begin();
  for (int i = 0; i < arraylen_; i++) {
    FileIndexReader fir(*idx_iterator, validate);
    dtr_array_[i] = fir.NewDocTableReader();
    itr_array_[i] = fir.NewIndexTableReader();
    idx_iterator++;
  }
}

QueryProcessor::~QueryProcessor() {
  // Delete the heap-allocated DocTableReader and IndexTableReader
  // object instances.
  Verify333(dtr_array_ != nullptr);
  Verify333(itr_array_ != nullptr);
  for (int i = 0; i < arraylen_; i++) {
    delete dtr_array_[i];
    delete itr_array_[i];
  }

  // Delete the arrays of DocTableReader*'s and IndexTableReader*'s.
  delete[] dtr_array_;
  delete[] itr_array_;
  dtr_array_ = nullptr;
  itr_array_ = nullptr;
}

// This structure is used to store a index-file-specific query result.
typedef struct {
  DocID_t docid;  // The document ID within the index file.
  int rank;       // The rank of the result so far.
} IdxQueryResult;

// Build a vector of IdxQueryResult structs using the given DocIDTableReader
// for a query word, and save the results into a return parameter.
//
// Arguments:
// - docid_table_reader: a DocID table reader which is used to get information
//                       about all the matching documents for a query word
//                       and the positions where that word appears in
//                       each document.
// - result: a return parameter that stores the query results of all the
//           matching documents of a query word and their corresponding ranks.
static void BuildIdxQueryResult(DocIDTableReader *docid_table_reader,
                                vector<IdxQueryResult> *result);

// Return a vector of IdxQueryResult structs for the first query word
// by finding all the matching documents and their corresponding ranks
// in the current index file.
//
// Arguments:
// - word: the first query word that is to be searched for matching documents.
// - itable_reader: an index table reader that is used to look up a query word
//                  in the current index file.
//
// Returns:
// - a vector of IdxQueryResult structs which stores all the matching documents
//   for the first query word, and the rank of each matching document, if
//   the matching documents are found.
// - an empty vector if no matching documents can be found.
static vector<IdxQueryResult>
ProcessFirstWord(const string &word, IndexTableReader *itable_reader);

// Look up the next query word in the given index file and stores a vector of
// IdxQueryResult structs which contains matching documents and their
// corresponding ranks for the given query word, through a return parameter.
//
// Arguments:
// - word: the next query word that is to be processed
// - itable_reader: an index table reader that is used to look up the query word
//                  in the current index file.
// - curr_result: a return parameter that stores the query results of all
//                the matching documents of the given query word and their
//                corresponding ranks.
static void
ProcessEachWord(const string &word, IndexTableReader *itable_reader,
                vector<IdxQueryResult> *curr_result);

// Get the final result from a vector of IdxQueryResult structs, by
// changing each IdxQueryResult to a QueryResult. Store the resulting
// vector into a return parameter.
//
// Arguments:
// - curr_result: a vector of IdxQueryResult structs that is to be converted
// - finalresult: a return parameter that stores the final result as a vector
//                of QueryResult structs.
// - doc_table_reader: a doc table reader that can convert a document ID to
//                     its corresponding document name.
static void GetFinalResult(vector<IdxQueryResult> *curr_result,
                           vector<QueryProcessor::QueryResult> *finalresult,
                           DocTableReader *doc_table_reader);

vector<QueryProcessor::QueryResult>
QueryProcessor::ProcessQuery(const vector<string> &query) {
  Verify333(query.size() > 0);

  // STEP 1.
  vector<QueryProcessor::QueryResult> finalresult;
  for (int i = 0; i < arraylen_; i++) {
    // Process the first query word and get the results of matching
    // documents and their corresponding ranks.
    vector<IdxQueryResult> result = ProcessFirstWord(query[0], itr_array_[i]);
    // If we can't find any matching document for the first query word, go to
    // the next index file to search
    if (result.size() == 0) {
      continue;
    }
    // Keep looking up the next query word and update the results
    // to match all the query words.
    for (size_t j = 1; j < query.size(); j++) {
      ProcessEachWord(query[j], itr_array_[i], &result);
      // If we can't find any matching document at this point, there is
      // no need to keep processing the other query words. Break out
      // of this for loop and go to the next index file to search.
      if (result.size() == 0) {
        break;
      }
    }
    // Get the final result by converting a vector of IdxQueryResult
    // structs to a vector of QueryResult structs.
    GetFinalResult(&result, &finalresult, dtr_array_[i]);
  }

  // Sort the final results.
  sort(finalresult.begin(), finalresult.end());
  return finalresult;
}

static void BuildIdxQueryResult(DocIDTableReader *docid_table_reader,
                                vector<IdxQueryResult> *result) {
  // Get a list of DocIDElementHeader structs which contains information
  // about the query word's matching documents and their corresponding
  // ranks.
  list<DocIDElementHeader> docid_eles = docid_table_reader->GetDocIDList();
  // delete the DocIDTableReader once we are done with it
  delete docid_table_reader;

  list<DocIDElementHeader>::iterator it;
  // Iterate through this list of DocIDElementHeader structs
  for (auto it = docid_eles.begin(); it != docid_eles.end(); ++it) {
    // Get the current header
    DocIDElementHeader curr_header = *it;
    // Get the docid and numpositions in this current header
    DocID_t docid = curr_header.docID;
    int32_t num_positions = curr_header.numPositions;
    // Store the docid and numpositions into an IdxQueryResult
    // struct and add it to the end of the vector that stores
    // all the results.
    IdxQueryResult query_result;
    query_result.docid = docid;
    query_result.rank = num_positions;
    result->push_back(query_result);
  }
}

static vector<IdxQueryResult>
ProcessFirstWord(const string &word, IndexTableReader *itable_reader) {
  // Store the results of matching documents
  // and their corresponding ranks.
  vector<IdxQueryResult> result;
  // Check if the first query word exists in the current index file
  DocIDTableReader *docid_table_reader = itable_reader->LookupWord(word);

  // If we can't find this word in this index file, go to the next one
  if (docid_table_reader == nullptr) {
    return result;
  }

  // If the first query word can be found in the current index file,
  // process this word by storing the results of its matching documents
  // and corresponding ranks, through a return parameter.
  BuildIdxQueryResult(docid_table_reader, &result);
  return result;
}

static void
ProcessEachWord(const string &word, IndexTableReader *itable_reader,
                vector<IdxQueryResult> *curr_result) {
  // Store the results of matching documents and their corresponding ranks
  // only for the current query word, so that we can filter out the results
  // for previous query words to match the current query word.
  vector<IdxQueryResult> result;
  // Check if the current query word exists in the given index file
  DocIDTableReader *docid_table_reader = itable_reader->LookupWord(word);

  // If the current query word can't be found, we can stop searching.
  // There are no documents that can match all the query words up to
  // this point, and thus empty out the results of previous query
  // words.
  if (docid_table_reader == nullptr) {
    curr_result->clear();
    return;
  }

  // If the current query word can be found in the current index file,
  // process this word by storing the results of its matching documents
  // and corresponding ranks, through a return parameter.
  BuildIdxQueryResult(docid_table_reader, &result);

  vector<IdxQueryResult>::iterator it_result = curr_result->begin();

  // Iterate through the vector of results for previous query words
  // to check if there are documents which also match the current
  // query word. If that is the case, update the matching document's
  // rank. If the documents don't match all the query words up to
  // this point, erase them.
  while (it_result != curr_result->end()) {
    // Get the current IdxQueryResult struct that matches
    // the previous query words.
    IdxQueryResult prev_result = *it_result;
    // Indicate whether this is a matching document for all the
    // query words up to this point.
    bool docid_exist = false;
    // Loop through the results for the current query word to see
    // if there is any document that can match all the query words.
    for (size_t j = 0; j < result.size(); j++) {
      IdxQueryResult new_result = result[j];
      // If there is a matching document, update its rank in
      // curr_result.
      if (prev_result.docid == new_result.docid) {
        it_result->rank += new_result.rank;
        docid_exist = true;
        // Break out of the loop since we found a matching
        // document that is unique in the vector and there
        // is no need to keep searching.
        break;
      }
    }

    // If we didn't find a matching document that satisfies
    // all the query words up to this point, erase it from
    // the results for previous query words.
    if (!docid_exist) {
      it_result = curr_result->erase(it_result);
    } else {
      // move to the next position within the iterator
      it_result++;
    }
  }
}

static void GetFinalResult(vector<IdxQueryResult> *curr_result,
                           vector<QueryProcessor::QueryResult> *finalresult,
                           DocTableReader *doc_table_reader) {
  // Loop through the vector of IdxQueryResult structs and convert
  // each of them to a QueryResult struct.
  for (size_t i = 0; i < curr_result->size(); i++) {
    IdxQueryResult idx_result = (*curr_result)[i];
    string filename;
    // Find the filename that corresponds to this
    // DocID, through a return parameter.
    Verify333(doc_table_reader->LookupDocID(idx_result.docid, &filename) == 1);
    // Create a QueryResult struct with the correct document name
    // and its rank, and add it to the end of the vector that
    // stores all the results.
    QueryProcessor::QueryResult query_result;
    query_result.documentName = filename;
    query_result.rank = idx_result.rank;
    finalresult->push_back(query_result);
  }
}

}  // namespace hw3
