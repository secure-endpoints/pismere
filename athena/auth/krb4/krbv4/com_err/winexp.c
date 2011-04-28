
#include <windows.h>
#include <com_err.h>

int (*com_err)(LPSTR,long,LPSTR,...);
LPSTR (*error_message)(long);
LPSTR (*error_table_name)(long);  


void com_err_load(HINSTANCE hInstance)
{
    (FARPROC)com_err=MakeProcInstance((FARPROC)com_err_export,hInstance);
    (FARPROC)error_message=MakeProcInstance((FARPROC)error_message_export,
					    hInstance);
    (FARPROC)error_table_name=MakeProcInstance((FARPROC)error_table_name_export,
					       hInstance);
}

struct et_list {
#ifdef WINDOWS
	HANDLE next;
#else
	struct et_list *next;
#endif
	const struct error_table * table;
};

#ifdef WINDOWS
extern HANDLE _et_list;
#else
extern struct et_list *_et_list;
#endif

void com_err_unload()
{
#ifdef WINDOWS
    HANDLE hel,helnext;
#endif
    struct et_list *_el,*_elnext;

    if (com_err) FreeProcInstance((FARPROC)com_err);
    if (error_message) FreeProcInstance((FARPROC)error_message);
    if (error_table_name) FreeProcInstance((FARPROC)error_table_name);

#ifdef WINDOWS
    hel=_et_list;
    while (hel) {
	_el=GlobalLock(hel);
	if( _el == NULL ){
	    break;
	}
	helnext=_el->next;
	GlobalUnlock(hel);
	GlobalFree(hel);
	hel=helnext;
    }
#else
    _el=_et_list;
    while (_el) {
	_elnext=_el->next;
	free(_el);
	_el=_elnext;
    }
#endif
}
