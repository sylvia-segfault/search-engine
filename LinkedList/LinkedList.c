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

#include "CSE333.h"
#include "LinkedList.h"
#include "LinkedList_priv.h"


///////////////////////////////////////////////////////////////////////////////
// LinkedList implementation.

LinkedList* LinkedList_Allocate(void) {
  // Allocate the linked list record.
  LinkedList *ll = (LinkedList *) malloc(sizeof(LinkedList));
  Verify333(ll != NULL);

  // STEP 1: initialize the newly allocated record structure.
  ll->num_elements = 0;
  ll->head = NULL;
  ll->tail = NULL;
  // Return our newly minted linked list.
  return ll;
}

void LinkedList_Free(LinkedList *list,
                     LLPayloadFreeFnPtr payload_free_function) {
  Verify333(list != NULL);
  Verify333(payload_free_function != NULL);

  // STEP 2: sweep through the list and free all of the nodes' payloads as
  // well as the nodes themselves.

  while (list->head != NULL) {
    // free the payload of current node
    payload_free_function(list->head->payload);
    // the node that is to be freed
    LinkedListNode* tempCurrNode = list->head;
    // make the head pointer point to the next node
    list->head = tempCurrNode->next;
    free(tempCurrNode);
  }

  free(list);
}

int LinkedList_NumElements(LinkedList *list) {
  Verify333(list != NULL);
  return list->num_elements;
}

void LinkedList_Push(LinkedList *list, LLPayload_t payload) {
  Verify333(list != NULL);

  // Allocate space for the new node.
  LinkedListNode *ln = (LinkedListNode *) malloc(sizeof(LinkedListNode));
  Verify333(ln != NULL);

  // Set the payload
  ln->payload = payload;

  if (list->num_elements == 0) {
    // Degenerate case; list is currently empty
    Verify333(list->head == NULL);
    Verify333(list->tail == NULL);
    ln->next = ln->prev = NULL;
    list->head = list->tail = ln;
    list->num_elements = 1;
  } else {
    // STEP 3: typical case; list has >=1 elements
    Verify333(list->head != NULL);
    Verify333(list->tail != NULL);
    LinkedListNode* tempNode = list->head;
    // Push the new node onto the head of the list,
    // set its previous to NULL and its next to
    // the original head of the list.
    list->head = ln;
    ln->prev = NULL;
    ln->next = tempNode;
    tempNode->prev = ln;
    list->num_elements += 1;
  }
}

bool LinkedList_Pop(LinkedList *list, LLPayload_t *payload_ptr) {
  Verify333(payload_ptr != NULL);
  Verify333(list != NULL);

  // STEP 4: implement LinkedList_Pop.  Make sure you test for
  // and empty list and fail.  If the list is non-empty, there
  // are two cases to consider: (a) a list with a single element in it
  // and (b) the general case of a list with >=2 elements in it.
  // Be sure to call free() to deallocate the memory that was
  // previously allocated by LinkedList_Push().

  bool popSuccess = false;
  // If the list is not empty, we can pop an element from the head
  if (list->num_elements != 0) {
    // Save the payload into the return parameter
    *payload_ptr = list->head->payload;
    popSuccess = true;
    // Store the original head of the list
    LinkedListNode* currNode = list->head;
    // If the list only contains one element, we need to set list's
    // head and tail to both NULL after removing that element.
    if (list->num_elements == 1) {
      Verify333(currNode->prev == NULL);
      Verify333(currNode->next == NULL);
      list->num_elements = 0;
      list->head = list->tail = NULL;
    // If the list contains more than one element, we move the head
    // pointer so that it points to the next element and then set
    // its previous to NULL.
    } else {
      list->head = currNode->next;
      list->head->prev = NULL;
      list->num_elements -= 1;
    }
    // Free the node that is pointed by head
    free(currNode);
  }
  return popSuccess;
}

void LinkedList_Append(LinkedList *list, LLPayload_t payload) {
  Verify333(list != NULL);

  // STEP 5: implement LinkedList_Append.  It's kind of like
  // LinkedList_Push, but obviously you need to add to the end
  // instead of the beginning.

  // Allocate space for the new node.
  LinkedListNode *ln = (LinkedListNode *) malloc(sizeof(LinkedListNode));
  Verify333(ln != NULL);

  // Set the payload
  ln->payload = payload;

  // If list was empty, we set both its head and tail to point to
  // the node we are appending. Both the new node's previous and
  // next are set to NULL.
  if (list->num_elements == 0) {
    // Degenerate case; list is currently empty
    Verify333(list->head == NULL);
    Verify333(list->tail == NULL);
    ln->next = ln->prev = NULL;
    list->head = list->tail = ln;
    list->num_elements = 1;
  // If list contained more than one element, we store the node that
  // was pointed by the tail pointer into a temp node variable, and
  // then make the tail pointer point to the node we are appending.
  // Set the new node's previous to point to the temp node, and set
  // the new node's next to NULL.
  } else {
    Verify333(list->head != NULL);
    Verify333(list->tail != NULL);
    LinkedListNode* tempNode = list->tail;
    list->tail = ln;
    ln->next = NULL;
    ln->prev = tempNode;
    ln->prev->next = ln;
    list->num_elements += 1;
  }
}

