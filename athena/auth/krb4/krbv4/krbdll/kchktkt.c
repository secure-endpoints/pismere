#include <conf.h>
#include <des.h>
#include <krb.h>
#include <timeval.h>

/* krb_check_serv is a function to check the expiration date on the
   specified credential.  Returns KSUCCESS if we have a valid, non-
   expired credential.

   -CRS 073094
*/

#define MAXHOSTNAME 40        

int FAR krb_check_serv(char *service)                                  
{
    int errcode;  
    // int id;
    // struct timeval local_time;
    CREDENTIALS c;   
    // char *ipAddr;
    // KTEXT tkt;
    // AUTH_DAT *ad;
    // char tmp[ANAME_SZ];
    // int i;

    errcode=krb_get_cred(service,0L,0L,&c);
    return(errcode);

    // if it is expired (by time or IP address) it will return KDC_SERVICE_EXP
    // if this service ticket doesn't exist it will return GC_NOTKT
    // if it is good it will return KSUCCESS
    // there are other error conditions ...

           
    // gettimeofday(&local_time,0);
    // if (errcode=krb_get_cred(service,0L,0L,&c)){
    //	  //MessageBox(NULL," errcode !!","expired",MB_OK);
    //	  return errcode;
    // }
   
    // if ((long)((c.issue_date + c.lifetime * 5L * 60L) -
    //	   (long)local_time.tv_sec) < 0)  {
    //	return KDC_SERVICE_EXP;
    // }

    // new stuff, looking at the IP address
    // if( 0 != com_addr()){
    //	   return KDC_SERVICE_EXP;
    // }
    // return KSUCCESS;
} 
