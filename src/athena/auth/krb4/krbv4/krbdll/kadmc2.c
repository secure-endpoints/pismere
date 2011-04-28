/*
 * Taken from the Macintosh src code.
 */

#include <conf.h>

#ifdef WINDOWS
#include <windows.h>
#endif

#ifndef WINSOCK
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <netdb.h>
#include <sys/socket.h>

#else /* WINSOCK */

#include <winsock.h>

#if defined(WSHELPER)
#include <wshelper.h>
#define GETHOSTBYNAME rgethostbyname
#else /* !WSHELPER */
#define GETHOSTBYNAME gethostbyname
#endif /* !WSHELPER */

#include <errno.h> /* jthwang 2-2-94 */

#endif /* WINSOCK */

#include <com_err.h>
#include <kadm.h>
#include "kadm_err.h"

#include "krberr.h"

#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#ifndef NULL
#define NULL 0
#endif

#include "kadmprot.h" /* prototypes */

extern char *WSErrorString(UINT err);

static Kadm_Client client_parm;


/* Macros for use in returning data... used in kadm_cli_send */
#define RET_N_FREE(r) {clear_secrets(); free((char *)act_st); free((char *)priv_pak); return r;}

/* Keys for use in the transactions */
static des_cblock sess_key;            /* to be filled in by kadm_cli_keyd */
static Key_schedule sess_sched;

static
void
clear_secrets()
{
    bzero((char *)sess_key, sizeof(sess_key));
    bzero((char *)sess_sched, sizeof(sess_sched));
    return;
}

Clt_Parm clientParm;

/*
 * kadm_init_link
 *      receives    : name, inst, realm
 *
 * initializes client parm, the Kadm_Client structure which holds the
 * data about the connection between the server and client, the services
 * used, the locations and other fun things
 */
long /* DAMMIT! USE LONGS! */
kadm_init_link(
    char n[],
    char i[],
    char r[]
    )
{
    struct servent FAR *sep;           /* service we will talk to */
    struct hostent FAR *hop;           /* host we will talk to */
    char adm_hostname[MAXHOSTNAMELEN];

    (void) strcpy(clientParm.sname, n);
    (void) strcpy(clientParm.sinst, i);
    (void) strcpy(clientParm.krbrlm, r);
    clientParm.admin_fd = -1;
    clientParm.nSocketState = STATE_NONE;

    /* set up the admin_addr - fetch name of admin host */
    if (krb_get_admhst(adm_hostname, clientParm.krbrlm, 1) != KSUCCESS)
        return KADM_NO_HOST;
    bzero((char *) &clientParm.admin_addr,
          sizeof(clientParm.admin_addr));
    if ((hop = GETHOSTBYNAME(adm_hostname)) == NULL)
        return KADM_UNK_HOST;  /* couldnt find the admin servers address */

    bcopy(hop->h_addr, &clientParm.admin_addr.sin_addr, hop->h_length);
    clientParm.admin_addr.sin_family = hop->h_addrtype;

    if (sep = getservbyname(KADM_SNAME, "tcp"))
        clientParm.admin_addr.sin_port = sep->s_port;
    else if (KADM_DEFAULT_PORT) /* set to 0 to turn this off */
        clientParm.admin_addr.sin_port = htons(KADM_DEFAULT_PORT);
    else
        return KADM_NO_SERV;   /* couldnt find the admin service */

    return KADM_SUCCESS;
}                                      /* procedure kadm_init_link */

/*
 * kadm_change_pw
 * receives    : key
 *
 * Replaces the password (i.e. des key) of the caller with that specified in
 * key. Returns no actual data from the master server, since this is called
 * by a user
 */

/* ARGH! */
long kadm_change_pw(des_cblock newkey)
/* The DES form of the users key */
{
    u_char  *ret_st;
    long /* int */  retval;

    retval = kadm_change_pw2(newkey, 0, &ret_st);
    if (ret_st)
        free(ret_st); /* free???? in windows???? WHY??????????? Bleah!*/
    return(retval);
}

