// This program only needs the essential windows header files.
#define WIN32_LEAN_AND_MEAN 1

#include <windows.h>
#include <tlhelp32.h>
 
#include <winperf.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
//#include <pdh.h>

#if defined(_WIN32)
  extern OSVERSIONINFO myInfo;
#else 
  #error This source is for Win32 only!
#endif

// if this 32-bit DLL is being thunked to from a  16-bit executable,
// we cannot be a child thread off it.  therefore,
// every OLDWINSLEEPS times through the process/module search loop,
// we want to Sleep(0).
#define OLDWINSLEEPS 16

#define KVIEW16NAME "KVIEW"
#define KVIEW32NAME "KVIEW32"
#define KVIEWNAME KVIEW32NAME

#define KVIEW16GONAME "KVIEW"
#define KVIEW32GONAME "KVIEW32"
#define KVIEWGONAME KVIEW32GONAME



/* hey, why do I use GetProcAddress instead of direct references? */
/* so I can use the same binary for Win95 and NT.  if I have hard */
/* links to toolhelp32 functions, this won't load under NT. */

typedef HANDLE (WINAPI *PCT32S) (DWORD dwFlags, DWORD th32ProcessID);
typedef BOOL (WINAPI *PP32F)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef BOOL (WINAPI *PP32N)(HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef BOOL (WINAPI *PM32F)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
typedef BOOL (WINAPI *PM32N)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);


/* TOTALBYTES, BYTEINCREMENT, and the two LPSTR's after that */
/* are to support the code later on that navigates the NT    */
/* performance information. */
 
#define TOTALBYTES    8192
#define BYTEINCREMENT 1024

static char shorty[1] = {'\0'};

static LPSTR *lpNamesArray = NULL;
static LPSTR lpNameStrings = NULL;

// indicates what extension the given filename has
enum {eNoDot, eIsExe, eIsDll, eOtherHasDot};
// eNoDot : the filename does not include an extension
// eIsExe : the filename ends with .exe
// eIsDll : the filename ends with .dll
// eOtherHasDot : the filename ends with a dot followed by some
//                other extension


/* forward references */

/* for NT, makes a list of ID's of currently-loaded processes. */
LPDWORD MakeProcIDArray(void);

BOOL FindEitherAppNT(LPCSTR sFirst, LPCSTR sSecond, BOOL bCanBeSame);



// return value tells what kind of filename we have; based either on
// the string following the backslash, or the whole string if there
// is no backslash.
WORD DetermineExtension(LPCSTR sString)
{
	LPSTR sSlash;
	LPSTR sDot;

	if (sString == NULL)
		return eNoDot;
	
	// find last backslash if any
	sSlash = strrchr(sString,'\\');

	// find last dot if any
	sDot = strrchr(sString,'.');
	
	// if there is no dot, we have eNoDot.
	if (sDot == NULL)
		return eNoDot;

	// if there is a backslash and the dot is before it, we have
	// eNoDot.
	if (sSlash != NULL && sDot < sSlash)
		return eNoDot;

	// check for .exe or .dll; otherwise it's eOtherHasDot
	if (0 == stricmp(sDot,".exe"))
		return eIsExe;
	if (0 == stricmp(sDot,".dll"))
		return eIsDll;

	return eOtherHasDot;
}




/* do a comparison between two filename fragments.  use the      */
/* information from the extra arguments, which was determined in */
/* advance. returns TRUE if the fragments compare sufficiently   */
/* equal, FALSE if not.*/
BOOL StrangeCompare(LPCSTR sOne, WORD wOneKind, WORD wOneLength,
					LPCSTR sTwo, WORD wTwoKind)
{
	LPSTR sOtherDot;
	WORD wOtherLength;
	WORD wTwoLength;
	
	if (wOneKind != eNoDot && wTwoKind != eNoDot)
	{
		/* if neither one is a NoDot, they both have to be the same kind */
		if (wOneKind != wTwoKind)
			return FALSE;
		/* and they have to be exactly equal, ignoring case */
		return (0 == stricmp(sOne,sTwo));
	}
	
	/* if they're BOTH NoDot, they ALSO have to be exactly equal, ignoring case */
	if (wOneKind == eNoDot && wTwoKind == eNoDot)
		return (0 == stricmp(sOne,sTwo));

	/* now we have one that's NoDot, and one that isn't. the NoDot
	   has to match everything before the other one's dot ... or it is not.
	   got? */

	if (wOneKind == eNoDot)
	{
		sOtherDot = strrchr(sTwo,'.');
		wTwoLength = (WORD)(sOtherDot - sTwo);
		if (wOneLength != wTwoLength)
			return FALSE;
		return (0 == strnicmp(sOne,sTwo,wTwoLength));
	}
	else
	{
		sOtherDot = strrchr(sOne,'.');
		wOtherLength = (WORD)(sOtherDot - sOne);
		wTwoLength = strlen(sTwo);
		if (wOtherLength != wTwoLength)
			return FALSE;
		return (0 == strnicmp(sOne,sTwo,wTwoLength));
	}
}




