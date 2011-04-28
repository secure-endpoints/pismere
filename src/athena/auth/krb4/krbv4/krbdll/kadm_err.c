/*
 * kadm_err.c
 * This file is the C file for kadm_err.et.
 * Please do not edit it as it is automatically generated.
 */

#ifndef WINDOWS
#define FAR
#include <stdlib.h>
#else
#include <stdlib.h>
#include <windows.h>
#endif
#define LPSTR char FAR *
static const char FAR * const text[] = {

    "$Header: /cvs/pismere/pismere/athena/auth/krb4/krbv4/krbdll/kadm_err.c,v 1.2 1999/05/14 02:32:19 artl Exp $",
	"Cannot fetch local realm",
	"Unable to fetch credentials",
	"Bad key supplied",
	"Can't encrypt data",
	"Cannot encode/decode authentication info",
	"Principal attemping change is in wrong realm",
	"Packet is too large",
	"Version number is incorrect",
	"Checksum does not match",
	"Unsealing private data failed",
	"Unsupported operation",
	"Could not find administrating host",
	"Administrating host name is unknown",
	"Could not find service name in services database",
	"Could not create socket",
	"Could not connect to server",
	"Could not fetch local socket address",
	"Could not fetch master key",
	"Could not verify master key",
	"Entry already exists in database",
	"Database store error",
	"Database read error",
	"Insufficient access to perform requested operation",
	"Data is available for return to client",
	"No such entry in the database",
	"Memory exhausted",
	"Could not fetch system hostname",
	"Could not bind port",
	"Length mismatch problem",
	"Illegal use of wildcard",
	"Database locked or in use",
	"Insecure password rejected",
	"Cleartext password and DES key did not match",
    0
};

typedef LPSTR (*err_func)(int, long);
struct error_table {
    char const FAR * const FAR * msgs;
    err_func func;
	long base;
	int n_msgs;
};
struct et_list {
#ifdef WINDOWS
	HANDLE next;
#else
	struct et_list *next;
#endif
	const struct error_table * table;
};

static const struct error_table et = { text, (err_func)0, -1783126272L, 34 };

#ifdef WINDOWS
void initialize_kadm_error_table(HANDLE *__et_list) {
    struct et_list *_link,*_et_list;
    HANDLE ghlink;

    ghlink=GlobalAlloc(GHND,sizeof(struct et_list));
    _link=GlobalLock(ghlink);
    _link->next=*__et_list;
    _link->table=&et;
    GlobalUnlock(ghlink);
    *__et_list=ghlink;
}
#else
void initialize_kadm_error_table(struct et_list **__et_list) {
    struct et_list *_link;

    _link=malloc(sizeof(struct et_list));
    _link->next=*__et_list;
    _link->table=&et;
    *__et_list=_link;
}
#endif
