// scheduler.cc 
//	Routines to choose the next thread to run, and to dispatch to
//	that thread.
//
// 	These routines assume that interrupts are already disabled.
//	If interrupts are disabled, we can assume mutual exclusion
//	(since we are on a uniprocessor).
//
// 	NOTE: We can't use Locks to provide mutual exclusion here, since
// 	if we needed to wait for a lock, and the lock was busy, we would 
//	end up calling FindNextToRun(), and that would put us in an 
//	infinite loop.
//
// 	Very simple implementation -- no priorities, straight FIFO.
//	Might need to be improved in later assignments.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "debug.h"
#include "scheduler.h"
#include "main.h"

#define AGING 10
// define aging increase by 10
//#define REINSERT(LIST, t) LIST ## _ReadyList->Remove(t); LIST ## _ReadyList->Insert(t)
//#define REAPPEND(LIST, t) LIST ## _ReadyList->Remove(t); LIST ## _ReadyList->Append(t)


//----------------------------------------------------------------------
// Scheduler::Scheduler
// 	Initialize the list of ready but not running threads.
//	Initially, no ready threads.
//----------------------------------------------------------------------

// Chanwei add
int SJF(Thread *a, Thread *b){
	double ta = a->getBurstTime(), tb = b->getBurstTime();
	if(ta == tb)
		return a->getID() < b->getID() ? -1 : 1;
		// if burst time are the same, compare the ID and return it
	return ta < tb ? -1 : 1;
}

int Priority_Job(Thread *a, Thread *b)
{
    int pa = a->getPriority(), pb = b->getPriority();
    if(pa == pb)
        return a->getID() < b->getID() ? -1 : 1;
		// simply compare its ID
	return pa > pb ? -1 : 1;
}

Scheduler::Scheduler()
{
    readyList = new List<Thread *>;
    // Chanwei add
    intHandler = new SchedulerIntHandler();
    SJF_ReadyList = new SortedList<Thread *>(SJF);
    PJ_ReadyList = new SortedList<Thread *>(Priority_Job);
    RR_ReadyList = new List<Thread *>;
    // end Chanwei add
    toBeDestroyed = NULL;
}
// end Chanwei add


//----------------------------------------------------------------------
// Scheduler::~Scheduler
// 	De-allocate the list of ready threads.
//----------------------------------------------------------------------

Scheduler::~Scheduler()
{ 
    delete readyList; 
	// Chanwei add
	delete SJF_ReadyList;
	delete PJ_ReadyList;
	delete RR_ReadyList;
	// end Chanwei add
} 

//----------------------------------------------------------------------
// Scheduler::ReadyToRun
// 	Mark a thread as ready, but not running.
//	Put it on the ready list, for later scheduling onto the CPU.
//
//	"thread" is the thread to be put on the ready list.
//----------------------------------------------------------------------

void
Scheduler::ReadyToRun (Thread *thread)
{
    ASSERT(kernel->interrupt->getLevel() == IntOff);
    DEBUG(dbgThread, "Putting thread on ready list: " << thread->getName());
    
	// Chanwei add
	Thread *curThread = kernel->currentThread;
	int currentTime = kernel->stats->totalTicks;
    int p = thread->getPriority();
	thread->setStatus(READY);
    thread->setReadyTime(currentTime);

	if(p >= 100){
    	// L1 queue (SJF with preemptive)
		InsertToQueue(thread, 1);
    } 
	else if(p >= 50){
        // L2 queue
        InsertToQueue(thread, 2);
    } 
	else{
        // L3 queue
        InsertToQueue(thread, 3);
    }
	// end Chanwei add
	
}

//----------------------------------------------------------------------
// Scheduler::FindNextToRun
// 	Return the next thread to be scheduled onto the CPU.
//	If there are no ready threads, return NULL.
// Side effect:
//	Thread is removed from the ready list.
//----------------------------------------------------------------------

