// mylist.cc
//
//     	Routines to manage a singly-linked list of "things".
//
// 	A "MyListElement" is allocated for each item to be put on the
//	list; it is de-allocated when the item is removed. This means
//      we don't need to keep a "next" pointer in every object we
//      want to put on a list.
//
//     	NOTE: Mutual exclusion must be provided by the caller.
//  	If you want a synchronized list, you must use the routines
//	in synchlist.cc.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "mylist.h"
#include "system.h"

#define TICK {\
int x;\
for (x = 0; x < 100; ++x) {\
interrupt->OneTick();\
}}

//----------------------------------------------------------------------
// MyListElement::MyListElement
// 	Initialize a list element, so it can be added somewhere on a list.
//
//	"itemPtr" is the item to be put on the list.  It can be a pointer
//		to anything.
//	"sortKey" is the priority of the item, if any.
//----------------------------------------------------------------------

MyListElement::MyListElement(void *itemPtr, int sortKey)
{
  item = itemPtr;
  key = sortKey;
  next = NULL;	// assume we'll put it at the end of the list
}

//----------------------------------------------------------------------
// MyList::MyList
//	Initialize a list, empty to start with.
//	Elements can now be added to the list.
//----------------------------------------------------------------------

MyList::MyList()
{
  first = last = NULL;
}

//----------------------------------------------------------------------
// MyList::~MyList
//	Prepare a list for deallocation.  If the list still contains any
//	MyListElements, de-allocate them.  However, note that we do *not*
//	de-allocate the "items" on the list -- this module allocates
//	and de-allocates the MyListElements to keep track of each item,
//	but a given item may be on multiple lists, so we can't
//	de-allocate them here.
//----------------------------------------------------------------------

MyList::~MyList()
{
  while (Remove() != NULL)
    ;	 // delete all the list elements
}

//----------------------------------------------------------------------
// MyList::Append
//      Append an "item" to the end of the list.
//
//	Allocate a MyListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//	Otherwise, put it at the end.
//
//	"item" is the thing to put on the list, it can be a pointer to
//		anything.
//----------------------------------------------------------------------

void
MyList::Append(void *item)
{
  MyListElement *element = new MyListElement(item, 0);
  
  if (IsEmpty()) {		// list is empty
    TICK;
    first = element;
    TICK;
    last = element;
  } else {			// else put it after last
    TICK;
    last->next = element;
    TICK;
    last = element;
  }
}

//----------------------------------------------------------------------
// MyList::Prepend
//      Put an "item" on the front of the list.
//
//	Allocate a MyListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//	Otherwise, put it at the beginning.
//
//	"item" is the thing to put on the list, it can be a pointer to
//		anything.
//----------------------------------------------------------------------

void
MyList::Prepend(void *item)
{
  MyListElement *element = new MyListElement(item, 0);
  
  if (IsEmpty()) {		// list is empty
    TICK;
    first = element;
    TICK;
    last = element;
  } else {			// else put it before first
    TICK;
    element->next = first;
    TICK;
    first = element;
  }
}

//----------------------------------------------------------------------
// MyList::Remove
//      Remove the first "item" from the front of the list.
//
// Returns:
//	Pointer to removed item, NULL if nothing on the list.
//----------------------------------------------------------------------

void *
MyList::Remove()
{
  return SortedRemove(NULL);  // Same as SortedRemove, but ignore the key
}

//----------------------------------------------------------------------
// MyList::Mapcar
//	Apply a function to each item on the list, by walking through
//	the list, one element at a time.
//
//	Unlike LISP, this mapcar does not return anything!
//
//	"func" is the procedure to apply to each element of the list.
//----------------------------------------------------------------------

void
MyList::Mapcar(VoidFunctionPtr func)
{
  for (MyListElement *ptr = first; ptr != NULL; ptr = ptr->next) {
    DEBUG('l', "In mapcar, about to invoke %x(%x)\n", func, ptr->item);
    (*func)((int)ptr->item);
  }
}

//----------------------------------------------------------------------
// MyList::IsEmpty
//      Returns TRUE if the list is empty (has no items).
//----------------------------------------------------------------------

bool
MyList::IsEmpty()
{
  if (first == NULL)
    return TRUE;
  else
    return FALSE;
}

//----------------------------------------------------------------------
// MyList::SortedInsert
//      Insert an "item" into a list, so that the list elements are
//	sorted in increasing order by "sortKey".
//
//	Allocate a MyListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//	Otherwise, walk through the list, one element at a time,
//	to find where the new item should be placed.
//
//	"item" is the thing to put on the list, it can be a pointer to
//		anything.
//	"sortKey" is the priority of the item.
//----------------------------------------------------------------------

void
MyList::SortedInsert(void *item, int sortKey)
{
  MyListElement *element = new MyListElement(item, sortKey);
  MyListElement *ptr;		// keep track
  
  if (IsEmpty()) {	// if list is empty, put
    TICK;
    first = element;
    TICK;
    last = element;
  } else if (sortKey < first->key) {
    TICK;
    // item goes on front of list
    element->next = first;
    TICK;
    first = element;
  } else {		// look for first elt in list bigger than item
    TICK;
    for (ptr = first; ptr->next != NULL; ptr = ptr->next) {
      if (sortKey < ptr->next->key) {
        element->next = ptr->next;
        TICK;
        ptr->next = element;
        return;
      }
    }
    TICK;
    last->next = element;		// item goes at end of list
    TICK;
    last = element;
  }
}

//----------------------------------------------------------------------
// MyList::SortedRemove
//      Remove the first "item" from the front of a sorted list.
//
// Returns:
//	Pointer to removed item, NULL if nothing on the list.
//	Sets *keyPtr to the priority value of the removed item
//	(this is needed by interrupt.cc, for instance).
//
//	"keyPtr" is a pointer to the location in which to store the
//		priority of the removed item.
//----------------------------------------------------------------------

void *
MyList::SortedRemove(int *keyPtr)
{
  MyListElement *element = first;
  void *thing;
  
  if (IsEmpty()) {
    TICK;
    return NULL;
  }
  
  thing = first->item;
  TICK;
  if (first == last) {	// list had one item, now has none
    TICK;
    first = NULL;
    TICK;
    last = NULL;
  } else {
    TICK;
    first = element->next;
  }
  TICK;
  if (keyPtr != NULL) {
    TICK;
    *keyPtr = element->key;
  }
  delete element;
  return thing;
}

