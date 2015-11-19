// Swtimer.cc
//      This class is a software timer that controls the execution
//      of threads for multi threading. There is a contstructor and
//      destructor. The methods are all used in the program to check
//      for if it is time to put a thread back into the ready queue.
//      The only method that is not used is the Print method which
//      was used to debug the holding queue.






#ifndef SWTIMER_H
#define SWTIMER_H

#include "list.h"


class SWTimer {
 public:  
  SWTimer();                            // initialize a software timer
  ~SWTimer();                           // deallocate a software timer
  int GoToSleepFor(int howLong);        // puts a thread to sleep for a certain time
  void Print(int elem_ptr);             // prints all the threads in the holdingQueue when used with MapCar()
  void WakeUp();                        // wakes a thread up (i.e. puts it back in the ready queue)
  void IncGlobalTime();                 // incerements the globalTime variable and calls WakeUp
 private:
  List *holdingQueue;                   // holds the threads that are sleeping
  int globalTime;                       // keeps track of the current system time
};


#endif