Thread *
Scheduler::FindNextToRun ()
{
    ASSERT(kernel->interrupt->getLevel() == IntOff);
	int currentTime = kernel->stats->totalTicks;
    Thread* t = NULL;

	// Chanwei comment and add
	
	Aging(SJF_ReadyList);
    Aging(PJ_ReadyList);
    Aging(RR_ReadyList);

	if(!(SJF_ReadyList->IsEmpty())){
        t = SJF_ReadyList->RemoveFront();
		cout << "Tick [" << currentTime << "] : Thread " << t->getID() << " is removed from queue L1" << endl;
    } 
	else if(!(PJ_ReadyList->IsEmpty())){
        t = PJ_ReadyList->RemoveFront();
		cout << "Tick [" << currentTime << "] : Thread " << t->getID() << " is removed from queue L2" << endl;
	} 
	else if(!RR_ReadyList->IsEmpty()){
        t = RR_ReadyList->RemoveFront();
    	cout << "Tick [" << currentTime << "] : Thread " << t->getID() << " is removed from queue L3" << endl;
	}

    return t;
	// end Chanwei add
}

//----------------------------------------------------------------------
// Scheduler::Run
// 	Dispatch the CPU to nextThread.  Save the state of the old thread,
//	and load the state of the new thread, by calling the machine
//	dependent context switch routine, SWITCH.
//
//      Note: we assume the state of the previously running thread has
//	already been changed from running to blocked or ready (depending).
// Side effect:
//	The global variable kernel->currentThread becomes nextThread.
//
//	"nextThread" is the thread to be put into the CPU.
//	"finishing" is set if the current thread is to be deleted
//		once we're no longer running on its stack
//		(when the next thread starts running)
//----------------------------------------------------------------------

void
Scheduler::Run (Thread *nextThread, bool finishing)
{
    Thread *oldThread = kernel->currentThread;
    
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    if (finishing) {	// mark that we need to delete current thread
         ASSERT(toBeDestroyed == NULL);
	 toBeDestroyed = oldThread;
    }
    
    if (oldThread->space != NULL) {	// if this thread is a user program,
        oldThread->SaveUserState(); 	// save the user's CPU registers
	oldThread->space->SaveState();
    }
    
    oldThread->CheckOverflow();		    // check if the old thread
					    // had an undetected stack overflow

	// Chanwei add
	int currentTime = kernel->stats->totalTicks;
	int executionTime = currentTime - oldThread->getStartTime();

	if(!oldThread->isSleep()){	// if didn't sleep, calculate execution time here
		oldThread->setExecutionTime(executionTime);
	}

    if(oldThread->isPreempted()) {
        oldThread->addLastBurst(executionTime);
        oldThread->resetPreempt();
    }
	
	nextThread->setStartTime(currentTime);
	kernel->currentThread = nextThread;  // switch to the next thread
    nextThread->setStatus(RUNNING);      // nextThread is now running
    
	cout << "Tick [" << currentTime << "] : Thread " << nextThread->getID() << " is now selected for execution" << endl;
	if(nextThread->getPriority() >= 100)		
		//cout << "Thread " << nextThread->getID() << " Burst: " << nextThread->getBurstTime() << endl;
	cout << "Tick [" << currentTime << "] : Thread " << oldThread->getID() << " is replaced, and it has executed " << oldThread->getExecutionTime() << " ticks" << endl;	
	oldThread->resetSleep();
	// end Chanwei add
	
    DEBUG(dbgThread, "Switching from: " << oldThread->getName() << " to: " << nextThread->getName());
    
    // This is a machine-dependent assembly language routine defined 
    // in switch.s.  You may have to think
    // a bit to figure out what happens after this, both from the point
    // of view of the thread and from the perspective of the "outside world".

    SWITCH(oldThread, nextThread);

    // we're back, running oldThread
      
    // interrupts are off when we return from switch!
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    DEBUG(dbgThread, "Now in thread: " << oldThread->getName());

    CheckToBeDestroyed();		// check if thread we were running
					// before this one has finished
					// and needs to be cleaned up
    
    if (oldThread->space != NULL) {	    // if there is an address space
        oldThread->RestoreUserState();     // to restore, do it.
	oldThread->space->RestoreState();
    }
}

