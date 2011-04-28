/*
 * Copyright 1997 by the Massachusetts Institute of Technology
 * For distribution and copying rights, see the file "mit-copyright.h"
 *
 * History:
 *   03-03-97  dalmeida  created.
 */

/*
This are the basic steps preformed by pioctl on NT (as reverse-engineered
from debugging fs):

 1) Check if valid drive and if REMOTE
 GetCurrentDirectory to get the drive letter (_getdrive in CRT) from 
 char before ':' (when ':' present).
 GetLogicalDriveStrings to see if valid drive.
 GetDriveType to see if remote (DRIVE_REMOTE)

 2) Get handle to "z:\_._AFS_IOCTL_._"  (GetIoctlHandle in Transarc code)

 Get drive letter from path.  Otherwise, can just use \_._AFS_IOCTL_._, 
 right?

 3) InitFS(REQ_TYPE *rp)

 4) MarshallLong(REQ_TYPE *rp, int opcode)

 Puts opcode at beginning of data with a memcpy based on mp,
 updates mp to point past new opcode data.

 5) fsGetFullPath(relpath, fullpath, buffersize)

 Gets full path from relative path (to pass into ioctl)
 Checks for ':' then '/' or '\'
 The fullpath is just the afs path minus root part...
 (ie. z:\foobob\baz -> foobob\baz OR \foobob\baz (can't remember which))
 If no pathname given, return "\"

 6) Marshall the full path -> MarshallString(&req, fullpath)

 7) If ioctl struct has insize data, then add it to the rp data

 NOTE: rp->mp points to end of rp->data
       rp->nbytes has 0
       rp->data has [IOCTL ID] + [ZERO-TERMINATED FULL PATH] + [IN DATA]

 8) Send off the request into the iocl handle

 NOTE: rp->data has [ERROR CODE] + [OUT DATA]

 9) Extract error code

 10) Gets rest of data...

 reduce nbytes by 4...to get rid of int error code...
 First, check if pvi->out_size >= nbytes, else barf...
 Then, get data from rp->mp, copy rp->nbytes of it to pvi->out...
 Next, set pvi->out_size to rp->nbytes
 Close handle and return

 */

#include <windows.h>
#include "pioctl.h"

#define AFS_IOCTL_NAME "_._AFS_IOCTL_._"

#define MAX_REQ_SIZE 8192

typedef struct req_type {
    char *mp;
    int nbytes;
    char data[2048];
} REQ_TYPE;


HANDLE GetIoctlHandle(char *pszPath)
{
    char pszIoctlPath[MAX_PATH];
    char pDrive[MAX_PATH];

    ZeroMemory(pDrive, sizeof(pDrive));
    pDrive[1] = ':';
    pDrive[2] = '\\';

	if (pszPath && pszPath[0] == '\\' && pszPath[1] == '\\') {
		char *drivep;
		int nUncRoot;

		drivep = strchr(pszPath + 2, '\\');
		if(drivep) /* \\foo\bar... : foo could be null */
		{
			drivep = strchr(drivep + 1, '\\');
			if(drivep) /* \\foo\bar\... */
			{
				nUncRoot = (drivep - pszPath);
				strncpy( pDrive, pszPath, nUncRoot );
				pDrive[nUncRoot] = 0;
			}
			else /* \\foo\bar */
			{
				strcpy( pDrive, pszPath );
			}
		}
		else /* \\foo : is invalid and will fail later */
		{
			strcpy( pDrive, pszPath );
		}
		strcat(pDrive,"\\");
	}
	else if (!pszPath || pszPath[1] != ':') {
        GetCurrentDirectory(sizeof(pszIoctlPath), pszIoctlPath);
        if (pszIoctlPath[1] == ':')
            pDrive[0] = pszIoctlPath[0]; // Otherwise use NULL
        else {
            pDrive[0] = '\\';
            pDrive[1] = 0;
        }
    } else {
        pDrive[0] = pszPath[0];
    }

    if (GetDriveType(pDrive) != DRIVE_REMOTE)
        return INVALID_HANDLE_VALUE;

    strcpy(pszIoctlPath, pDrive);
    strcat(pszIoctlPath, AFS_IOCTL_NAME);
    return CreateFile(pszIoctlPath,
                      GENERIC_READ | GENERIC_WRITE, // dwDesiredAccess
                      0,                            // dwShareMode
                      NULL,                         // lpSecurityAttributes
                      OPEN_EXISTING,                // dwCreationDistribution
                      FILE_FLAG_NO_BUFFERING,       // dwFlagsAndAttributes
                      NULL);                        // hTemplateFile
}

