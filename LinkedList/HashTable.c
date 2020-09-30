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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "CSE333.h"
#include "HashTable.h"
#include "HashTable_priv.h"

///////////////////////////////////////////////////////////////////////////////
// Internal helper functions.
//
#define INVALID_IDX -1

// Grows the hashtable (ie, increase the number of buckets) if its load
// factor has become too high.
static void MaybeResize(HashTable *ht);

// Search for the given key from the table and conditionally remove
// the key, depending on the customer's query. The old key-value
// pair is stored through the return parameter.
//
// Arguments:
// - table: the table where we are looking for the key.
// - key: the key we are searching for
// - oldKV: the return parameter that stores old key-value pair
// - remove: indicate whether to remove the key
//
// Returns:
// - True if the key was found on success, and the old key-value
//   is returned through the return parameter.
// - False if the key does not exist, on failure.
static bool RemoveKey(HashTable * table, HTKey_t key,
                      HTKeyValue_t* oldKV, bool remove);

int HashKeyToBucketNum(HashTable *ht, HTKey_t key) {
  return key % ht->num_buckets;
}

// Deallocation functions that do nothing.  Useful if we want to deallocate
// the structure (eg, the linked list) without deallocating its elements or
// if we know that the structure is empty.
static void LLNoOpFree(LLPayload_t freeme) { }
static void HTNoOpFree(HTValue_t freeme) { }


///////////////////////////////////////////////////////////////////////////////
// HashTable implementation.

HTKey_t FNVHash64(unsigned char *buffer, int len) {
  // This code is adapted from code by Landon Curt Noll
  // and Bonelli Nicola:
  //     http://code.google.com/p/nicola-bonelli-repo/
  static const uint64_t FNV1_64_INIT = 0xcbf29ce484222325ULL;
  static const uint64_t FNV_64_PRIME = 0x100000001b3ULL;
  unsigned char *bp = (unsigned char *) buffer;
  unsigned char *be = bp + len;
  uint64_t hval = FNV1_64_INIT;

  // FNV-1a hash each octet of the buffer.
  while (bp < be) {
    // XOR the bottom with the current octet.
    hval ^= (uint64_t) * bp++;
    // Multiply by the 64 bit FNV magic prime mod 2^64.
    hval *= FNV_64_PRIME;
  }
  return hval;
}

HashTable* HashTable_Allocate(int num_buckets) {
  HashTable *ht;
  int i;

  Verify333(num_buckets > 0);

  // Allocate the hash table record.
  ht = (HashTable *) malloc(sizeof(HashTable));

  // Initialize the record.
  ht->num_buckets = num_buckets;
  ht->num_elements = 0;
  ht->buckets = (LinkedList **) malloc(num_buckets * sizeof(LinkedList *));
  for (i = 0; i < num_buckets; i++) {
    ht->buckets[i] = LinkedList_Allocate();
  }

  return ht;
}

void HashTable_Free(HashTable *table,
                    ValueFreeFnPtr value_free_function) {
  int i;

  Verify333(table != NULL);

  // Free each bucket's chain.
  for (i = 0; i < table->num_buckets; i++) {
    LinkedList *bucket = table->buckets[i];
    HTKeyValue_t *kv;

    // Pop elements off the chain list one at a time.  We can't do a single
    // call to LinkedList_Free since we need to use the passed-in
    // value_free_function -- which takes a HTValue_t, not an LLPayload_t -- to
    // free the caller's memory.
    while (LinkedList_NumElements(bucket) > 0) {
      Verify333(LinkedList_Pop(bucket, (LLPayload_t *)&kv));
      value_free_function(kv->value);
      free(kv);
    }
    // The chain is empty, so we can pass in the
    // null free function to LinkedList_Free.
    LinkedList_Free(bucket, LLNoOpFree);
  }

  // Free the bucket array within the table, then free the table record itself.
  free(table->buckets);
  free(table);
}

int HashTable_NumElements(HashTable *table) {
  Verify333(table != NULL);
  return table->num_elements;
}