//----------------------------------------------------------------------
// Scheduler::CheckToBeDestroyed
// 	If the old thread gave up the processor because it was finishing,
// 	we need to delete its carcass.  Note we cannot delete the thread
// 	before now (for example, in Thread::Finish()), because up to this
// 	point, we were still running on the old thread's stack!
//----------------------------------------------------------------------

void
Scheduler::CheckToBeDestroyed()
{
    if (toBeDestroyed != NULL) {
        delete toBeDestroyed;
	toBeDestroyed = NULL;
    }
}
 
//----------------------------------------------------------------------
// Scheduler::Print
// 	Print the scheduler state -- in other words, the contents of
//	the ready list.  For debugging.
//----------------------------------------------------------------------
void
Scheduler::Print()
{
    cout << "Ready list contents:\n";
    readyList->Apply(ThreadPrint);
}

// Following functions are added by Chanwei

//----------------------------------------------------------------------
// Scheduler::Aging
//  if the thread wait for more than 1500 ticks, increase priority with 10
//----------------------------------------------------------------------  

void
Scheduler::Aging(List<Thread *> *list)
{
    ListIterator<Thread *> *iterator = new ListIterator<Thread *>(list);
    for( ; !(iterator->IsDone()); iterator->Next()) {
        int currentTime = kernel->stats->totalTicks;
        Thread* t = iterator->Item();
        if((currentTime - t->getReadyTime()) >= 1500){
            int old = t->getPriority();
            t->Aging(AGING);// priority increase 10
            t->setReadyTime(currentTime); // reset time ticks.
            cout << "Tick [" << currentTime << "] : Thread " << t->getID() << " changes its priority from "<< old << " to " << t->getPriority() << endl;
			CheckAndMove(t, old);
        }
    }
}

//----------------------------------------------------------------------
// Scheduler::InsertToQueue
//  insert process to queue
//-----------------------------------------------------------------------

void
Scheduler::InsertToQueue(Thread* t, int level)
{
    int currentTime = kernel->stats->totalTicks;
    if(level == 1){
        SJF_ReadyList->Insert(t);
    } 
	else if(level == 2){
        PJ_ReadyList->Insert(t);
    } 
	else if(level == 3){
        RR_ReadyList->Append(t);
		// for Round Robin, just simply append thread to the end of the list
    }
	cout << "Tick [" << currentTime << "] : Thread " << t->getID() << " is inserted into queue L" << level << endl;
}

//----------------------------------------------------------------------
// Scheduler::RemoveFromQueue
// Remove process from queue
//----------------------------------------------------------------------
void
Scheduler::RemoveFromQueue(Thread* t, int level)
{
    int currentTime = kernel->stats->totalTicks;
    if(level == 1){
        SJF_ReadyList->Remove(t);
    } 
	else if(level == 2){
        PJ_ReadyList->Remove(t);
    } 
	else if(level == 3){
        RR_ReadyList->Remove(t);
    }
	cout << "Tick [" << currentTime << "] : Thread " << t->getID() << " is removed from queue L" << level << endl;
}

void
Scheduler::CheckAndMove(Thread* t, int oldPriority)
{
    int p = t->getPriority();
    if(oldPriority < 50 && p >= 50){
        RemoveFromQueue(t, 3);
		ReadyToRun(t);
    } 
	else if(oldPriority < 100 && p >= 100){
        RemoveFromQueue(t, 2);
		ReadyToRun(t);
    } 
}

void
Scheduler::UpdateBurstTime(Thread *t, int currentTime)
{
	int preBurst = t->getPreBurst();
    int executionTime = currentTime - t->getStartTime() ;
	double newBurst = (executionTime + preBurst) / 2;
	
    t->setBurstTime(newBurst);
    t->resetLastBurst();
	cout << "Tick [" << currentTime << "] : Thread " << t->getID() << " has nextBurst : " << t->getBurstTime() << endl;
}

void
SchedulerIntHandler::CallBack()
{
    kernel->interrupt->YieldOnReturn();
}

void
SchedulerIntHandler::Schedule(int time)
{
    kernel->interrupt->Schedule(this, time, SwitchInt);
}
