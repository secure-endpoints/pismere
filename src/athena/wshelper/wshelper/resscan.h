/*

  @doc

  @module resscan.h |
	WSHelper DNS/Hesiod Library for WINSOCK
	
*/

/* This file contains definitions for the name resolver scanning routines
 * used by the Hesiod library.
 *
 * For copying and distribution information, see the file <mit-copyright.h>.
 *
 * Original version by Steve Dyer, IBM/Project Athena, and Sam Hsu,
 * DEC/Project Athena.
 * 
 */

#ifndef WINDOWS
typedef struct rr {
    short type;			     /* RR type */
    short class;		     /* RR class */
    int dlen;			     /* len of data section */
    char *data;			     /* pointer to data */
} rr_t, *rr_p;
#else
// @struct rr_t | This structure is used by the resolve code
typedef struct rr {
    short type;			     /* @field RR type */
    short class;		     /* @field RR class */
    int dlen;			     /* @field len of data section */
    LPSTR data; 		     /* @field pointer to data */
} rr_t, far *rr_p;
#endif

#ifndef WINDOWS
typedef struct nsmsg {
    int len;			     /* sizeof(msg) */
    int ns_off;			     /* offset to name server RRs */
    int ar_off;			     /* offset to additional RRs */
    int count;			     /* total number of RRs */
    HEADER *hd;			     /* message header */
    rr_t rr;			     /* vector of (stripped-down) RR descriptors */
} nsmsg_t, *nsmsg_p;
#else
// @struct nsmsg_t | name server message structure
typedef struct nsmsg {
    int len;			     /* @field sizeof(msg) */
    int ns_off;			     /* @field offset to name server RRs */
    int ar_off;			     /* @field offset to additional RRs */
    int count;			     /* @field total number of RRs */
    HEADER far *hd;			 /* @field message header */
    rr_t rr;			     /* @field vector of (stripped-down) RR descriptors */
} nsmsg_t, far *nsmsg_p;
#endif

// @struct retransXretry | holds the retransmit and retry values
typedef struct retransXretry {
    short retrans;	// @field retransmit count
    short retry;	// @field retry count
} retransXretry_t, *retransXretry_p;


#define RES_INITCHECK() if(!(_res.options&RES_INIT))res_init();

#ifndef WINDOWS
extern struct state _res;
#endif /*WINDOWS*/

#ifdef WINDOWS
extern struct nsmsg far *res_scan();
extern struct nsmsg far *resolve();
extern struct nsmsg far *_resolve();
#endif
