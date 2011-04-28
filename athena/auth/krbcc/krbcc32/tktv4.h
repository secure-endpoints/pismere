//
// $Id: tktv4.h,v 1.1 1999/03/12 23:06:40 dalmeida Exp $
//
// TktV4.h: V4 ticket interface.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TKTV4_H__7005A5CB_2088_11D1_BA6D_00C04FD49F29__INCLUDED_)
#define AFX_TKTV4_H__7005A5CB_2088_11D1_BA6D_00C04FD49F29__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

	int 			v4DestroyTkt(V4Cred_type* pTkt);
	V4Cred_type* 	v4CreateTkt();

	int 			v4Set_ticket(V4Cred_type* pTkt, const unsigned char* ticket);
	int 			v4Set_address(V4Cred_type* pTkt, const char* address);
	int 			v4Set_lifetime(V4Cred_type* pTkt, const cc_int32 lifetime);
	int 			v4Set_issue_date(V4Cred_type* pTkt, const long issue_date);
	int 			v4Set_str_to_key(V4Cred_type* pTkt, const cc_int32 str_to_key);
	int 			v4Set_kvno(V4Cred_type* pTkt, const cc_int32 kvno);
	int 			v4Set_session_key(V4Cred_type* pTkt, const unsigned char* session_key);
	int 			v4Set_realm(V4Cred_type* pTkt, const char* realm);
	int 			v4Set_service_instance(V4Cred_type* pTkt, const char* service_instance);
	int 			v4Set_service(V4Cred_type* pTkt, const char* service);
	int 			v4Set_principal_instance(V4Cred_type* pTkt, const char* principal_instance);
	int 			v4Set_principal(V4Cred_type* pTkt, const char* principal);
	int 			v4Set_kversion(V4Cred_type* pTkt, BYTE kversion);

	unsigned char*	v4Get_ticket(V4Cred_type* pTkt);
	char*			v4Get_address(V4Cred_type* pTkt);
	cc_int32		v4Get_lifetime(V4Cred_type* pTkt);
	long			v4Get_issue_date(V4Cred_type* pTkt);
	cc_int32		v4Get_str_to_key(V4Cred_type* pTkt);
	cc_int32		v4Get_kvno(V4Cred_type* pTkt);
	unsigned char* 	v4Get_session_key(V4Cred_type* pTkt);
	char*			v4Get_realm(V4Cred_type* pTkt);
	char*			v4Get_service_instance(V4Cred_type* pTkt);
	char*			v4Get_service(V4Cred_type* pTkt);
	char*			v4Get_principal_instance(V4Cred_type* pTkt);
	char*			v4Get_principal(V4Cred_type* pTkt);
	BYTE 			v4Get_kversion(V4Cred_type* pTkt);


#endif // !defined(AFX_TKTV4_H__7005A5CB_2088_11D1_BA6D_00C04FD49F29__INCLUDED_)
