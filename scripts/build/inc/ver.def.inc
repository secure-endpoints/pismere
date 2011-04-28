#include <winver.h>

#define stringer(x) #x

#undef VER_EXTRA_LABEL
#undef VER_EXTRA_VALUE
#undef VER_COMMENT
#undef VER_VENDOR

#if !defined(VER_TARGET) || !defined(VER_NAME) || !defined(VER_USERNAME) || !defined(VER_HOSTNAME) || !defined(VER_DATE)
#error You need to invoke the build script!
#endif

#define VER_DERIVED_VER "*current*"
#define VER_BUILT       "Build of " VER_DATE " by " VER_USERNAME " on " VER_HOSTNAME

#define VER_DERIVED     "Derived from " VER_DERIVED_VER

#ifdef BUILD_OFFICIAL
#include <buildnum.inc>
#else
#define VER_BUILDNUM 0
#endif

#if VER_BUILDNUM == 0
#ifdef VENDORMOD
#define VER_PROD_TAG    "Modified by " VENDORMOD
#define VER_VENDOR     VENDORMOD
#else
#define VER_PROD_TAG    "*UNOFFICIAL*"
#endif
#else
#ifdef VENDORMOD
#error Is this a numbered build or a vendor modification?
#endif
#endif

#ifdef VER_SUBTARGET
#define VER_FILESUBTYPE 		VER_SUBTARGET
#else
#define VER_FILESUBTYPE 		VFT2_UNKNOWN
#endif

/* Should this be ifndef? */
#ifndef VER_FILETYPE
#define VER_FILETYPE			VER_TARGET
#endif

/* Should this be ifndef? */
#ifndef VER_INTERNALNAME_STR
#define VER_INTERNALNAME_STR		VER_NAME
#endif

#if VER_TARGET == VFT_APP
#define VER_EXTENSION ".exe"
#elif VER_TARGET == VFT_DLL
#define VER_EXTENSION ".dll"
#elif VER_TARGET == VFT_STATIC_LIBRARY
#define VER_EXTENSION ".lib"
#else
#error I only know about .exe, .dll, and .lib files!
#endif

/* Should this be ifndef? */
#ifndef VER_ORIGINALFILENAME_STR
#define VER_ORIGINALFILENAME_STR	VER_NAME VER_EXTENSION
#endif

/* default is nodebug */
#ifdef _DEBUG
#define VER_DEBUG                   VS_FF_DEBUG
#else
#define VER_DEBUG                   0
#endif

/* default is prerelease */
#if defined(BUILD_OFFICIAL) && !defined(BETA)
#define VER_PRERELEASE              0
#else
#define VER_PRERELEASE              VS_FF_PRERELEASE
#endif
#if BETA
#if BETA < 0
#define VERS_BETA                   " Beta"
#else
#define VERS_BETA                   " Beta " BETA_STR
#endif
#else
#define VERS_BETA
#endif

#define VER_FILEFLAGSMASK           VS_FFI_FILEFLAGSMASK
#define VER_FILEOS                  VOS__WINDOWS32
#define VER_FILEFLAGS               (VER_PRERELEASE|VER_DEBUG)

#define VER_MIT "Massachusetts Institute of Technology"

#ifndef VER_COMPANYNAME_STR
#define VER_COMPANYNAME_STR         VER_MIT
#endif

#ifndef VER_LEGALCOPYRIGHT_YEARS
#define VER_LEGALCOPYRIGHT_YEARS    "1993-2001"
#endif

#ifndef VER_LEGALCOPYRIGHT_STR
#define VER_LEGALCOPYRIGHT_STR      "Copyright (C) " VER_LEGALCOPYRIGHT_YEARS " by the " VER_MIT
#endif

#ifndef VER_PRODUCTNAME_STR
#error Someone forgot the define VER_PRODUCTNAME_STR!
#endif

#ifndef VER_FILEDESCRIPTION_STR
#error Someone forgot the define VER_FILEDESCRIPTION_STR!
#endif

/*****************************************************************************/

#if (defined(VER_PROD_MAJOR) ^ defined(VER_PROD_MAJOR_STR)) || \
    (defined(VER_PROD_MINOR) ^ defined(VER_PROD_MAJOR_STR)) || \
    (defined(VER_PROD_REV) ^ defined(VER_PROD_REV_STR))
#error Version improperly defined.
#endif

