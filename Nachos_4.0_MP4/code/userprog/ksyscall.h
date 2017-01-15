/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"

#include "synchconsole.h"


void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

#ifdef FILESYS_STUB
int SysCreate(char *filename)
{
	// return value
	// 1: success
	// 0: failed
	return kernel->interrupt->CreateFile(filename);
}
#endif

int SysCreate(char *filename, int size)
{
    // return value
    // 1: success
    // 0: failed
	return kernel->interrupt->CreateFile(filename, size);
}

OpenFileId SysOpen(char *filename)
{
	return kernel->interrupt->OpenFile(filename);
}

int SysWrite(char *buffer, int size, OpenFileId id)
{
    //DEBUG(dbChanwei,"SysWrite in ksyscall.h ok!");
    return kernel->interrupt->WriteFile(buffer,size,id);
}

int SysRead(char *buffer, int size, OpenFileId id)
{
    //DEBUG(dbChanwei,"SysRead in ksyscall.h ok!");
    return kernel->interrupt->ReadFile(buffer,size,id);
}

int SysClose(OpenFileId id){
    //DEBUG(dbChanwei,"SysClose in ksyscall.h ok!");
    return kernel->interrupt->CloseFile(id);
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
