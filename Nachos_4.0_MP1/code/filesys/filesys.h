// filesys.h 
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system. 
//	The "STUB" version just re-defines the Nachos file system 
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.
//
//	The other version is a "real" file system, built on top of 
//	a disk simulator.  The disk is simulated using the native UNIX 
//	file system (in a file named "DISK"). 
//
//	In the "real" implementation, there are two key data structures used 
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.  
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized. 
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "sysdep.h"
#include "openfile.h"
#include "debug.h"

typedef int OpenFileId;
#ifdef FILESYS_STUB 			// Temporarily implement file system calls as 
//#ifdef FILESYS				// calls to UNIX, until the real file system
								// implementation is available
class FileSystem {
  public:
	//OpenFile* now_open = NULL;

    FileSystem() { for (int i = 0; i < 20; i++) fileDescriptorTable[i] = NULL; }

    bool Create(char *name) {
    	int fileDescriptor = OpenForWrite(name);

		DEBUG(dbChanwei, "Create in filesys.h ok!");
    	if (fileDescriptor == -1) return FALSE;
    	Close(fileDescriptor);
    	return TRUE;
    }

    OpenFile* Open(char *name) {
      int fileDescriptor = OpenForReadWrite(name, FALSE);

      if (fileDescriptor == -1) return NULL;
      return new OpenFile(fileDescriptor);
      }

	OpenFileId Open_File(char *name){
		int fileDescriptor = OpenForReadWrite(name, FALSE);
		int open_id;
		if (fileDescriptor == -1){
			open_id = -1;
		}
		else {
			open_id = fileDescriptor;
		}
		DEBUG(dbChanwei, "Open_File in filesys.h ok!");
		return open_id;
	}

	int Write(char *buffer, int size, OpenFileId id){
		int write_success;
		if(id >= 0){
			WriteFile(id, buffer, size);
			DEBUG(dbChanwei, "Write in filesys.h ok!");
			write_success = size;
		}
		else{
			write_success = -1; // write in failed
		}
		return write_success;
	}

	int Readfile(char *buffer, int size, OpenFileId id){
		int read_success;
		if(id >= 0){
        	Read(id, buffer, size);
        	DEBUG(dbChanwei, "Read in filesys.h ok!");
        	read_success = size;
		}
		else{
			read_success = -1; // read file failed
		}
		return read_success;
     }

	int close(OpenFileId id){
		int success;
		if(id<=0){
			success = 0;
		}
		else{
			success = 1;
			Close(id);
		}	
		return success;
	}

    bool Remove(char *name) { return Unlink(name) == 0; }

    OpenFile *fileDescriptorTable[20];

};


#else // FILESYS
class FileSystem {
  public:
    FileSystem(bool format);		// Initialize the file system.
					// Must be called *after* "synchDisk" 
					// has been initialized.
    					// If "format", there is nothing on
					// the disk, so initialize the directory
    					// and the bitmap of free blocks.

    bool Create(char *name, int initialSize);  	
					// Create a file (UNIX creat)

    OpenFile* Open(char *name); 	// Open a file (UNIX open)
	//OpenFileId FindId(char *name);


    bool Remove(char *name);  		// Delete a file (UNIX unlink)

    void List();			// List all the files in the file system

    void Print();			// List all the files and their contents

	OpenFile* now_openFile;
  private:
   OpenFile* freeMapFile;		// Bit map of free disk blocks,
					// represented as a file
   OpenFile* directoryFile;		// "Root" directory -- list of 
					// file names, represented as a file
};
#endif // FILESYS

#endif // FS_H
