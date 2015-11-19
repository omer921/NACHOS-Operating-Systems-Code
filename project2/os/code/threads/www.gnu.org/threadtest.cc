// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "project2.h"
#include "synch.h"
#include "request.h"
#include "mylist.h"

FILE * pFile = fopen ("url.txt" , "r"); 
Semaphore *clientS = new Semaphore("CS", 1);
Semaphore *listS = new Semaphore("LS", 1);
Semaphore *serverS = new Semaphore("SS", 0);
MyList *globalList = new MyList();
int id = 0;
int workerID = 0;

void
Print(int elem_ptr) {
  Request *req_ptr = (Request*)elem_ptr;
  printf("%s\n", req_ptr->GetName());
}


//----------------------------------------------------------------------
// ClientThread
//      Accesses the url.txt file and loads the url into a request object
//      when the request object is loaded into the globalList, the client
//      signals the server that it has something in the list. This makes
//      sure that the server does not busy wait but rather sleeps in the
//      serverS semaphore queue.
//----------------------------------------------------------------------


void
ClientThread(int which) {
  while (1){
    clientS->P();
    char* rPointer = readline(pFile);
    clientS->V();
    
    if (rPointer == NULL)
      break;

    Request *r = new Request();
    
    r->SetName(rPointer);
    r->SetId(id);
    
    id++;

    listS->P();
    globalList->Append(r);
    listS->V();
    
    serverS->V();
                         
  }
}


//---------------------------------------------------------------------------
// WorkerThread
//      Takes in the pointer of the request element as an int and casts a new
//      request element. Prints the id of the worker thread, the url, and
//      the id of the request object. It then calls the serve function which
//      downloads the url contents into a folder. The argument is cast as a
//      char* because Request::GetName() returns a const char*
//---------------------------------------------------------------------------

void
WorkerThread(int elem) {
  Request *r = (Request*)elem;
  printf("worker thread %i is dispatching new urls %s with request id %i\n", workerID, r->GetName(), r->GetId());
  serve((char*)(r->GetName()));
  workerID++;
}

//----------------------------------------------------------------------
// ServerThread
//      First checks if there are any requests waiting to be handled. If
//      there are none then it will get blocked in the serverS semaphore
//      queue. If there are then it will go into the critical section and
//      create a new request oobject from the object it gets off the list
//      the it forks a new worker thread which contains the pointer of
//      that request object.
//----------------------------------------------------------------------

void 
ServerThread(int which) {
  while(1) {
    serverS->P();  
    
      listS->P();
      Request *elem = static_cast<Request *>(globalList->Remove());
      listS->V();  
      Thread *t = new Thread("worker thread");
      t->Fork(WorkerThread, (int)elem);

      }
}
      

//----------------------------------------------------------------------
// ThreadTest
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest()
{
  Thread *t = new Thread("Client 0");
  t->Fork(ClientThread, 0);
 
  Thread *t1 = new Thread("Client 1");
  t1->Fork(ClientThread, 1);
  
  Thread *t2 = new Thread("Server");
  t2->Fork(ServerThread, 1);
}