bool HashTable_Insert(HashTable *table,
                      HTKeyValue_t newkeyvalue,
                      HTKeyValue_t *oldkeyvalue) {
  int bucket;
  LinkedList *chain;

  Verify333(table != NULL);
  MaybeResize(table);

  // Calculate which bucket and chain we're inserting into.
  bucket = HashKeyToBucketNum(table, newkeyvalue.key);
  chain = table->buckets[bucket];

  // STEP 1: finish the implementation of InsertHashTable.
  // This is a fairly complex task, so you might decide you want
  // to define/implement a helper function that helps you find
  // and optionally remove a key within a chain, rather than putting
  // all that logic inside here.  You might also find that your helper
  // can be reused in steps 2 and 3.

  // A helper function that checks if the given key exists. In this case,
  // we want to remove the key if it is there.
  bool keyExist = RemoveKey(table, newkeyvalue.key, oldkeyvalue, true);
  // Allocate space for the payload and stores the key-value pair into
  // the payload. Append this payload to the end of the list.
  HTKeyValue_t* payloadKV = (HTKeyValue_t*) malloc(sizeof(HTKeyValue_t));
  *payloadKV = newkeyvalue;
  LinkedList_Append(chain, (LLPayload_t) payloadKV);
  table->num_elements += 1;
  return keyExist;
}

bool HashTable_Find(HashTable *table,
                    HTKey_t key,
                    HTKeyValue_t *keyvalue) {
  Verify333(table != NULL);

  // STEP 2: implement HashTable_Find.

  // A helper function that checks if the given key exists.
  // We only want to search for the key instead of removing
  // it from the table.
  return RemoveKey(table, key, keyvalue, false);
}

bool HashTable_Remove(HashTable *table,
                      HTKey_t key,
                      HTKeyValue_t *keyvalue) {
  Verify333(table != NULL);
  // STEP 3: implement HashTable_Remove.

  // A helper function that checks if the given key
  // exists. We want to remove the key if it is there.
  return RemoveKey(table, key, keyvalue, true);
}


///////////////////////////////////////////////////////////////////////////////
// HTIterator implementation.

HTIterator* HTIterator_Allocate(HashTable *table) {
  HTIterator *iter;
  int         i;

  Verify333(table != NULL);

  iter = (HTIterator *) malloc(sizeof(HTIterator));

  // If the hash table is empty, the iterator is immediately invalid,
  // since it can't point to anything.
  if (table->num_elements == 0) {
    iter->ht = table;
    iter->bucket_it = NULL;
    iter->bucket_idx = INVALID_IDX;
    return iter;
  }

  // Initialize the iterator.  There is at least one element in the
  // table, so find the first element and point the iterator at it.
  iter->ht = table;
  for (i = 0; i < table->num_buckets; i++) {
    if (LinkedList_NumElements(table->buckets[i]) > 0) {
      iter->bucket_idx = i;
      break;
    }
  }
  Verify333(i < table->num_buckets);  // make sure we found it.
  iter->bucket_it = LLIterator_Allocate(table->buckets[iter->bucket_idx]);
  return iter;
}

void HTIterator_Free(HTIterator *iter) {
  Verify333(iter != NULL);
  if (iter->bucket_it != NULL) {
    LLIterator_Free(iter->bucket_it);
    iter->bucket_it = NULL;
  }
  free(iter);
}

bool HTIterator_IsValid(HTIterator *iter) {
  Verify333(iter != NULL);

  // STEP 4: implement HTIterator_IsValid.

  // Check if the current bucket's iterator is valid.
  return iter->bucket_it != NULL;
}

bool HTIterator_Next(HTIterator *iter) {
  Verify333(iter != NULL);

  // STEP 5: implement HTIterator_Next.

  // Immediately return false if the hash table iterator
  // is not valid.
  if (!HTIterator_IsValid(iter)) {
    return false;
  }

  // Return true if the iterator for the chain inside
  // the current bucket has more elements to look at.
  if (LLIterator_Next(iter->bucket_it)) {
    return true;
  }
  // Free the current chain's iterator because we are done
  // with using it.
  LLIterator_Free(iter->bucket_it);
  // Set the current chain's iterator to NULL.
  iter->bucket_it = NULL;

  // Starting from the next bucket in the hash table, we keep looking
  // for a bucket that contains elements.
  for (int i = iter->bucket_idx + 1; i < iter->ht->num_buckets; i++) {
    iter->bucket_idx = i;  // Update the bucket's index
    // If there are elements in this bucket, we allocate an iterator
    // for the chain in this bucket and save it into the hash table
    // iterator. Return true since we found more elements to look at.
    if (LinkedList_NumElements(iter->ht->buckets[i]) > 0) {
      iter->bucket_it = LLIterator_Allocate(iter->ht->buckets[i]);
      return true;
    }
  }
  // When we reach this point, we are at the end of the table. There
  // are no more buckets to look at, and thus the bucket index should
  // be set to invalid (-1).
  iter->bucket_idx = INVALID_IDX;
  return false;
}