#ifndef VER_PROD_MAJOR
#define VER_PROD_MAJOR      1
#define VER_PROD_MAJOR_STR "1"
#endif
#ifndef VER_PROD_MINOR
#define VER_PROD_MINOR      0
#define VER_PROD_MINOR_STR "0"
#endif
#ifndef VER_PROD_REV
#define VER_PROD_REV        0
#define VER_PROD_REV_STR   "0"
#endif

#define VERPS_MAJOR VER_PROD_MAJOR_STR
#define VERPS_MINOR "." VER_PROD_MINOR_STR

#if 0 && VER_PROD_REV == 0
#define VERPS_REV
#else
#define VERPS_REV   "." VER_PROD_REV_STR
#endif

#if VER_BUILDNUM == 0
#define VERPS_BUILD
#else
#define VERPS_BUILD "." VER_BUILDSTR
#endif

/*****************************************************************************/

#if (defined(VER_FILE_MAJOR) ^ defined(VER_FILE_MAJOR_STR)) || \
    (defined(VER_FILE_MINOR) ^ defined(VER_FILE_MAJOR_STR)) || \
    (defined(VER_FILE_REV) ^ defined(VER_FILE_REV_STR))
#error Version improperly defined.
#endif

#ifndef VER_FILE_MAJOR
#define VER_FILE_MAJOR      VER_PROD_MAJOR
#define VER_FILE_MAJOR_STR  VER_PROD_MAJOR_STR
#endif
#ifndef VER_FILE_MINOR
#define VER_FILE_MINOR      VER_PROD_MINOR
#define VER_FILE_MINOR_STR  VER_PROD_MINOR_STR
#endif
#ifndef VER_FILE_REV
#define VER_FILE_REV        VER_PROD_REV
#define VER_FILE_REV_STR    VER_PROD_REV_STR
#endif

#define VERFS_MAJOR VER_FILE_MAJOR_STR
#define VERFS_MINOR "." VER_FILE_MINOR_STR

#if 0 && VER_FILE_REV == 0
#define VERFS_REV
#else
#define VERFS_REV   "." VER_FILE_REV_STR
#endif

#if VER_BUILDNUM == 0
#define VERFS_BUILD
#else
#define VERFS_BUILD "." VER_BUILDSTR
#endif

/*****************************************************************************/

#if 1 || VER_BUILDNUM == 0
#define VERS_BUILD
#else
#define VERS_BUILD " (Build " VER_BUILDSTR ")"
#endif

#ifndef VER_PRODUCTVERSION
#define VER_PRODUCTVERSION VER_PROD_MAJOR,VER_PROD_MINOR,VER_PROD_REV,VER_BUILDNUM
#endif

#ifndef VER_FILEVERSION
#define VER_FILEVERSION VER_FILE_MAJOR,VER_FILE_MINOR,VER_FILE_REV,VER_BUILDNUM
#endif

#if !(defined(VER_PROD_TAG) ^ (VER_BUILDNUM != 0))
#error Hey!  Are you trying to mess with the build?  See the license agreement.
#endif

#ifndef VER_PROD_TAG
#define VERS_TAG
#else
#define VERS_TAG " (" VER_PROD_TAG ")"
#endif

#define VERS_BUILT " (" VER_BUILT ")"
#define VERS_DERIVED " (" VER_DERIVED ")"

#ifndef VER_PRODUCTVERSION_STR
#define VER_PRODUCTVERSION_STR VERPS_MAJOR VERPS_MINOR VERPS_REV VERPS_BUILD VERS_BETA VERS_BUILD VERS_TAG
#endif

#if defined(VER_PROD_TAG)
#define VER_COMMENT VERS_TAG VERS_DERIVED VERS_BUILT
#endif

#ifndef VER_FILEVERSION_STR
#define VER_FILEVERSION_STR VERFS_MAJOR VERFS_MINOR VERFS_REV VERFS_BUILD VERS_BETA VERS_BUILD VERS_TAG
#endif

#ifndef VER_ORIGINALFILENAME_STR
#define VER_ORIGINALFILENAME_STR VER_INTERNALNAME_STR
#endif

#ifdef EXPORT_CONTROLLED

#ifdef EXPORT
#define EXPORT_TAG  " (Export Version)"
#else
#define EXPORT_TAG  " (Domestic Use Only)"
#endif

#else           /* Not Export Controlled */

#define EXPORT_TAG

#endif