/* returns TRUE if either of the module names is in use from any */
/* process on the system.  can be used only from Win 95 or       */
/* Windows NT.  if third parameter is TRUE, the module can be    */
/* from the current process.  if FALSE, it must not be from the  */
/* current process. */

BOOL FindEitherModule(LPCSTR sFirst, LPCSTR sSecond, BOOL bCanBeSame)
{
	DWORD thisProcID;			/* id of the current proc */
	DWORD tempProcID;			/* id of other proc we're inspecting */
	HANDLE tProcSnapshotHandle;		/* all you processes: say "cheese!" */
	HANDLE tModuleSnapshotHandle;	/* all you modules: say "cheese!" */
	HINSTANCE hTH32Lib;			/* handle of toolhelp32 library */
	PROCESSENTRY32 myProcEntry;		/* iterate through the processes */
	MODULEENTRY32 myModuleEntry;	/* iterate through the modules */
	BOOL bRetVal;
	BOOL bSixteen;		/* is current proc 16-bit (thunked to call this)? */
	PCT32S pCreateToolhelp32Snapshot;	/* these five are proc pointers */
	PP32F pProcess32First;
	PP32N pProcess32Next;
	PM32F pModule32First;
	PM32N pModule32Next;
	LPDWORD lpProcIDArray;			/* array of all proc ID's */
	DWORD iterate;
	DWORD tProcessVersion;
	WORD wFirstKind;	/* does this string contain extension? what kind? */
	WORD wSecondKind;
	WORD wOtherKind;
	WORD wFirstLength;
	WORD wSecondLength;
	LPSTR sExeName;		/* temp pointer to exe name of iterated module */

	if (sFirst == NULL && sSecond == NULL)
		return FALSE;

	thisProcID = GetCurrentProcessId();
	// are we running in a 16-bit or 32-bit process??
	tProcessVersion = GetProcessVersion(0);
	bSixteen = FALSE;
	if ( ((tProcessVersion >> 16) & 0x0000ffff) == 3 &&
		 (tProcessVersion & 0x0000ffff) < 95 )
		bSixteen = TRUE;

#if defined(_DEBUG)
		OutputDebugString("looking a\n");
		OutputDebugString(sFirst);
		OutputDebugString(sSecond);
		MessageBox(NULL,"meep","KClien32",MB_OK | MB_ICONINFORMATION);
#endif

	if (myInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	/**************** the Windows 95 way ****************/
	{
		/* algorithm: use toolhelp32.  take a snapshot, then iterate     */
		/* through all the processes.  for each process, iterate through */
		/* all the modules to  determine if it is using one of the target*/
		/* dll's */

#if defined(_DEBUG)
		OutputDebugString("looking b\n");
#endif

		hTH32Lib = LoadLibrary("KERNEL32.DLL");
		if (hTH32Lib == NULL)
			return FALSE;

#if defined(_DEBUG)
		OutputDebugString("looking c\n");
#endif
		pCreateToolhelp32Snapshot = (PCT32S)GetProcAddress(hTH32Lib,"CreateToolhelp32Snapshot");
		pProcess32First = (PP32F)GetProcAddress(hTH32Lib,"Process32First");
		pProcess32Next = (PP32N)GetProcAddress(hTH32Lib,"Process32Next");
		pModule32First = (PM32F)GetProcAddress(hTH32Lib,"Module32First");
		pModule32Next = (PM32N)GetProcAddress(hTH32Lib,"Module32Next");

		if ( (pCreateToolhelp32Snapshot == 0) ||
			 (pProcess32First == 0) || (pProcess32Next == 0) ||
			 (pModule32First == 0) || (pModule32Next == 0) )
		{
			FreeLibrary(hTH32Lib);
			return FALSE;
		}

		tProcSnapshotHandle = pCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (tProcSnapshotHandle == 0)
		{
			FreeLibrary(hTH32Lib);
			return FALSE;
		}

		wFirstKind = DetermineExtension(sFirst);
		wSecondKind = DetermineExtension(sSecond);
		wFirstLength = strlen(sFirst);
		wSecondLength = strlen(sSecond);

		tModuleSnapshotHandle = 0;

		bRetVal = FALSE;

		myProcEntry.dwSize = sizeof(PROCESSENTRY32);
		myModuleEntry.dwSize = sizeof(MODULEENTRY32);

#if defined(_DEBUG)
		OutputDebugString("looking d\n");
#endif

		iterate = 0;

		if (pProcess32First(tProcSnapshotHandle,&myProcEntry))
			do {
				if ( (++iterate % OLDWINSLEEPS) == 0 && bSixteen)
					Sleep(0);
				tempProcID = myProcEntry.th32ProcessID;
				if (bCanBeSame || thisProcID != tempProcID)
				{
					sExeName = NULL;
					if (sFirst != NULL && (wFirstKind == eNoDot || wFirstKind == eIsExe))
					{
						if (NULL == (sExeName = strrchr(myProcEntry.szExeFile,'\\')) )
							sExeName = myProcEntry.szExeFile;
						else
							sExeName++;	/* skip the slash */
#if defined(_DEBUG)
						OutputDebugString(sExeName);
#endif
						wOtherKind = DetermineExtension(sExeName);
						bRetVal = StrangeCompare(sFirst,wFirstKind,wFirstLength,
									sExeName,wOtherKind);
						if (bRetVal == TRUE)
						{
#if defined(_DEBUG)
							OutputDebugString("found\n");
#endif
							break;
						}
					}
					if (sSecond != NULL && (wSecondKind == eNoDot || wSecondKind == eIsExe))
					{
						if (sExeName == NULL)
						{
							if (NULL == (sExeName = strrchr(myProcEntry.szExeFile,'\\')) )
								sExeName = myProcEntry.szExeFile;
							else
								sExeName++;	/* skip the slash */
							wOtherKind = DetermineExtension(sExeName);
						}
						bRetVal = StrangeCompare(sSecond,wSecondKind,wSecondLength,
									sExeName,wOtherKind);
						if (bRetVal == TRUE)
						{
#if defined(_DEBUG)
							OutputDebugString("found\n");
#endif
							break;
						}
					}

					tModuleSnapshotHandle = pCreateToolhelp32Snapshot(TH32CS_SNAPMODULE, tempProcID);
					if (pModule32First(tModuleSnapshotHandle,&myModuleEntry))
						do {
							if ( (++iterate % OLDWINSLEEPS) == 0 && bSixteen)
								Sleep(0);

							if (NULL == (sExeName = strrchr(myModuleEntry.szModule,'\\')) )
								sExeName = myModuleEntry.szModule;
							else
								sExeName++;	/* skip the slash */
							wOtherKind = DetermineExtension(sExeName);
							
							if (sFirst != NULL)
								bRetVal = StrangeCompare(sFirst,wFirstKind,wFirstLength,
									sExeName,wOtherKind);

							if (bRetVal == FALSE && sSecond != NULL)
								bRetVal = StrangeCompare(sSecond,wSecondKind,wSecondLength,
									sExeName,wOtherKind);

							if (bRetVal == TRUE)
							{
								/* FOUND! */
#if defined(_DEBUG)
								OutputDebugString("found\n");
#endif
								break;
							}
#if defined(_DEBUG)
							else
								OutputDebugString(myModuleEntry.szModule);
#endif
							
						} while(pModule32Next(tModuleSnapshotHandle,&myModuleEntry));
				}
				if (TRUE == bRetVal)
					break;

			} while (pProcess32Next(tProcSnapshotHandle,&myProcEntry));

		if (tModuleSnapshotHandle != 0)
			CloseHandle(tModuleSnapshotHandle);

		CloseHandle(tProcSnapshotHandle);

		FreeLibrary(hTH32Lib);

		return bRetVal;
	}
	else
	/**************** the Windows NT way ****************/
	{
	/* algorithm:
		1) get the title database
		2) get the list of processes
		3) for each process:
		a) get the list of module names
		b) iterate through the module names for the process
		c) if it matches, you've found it
		4) free the data structures
	*/

		/* this does steps 1 through 3a */
		lpProcIDArray = MakeProcIDArray();

		if (lpProcIDArray == NULL)
			return FALSE;

		iterate = 0;

		while (lpProcIDArray[iterate] != 0)
		{
			if (bCanBeSame || thisProcID != lpProcIDArray[iterate])
			{
				// ??? aherm, i can't figure out how to do this yet
			}

			iterate++;
		}
	}
}


/* returns TRUE if either of the module names is the launch      */
/* module of any process on the system.  can be used only from   */
/* Win 95 or Windows NT.  if third parameter is TRUE, the module */
/* can be that of the current process.  if FALSE, it must not be */
/* from the current process.  if you wish to find a module even  */
/* if it is merely USED BY any process (i.e. as a DLL), then     */
/* call FindEitherModule. */

BOOL FindEitherApp(LPCSTR sFirst, LPCSTR sSecond, BOOL bCanBeSame)
{
	if (myInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
		return FindEitherModule(sFirst,sSecond,bCanBeSame);
	else
		return FindEitherAppNT(sFirst,sSecond,bCanBeSame);

}



/* determine whether it's necessary to load KView, and do so. */
void LoadKView(void)
{
	HANDLE tLoaderMutex;
	BOOL bGotIt;
	DWORD dWaitRet;

	bGotIt = FALSE;

	tLoaderMutex = CreateMutex(NULL, FALSE, "KClientLoadsKView");
	dWaitRet = WaitForSingleObject(tLoaderMutex,5000);
	if (dWaitRet == WAIT_ABANDONED || dWaitRet == WAIT_OBJECT_0)
		bGotIt = TRUE;

	if (!FindEitherApp(KVIEW16NAME,KVIEW32NAME,TRUE))
	{
		WinExec (KVIEWGONAME,  SW_MINIMIZE) ;
	}

	if (bGotIt)
	{
		ReleaseMutex(tLoaderMutex);
		bGotIt = FALSE;
	}
	CloseHandle(tLoaderMutex);
}



/* The following code is used when running under Windows NT.
   It finds out whether a particular module (EXE or DLL) is currently
   loaded by any process on the system.  This code has been modified
   from an example in the Win32 SDK docs. */

/*****************************************************************
 *                                                               *
 * Functions used to navigate through the performance data.      *
 *                                                               *
 *****************************************************************/

PPERF_OBJECT_TYPE FirstObject( PPERF_DATA_BLOCK PerfData )
{
    return( (PPERF_OBJECT_TYPE)((PBYTE)PerfData + 
        PerfData->HeaderLength) );
}

PPERF_OBJECT_TYPE NextObject( PPERF_OBJECT_TYPE PerfObj )
{
    return( (PPERF_OBJECT_TYPE)((PBYTE)PerfObj + 
        PerfObj->TotalByteLength) );
}

PPERF_INSTANCE_DEFINITION FirstInstance( PPERF_OBJECT_TYPE PerfObj )
{
    return( (PPERF_INSTANCE_DEFINITION)((PBYTE)PerfObj + 
        PerfObj->DefinitionLength) );
}

PPERF_INSTANCE_DEFINITION NextInstance( 
    PPERF_INSTANCE_DEFINITION PerfInst )
{
    PPERF_COUNTER_BLOCK PerfCntrBlk;

    PerfCntrBlk = (PPERF_COUNTER_BLOCK)((PBYTE)PerfInst + 
        PerfInst->ByteLength);

    return( (PPERF_INSTANCE_DEFINITION)((PBYTE)PerfCntrBlk + 
        PerfCntrBlk->ByteLength) );
}

PPERF_COUNTER_DEFINITION FirstCounter( PPERF_OBJECT_TYPE PerfObj )
{
    return( (PPERF_COUNTER_DEFINITION) ((PBYTE)PerfObj + 
        PerfObj->HeaderLength) );
}

PPERF_COUNTER_DEFINITION NextCounter( 
    PPERF_COUNTER_DEFINITION PerfCntr )
{
    return( (PPERF_COUNTER_DEFINITION)((PBYTE)PerfCntr + 
        PerfCntr->ByteLength) );
}

// returns a pointer to the (always) *UNICODE* name of the instance
LPWSTR InstanceName (PPERF_INSTANCE_DEFINITION pInst)
{
    if (pInst)
        return (LPWSTR) ((PCHAR) pInst + pInst->NameOffset);
    else
        return NULL;
}

/*****************************************************************
 *                                                               *
 * Throw out either of the name strings if they exist.           *
 *                                                               *
 *****************************************************************/

void KillNameStrings(LPSTR **lpNamesArray, LPSTR* lpNameStrings )
{
	if (lpNamesArray != NULL)
	{
		// this is just an array of pointers to the other one;
		// don't need to delete each pointer individually
		free(*lpNamesArray);
		*lpNamesArray = NULL;
	}

	if (lpNameStrings != NULL)
	{
		free(*lpNameStrings);
		lpNameStrings = NULL;
	}

}

/*****************************************************************
 *                                                               *
 * Load the counter and object names from the registry to the    *
 * global variable lpNamesArray.                                 *
 *                                                               *
 *****************************************************************/

void GetNameStrings(LPSTR **lpNamesArray, LPSTR* lpNameStrings )
{
    HKEY hKeyPerflib;      // handle to registry key
    HKEY hKeyPerflib009;   // handle to registry key
    DWORD dwMaxValueLen;   // maximum size of key values
    DWORD dwBuffer;        // bytes to allocate for buffers
    DWORD dwBufferSize;    // size of dwBuffer
    LPSTR lpCurrentString; // pointer for enumerating data strings
    DWORD dwCounter;       // current counter index
		
	// did we already make it?
	if (*lpNamesArray != NULL && *lpNameStrings != NULL)
		return;

	// well, there might be an error such that we only have one
	// of them, so delete any leftovers.
	KillNameStrings(lpNamesArray,lpNameStrings);

// Get the number of Counter items.

    RegOpenKeyEx( HKEY_LOCAL_MACHINE,
        "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Perflib",
        0,
        KEY_READ,
        &hKeyPerflib);

    dwBufferSize = sizeof(dwBuffer);

    RegQueryValueEx( hKeyPerflib,
        "Last Counter",
        NULL,
        NULL,
        (LPBYTE) &dwBuffer,
        &dwBufferSize );

    RegCloseKey( hKeyPerflib );

// Allocate memory for the names array.

    *lpNamesArray = malloc( (dwBuffer+2) * sizeof(LPSTR) );

	for (dwCounter = 0; dwCounter <= dwBuffer; dwCounter++)
		(*lpNamesArray)[dwCounter] = NULL;

	(*lpNamesArray)[dwBuffer+1] = shorty;	/* end marker */

// Open key containing counter and object names.

    RegOpenKeyEx( HKEY_LOCAL_MACHINE,
    "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Perflib\\009",
        0,
        KEY_READ,
        &hKeyPerflib009);

// Get the size of the largest value in the key (Counter or Help).

    RegQueryInfoKey( hKeyPerflib009,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        &dwMaxValueLen,
        NULL,
        NULL);

// Allocate memory for the counter and object names.

    dwBuffer = dwMaxValueLen + 1;

    *lpNameStrings = malloc( dwBuffer * sizeof(CHAR) );

// Read Counter value.

    RegQueryValueEx( hKeyPerflib009,
        "Counter",
        NULL,
        NULL,
        *lpNameStrings, &dwBuffer );

// Load names into an array, by index.

    for( lpCurrentString = *lpNameStrings; *lpCurrentString;
         lpCurrentString += (lstrlen(lpCurrentString)+1) )
    {
        dwCounter = atol( lpCurrentString );

        lpCurrentString += (lstrlen(lpCurrentString)+1);

        (*lpNamesArray)[dwCounter] = (LPSTR) lpCurrentString;
    }

}


/*****************************************************************
 *                                                               *
 * Find the index of an object or counter given its name.        *
 * GetNameStrings must already have been called.                 *
 *                                                               *
 *****************************************************************/

LONG GetCounterIndex(LPSTR theName,LPSTR* lpNamesArray)
{
	LONG theAnswer;

	for (theAnswer = 0; TRUE; theAnswer++)
	{
		if (lpNamesArray[theAnswer] != NULL)
		{
			if (lpNamesArray[theAnswer][0] == '\0')
				return -1;
			if (0 == stricmp(lpNamesArray[theAnswer],theName))
				return theAnswer;
		}
	}
}

/*****************************************************************
 *                                                               *
 * Add an element to the array of the ID's of all                *
 * current procs.  Returns FALSE if it fails.                    *
 *                                                               *
 *****************************************************************/

BOOL AddToProcIDArray(LPDWORD* lplpProcIDArray,LPDWORD lpSlotsInIDArray,
				  LPDWORD lpNextSlotInIDArray,DWORD newValue)
{
	LPDWORD lpNewArray;

	if (*lpNextSlotInIDArray == *lpSlotsInIDArray)
	{
		lpNewArray = (LPDWORD)realloc(*lplpProcIDArray,
						((*lpSlotsInIDArray)+50)*sizeof(DWORD));
		if (lpNewArray == NULL)
			return FALSE;
		*lplpProcIDArray = lpNewArray;
		*lpSlotsInIDArray += 50;
	}
	(*lplpProcIDArray)[*lpNextSlotInIDArray] = newValue;
	(*lpNextSlotInIDArray)++;
}


/*****************************************************************
 *                                                               *
 * Make an array of the ID numbers of all current processes      *
 *                                                               *
 *****************************************************************/

LPDWORD MakeProcIDArray(void)
{

	LPDWORD lpProcIDArray;
	LPDWORD lpNewValue;

	DWORD slotsInIDArray;
	DWORD nextSlotInIDArray;
    
	PPERF_DATA_BLOCK PerfData = NULL;
    PPERF_OBJECT_TYPE PerfObj;
    PPERF_INSTANCE_DEFINITION PerfInst;
    PPERF_COUNTER_DEFINITION PerfCntr, CurCntr;
    PPERF_COUNTER_BLOCK PtrToCntr;
    DWORD BufferSize = TOTALBYTES;
    DWORD i, j;
	LONG k; /* NumInstances is a LONG */
	LONG procObjectIndex;
	LONG procIDIndex;
	char indexChars[30];

	// Get the name strings through the registry.

    GetNameStrings(&lpNamesArray,&lpNameStrings);

	procObjectIndex = GetCounterIndex("Process",lpNamesArray);
	procIDIndex = GetCounterIndex("ID Process",lpNamesArray);

	if (procObjectIndex == -1 || procIDIndex == -1)
	{
	    KillNameStrings(&lpNamesArray,&lpNameStrings);
		return NULL;
	}

	_ltoa(procObjectIndex,indexChars,10);

	// Allocate the buffer for the performance data.

    PerfData = (PPERF_DATA_BLOCK) malloc( BufferSize );

	// Get just the Process data
    while( RegQueryValueEx( HKEY_PERFORMANCE_DATA,
                               indexChars,
                               NULL,
                               NULL,
                               (LPBYTE) PerfData,
                               &BufferSize ) == ERROR_MORE_DATA )
    {
		// Get a buffer that is big enough.

        BufferSize += BYTEINCREMENT;
        PerfData = (PPERF_DATA_BLOCK) realloc( PerfData, BufferSize );
    }

	// Get space for the resulting ID array

	lpProcIDArray = (LPDWORD)malloc(100*sizeof(DWORD));

	if (lpProcIDArray == NULL)
	{
		free(PerfData);
	    KillNameStrings(&lpNamesArray,&lpNameStrings);
		return lpProcIDArray;
	}

	slotsInIDArray = 100;
	nextSlotInIDArray = 0;

	// Get the first object type.

    PerfObj = FirstObject( PerfData );

	// Process all objects.

    for( i=0; i < PerfData->NumObjectTypes; i++ )
    {
		if (PerfObj->ObjectNameTitleIndex == (DWORD)procObjectIndex)
		{

			// Get the first counter.

			PerfCntr = FirstCounter( PerfObj );

			if( PerfObj->NumInstances > 0 )
			{
				// Get the first instance.

				PerfInst = FirstInstance( PerfObj );

				// Retrieve all instances.

				for( k=0; k < PerfObj->NumInstances; k++ )
				{
					CurCntr = PerfCntr;

					// Retrieve all counters.

					for( j=0; j < PerfObj->NumCounters; j++ )
					{
						if (CurCntr->CounterNameTitleIndex == (DWORD)procIDIndex)
						{
							// get the value and add it to the array
							lpNewValue = (LPDWORD)((PBYTE)PerfCntr +
									PerfCntr->CounterOffset);
							
							if (!AddToProcIDArray(&lpProcIDArray,
									&slotsInIDArray,&nextSlotInIDArray,
									*lpNewValue))
							{
								lpProcIDArray[nextSlotInIDArray] = 0;
							
								free(PerfData);
								return lpProcIDArray;
							}

						}

						// Get the next counter.

						CurCntr = NextCounter( CurCntr );
					}

					// Get the next instance.

					PerfInst = NextInstance( PerfInst );
				}
			}
			else
			{
				// Get the counter block.

				PtrToCntr = (PPERF_COUNTER_BLOCK) ((PBYTE)PerfObj +
						PerfObj->DefinitionLength );

				// Retrieve all counters.

				for( j=0; j < PerfObj->NumCounters; j++ )
				{
					if (CurCntr->CounterNameTitleIndex == (DWORD)procIDIndex)
					{
						// get the value and add it to the array
						lpNewValue = (LPDWORD)((PBYTE)PerfCntr +
								PerfCntr->CounterOffset);
							
						if (!AddToProcIDArray(&lpProcIDArray,
								&slotsInIDArray,&nextSlotInIDArray,
								*lpNewValue))
						{
							lpProcIDArray[nextSlotInIDArray] = 0;
							
							free(PerfData);
							return lpProcIDArray;
						}
					}

					// Get the next counter.

					PerfCntr = NextCounter( PerfCntr );
				}
			}

			// Get the next object type.

			PerfObj = NextObject( PerfObj );
		}
	}

	lpProcIDArray[nextSlotInIDArray] = 0;

	free(PerfData);
	return lpProcIDArray;
}


/*****************************************************************
 *                                                               *
 * Look for a process whose name is either of the given strings. *
 * if bCanBeSame is true, it can be the current process, other-  *
 * wise it must be a different one.                              *
 *                                                               *
 *****************************************************************/

BOOL FindEitherAppNT(LPCSTR sFirst, LPCSTR sSecond, BOOL bCanBeSame)
{

	LPDWORD lpNewValue;

	LPWSTR uNameString;
	LPWSTR uFirst;
	LPWSTR uSecond;
	DWORD lUFirst;
	DWORD lUSecond;

	PPERF_DATA_BLOCK PerfData = NULL;
    PPERF_OBJECT_TYPE PerfObj;
    PPERF_INSTANCE_DEFINITION PerfInst;
    PPERF_COUNTER_DEFINITION PerfCntr, CurCntr;
    PPERF_COUNTER_BLOCK PtrToCntr;
    DWORD BufferSize = TOTALBYTES;
    DWORD i, j;
	LONG k; /* NumInstances is a LONG */
	LONG procObjectIndex;
	LONG procIDIndex;
#if defined(_DEBUG)
	#define UNIBUFFER_SIZE 200
	char indexChars[UNIBUFFER_SIZE];
#else
	char indexChars[20];
#endif

	DWORD thisProcID;
	int iRetVal;
	BOOL bSixteen;
	DWORD tProcessVersion;

	thisProcID = GetCurrentProcessId();
	// are we running in a 16-bit or 32-bit process??
	tProcessVersion = GetProcessVersion(0);
	bSixteen = FALSE;
	if ( ((tProcessVersion >> 16) & 0x0000ffff) == 3 &&
		 (tProcessVersion & 0x0000ffff) < 95 )
		bSixteen = TRUE;

 	// Get the name strings through the registry.

    GetNameStrings(&lpNamesArray,&lpNameStrings);

	procObjectIndex = GetCounterIndex("Process",lpNamesArray);
	procIDIndex = GetCounterIndex("ID Process",lpNamesArray);

	if (procObjectIndex == -1 || procIDIndex == -1)
	{
	    KillNameStrings(&lpNamesArray,&lpNameStrings);
		return FALSE;
	}

	_ltoa(procObjectIndex,indexChars,10);

	// Allocate the buffer for the performance data.

    PerfData = (PPERF_DATA_BLOCK) malloc( BufferSize );

	// Get just the Process data
    while( RegQueryValueEx( HKEY_PERFORMANCE_DATA,
                               indexChars,
                               NULL,
                               NULL,
                               (LPBYTE) PerfData,
                               &BufferSize ) == ERROR_MORE_DATA )
    {
		// Get a buffer that is big enough.

        BufferSize += BYTEINCREMENT;
        PerfData = (PPERF_DATA_BLOCK) realloc( PerfData, BufferSize );
    }


	
	// we need to convert both arguments to Unicode, because that's
	// what's in the performance data.

	// get space for converting arguments to Unicode

	lUFirst = strlen(sFirst)+8;
	lUSecond = strlen(sSecond)+8;
	uFirst = (LPWSTR)malloc(lUFirst*sizeof(WCHAR));
	if (uFirst != NULL)
	{
		uSecond = (LPWSTR)malloc(lUSecond*sizeof(WCHAR));
		if (uSecond == NULL)
			free(uFirst);
	}

	if (uFirst == NULL || uSecond == NULL)
	{
		free(PerfData);
		KillNameStrings(&lpNamesArray,&lpNameStrings);
		return FALSE;
	}

	// convert both arguments to Unicode
	iRetVal = MultiByteToWideChar(CP_ACP,MB_ERR_INVALID_CHARS,sFirst,-1,
		uFirst,lUFirst);
	if (iRetVal > 0)
		iRetVal = MultiByteToWideChar(CP_ACP,MB_ERR_INVALID_CHARS,sSecond,-1,
			uSecond,lUSecond);

	if (iRetVal == 0)
	{
		free(PerfData);
		free(uFirst);
		free(uSecond);
		KillNameStrings(&lpNamesArray,&lpNameStrings);
		return FALSE;
	}
	
	// Get the first object type.

    PerfObj = FirstObject( PerfData );

	// Process all objects.

    for( i=0; i < PerfData->NumObjectTypes; i++ )
    {
		if (PerfObj->ObjectNameTitleIndex == (DWORD)procObjectIndex)
		{

			// Get the first counter.

			PerfCntr = FirstCounter( PerfObj );

			if( PerfObj->NumInstances > 0 )
			{
				// Get the first instance.

				PerfInst = FirstInstance( PerfObj );

				// Retrieve all instances.

				for( k=0; k < PerfObj->NumInstances; k++ )
				{
					CurCntr = PerfCntr;
					lpNewValue = 0;

					// Retrieve all counters.

					for( j=0; j < PerfObj->NumCounters; j++ )
					{
						if (CurCntr->CounterNameTitleIndex == (DWORD)procIDIndex)
						{
							// get the value
							lpNewValue = (LPDWORD)((PBYTE)PerfCntr +
									PerfCntr->CounterOffset); 
						}

						// Get the next counter.

						CurCntr = NextCounter( CurCntr );
					}

					if (bCanBeSame || (lpNewValue != 0 && *lpNewValue != thisProcID) )
					{
						// get pointer to the name field.
						uNameString = InstanceName(PerfInst);
						// now do a Unicode compare against uFirst and uSecond.
						// if it matches either one, this is it.
						if (0 == _wcsicmp(uFirst,uNameString) ||
							0 == _wcsicmp(uSecond,uNameString))
						{
							free(PerfData);
							free(uFirst);
							free(uSecond);
							KillNameStrings(&lpNamesArray,&lpNameStrings);
							return TRUE;
						}
//#if defined(_DEBUG)
//						WideCharToMultiByte(CP_ACP,0,uNameString,-1,
//							indexChars,UNIBUFFER_SIZE,NULL,NULL);
//						MessageBox(NULL,indexChars,"KClient",MB_OK | MB_ICONINFORMATION);
//#endif
					}

					// Get the next instance.

					PerfInst = NextInstance( PerfInst );
				}
			}
			else
			{
				// Get the counter block.

				PtrToCntr = (PPERF_COUNTER_BLOCK) ((PBYTE)PerfObj +
						PerfObj->DefinitionLength );

				lpNewValue = 0;

				// Retrieve all counters.

				for( j=0; j < PerfObj->NumCounters; j++ )
				{
					if (CurCntr->CounterNameTitleIndex == (DWORD)procIDIndex)
					{
						// get the value and add it to the array
						lpNewValue = (LPDWORD)((PBYTE)PerfCntr +
								PerfCntr->CounterOffset);
							
					}

					// Get the next counter.

					PerfCntr = NextCounter( PerfCntr );
				}

				if (bCanBeSame || (lpNewValue != 0 && *lpNewValue != thisProcID) )
				{
					// get pointer to the name field.
					uNameString = InstanceName(PerfInst);
					// now do a Unicode compare against uFirst and uSecond.
					// if it matches either one, this is it.
					if (0 == _wcsicmp(uFirst,uNameString) ||
						0 == _wcsicmp(uSecond,uNameString))
					{
						free(PerfData);
						free(uFirst);
						free(uSecond);
						KillNameStrings(&lpNamesArray,&lpNameStrings);
						return TRUE;
					}
				}
			}

			// Get the next object type.

			PerfObj = NextObject( PerfObj );
		}
	}

	free(PerfData);
	free(uFirst);
	free(uSecond);
	KillNameStrings(&lpNamesArray,&lpNameStrings);
	return FALSE;
}