bool HTIterator_Get(HTIterator *iter, HTKeyValue_t *keyvalue) {
  Verify333(iter != NULL);

  // STEP 6: implement HTIterator_Get.
  // If the hash table iterator is valid, then we can access
  // its elements.
  if (HTIterator_IsValid(iter)) {
    // A temp pointer that will store the current key-value pair
    HTKeyValue_t* tempKV;
    // An intermediate pointer that stores the address of the
    // tempKV we defined above.
    LLPayload_t* tempPayload = (LLPayload_t) &tempKV;
    // Get the current payload/key-value pair and set tempPayload
    // to point to the payload/key-value pair. After calling
    // LLIterator_Get, tempPayload is pointing to tempKV which
    // points to the payload/key-value pair we want.
    LLIterator_Get(iter->bucket_it, tempPayload);
    // Dereference tempKV and store the payload/key-value pair
    // into the return parameter.
    *keyvalue = *tempKV;
    return true;
  }
  return false;
}

bool HTIterator_Remove(HTIterator *iter, HTKeyValue_t *keyvalue) {
  HTKeyValue_t kv;

  Verify333(iter != NULL);

  // Try to get what the iterator is pointing to.
  if (!HTIterator_Get(iter, &kv)) {
    return false;
  }

  // Advance the iterator.  Thanks to the above call to
  // HTIterator_Get, we know that this iterator is valid (though it
  // may not be valid after this call to HTIterator_Next).
  HTIterator_Next(iter);

  // Lastly, remove the element.  Again, we know this call will succeed
  // due to the successful HTIterator_Get above.
  Verify333(HashTable_Remove(iter->ht, kv.key, keyvalue));
  Verify333(kv.key == keyvalue->key);
  Verify333(kv.value == keyvalue->value);

  return true;
}

static void MaybeResize(HashTable *ht) {
  HashTable *newht;
  HashTable tmp;
  HTIterator *it;

  // Resize if the load factor is > 3.
  if (ht->num_elements < 3 * ht->num_buckets)
    return;

  // This is the resize case.  Allocate a new hashtable,
  // iterate over the old hashtable, do the surgery on
  // the old hashtable record and free up the new hashtable
  // record.
  newht = HashTable_Allocate(ht->num_buckets * 9);

  // Loop through the old ht copying its elements over into the new one.
  for (it = HTIterator_Allocate(ht);
       HTIterator_IsValid(it);
       HTIterator_Next(it)) {
    HTKeyValue_t item, unused;

    Verify333(HTIterator_Get(it, &item));
    HashTable_Insert(newht, item, &unused);
  }

  // Swap the new table onto the old, then free the old table (tricky!).  We
  // use the "no-op free" because we don't actually want to free the elements;
  // they're owned by the new table.
  tmp = *ht;
  *ht = *newht;
  *newht = tmp;

  // Done!  Clean up our iterator and temporary table.
  HTIterator_Free(it);
  HashTable_Free(newht, &HTNoOpFree);
}

static bool RemoveKey(HashTable * table, HTKey_t key,
                      HTKeyValue_t* oldKV, bool remove) {
  // Hash the key to find the bucket index
  int bucket = HashKeyToBucketNum(table, key);
  LLIterator* chainIter = LLIterator_Allocate(table->buckets[bucket]);
  // We keep searching for the key if there are still
  // more elements in the bucket chain.
  while (LLIterator_IsValid(chainIter)) {
    HTKeyValue_t* kv;
    // Get the current payload/Key-value struct by storing it into a pointer
    LLIterator_Get(chainIter, (LLPayload_t *)&kv);
    if (kv->key == key) {
      *oldKV = *kv;  // Save the old key-value struct into the return parameter
      // Remove the key
      if (remove) {
        // We need to free the pointer that points to the key-value pair
        // we are searching for.
        free(kv);
        LLIterator_Remove(chainIter, &LLNoOpFree);
        table->num_elements -= 1;
      }
      LLIterator_Free(chainIter);  // Free the LL iterator we just allocated
      return true;
    }
    LLIterator_Next(chainIter);  // Advance to the next element in the chain
  }
  LLIterator_Free(chainIter);
  return false;
}