void LinkedList_Sort(LinkedList *list, bool ascending,
                     LLPayloadComparatorFnPtr comparator_function) {
  Verify333(list != NULL);
  if (list->num_elements < 2) {
    // No sorting needed.
    return;
  }

  // We'll implement bubblesort! Nnice and easy, and nice and slow :)
  int swapped;
  do {
    LinkedListNode *curnode;

    swapped = 0;
    curnode = list->head;
    while (curnode->next != NULL) {
      int compare_result = comparator_function(curnode->payload,
                                               curnode->next->payload);
      if (ascending) {
        compare_result *= -1;
      }
      if (compare_result < 0) {
        // Bubble-swap the payloads.
        LLPayload_t tmp;
        tmp = curnode->payload;
        curnode->payload = curnode->next->payload;
        curnode->next->payload = tmp;
        swapped = 1;
      }
      curnode = curnode->next;
    }
  } while (swapped);
}


///////////////////////////////////////////////////////////////////////////////
// LLIterator implementation.

LLIterator* LLIterator_Allocate(LinkedList *list) {
  Verify333(list != NULL);

  // OK, let's manufacture an iterator.
  LLIterator *li = (LLIterator *) malloc(sizeof(LLIterator));
  Verify333(li != NULL);

  // Set up the iterator.
  li->list = list;
  li->node = list->head;

  return li;
}

void LLIterator_Free(LLIterator *iter) {
  Verify333(iter != NULL);
  free(iter);
}

bool LLIterator_IsValid(LLIterator *iter) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);

  return iter->node != NULL;
}

bool LLIterator_Next(LLIterator *iter) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);
  Verify333(iter->node != NULL);

  // STEP 6: advance to the node beyond the iterator and return true if
  // it was valid.
  iter->node = iter->node->next;
  return iter->node != NULL;
}

void LLIterator_Get(LLIterator *iter, LLPayload_t *payload) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);
  Verify333(iter->node != NULL);

  // Set the return parameter to the current node's payload
  *payload = iter->node->payload;
}

bool LLIterator_Remove(LLIterator *iter,
                       LLPayloadFreeFnPtr payload_free_function) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);
  Verify333(iter->node != NULL);

  // STEP 7: implement LLIterator_Remove.  This is the most
  // complex function you'll build.  There are several cases
  // to consider:
  // - degenerate case: the list becomes empty after deleting.
  // - degenerate case: iter points at head
  // - degenerate case: iter points at tail
  // - fully general case: iter points in the middle of a list,
  //                       and you have to "splice".
  //
  // Be sure to call the payload_free_function to free the payload
  // the iterator is pointing to, and also free any LinkedList
  // data structure element as appropriate.

  bool listNotEmpty = true;
  LinkedListNode* currNode = iter->node;
  payload_free_function(iter->node->payload);  // free the payload

  // If the list pointed by the iterator only contains one
  // element, we set iterator to point to NULL. The changes
  // should be reflected on Linked List, and thus list's tail
  // and head become NULL as well.
  if (iter->list->num_elements == 1) {
    Verify333(iter->node->prev == NULL);
    Verify333(iter->node->next == NULL);
    listNotEmpty = false;
    iter->node = NULL;
    iter->list->head = iter->list->tail = iter->node;
  // If the list contains more than one element, we need to break
  // this case into 3 sub cases.
  } else {
    // When the deleted node is at the beginning of the list, we set
    // the iterator to point to its successor.
    if (iter->node->prev == NULL) {
      iter->node = currNode->next;
      iter->node->prev = currNode->prev;
      iter->list->head = iter->node;
    // When the deleted node is at the end of the list, we set the
    // iterator to point to its predecessor.
    } else if (iter->node->next == NULL) {
      iter->node = currNode->prev;
      iter->node->next = currNode->next;
      iter->list->tail = iter->node;
    // When the deleted node is in the middle of the list, we set
    // the iterator to point to its predecessor.
    } else {
      iter->node = currNode->next;
      iter->node->prev = currNode->prev;
      currNode->prev->next = iter->node;
    }
  }
  iter->list->num_elements -= 1;
  free(currNode);
  return listNotEmpty;
}


///////////////////////////////////////////////////////////////////////////////
// Helper functions

bool LinkedList_Slice(LinkedList *list, LLPayload_t *payload_ptr) {
  Verify333(payload_ptr != NULL);
  Verify333(list != NULL);

  // STEP 8: implement LinkedList_Slice.
  bool sliceSuccess = false;
  // When the list is not currently empty, we are able to slice
  // the element from the end of the list.
  if (list->num_elements != 0) {
    // Set the return parameter to the tail's payload
    *payload_ptr = list->tail->payload;
    sliceSuccess = true;
    // Store the element at the end of the list into a temp node
    LinkedListNode* currNode = list->tail;
    // When the list only contains one element orginially, we
    // set both head and tail of the list to NULL after removing
    // it.
    if (list->num_elements == 1) {
      Verify333(currNode->prev == NULL);
      Verify333(currNode->next == NULL);
      list->num_elements = 0;
      list->head = list->tail = NULL;
    // When the list originally contained more than one element,
    // we move the tail pointer to point to the previous node
    // of the delted node, and then set tail's next to NULL.
    } else {
      list->tail = currNode->prev;
      list->tail->next = NULL;
      list->num_elements -= 1;
    }
    // Free the node that is pointed by head
    free(currNode);
  }

  return sliceSuccess;
}

void LLIterator_Rewind(LLIterator *iter) {
  iter->node = iter->list->head;
}

