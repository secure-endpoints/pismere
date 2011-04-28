/*
 * CCILock.h
 *
 * $Header: /cvs/pismere/pismere/athena/auth/krbcc/src/CCacheLib/Sources/Headers/CCILock.h,v 1.4 2000/06/16 21:44:08 dalmeida Exp $
 */
 
#include "CCache.h"
#include "CCache.internal.h"

#pragma once

enum {
	cci_lock_block_mask		= 0x00000001,
	cci_lock_write_mask		= 0x00000002
};

#define		cci_lock_is_blocking(lock) ((lock) & cci_lock_block_mask != 0)
#define		cci_lock_is_nonblocking(lock) ((lock) & cci_lock_block_mask == 0)
#define		cci_lock_is_write(lock) ((lock) & cci_lock_write_mask != 0)
#define		cci_lock_is_read(lock) ((lock) & cci_lock_write_mask == 0)
 
struct cci_lock {
	cc_uint32				magic;
	cc_lock_d				publicData;
};
typedef struct cci_lock cci_lock;

cc_int32 cce_lock_release (
	cc_lock_t				inLock);
	
/*
 * Private (internal) functions
 */
 
void cci_lock_release (
	cci_lock*			inLock);

CCIResult cci_lock_new (
	cc_uint32			inLockType,
	cci_lock**			outLock);

void cci_lock_initialize (
	cc_uint32			inLockType,
	cci_lock*			ioLock);

cc_lock_t cci_lock_externalize (
	cci_lock*			inLock);
	
cc_bool cci_lock_validate (
	cci_lock*			inLock);

CCIResult cci_lock_internalize (
	cc_lock_t			inLock,
	cci_lock**			outLock);

