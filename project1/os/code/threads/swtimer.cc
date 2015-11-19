#include "swtimer.h"
#include "list.h"
#include "system.h"


//----------------------------------------------------------------------
// SWTimer::SWTimer
//      Initialize a Software Timer so that it can control the execution
//      of threads
//
//      holdingQueue is a list that holds the threads that are put to
//      sleep
//
//      globalTime is the variable that counts the current time of the
//      system and when the next thread should be waken up
//---------------------------------------------------------------------


SWTimer::SWTimer() {
  holdingQueue = new List();
  globalTime = 0;
}


//----------------------------------------------------------------------
// SWTimer::~SWTimer
//
//      dealocates the software timer from memory so the system can use the
//      space for other data
//
//---------------------------------------------------------------------


SWTimer::~SWTimer() {
  delete holdingQueue; 
}

//used for debugging the holdingQueue
/*
void 
SWTimer::Print(int elem_ptr) {
  Thread *thr_ptr = (Thread*)elem_ptr;
  printf("%s %i g: %i\n", thr_ptr->getName(), thr_ptr->timeToWake, this->globalTime);
}
*/


//----------------------------------------------------------------------
// SWTimer::GoToSleepFor
//
//      A method that puts threads to sleep and places them in the
//      holdingQueue. The method first of all checks that time is even
//      because the interrupt is triggered every 2 msec so there cannot
//      be an odd amount of sleep time. If it is not even then the
//      thread is not added to the queue and -1 is returned.
//
//---------------------------------------------------------------------



int
SWTimer::GoToSleepFor(int howLong) {

  IntStatus oldLevel = interrupt->SetLevel(IntOff);


  if ((howLong % 2) == 0) {
    if (currentThread->sleepNumber <= 3) {
      if (currentThread->totalTimeSlept <= 30) {
          if ((currentThread->totalTimeSlept + howLong) >= 30) {
            printf("%s wanted to sleep for %i msec but put to sleep for %i msec\n", currentThread->getName(), howLong, (30-currentThread->totalTimeSlept));  
	    currentThread->timeToWake = (30 - currentThread->totalTimeSlept) + globalTime;
              currentThread->sleepNumber++;
              currentThread->totalTimeSlept = 31;
              holdingQueue->SortedInsert(currentThread, currentThread->timeToWake);
              currentThread->Sleep();
              return 0;
          }
          else {
              currentThread->sleepNumber++;
              currentThread->totalTimeSlept += howLong;
              currentThread->timeToWake = howLong + globalTime;
              holdingQueue->Append(currentThread);
              currentThread->Sleep();
              return 0;
          }
          
      }
      else { 
	printf("%s is too lazy and the system terminated it forever!, because it has slept more than 30 msec \n", currentThread->getName());
	currentThread->Finish();
	return -1;
      }
    }
    else { 
      printf("%s is too lazy and the system terminated it forever!, beacuse it has slept 3 times already \n", currentThread->getName());
      currentThread->Finish();
      return -1;
    } 
  }	
  else 
    return -1;


  (void) interrupt->SetLevel(oldLevel);

}


//----------------------------------------------------------------------
// SWTimer::WakeUp
//
//      This method removes threads from the holdingQueue by using the
//      remove method and then calls the scheduler object to add it back
//      into the ready queue. The remove method returns a pointer of type
//      void so the pointer needs to be casted as a thread in order to be
//      added back into the ready queue.
//
//---------------------------------------------------------------------

void 
SWTimer::WakeUp() {
  IntStatus oldLevel = interrupt->SetLevel(IntOff);
  scheduler->ReadyToRun(static_cast<Thread*>(holdingQueue->SortedRemove(NULL)));
  (void) interrupt->SetLevel(oldLevel);
}


//----------------------------------------------------------------------
// SWTimer::IncGlobalTime
//
//      This method is called by the threadInterruptHandler method in
//      system.cc. If the holdingQueue is not empty then it will check
//      if any threads are ready to be put back into the ready queue.
//      
//
//---------------------------------------------------------------------

void 
SWTimer::IncGlobalTime() {
  if (holdingQueue->IsEmpty()) {
      globalTime+=2;
  }
  else {

  Thread *nThread = static_cast<Thread*>(holdingQueue->SortedRemove(NULL));
  
  if (globalTime >= nThread->timeToWake) {
    holdingQueue->SortedInsert(nThread, nThread->timeToWake);
    globalTime+=2;
    WakeUp();
    //holdingQueue->Mapcar(Print);
 }
  else {
    holdingQueue->Prepend(nThread);
    globalTime += 2;
  }
  }
} 
