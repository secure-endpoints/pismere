========================================================================
       MICROSOFT FOUNDATION CLASS LIBRARY : WSHTEST
========================================================================

AppWizard has generated this WSHTEST application for you.  This application
not only demonstrates the basics of using the Microsoft Foundation classes
but is also a starting point for writing your application.

This file contains a summary of what you will find in each of the files that
make up your WSHTEST application.


WSHTEST.MAK
    This project file is compatible with the Visual C++ Workbench.

    It is also compatible with the NMAKE program provided with the
    Professional Edition of Visual C++.

    To build a debug version of the program from the MS-DOS prompt, type
	nmake DEBUG=1 /f WSHTEST.MAK
    or to build a release version of the program, type
	nmake DEBUG=0 /f WSHTEST.MAK

WSHTEST.H
    This is the main include file for the application.  It includes other
    project specific includes (including RESOURCE.H) and declares the
    CWshtestApp application class.

WSHTEST.CPP
    This is the main application source file that contains the application
    class CWshtestApp.

WSHTEST.RC
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited with App Studio.

RES\WSHTEST.ICO
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file WSHTEST.RC.

RES\WSHTEST.RC2
    This file contains resources that are not edited by App Studio.  Initially
    this contains a VERSIONINFO resource that you can customize for your
    application.  You should place other non-App Studio editable resources
    in this file.

WSHTEST.DEF
    This file contains information about the application that must be
    provided to run with Microsoft Windows.  It defines parameters
    such as the name and description of the application, and the size
    of the initial local heap.  The numbers in this file are typical
    for applications developed with the Microsoft Foundation Class Library.
    The default stack size can be adjusted by editing the project file.

WSHTEST.CLW
    This file contains information used by ClassWizard to edit existing
    classes or add new classes.  ClassWizard also uses this file to store
    information needed to generate and edit message maps and dialog data
    maps and to generate prototype member functions.

/////////////////////////////////////////////////////////////////////////////

For the main frame window:

MAINFRM.H, MAINFRM.CPP
    These files contain the frame class CMainFrame, which is derived from
    CFrameWnd and controls all SDI frame features.


/////////////////////////////////////////////////////////////////////////////

AppWizard creates one document type and one view:

WSHTEDOC.H, WSHTEDOC.CPP - the document
    These files contain your CWshtestDoc class.  Edit these files to
    add your special document data and to implement file saving and loading
    (via CWshtestDoc::Serialize).

WSHTEVW.H, WSHTEVW.CPP - the view of the document
    These files contain your CWshtestView class.
    CWshtestView objects are used to view CWshtestDoc objects.



/////////////////////////////////////////////////////////////////////////////
Other standard files:

STDAFX.H, STDAFX.CPP
    These files are used to build a precompiled header (PCH) file
    named STDAFX.PCH and a precompiled types (PCT) file named STDAFX.OBJ.

RESOURCE.H
    This is the standard header file, which defines new resource IDs.
    App Studio reads and updates this file.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
