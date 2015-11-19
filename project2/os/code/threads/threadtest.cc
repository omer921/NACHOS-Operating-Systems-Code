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

/////////////////////////////////////////////////////
//Ryan Pierce
//CIS486 Project 2
/////////////////////////////////////////////////////
#include "mylist.h"
#include "copyright.h"
#include "system.h"
#include "request.h"
#include "project2.h"
#include "synch.h"
#include <stdio.h>
#include <stdlib.h>

MyList *list = new MyList();
Semaphore *mutex0 = new Semaphore("mutex for global FILE", 1);
Semaphore *mutex = new Semaphore("mutex for global list", 1);
Semaphore *synch = new Semaphore("synchronization for busy-wait", 0);
FILE *fp = fopen("url.txt", "r");
int count = 0;

void
Worker(int arg) {
  Request *req = (Request*)arg;
  //printf("%s", req->GetName());
  printf("Im worker %s\n", currentThread->getName());
  printf("Server: %s   ID: %d\n", req->GetName(), req->GetId());  
  serve((char*)req->GetName());
  delete req;
}

void
Server(int arg) {
  Request *req; 
  while(true) {

    synch->P();

    mutex->P();
    void* ptr = list->Remove();
    mutex->V();
    
    req =(Request*)ptr;

    Thread *worker = new Thread((char*)req->GetName());
    worker->Fork(Worker, (int)req);
  }
}

void
Client(int arg) {
  char* url;
 
  while(true){
    
    mutex0->P();
    url = readline(fp);
    mutex0->V();
   
    if(url == NULL)
       break;
   
    Request *req = new Request();
    req->SetName(url);
    req->SetId(count); 
    
    mutex->P();  
    list->Append(req);
    mutex->V();
    
    synch->V();

    count++;
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
    //    Thread *server = new Thread("server");
    Thread *s1 = new Thread("server1");
    //    server->Fork(Server, 0);
    s1->Fork(Server, 0);

    Thread *c1 = new Thread("client1");
    Thread *c2 = new Thread("client2");
    c1->Fork(Client, 0);
    c2->Fork(Client, 0);  
    
    //    mutex->P();
    //    mutex->V();

    //    Request *req = new Request();
    //    req->SetName("Whatever");
    //    req->SetId(123);

    //    printf("Name = \"%s\", Id = %d\n", req->GetName(), req->GetId());

    //    char* line;
    //    while((line = readline(fp)) != NULL) {
    //      printf("Downloading %s\n", line);
    //      serve(line);
    //    }

    //    printf("===========================\n");
    //    req->SetId(123);

}