/*
 * kadm_change_pw2
 * recieves    : key, pw_string, ret_string
 *
 * Replaces the password (i.e. des key) of the caller with that specified in
 * key. Returns no actual data from the master server, since this is called
 * by a user
 */
long kadm_change_pw2(des_cblock newkey, char *pwstring, u_char **retstring)
{
    /* The DES form of the users key */
    int stsize;
    long retc;              /* stream size and return code */
    u_char *send_st;               /* send stream */
    u_char *ret_st;
    int ret_sz;
    u_long keytmp;

    if ((retc = kadm_cli_conn()) != KADM_SUCCESS)
        return(retc);
    /* possible problem with vts_long on a non-multiple of four boundary */

    stsize = 0;                    /* start of our output packet */
    send_st = (u_char *) malloc(1);/* to make it reallocable */
    send_st[stsize++] = (u_char) CHANGE_PW;

    /* change key to stream */

    bcopy((char *) (((long *) newkey) + 1), (char *) &keytmp, 4);
    keytmp = htonl(keytmp);
    stsize += vts_long(keytmp, &send_st, stsize);

    bcopy((char *) newkey, (char *) &keytmp, 4);
    keytmp = htonl(keytmp);
    stsize += vts_long(keytmp, &send_st, stsize);

    if (pwstring) {
        stsize += vts_string(pwstring, &send_st, stsize);
    }

    ret_sz = 0;
    retc = kadm_cli_send(send_st, stsize, &ret_st, &ret_sz);
    free((char *)send_st);
    if (ret_sz)
        *retstring = ret_st;
    else {
        *retstring = 0;
        free(ret_st);
    }
    kadm_cli_disconn();
    return(retc);
}

/*
 * kadm_add
 *      receives    : vals
 *      returns     : vals
 *
 * Adds and entry containing values to the database returns the values of the
 * entry, so if you leave certain fields blank you will be able to determine
 * the default values they are set to
 */
long kadm_add(vals)
    Kadm_vals *vals;
{
    u_char *st, *st2;              /* st will hold the stream of values */
    int st_len;                    /* st2 the final stream with opcode */
    long /* int */ retc;                   /* return code from call */
    u_char *ret_st;
    int ret_sz;

    if ((retc = kadm_cli_conn()) != KADM_SUCCESS)
        return(retc);
    st_len = vals_to_stream(vals, &st);
    st2 = (u_char *) malloc((unsigned)(1 + st_len));
    *st2 = (u_char) ADD_ENT;       /* here's the opcode */
    bcopy((char *) st, (char *) st2 + 1, st_len);   /* append st on */
    retc = kadm_cli_send(st2, st_len + 1, &ret_st, &ret_sz);
    free((char *)st);
    free((char *)st2);
    if (retc == KADM_SUCCESS) {
        /* ret_st has vals */
        if (stream_to_vals(ret_st, vals, ret_sz) < 0)
            retc = KADM_LENGTH_ERROR;
        free((char *)ret_st);
    }
    kadm_cli_disconn();
    return(retc);
}

/*
 * kadm_mod
 *      receives    : KTEXT, {values, values}
 *      returns     : CKSUM,  RETCODE, {values}
 *      acl         : su, sms (as register or dealloc)
 *
 * Modifies all entries corresponding to the first values so they match the
 * second values. returns the values for the changed entries in vals2
 */
/* WAAAAHHH! */
long kadm_mod(vals1, vals2)
    Kadm_vals *vals1;
