/*
 * krberr.c
 * This file is the C file for krberr.et.
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

static  struct error_table et = { 0, (err_func)0, 39525376L, 256 };

#ifdef WINDOWS
void initialize_krb_error_func(err_func func, HANDLE *__et_list) {
    struct et_list *_link,*_et_list;
    struct error_table *et_new;
    HANDLE ghlink;

    et.func=func;
    ghlink=GlobalAlloc(GHND,sizeof(struct et_list));
    et_new=malloc(sizeof(struct error_table));
    memcpy(et_new,&et,sizeof(struct error_table));
    _link=GlobalLock(ghlink);
    _link->next=*__et_list;
    _link->table=et_new;
    GlobalUnlock(ghlink);
    *__et_list=ghlink;
}
#else
void initialize_krb_error_func(err_func func, struct et_list **__et_list) {
    struct et_list *_link;
    struct error_table *et_new;

    et.func=func;
    et_new=malloc(sizeof(struct error_table));
    memcpy(et_new,&et,sizeof(struct error_table));
    _link=malloc(sizeof(struct et_list));
    _link->next=*__et_list;
    _link->table=et_new;
    *__et_list=_link;
}
#endif
