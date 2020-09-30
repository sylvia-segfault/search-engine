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

#include "./HashTableReader.h"

extern "C" {
  #include "libhw1/CSE333.h"
}
#include "./Utils.h"  // for FileDup().


using std::list;

namespace hw3 {

HashTableReader::HashTableReader(FILE *f, IndexFileOffset_t offset)
  : file_(f), offset_(offset) {
  // STEP 1.
  // fread() the bucket list header in this hashtable from its
  // "num_buckets" field, and convert to host byte order.
  Verify333(fseek(file_, offset, SEEK_SET) == 0);
  Verify333(fread(&header_, sizeof(BucketListHeader), 1, file_) == 1);
  header_.toHostFormat();
}

HashTableReader::~HashTableReader() {
  fclose(file_);
  file_ = nullptr;
}

list<IndexFileOffset_t>
HashTableReader::LookupElementPositions(HTKey_t hashKey) {
  // Figure out which bucket the hash value is in.  We assume
  // hash values are mapped to buckets using the modulo (%) operator.
  int bucketNum = hashKey % header_.numBuckets;

  // Figure out the offset of the "bucket_rec" field for this bucket.
  IndexFileOffset_t bucketrecOffset = offset_
                                        + sizeof(BucketListHeader)
                                        + sizeof(BucketRecord) * bucketNum;
  // STEP 2.
  // Read the "chain len" and "bucket position" fields from the
  // bucket record, and convert from network to host order.
  BucketRecord bucketRec;
  Verify333(fseek(file_, bucketrecOffset, SEEK_SET) == 0);
  Verify333(fread(&bucketRec, sizeof(BucketRecord), 1, file_) == 1);
  bucketRec.toHostFormat();

  // This will be our returned list of element positions.
  list<IndexFileOffset_t> retval;


  // STEP 3.
  // Read the "element positions" fields from the "bucket" header into
  // the returned list.  Be sure to insert into the list in the
  // correct order (i.e., append to the end of the list).
  // fseek to the postion of this bucket record
  Verify333(fseek(file_, bucketRec.position, SEEK_SET) == 0);
  // Loop through the elements in this bucket
  for (int i = 0; i < bucketRec.chainNumElements; i++) {
    ElementPositionRecord ele_pos;
    // read the element position and push it to the end of
    // the returned list.
    if (fread(&ele_pos, sizeof(ElementPositionRecord), 1, file_) == 1) {
      ele_pos.toHostFormat();
      retval.push_back(ele_pos.position);
    }
  }

  // Return the list.
  return retval;
}

}  // namespace hw3
