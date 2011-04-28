/*
 * krb_err.c
 * This file is the C file for krb_err.et.
 * Please do not edit it as it is automatically generated.
 */

#ifndef WINDOWS
#define FAR
#else
#include <windows.h>
#endif
#define LPSTR char FAR *
static const char FAR * const text[] = {
	"Kerberos successful",
	"Kerberos principal expired",
	"Kerberos service expired",
	"Kerberos auth expired",
	"Incorrect kerberos master key version",
	"Incorrect kerberos master key version",
	"Incorrect kerberos master key version",
	"Kerberos error: byte order unknown",
	"Kerberos principal unknown",
	"Kerberos principal not unique",
	"Kerberos principal has null key",
	"Reserved 11",
	"Reserved 12",
	"Reserved 13",
	"Reserved 14",
	"Reserved 15",
	"Reserved 16",
	"Reserved 17",
	"Reserved 18",
	"Reserved 19",
	"Generic error from Kerberos KDC",
	"Can't read Kerberos ticket file",
	"Can't find Kerberos ticket or TGT",
	"Reserved 23",
	"Reserved 24",
	"Reserved 25",
	"Kerberos TGT Expired",
	"Reserved 27",
	"Reserved 28",
	"Reserved 29",
	"Reserved 30",
	"Kerberos error: Can't decode authenticator",
	"Kerberos ticket expired",
	"Kerberos ticket not yet valid",
	"Kerberos error: Repeated request",
	"The kerberos ticket isn't for us",
	"Kerberos request inconsistent",
	"Kerberos error: delta_t too big",
	"Kerberos error: incorrect net address",
	"Kerberos protocol version mismatch",
	"Kerberos error: invalid msg type",
	"Kerberos error: message stream modified",
	"Kerberos error: message out of order",
	"Kerberos error: unauthorized request",
	"Reserved 44",
	"Reserved 45",
	"Reserved 46",
	"Reserved 47",
	"Reserved 48",
	"Reserved 49",
	"Reserved 50",
	"Kerberos error: current PW is null",
	"Kerberos error: Incorrect current password",
	"Kerberos protocol error",
	"Error returned by Kerberos KDC",
	"Null Kerberos ticket returned by KDC",
	"Kerberos error: Retry count exceeded",
	"Kerberos error: Can't send request",
	"Reserved 58",
	"Reserved 59",
	"Reserved 60",
	"Kerberos error: not all tickets returned",
	"Kerberos error: incorrect password",
	"Kerberos error: Protocol Error",
	"Reserved 64",
	"Reserved 65",
	"Reserved 66",
	"Reserved 67",
	"Reserved 68",
	"Reserved 69",
	"Other error",
	"Don't have Kerberos ticket-granting ticket",
	"Reserved 72",
	"Reserved 73",
	"Reserved 74",
	"Reserved 75",
	"No ticket file found",
	"Couldn't access ticket file",
	"Couldn't lock ticket file",
	"Bad ticket file format",
	"tf_init not called first",
	"Bad Kerberos name format",
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
	struct et_list *next;
	const struct error_table * table;
};
extern struct et_list *_et_list;

static const struct error_table et = { text, (err_func)0, 39525376L, 82 };

static struct et_list link = { 0, 0 };

void initialize_krb_error_table(void) {
    if (!link.table) {
        link.next = _et_list;
        link.table = &et;
        _et_list = &link;
    }
}
