// scheduler.h 
//	Data structures for the thread dispatcher and scheduler.
//	Primarily, the list of threads that are ready to run.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "copyright.h"
#include "list.h"
#include "thread.h"
#include "callback.h"

// The following class defines the scheduler/dispatcher abstraction -- 
// the data structures and operations needed to keep track of which 
// thread is running, and which threads are ready but not running.

class SchedulerIntHandler : public CallBackObj {
  public:
    void CallBack();
    void Schedule(int time);
};

class Scheduler {
  public:
    Scheduler();		// Initialize list of ready threads 
    ~Scheduler();		// De-allocate ready list

    void ReadyToRun(Thread* thread);	
    				// Thread can be dispatched.
    Thread* FindNextToRun();	// Dequeue first thread on the ready 
				// list, if any, and return thread.
    void Run(Thread* nextThread, bool finishing);
    				// Cause nextThread to start running
    void CheckToBeDestroyed();// Check if thread that had been
    				// running needs to be deleted
    void Print();		// Print contents of ready list
    
    // SelfTest for scheduler is implemented in class Thread

	// Chanwei add
	void CheckAndMove(Thread* t, int oldPriority);
	void UpdateBurstTime(Thread *t, int currentTime);
	void Aging(List<Thread *> *list);	// aging mechanism
    void CallBack();
	// end Chanwei add
	void RemoveFromQueue(Thread* t, int level);    

  private:
    List<Thread *> *readyList;  // queue of threads that are ready to run,
				// but not running
    Thread *toBeDestroyed;	// finishing thread to be destroyed
    				// by the next thread that runs

	// Chanwei add
	void InsertToQueue(Thread* t, int level);
    //void RemoveFromQueue(Thread* t, int level);
	SchedulerIntHandler* intHandler;
    SortedList<Thread *> *SJF_ReadyList;// ready list for SJF
    SortedList<Thread *> *PJ_ReadyList;	// ready list for priority 
    List<Thread *> *RR_ReadyList;		// ready list for Round robin
	// end Chanwei add
};

#endif // SCHEDULER_H