void
InitFS(REQ_TYPE *rp)
{
    rp->mp = rp->data;
    rp->nbytes = 0;
}

void
MarshallLong(REQ_TYPE *rp, int opcode)
{
    CopyMemory(rp->mp, &opcode, sizeof(long));
    rp->mp += sizeof(long);
}

void
MarshallString(REQ_TYPE *rp, char *string)
{
    CopyMemory(rp->mp, string, strlen(string) + 1);
    rp->mp += strlen(string) + 1;
}

void
MarshallIoctl(REQ_TYPE *rp, struct ViceIoctl *pvi)
{
    CopyMemory(rp->mp, pvi->in, pvi->in_size);
    rp->mp += pvi->in_size;
}

void
UnMarshallLong(REQ_TYPE *rp, int *pi)
{
    CopyMemory(pi, rp->mp, sizeof(int));
    rp->mp += sizeof(int);
}

int
fsGetFullPath(char *relpath,
              char *fullpath,
              size_t buffersize)
{
    char current[MAX_PATH];

	/* hanlde unc paths first */
	if (relpath && relpath[0]=='\\' && relpath[1]=='\\') {
		char *uncPart;
		uncPart = strchr(relpath+2, '\\');
		if(uncPart) {
			uncPart = strchr(uncPart + 1, '\\');
			if(uncPart)
				strcpy(fullpath, uncPart + 1);
			else
				*fullpath=0;
		}
		else
			*fullpath = 0;

		return 0;
	}
    else if (!relpath || !relpath[0] || 
        (relpath[1] != ':' && relpath[0] != '/' && relpath[0] != '\\')) {
        GetCurrentDirectory(sizeof(current), current);
        if (current[1] == ':') {
            if ((strlen(current)-2+strlen(relpath)) < buffersize) {
                strcpy(fullpath, current+2);
                strcat(fullpath, relpath);
                return 0;
            }
        }
    }
    else if (relpath[0] == '/' || relpath[0] == '\\') {
        if (strlen(relpath) < buffersize) {
            strcpy(fullpath, relpath);
            return 0;
        }
    }
    else if (relpath[1] == ':') {
        if ((strlen(relpath)-2) < buffersize) {
            strcpy(fullpath, relpath+2);
            return 0;
        }
    }
    return -1;
}

int
Transceive(HANDLE handle, REQ_TYPE *preq)
{
    int rcount = preq->mp - preq->data;
    int resultcount;
    if (!rcount)
        return ERROR_SUCCESS;
    if(!WriteFile(handle, preq->data, rcount, &resultcount, 0))
        return GetLastError();
    if (!ReadFile(handle, preq->data, MAX_REQ_SIZE, &resultcount, 0))
        return GetLastError();
    preq->nbytes = resultcount;
    preq->mp = preq->data;
    return ERROR_SUCCESS;
}

int
pioctl(char *path,
       int id,
       struct ViceIoctl *pvio,
       int follow)
{
    int status;
    char fullpath[MAX_PATH];
    REQ_TYPE req;
    HANDLE hIoctl = GetIoctlHandle(path);

    if (hIoctl == INVALID_HANDLE_VALUE) {
        CloseHandle(hIoctl);
        return GetLastError();
    }
    InitFS(&req);
    MarshallLong(&req, id);
    if (fsGetFullPath(path, fullpath, sizeof(fullpath))) {
        CloseHandle(hIoctl);
        return -1;
    }
    MarshallString(&req, fullpath);
    if (pvio->in_size)
        MarshallIoctl(&req, pvio);
    status = Transceive(hIoctl, &req);
    CloseHandle(hIoctl);
    if (status != ERROR_SUCCESS) {
        // Could do fancier errno stuff in here.
        return status;
    }
    UnMarshallLong(&req, &status);
	// if status != 0, then the rest might not be relevant
	if(status)
		return status;

    // Get rest of stuff...
    req.nbytes -= 4;
    if (pvio->out_size < req.nbytes)
        return -1;
    CopyMemory(pvio->out, req.mp, req.nbytes);
    pvio->out_size = req.nbytes;
    return status;
}
