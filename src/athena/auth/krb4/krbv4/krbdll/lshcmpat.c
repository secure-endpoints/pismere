/* Leash Compatibility code - by shabby */

#include "conf.h"
#include <windows.h>
#include <lsh_pwd.h>
#include <krb.h>
#include <com_err.h>
#include <leashwin.h>
#include <krberr.h>
#include <kadm_err.h>
#include <leasherr.h>

static HMODULE hmLeashDll=NULL;     

#ifdef WIN32
#define WINTYPE "W32"
#else
#define WINTYPE "WIN"
#endif

long FAR PASCAL hQKrbInst( void );
#undef krb_err_func
LPSTR krb_err_func(int offset, long code);
extern void initialize_krb_error_table(void); 
extern void _initialize_lsh_error_table(); 

void LoadLeashDll()
{
  void (FAR *llcec)(FARPROC,FARPROC,FARPROC);
  int (*com_err_proc)(LPSTR,long,LPSTR,...);



  if (!hmLeashDll) {

      hmLeashDll=LoadLibrary("LEASH" WINTYPE ".DLL");
      if( hmLeashDll ){

      /* com_err stuff */
#ifdef WIN16
	  com_err_load(hQKrbInst());
#endif
	   (FARPROC)llcec=GetProcAddress(hmLeashDll,"Leash_load_com_err_callback");
	   if (!llcec) return;
	   llcec((FARPROC)com_err,
		 (FARPROC)error_message,
		 (FARPROC)error_table_name);
	   init_krb_err_func(krb_err_func);
	   init_lsh_err_tbl();
	   init_kadm_err_tbl();
	   (FARPROC)com_err_proc=MakeProcInstance(GetProcAddress(hmLeashDll,"lsh_com_err_proc"),hQKrbInst());
	   set_com_err_hook(com_err_proc);
      }
    }
}

void UnloadLeashDll()
{
  if (hmLeashDll) FreeModule(hmLeashDll);
  hmLeashDll=NULL;
#ifdef WIN16
  com_err_unload();
#endif
}

int FAR PASCAL Lsh_Enter_Password_Dialog(HWND hParent, LPLSH_DLGINFO lpdlginfo)
{
  typedef int (FAR *lfunc)(HWND,LPLSH_DLGINFO);
  lfunc lf;    
  int ret;
  
  LoadLeashDll();
  
  (FARPROC)lf=GetProcAddress(hmLeashDll,"Leash_kinit_dlg");
  if (!lf) return KFAILURE;
  ret=lf(hParent,lpdlginfo);
  UnloadLeashDll();
  return ret;
}

int FAR PASCAL Lsh_Change_Password_Dialog(HWND hParent, LPLSH_DLGINFO lpdlginfo)
{
  typedef int (FAR *lfunc)(HWND,LPLSH_DLGINFO);
  lfunc lf;
  int ret;

  LoadLeashDll();

  (FARPROC)lf=GetProcAddress(hmLeashDll,"Leash_changepwd_dlg");
  if (!lf) return KFAILURE;
  ret=lf(hParent,lpdlginfo);
  UnloadLeashDll();
  return ret;
}

#ifdef NNOKRBLAYER
LONG FAR PASCAL lsh_get_lsh_errno( LONG FAR *err_val)
{
  typedef LONG (FAR *lfunc)(LONG FAR *);
  lfunc lf;
  LONG ret;
  
  LoadLeashDll();

  (FARPROC)lf=GetProcAddress(hmLeashDll,"Leash_get_lsh_errno");
  if (!lf) return KFAILURE;
  ret=lf(err_val);
  UnloadLeashDll();
  return ret;
}

LONG FAR PASCAL lsh_checkpwd(char *principal, char *password)
{
  typedef LONG (FAR *lfunc)(char FAR *,char FAR *);
  lfunc lf;
  LONG ret;

  LoadLeashDll();

  (FARPROC)lf=GetProcAddress(hmLeashDll,"Leash_checkpwd");
  if (!lf) return KFAILURE;
  ret=lf(principal,password);
  UnloadLeashDll();
  return ret;
}

LONG FAR PASCAL lsh_changepwd(char *principal, char *password, char *newpassword, LPSTR kadm_info)
{               
  typedef LONG (FAR *lfunc)(LPSTR,LPSTR,LPSTR,LPSTR);
  lfunc lf;
  LONG ret;
  
  LoadLeashDll();
  (FARPROC)lf=GetProcAddress(hmLeashDll,"Leash_changepwd");
  if (!lf) return KFAILURE;
  ret=lf(principal,password,newpassword,kadm_info);
  UnloadLeashDll();
  return ret;
}

LONG FAR PASCAL lsh_kinit(char *principal, char *password, int lifetime)
{
  typedef LONG (FAR *lfunc)(LPSTR,LPSTR,int);
  lfunc lf;
  LONG ret;
  
  LoadLeashDll();

  (FARPROC)lf=GetProcAddress(hmLeashDll,"Leash_kinit");
  if (!lf) return KFAILURE;
  ret=lf(principal,password,lifetime);
  UnloadLeashDll();
  return ret;
}

LONG FAR PASCAL lsh_klist(HWND hlist, TICKETINFO FAR *ticketinfo)
{
  typedef LONG (FAR *lfunc)(HWND,TICKETINFO FAR *);
  lfunc lf;
  LONG ret;
  
  LoadLeashDll();
 
  (FARPROC)lf=GetProcAddress(hmLeashDll,"Leash_klist");
  if (!lf) {
  	MessageBox(NULL,"call Leash_klist KFAILURE","KFAILURE",MB_OK);
  	return KFAILURE;
  }  
  		
  ret=lf(hlist,ticketinfo); 
  MessageBox(NULL,"call Leash_klist SUCCESS","KSUCCESS",MB_OK);
  UnloadLeashDll();
  return ret;
}

LONG FAR PASCAL lsh_kdestroy(void)
{
  typedef LONG (FAR *lfunc)(void);
  lfunc lf;
  LONG ret;
  
  LoadLeashDll();

  (FARPROC)lf=GetProcAddress(hmLeashDll,"Leash_kdestroy");
  if (!lf) return KFAILURE;
  ret=lf();
  UnloadLeashDll();
  return ret;
}
#endif

LPSTR krb_err_func(int offset, long code)
{
  return get_krb_err_txt()[offset];
}


LONG lsh_LoadKrb4LeashErrorTables(HMODULE hLeashDll, 
                                  INT useCallBackFunction)
{
    void (FAR *llcec)(FARPROC,FARPROC,FARPROC);
    int (*com_err_proc)(LPSTR,long,LPSTR,...);

    if (hLeashDll) 
    {
        if (hLeashDll)
        {
            (FARPROC)llcec = GetProcAddress(hLeashDll,
                                           "Leash_load_com_err_callback");

            if (!llcec) 
              return KFAILURE;

            llcec((FARPROC)com_err,
                  (FARPROC)error_message,
                  (FARPROC)error_table_name);

            init_krb_err_func(krb_err_func);
            init_kadm_err_tbl(); 
            _initialize_lsh_error_table();
            
            if (useCallBackFunction)
            {
                (FARPROC)com_err_proc = MakeProcInstance(GetProcAddress(hLeashDll,
                                                      "lsh_com_err_proc"),
                                                    hQKrbInst());
                set_com_err_hook(com_err_proc);
            }
       }
    }
    else
    {
        return KFAILURE;
    }
    
    return 0;
}