Kadm_vals *vals2;
{
    u_char *st, *st2;              /* st will hold the stream of values */
    int st_len, nlen;              /* st2 the final stream with opcode */
    u_char *ret_st;
    int ret_sz;

    /* nlen is the length of second vals */
    long /* int */ retc;                   /* return code from call */

    if ((retc = kadm_cli_conn()) != KADM_SUCCESS)
        return(retc);

    st_len = vals_to_stream(vals1, &st);
    st2 = (u_char *) malloc((unsigned)(1 + st_len));
    *st2 = (u_char) MOD_ENT;       /* here's the opcode */
    bcopy((char *) st, (char *) st2 + 1, st_len++); /* append st on */
    free((char *)st);
    nlen = vals_to_stream(vals2, &st);
    st2 = (u_char *) realloc((char *) st2, (unsigned)(st_len + nlen));
    bcopy((char *) st, (char *) st2 + st_len, nlen); /* append st on */
    retc = kadm_cli_send(st2, st_len + nlen, &ret_st, &ret_sz);
    free((char *)st);
    free((char *)st2);
    if (retc == KADM_SUCCESS) {
        /* ret_st has vals */
        if (stream_to_vals(ret_st, vals2, ret_sz) < 0)
            retc = KADM_LENGTH_ERROR;
        free((char *)ret_st);
    }
    kadm_cli_disconn();
    return(retc);
}

/*
 * kadm_get
 *      receives   : KTEXT, {values, flags}
 *      returns    : CKSUM, RETCODE, {count, values, values, values}
 *      acl        : su
 *
 * gets the fields requested by flags from all entries matching values returns
 * this data for each matching recipient, after a count of how many such
 * matches there were
 */
long /* DEATH TO 32-BIT INTS! */ 
kadm_get(
    Kadm_vals *vals,
    u_char fl[4]
    )
{
    int loop;                      /* for copying the fields data */
    u_char *st, *st2;              /* st will hold the stream of values */
    int st_len;                    /* st2 the final stream with opcode */
    long /* int */ retc;                   /* return code from call */
    u_char *ret_st;
    int ret_sz;

    if ((retc = kadm_cli_conn()) != KADM_SUCCESS)
        return(retc);
    st_len = vals_to_stream(vals, &st);
    st2 = (u_char *) malloc((unsigned)(1 + st_len + FLDSZ));
    *st2 = (u_char) GET_ENT;       /* here's the opcode */
    bcopy((char *) st, (char *) st2 + 1, st_len);   /* append st on */
    for (loop = FLDSZ - 1; loop >= 0; loop--)
        *(st2 + st_len + FLDSZ - loop) = fl[loop]; /* append the flags */
    retc = kadm_cli_send(st2, st_len + 1 + FLDSZ,  &ret_st, &ret_sz);
    free((char *)st);
    free((char *)st2);
    if (retc == KADM_SUCCESS) {
        /* ret_st has vals */
        if (stream_to_vals(ret_st, vals, ret_sz) < 0)
            retc = KADM_LENGTH_ERROR;
        free((char *)ret_st);
    }
    kadm_cli_disconn();
    return(retc);
}

/*
 * kadm_check_pw
 * recieves    : key, pw_string, ret_string
 *
 * Sends the password to the server and asks it if the password is a
 * "secure" password or not.  Returns the error KADM_INSECURE_PW and
 * optionally returns a message if the password is not considered secure.
 */
long /* int */
kadm_check_pw(
    des_cblock newkey, 
    char *pwstring, 
    u_char **retstring
    )
{
    /* newkey: The DES form of the users key */
    int stsize;
    long /* PAIN! */ retc;         /* stream size and return code */
    u_char *send_st;               /* send stream */
    u_char *ret_st;
    int ret_sz;
    u_long keytmp;

    if ((retc = kadm_cli_conn()) != KADM_SUCCESS)
        return(retc);
    /* possible problem with vts_long on a non-multiple of four boundary */

    stsize = 0;                    /* start of our output packet */
    send_st = (u_char *) malloc(1);/* to make it reallocable */
    send_st[stsize++] = (u_char) CHECK_PW;

    /* change key to stream */

    bcopy((char *) (((long *) newkey) + 1), (char *) &keytmp, 4);
    keytmp = htonl(keytmp);
    stsize += vts_long(keytmp, &send_st, stsize);

    bcopy((char *) newkey, (char *) &keytmp, 4);
    keytmp = htonl(keytmp);
    stsize += vts_long(keytmp, &send_st, stsize);

    if (pwstring) {
        stsize += vts_string(pwstring, &send_st, stsize);
    }

    ret_sz = 0;
    retc = kadm_cli_send(send_st, stsize, &ret_st, &ret_sz);
    free((char *)send_st);
    if (ret_sz)
        *retstring = ret_st;
    else {
        *retstring = 0;
        free(ret_st);
    }
    kadm_cli_disconn();
    return(retc);
}

