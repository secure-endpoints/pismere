/*
 * leasherr.c
 * This file is the C file for leasherr.et.
 * Please do not edit it as it is automatically generated.
 */

#ifndef WINDOWS
#define FAR
#else
#include <windows.h>
#endif
#define LPSTR char FAR *
static const char FAR * const text[] = {
	"Only one instance of Leash can be run at a time.",
	"Principal invalid.",
	"Realm failed.",
	"Instance invalid.",
	"Realm invalid.",
	"Unexpected end of Kerberos memory storage.",
	"Warning! Your Kerberos tickets expire soon.",
	"You did not type the same new password.",
	"You can only use printable characters in a password.",
	"Fatal error; cannot run this program.",
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

static const struct error_table et = { text, (err_func)0, 40591872L, 10 };

static struct et_list link = { 0, 0 };

void initialize_lsh_error_table(struct et_list *_et_list) {
    static int activated = 0;

    if (activated)
      return;
    activated = 1;
    if (!link.table) {
        link.next = _et_list;
        link.table = &et;
        _et_list = &link;
    }
}

extern struct et_list *_et_list;

void _initialize_lsh_error_table() 
{
    static int activated = 0;

    if (activated)
      return;
    activated = 1;
    if (!link.table) {
        link.next = _et_list;
        link.table = &et;
        _et_list = &link;
    }
}