/*
 * kadm_cli_send
 *      recieves   : opcode, packet, packet length, serv_name, serv_inst
 *      returns    : return code from the packet build, the server, or
 *                       something else
 *
 * It assembles a packet as follows:
 *       8 bytes    : VERSION STRING
 *       4 bytes    : LENGTH OF MESSAGE DATA and OPCODE
 *                  : KTEXT
 *                  : OPCODE       \
 *                  : DATA          > Encrypted (with make priv)
 *                  : ......       /
 *
 * If it builds the packet and it is small enough, then it attempts to open the
 * connection to the admin server.  If the connection is succesfully open
 * then it sends the data and waits for a reply.
 */
/* XXXXXXXXXXXXXXXX -- JMS 8/23/93 This function looks evil, unportable, etc.
		Somebody does a lot of type juggling, sizeof assumptions, etc....
		look for problems here.... */

long /* ARGGHHHHH! */
kadm_cli_send(
    u_char *st_dat,                         /* the actual data */
    int st_siz,                             /* length of said data */
    u_char **ret_dat,                       /* to give return info */
    int *ret_siz                            /* length of returned info */
    )
{
    int act_len;
    long /* !!!! */ retdat;        /* current offset into packet, return
          * data */
    KTEXT_ST authent;              /* the authenticator we will build */
    u_char *act_st;                /* the pointer to the complete packet */
    u_char *priv_pak;              /* private version of the packet */
    int priv_len;                  /* length of private packet */
    u_long cksum;                  /* checksum of the packet */
    MSG_DAT mdat;
    u_char *return_dat;

    act_st = (u_char *) malloc(KADM_VERSIZE); /* verstr stored first */
    (void) strncpy((char *)act_st, KADM_VERSTR, KADM_VERSIZE);
    act_len = KADM_VERSIZE;

    if ((retdat = kadm_cli_keyd(sess_key, sess_sched)) != KADM_SUCCESS) {
        free((char *)act_st);
        return retdat;         /* couldnt get key working */
    }
    priv_pak = (u_char *) malloc((unsigned)(st_siz + 200));
    /* 200 bytes for extra info case */
    if ((priv_len = krb_mk_priv(st_dat, priv_pak, (u_long)st_siz,
                                sess_sched, sess_key, &clientParm.my_addr,
                                &clientParm.admin_addr)) < 0)
        RET_N_FREE(KADM_NO_ENCRYPT);    /* whoops... we got a lose
                                         * here */
    /* here is the length of priv data.  receiver calcs
       size of authenticator by subtracting vno size, priv size, and
       sizeof(u_long) (for the size indication) from total size */

    act_len += vts_long((u_long) priv_len, &act_st, act_len);
#ifdef NOENCRYPTION
    cksum = 0;
#else
    cksum = quad_cksum(priv_pak, (u_long *)0, (long)priv_len, 0,
                       sess_key);
#endif
    if (retdat = krb_mk_req(&authent, clientParm.sname, clientParm.sinst,
                            clientParm.krbrlm, (long)cksum)) {
        /* authenticator? */
        RET_N_FREE(retdat + krb_err_base);
    }

    act_st = (u_char *) realloc((char *) act_st,
                                (unsigned) (act_len + authent.length
                                            + priv_len));
    if (!act_st) {
        clear_secrets();
        free((char *)priv_pak);
        return(KADM_NOMEM);
    }
    bcopy((char *) authent.dat, (char *) act_st + act_len, authent.length);
    bcopy((char *) priv_pak, (char *) act_st + act_len + authent.length,
          priv_len);
    free((char *)priv_pak);
    if ((retdat = kadm_cli_out(act_st,
                               act_len + authent.length + priv_len,
                               ret_dat, ret_siz)) != KADM_SUCCESS)
        RET_N_FREE(retdat);
    free((char *)act_st);
#define RET_N_FREE2(r) {free((char *)*ret_dat); *ret_dat = 0; *ret_siz = 0; clear_secrets(); return(r);}

    /* first see if it's a YOULOUSE */
    if ((*ret_siz >= KADM_VERSIZE) &&
        !strncmp(KADM_ULOSE, (char *)*ret_dat, KADM_VERSIZE)) {
        u_long errcode;
        /* it's a youlose packet */
        if (*ret_siz < KADM_VERSIZE + sizeof(u_long))
            RET_N_FREE2(KADM_BAD_VER);
        bcopy((char *)(*ret_dat) + KADM_VERSIZE, (char *)&errcode,
              sizeof(u_long));
        retdat = /* (int) NO! */ ntohl(errcode);
        RET_N_FREE2(retdat);
    }
    /* need to decode the ret_dat */
    if (retdat = krb_rd_priv(*ret_dat, (u_long)*ret_siz, sess_sched,
                             sess_key, &clientParm.admin_addr,
                             &clientParm.my_addr, &mdat))
        RET_N_FREE2(retdat+krb_err_base);
    if (mdat.app_length < KADM_VERSIZE + 4)
        /* too short! */
        RET_N_FREE2(KADM_BAD_VER);
    if (strncmp((char *)mdat.app_data, KADM_VERSTR, KADM_VERSIZE))
        /* bad version */
        RET_N_FREE2(KADM_BAD_VER);
    bcopy((char *)mdat.app_data+KADM_VERSIZE,
          (char *)&retdat, sizeof(u_long));
    retdat = ntohl((u_long)retdat);
    if ((mdat.app_length - KADM_VERSIZE - sizeof(u_long)) != 0) {
        if (!(return_dat = (u_char *)
              malloc((unsigned)(mdat.app_length - KADM_VERSIZE -
                                sizeof(u_long)))))
	    RET_N_FREE2(KADM_NOMEM);
        bcopy((char *) mdat.app_data + KADM_VERSIZE + sizeof(u_long),
              (char *)return_dat,
              (int)mdat.app_length - KADM_VERSIZE - sizeof(u_long));
    } else {
        /* If it's zero length, still need to malloc a 1 byte string; */
        /* malloc's of zero will return NULL on AIX & A/UX */
        if (!(return_dat = (u_char *) malloc((unsigned) 1)))
	    RET_N_FREE2(KADM_NOMEM);
        *return_dat = '\0';
    }
    free((char *)*ret_dat);
    clear_secrets();
    *ret_dat = return_dat;
    *ret_siz = mdat.app_length - KADM_VERSIZE - sizeof(u_long);
    return retdat;
}

/* takes in the sess_key and key_schedule and sets them appropriately */
long /* int */ kadm_cli_keyd(s_k, s_s)
    des_cblock s_k;                        /* session key */
des_key_schedule s_s;                  /* session key schedule */
{
    CREDENTIALS cred;              /* to get key data */
    long /* int */ stat;

    /* want .sname and .sinst here.... */
    if (stat = krb_get_cred(clientParm.sname, clientParm.sinst,
                            clientParm.krbrlm, &cred))
        return stat + krb_err_base;
    bcopy((char *) cred.session, (char *) s_k, sizeof(des_cblock));
    bzero((char *) cred.session, sizeof(des_cblock));
#ifdef NOENCRYPTION
    bzero(s_s, sizeof(des_key_schedule));
#else
    if (stat = key_sched(s_k, s_s))
        return stat + krb_err_base;
#endif
    return KADM_SUCCESS;
}
/* This code "works" */

static
long /* WHY INT??????????????? */
kadm_cli_conn(
    HANDLE hInst,
    HANDLE hWnd
    )
{
    /* this connects and sets my_addr */
    int on = 1;
    int ret = SOCKET_ERROR;
    u_long lOnOff = TRUE;
    int nWSAerror, i;
    struct sockaddr_in  intAddr;
    fd_set writefds; /* select(): file descriptor set */
    fd_set exceptfds; /* for exceptional condition pending */
    struct timeval connTimeOut;

    if (clientParm.nSocketState == STATE_NONE) {
    	if ((clientParm.admin_fd =
             socket(clientParm.admin_addr.sin_family, SOCK_STREAM,0)) < 0)
            return KADM_NO_SOCK;            /* couldnt create the socket */
    }
    clientParm.nSocketState = STATE_OPEN;

    clientParm.my_addr_len = sizeof(clientParm.my_addr);

    ret = connect(clientParm.admin_fd,
                  (struct sockaddr *) &clientParm.admin_addr,
                  sizeof(clientParm.admin_addr));

    for (i=0;;) {
        if (ret == SOCKET_ERROR) {
	    if (KADM_SUCCESS == (nWSAerror = WSAGetLastError())) {
                break; // has been connected!!
	    } else if ((nWSAerror != WSAEWOULDBLOCK) &&
                       (nWSAerror != WSAEALREADY))
            {
                if (WSAIsBlocking()) {
                    WSACancelBlockingCall();
                }
                ret = closesocket(clientParm.admin_fd);
#if 0
                if (ret == SOCKET_ERROR) {
                    MessageBox(NULL, (LPSTR) WSErrorString(WSAGetLastError()),
                               NULL, MB_OK);
                }
#endif
	    }
	    clientParm.admin_fd = -1;
#if 0
	    if (ret == SOCKET_ERROR) {
		MessageBox(NULL, (LPSTR) WSErrorString(WSAGetLastError()),
                           NULL, MB_OK);
	    }
#endif
	    return KADM_NO_CONN;    /* couldnt get the connect */
        }

        /*make the socket non-blocking */
        ret = ioctlsocket(clientParm.admin_fd, FIONBIO, (u_long FAR *)&lOnOff);
        if (ret) {
	    non_block_close(hInst,hWnd);
	    return( KADM_NO_CONN) ;
        }


        if (getsockname(clientParm.admin_fd,
                        (struct sockaddr *) &clientParm.my_addr,
                        &clientParm.my_addr_len) < 0)
        {
            (void) closesocket(clientParm.admin_fd);
            clientParm.admin_fd = -1;
            return KADM_NO_HERE;            /* couldnt find out who we are */

        }

//#ifdef PC
// #ifdef WINSOCK
//	(void) closesocket(clientParm.admin_fd);
// #else
//	(void) soclose(clientParm.admin_fd);
// #endif // end WINSOCK
//#else
//	(void) close(clientParm.admin_fd);
//#endif  // end PC
//	clientParm.admin_fd = -1;
//	return KADM_NO_CONN;            /* couldnt get the connect */
        //}

	FD_ZERO((fd_set FAR*)&(writefds));
	FD_ZERO((fd_set FAR*)&(exceptfds));
	FD_SET(clientParm.admin_fd,(fd_set FAR *)&(writefds));
	FD_SET(clientParm.admin_fd,(fd_set FAR *)&(exceptfds));

	connTimeOut.tv_sec = CONN_TIMEOUT;
	connTimeOut.tv_usec = 0;
	/* select(): either connected or timeout */
	/* check if connecting timeout, error or wrong file descriptor */
	ret = select(-1,NULL,(fd_set FAR *)&(writefds),
                     (fd_set FAR *)&(exceptfds),
                     (struct timeval FAR *)&(connTimeOut));
	if (ret == SOCKET_ERROR) {
            non_block_close(hInst, hWnd);
            return KADM_NO_CONN;
	} else if (ret > 0) {
            if (FD_ISSET(clientParm.admin_fd, (fd_set FAR *)&(exceptfds))) {
                non_block_close(hInst,hWnd);
#if 0
                MessageBox(NULL, WSErrorString("select(execptfds)"),
                           NULL, MB_OK);
#endif
                return KADM_NO_CONN;
            }
            if (FD_ISSET(clientParm.admin_fd, (fd_set FAR*)&(writefds))) {
                break;
            } else {
#if 0
                MessageBox(NULL,
                           WSErrorString("Unexpected error from select()"),
                           "Error!", MB_OK | MB_ICONHAND);
#endif
                non_block_close(hInst, hWnd);
                return KADM_NO_CONN;
            }

	} else {
            /* ret is 0, select() timeout */
            i++; /* timeout counter */

            if (i >= CONN_RETRIES) {
#if 0
                MessageBox(NULL, WSErrorString("select() timeout"),
                           "TimeOut: exceeded", MB_OK | MB_ICONHAND);
#endif
                non_block_close(hInst, hWnd);
                return KADM_NO_CONN;
            }
	}
    }  /* end for()*/
    clientParm.nSocketState = STATE_CONNECTED;
    return KADM_SUCCESS;
}

static
void
kadm_cli_disconn()
{
    (void) closesocket(clientParm.admin_fd);
    return;
}

long /* ARGH! */
kadm_cli_out(
    u_char *dat,
    int dat_len,
    u_char **ret_dat,
    int *ret_siz
    )
{
    /* extern int errno; */
    u_short dlen;
    long /* int */ retval;

    dlen = (u_short) dat_len;

    if (dat_len != (int)dlen)
        return (KADM_NO_ROOM);

    dlen = htons(dlen);
    if (krb_net_write(clientParm.admin_fd, (char *) &dlen,
                      sizeof(u_short)) < 0)
        return (errno);        /* XXX */
    /* jms 8/23/93 -- what's XXX about this? */

    if (krb_net_write(clientParm.admin_fd, (char *) dat, dat_len) < 0)
        return (errno);        /* XXX */

    if (retval = krb_net_read(clientParm.admin_fd, (char *) &dlen,
                              sizeof(u_short)) != sizeof(u_short)) {
        if (retval < 0)
            return(errno); /* XXX */
#if !defined(MacOS) && !defined(WINDOWS) && !defined(OS2)
        /* krb_net_read doesn't return short read, that's the whole point! */
        else
            return(EPIPE);          /* short read ! */
#endif
    }

    dlen = ntohs(dlen);
    *ret_dat = (u_char *)malloc((unsigned)dlen);
    if (!*ret_dat)
        return(KADM_NOMEM);

    if (retval = krb_net_read(clientParm.admin_fd, (char *) *ret_dat,
                              (int) dlen) != dlen) {
        if (retval < 0)
            return(errno);          /* XXX */
#if !defined(MacOS) && !defined(WINDOWS) && !defined(OS2)
        else
            return(EPIPE);          /* short read ! */
#endif
    }
    *ret_siz = (int) dlen;
    return KADM_SUCCESS;
}

/*
 * non-blocking mode close, it cannot handle an WSAEWOULDBLOCK error gracefully
 */
void
non_block_close(
    HANDLE hInst,
    HANDLE hWnd
    )
{
    int ret, nWSAerror;
    clientParm.nSocketState = STATE_CLOSE_PENDING;
    /* cancel any waiting process */
    if (WSAIsBlocking()) {
        WSACancelBlockingCall();
    }
    ret = closesocket(clientParm.admin_fd);
#if 0
    if (ret == SOCKET_ERROR) {
        if (WSAEWOULDBLOCK != WSAGetLastError()) {
            MessageBox(NULL, WSErrorString("closesocket()"), NULL, MB_OK);
        }
    }
#endif
    clientParm.nSocketState = STATE_NONE;
}
