

/* this ALWAYS GENERATED file contains the RPC server stubs */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Wed Jan 14 01:44:14 2004
 */
/* Compiler settings for ccapi.idl:
    Oic, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, oldnames
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extent to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#include <string.h>
#include "ccapi.h"

#define TYPE_FORMAT_STRING_SIZE   585                               
#define PROC_FORMAT_STRING_SIZE   583                               
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};

extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;

/* Standard interface: ccapi, ver. 1.0,
   GUID={0xf4862da3,0xa764,0x4a7c,{0x8c,0x03,0x85,0xf0,0xe0,0xc9,0x90,0x62}} */


extern const MIDL_SERVER_INFO ccapi_ServerInfo;

extern RPC_DISPATCH_TABLE ccapi_DispatchTable;

static const RPC_SERVER_INTERFACE ccapi___RpcServerInterface =
    {
    sizeof(RPC_SERVER_INTERFACE),
    {{0xf4862da3,0xa764,0x4a7c,{0x8c,0x03,0x85,0xf0,0xe0,0xc9,0x90,0x62}},{1,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    &ccapi_DispatchTable,
    0,
    0,
    0,
    &ccapi_ServerInfo,
    0x04000000
    };
RPC_IF_HANDLE ccapi_ServerIfHandle = (RPC_IF_HANDLE)& ccapi___RpcServerInterface;

extern const MIDL_STUB_DESC ccapi_StubDesc;

void __RPC_STUB
ccapi_rcc_store(
    PRPC_MESSAGE _pRpcMessage )
{
    CC_INT32 _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    void *_p_cred;
    NDR_SCONTEXT cache;
    CRED_UNION cred;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &ccapi_StubDesc);
    
    _p_cred = &cred;
    MIDL_memset(
               _p_cred,
               0,
               sizeof( CRED_UNION  ));
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[440] );
            
            cache = NdrServerContextUnmarshall(( PMIDL_STUB_MESSAGE  )&_StubMsg);
            
            NdrComplexStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                        (unsigned char * *)&_p_cred,
                                        (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[534],
                                        (unsigned char)0 );
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        
        _RetVal = rcc_store(( HCACHE  )*NDRSContextValue(cache),cred);
        
        _StubMsg.BufferLength = 8;
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char *) _pRpcMessage->Buffer;
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( CC_INT32 * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrComplexStructFree( &_StubMsg,
                              (unsigned char *)&cred,
                              &__MIDL_TypeFormatString.Format[534] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}

void __RPC_STUB
ccapi_rcc_remove_cred(
    PRPC_MESSAGE _pRpcMessage )
{
    CC_INT32 _RetVal;
    MIDL_STUB_MESSAGE _StubMsg;
    void *_p_cred;
    NDR_SCONTEXT cache;
    CRED_UNION cred;
    RPC_STATUS _Status;
    
    ((void)(_Status));
    NdrServerInitializeNew(
                          _pRpcMessage,
                          &_StubMsg,
                          &ccapi_StubDesc);
    
    _p_cred = &cred;
    MIDL_memset(
               _p_cred,
               0,
               sizeof( CRED_UNION  ));
    RpcTryFinally
        {
        RpcTryExcept
            {
            if ( (_pRpcMessage->DataRepresentation & 0X0000FFFFUL) != NDR_LOCAL_DATA_REPRESENTATION )
                NdrConvert( (PMIDL_STUB_MESSAGE) &_StubMsg, (PFORMAT_STRING) &__MIDL_ProcFormatString.Format[440] );
            
            cache = NdrServerContextUnmarshall(( PMIDL_STUB_MESSAGE  )&_StubMsg);
            
            NdrComplexStructUnmarshall( (PMIDL_STUB_MESSAGE) &_StubMsg,
                                        (unsigned char * *)&_p_cred,
                                        (PFORMAT_STRING) &__MIDL_TypeFormatString.Format[534],
                                        (unsigned char)0 );
            
            if(_StubMsg.Buffer > _StubMsg.BufferEnd)
                {
                RpcRaiseException(RPC_X_BAD_STUB_DATA);
                }
            }
        RpcExcept( RPC_BAD_STUB_DATA_EXCEPTION_FILTER )
            {
            RpcRaiseException(RPC_X_BAD_STUB_DATA);
            }
        RpcEndExcept
        
        _RetVal = rcc_remove_cred(( HCACHE  )*NDRSContextValue(cache),cred);
        
        _StubMsg.BufferLength = 8;
        _pRpcMessage->BufferLength = _StubMsg.BufferLength;
        
        _Status = I_RpcGetBuffer( _pRpcMessage ); 
        if ( _Status )
            RpcRaiseException( _Status );
        
        _StubMsg.Buffer = (unsigned char *) _pRpcMessage->Buffer;
        
        _StubMsg.Buffer = (unsigned char *)(((long)_StubMsg.Buffer + 3) & ~ 0x3);
        *(( CC_INT32 * )_StubMsg.Buffer)++ = _RetVal;
        
        }
    RpcFinally
        {
        NdrComplexStructFree( &_StubMsg,
                              (unsigned char *)&cred,
                              &__MIDL_TypeFormatString.Format[534] );
        
        }
    RpcEndFinally
    _pRpcMessage->BufferLength = 
        (unsigned int)(_StubMsg.Buffer - (unsigned char *)_pRpcMessage->Buffer);
    
}

extern const NDR_RUNDOWN RundownRoutines[];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT351_OR_WIN95_OR_LATER)
#error You need a Windows NT 3.51 or Windows95 or later to run this stub because it uses these features:
#error   old (-Oic) stubless proxies.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {
			0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 10 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 12 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */
/* 14 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 16 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 18 */	NdrFcLong( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x1 ),	/* 1 */
/* 24 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 26 */	0x30,		/* FC_BIND_CONTEXT */
			0xe0,		/* Ctxt flags:  via ptr, in, out, */
/* 28 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 30 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 32 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 34 */	NdrFcShort( 0xa ),	/* Type Offset=10 */
/* 36 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 38 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 40 */	NdrFcLong( 0x0 ),	/* 0 */
/* 44 */	NdrFcShort( 0x2 ),	/* 2 */
/* 46 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 48 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 50 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 52 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 54 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 56 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */
/* 58 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 60 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 62 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 64 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 66 */	NdrFcLong( 0x0 ),	/* 0 */
/* 70 */	NdrFcShort( 0x3 ),	/* 3 */
/* 72 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 74 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 76 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 78 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 80 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 82 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */
/* 84 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 86 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */
/* 88 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 90 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */
/* 92 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 94 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 96 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 98 */	NdrFcShort( 0x1e ),	/* Type Offset=30 */
/* 100 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 102 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 104 */	NdrFcLong( 0x0 ),	/* 0 */
/* 108 */	NdrFcShort( 0x4 ),	/* 4 */
/* 110 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 112 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 114 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 116 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 118 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 120 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */
/* 122 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 124 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */
/* 126 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 128 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 130 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 132 */	NdrFcShort( 0x26 ),	/* Type Offset=38 */
/* 134 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 136 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 138 */	NdrFcLong( 0x0 ),	/* 0 */
/* 142 */	NdrFcShort( 0x5 ),	/* 5 */
/* 144 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 146 */	0x30,		/* FC_BIND_CONTEXT */
			0xe0,		/* Ctxt flags:  via ptr, in, out, */
/* 148 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 150 */	0x1,		/* 1 */
			0x0,		/* 0 */
/* 152 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 154 */	NdrFcShort( 0x2e ),	/* Type Offset=46 */
/* 156 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 158 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 160 */	NdrFcLong( 0x0 ),	/* 0 */
/* 164 */	NdrFcShort( 0x6 ),	/* 6 */
/* 166 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 168 */	0x30,		/* FC_BIND_CONTEXT */
			0xe0,		/* Ctxt flags:  via ptr, in, out, */
/* 170 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 172 */	0x1,		/* 1 */
			0x0,		/* 0 */
/* 174 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 176 */	NdrFcShort( 0x2e ),	/* Type Offset=46 */
/* 178 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 180 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 182 */	NdrFcLong( 0x0 ),	/* 0 */
/* 186 */	NdrFcShort( 0x7 ),	/* 7 */
/* 188 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 190 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 192 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 194 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 196 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 198 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */
/* 200 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 202 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */
/* 204 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 206 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 208 */	NdrFcLong( 0x0 ),	/* 0 */
/* 212 */	NdrFcShort( 0x8 ),	/* 8 */
/* 214 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 216 */	0x30,		/* FC_BIND_CONTEXT */
			0xe0,		/* Ctxt flags:  via ptr, in, out, */
/* 218 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 220 */	0x2,		/* 2 */
			0x0,		/* 0 */
/* 222 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 224 */	NdrFcShort( 0x3e ),	/* Type Offset=62 */
/* 226 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 228 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 230 */	NdrFcLong( 0x0 ),	/* 0 */
/* 234 */	NdrFcShort( 0x9 ),	/* 9 */
/* 236 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 238 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 240 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 242 */	0x2,		/* 2 */
			0x0,		/* 0 */
/* 244 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 246 */	NdrFcShort( 0x46 ),	/* Type Offset=70 */
/* 248 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 250 */	NdrFcShort( 0x4a ),	/* Type Offset=74 */
/* 252 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 254 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 256 */	NdrFcLong( 0x0 ),	/* 0 */
/* 260 */	NdrFcShort( 0xa ),	/* 10 */
/* 262 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 264 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 266 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 268 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 270 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 272 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */
/* 274 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 276 */	NdrFcShort( 0x52 ),	/* Type Offset=82 */
/* 278 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 280 */	NdrFcShort( 0x5a ),	/* Type Offset=90 */
/* 282 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 284 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 286 */	NdrFcLong( 0x0 ),	/* 0 */
/* 290 */	NdrFcShort( 0xb ),	/* 11 */
/* 292 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 294 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 296 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 298 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 300 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 302 */	NdrFcShort( 0x12 ),	/* Type Offset=18 */
/* 304 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 306 */	NdrFcShort( 0x62 ),	/* Type Offset=98 */
/* 308 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 310 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 312 */	NdrFcLong( 0x0 ),	/* 0 */
/* 316 */	NdrFcShort( 0xc ),	/* 12 */
/* 318 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 320 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 322 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 324 */	0x1,		/* 1 */
			0x0,		/* 0 */
/* 326 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 328 */	NdrFcShort( 0xc6 ),	/* Type Offset=198 */
/* 330 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 332 */	NdrFcShort( 0xca ),	/* Type Offset=202 */
/* 334 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 336 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 338 */	NdrFcLong( 0x0 ),	/* 0 */
/* 342 */	NdrFcShort( 0xd ),	/* 13 */
/* 344 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 346 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 348 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 350 */	0x1,		/* 1 */
			0x0,		/* 0 */
/* 352 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 354 */	NdrFcShort( 0xc6 ),	/* Type Offset=198 */
/* 356 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 358 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 360 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */
/* 362 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 364 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 366 */	NdrFcLong( 0x0 ),	/* 0 */
/* 370 */	NdrFcShort( 0xe ),	/* 14 */
/* 372 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 374 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 376 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 378 */	0x1,		/* 1 */
			0x0,		/* 0 */
/* 380 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 382 */	NdrFcShort( 0xc6 ),	/* Type Offset=198 */
/* 384 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 386 */	NdrFcShort( 0xca ),	/* Type Offset=202 */
/* 388 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 390 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 392 */	NdrFcLong( 0x0 ),	/* 0 */
/* 396 */	NdrFcShort( 0xf ),	/* 15 */
/* 398 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 400 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 402 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 404 */	0x1,		/* 1 */
			0x0,		/* 0 */
/* 406 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 408 */	NdrFcShort( 0xc6 ),	/* Type Offset=198 */
/* 410 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 412 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */
/* 414 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 416 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 418 */	NdrFcLong( 0x0 ),	/* 0 */
/* 422 */	NdrFcShort( 0x10 ),	/* 16 */
/* 424 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 426 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 428 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 430 */	0x1,		/* 1 */
			0x0,		/* 0 */
/* 432 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 434 */	NdrFcShort( 0xc6 ),	/* Type Offset=198 */
/* 436 */	0x4e,		/* FC_IN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 438 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 440 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 442 */	NdrFcShort( 0xc6 ),	/* Type Offset=198 */
/* 444 */	
			0x4d,		/* FC_IN_PARAM */
			0x2,		/* x86 stack size = 2 */
/* 446 */	NdrFcShort( 0x216 ),	/* Type Offset=534 */
/* 448 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 450 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 452 */	NdrFcLong( 0x0 ),	/* 0 */
/* 456 */	NdrFcShort( 0x13 ),	/* 19 */
/* 458 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 460 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 462 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 464 */	0x1,		/* 1 */
			0x0,		/* 0 */
/* 466 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 468 */	NdrFcShort( 0xc6 ),	/* Type Offset=198 */
/* 470 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 472 */	NdrFcShort( 0x224 ),	/* Type Offset=548 */
/* 474 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 476 */	NdrFcShort( 0x22c ),	/* Type Offset=556 */
/* 478 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 480 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 482 */	NdrFcLong( 0x0 ),	/* 0 */
/* 486 */	NdrFcShort( 0x14 ),	/* 20 */
/* 488 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 490 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 492 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 494 */	0x1,		/* 1 */
			0x0,		/* 0 */
/* 496 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 498 */	NdrFcShort( 0xc6 ),	/* Type Offset=198 */
/* 500 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 502 */	NdrFcShort( 0x234 ),	/* Type Offset=564 */
/* 504 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 506 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 508 */	NdrFcLong( 0x0 ),	/* 0 */
/* 512 */	NdrFcShort( 0x15 ),	/* 21 */
/* 514 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 516 */	0x30,		/* FC_BIND_CONTEXT */
			0xe0,		/* Ctxt flags:  via ptr, in, out, */
/* 518 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 520 */	0x3,		/* 3 */
			0x0,		/* 0 */
/* 522 */	
			0x50,		/* FC_IN_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 524 */	NdrFcShort( 0x23c ),	/* Type Offset=572 */
/* 526 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 528 */	0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/* 530 */	NdrFcLong( 0x0 ),	/* 0 */
/* 534 */	NdrFcShort( 0x16 ),	/* 22 */
/* 536 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 538 */	0x30,		/* FC_BIND_CONTEXT */
			0x40,		/* Ctxt flags:  in, */
/* 540 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 542 */	0x3,		/* 3 */
			0x0,		/* 0 */
/* 544 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 546 */	NdrFcShort( 0x244 ),	/* Type Offset=580 */
/* 548 */	
			0x51,		/* FC_OUT_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 550 */	NdrFcShort( 0x22c ),	/* Type Offset=556 */
/* 552 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 554 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 556 */	NdrFcLong( 0x0 ),	/* 0 */
/* 560 */	NdrFcShort( 0x17 ),	/* 23 */
/* 562 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 564 */	
			0x4d,		/* FC_IN_PARAM */
			0x1,		/* x86 stack size = 1 */
/* 566 */	NdrFcShort( 0x1a ),	/* Type Offset=26 */
/* 568 */	0x53,		/* FC_RETURN_PARAM_BASETYPE */
			0x8,		/* FC_LONG */
/* 570 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x48,		/* Old Flags:  */
/* 572 */	NdrFcLong( 0x0 ),	/* 0 */
/* 576 */	NdrFcShort( 0x18 ),	/* 24 */
/* 578 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 580 */	0x5b,		/* FC_END */
			0x5c,		/* FC_PAD */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/*  4 */	NdrFcShort( 0x2 ),	/* Offset= 2 (6) */
/*  6 */	0x30,		/* FC_BIND_CONTEXT */
			0xa0,		/* Ctxt flags:  via ptr, out, */
/*  8 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 10 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 12 */	NdrFcShort( 0x2 ),	/* Offset= 2 (14) */
/* 14 */	0x30,		/* FC_BIND_CONTEXT */
			0xe1,		/* Ctxt flags:  via ptr, in, out, can't be null */
/* 16 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 18 */	0x30,		/* FC_BIND_CONTEXT */
			0x41,		/* Ctxt flags:  in, can't be null */
/* 20 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 22 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 24 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 26 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 28 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 30 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 32 */	NdrFcShort( 0x2 ),	/* Offset= 2 (34) */
/* 34 */	0x30,		/* FC_BIND_CONTEXT */
			0xa0,		/* Ctxt flags:  via ptr, out, */
/* 36 */	0x1,		/* 1 */
			0x5,		/* 5 */
/* 38 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 40 */	NdrFcShort( 0x2 ),	/* Offset= 2 (42) */
/* 42 */	0x30,		/* FC_BIND_CONTEXT */
			0xa0,		/* Ctxt flags:  via ptr, out, */
/* 44 */	0x1,		/* 1 */
			0x4,		/* 4 */
/* 46 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 48 */	NdrFcShort( 0x2 ),	/* Offset= 2 (50) */
/* 50 */	0x30,		/* FC_BIND_CONTEXT */
			0xe1,		/* Ctxt flags:  via ptr, in, out, can't be null */
/* 52 */	0x1,		/* 1 */
			0x0,		/* 0 */
/* 54 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 56 */	NdrFcShort( 0x2 ),	/* Offset= 2 (58) */
/* 58 */	0x30,		/* FC_BIND_CONTEXT */
			0xa0,		/* Ctxt flags:  via ptr, out, */
/* 60 */	0x2,		/* 2 */
			0x1,		/* 1 */
/* 62 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 64 */	NdrFcShort( 0x2 ),	/* Offset= 2 (66) */
/* 66 */	0x30,		/* FC_BIND_CONTEXT */
			0xe1,		/* Ctxt flags:  via ptr, in, out, can't be null */
/* 68 */	0x2,		/* 2 */
			0x0,		/* 0 */
/* 70 */	0x30,		/* FC_BIND_CONTEXT */
			0x41,		/* Ctxt flags:  in, can't be null */
/* 72 */	0x2,		/* 2 */
			0x0,		/* 0 */
/* 74 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 76 */	NdrFcShort( 0x2 ),	/* Offset= 2 (78) */
/* 78 */	0x30,		/* FC_BIND_CONTEXT */
			0xa0,		/* Ctxt flags:  via ptr, out, */
/* 80 */	0x1,		/* 1 */
			0x1,		/* 1 */
/* 82 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 84 */	NdrFcShort( 0x2 ),	/* Offset= 2 (86) */
/* 86 */	0x30,		/* FC_BIND_CONTEXT */
			0xe0,		/* Ctxt flags:  via ptr, in, out, */
/* 88 */	0x2,		/* 2 */
			0x1,		/* 1 */
/* 90 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 92 */	NdrFcShort( 0x2 ),	/* Offset= 2 (94) */
/* 94 */	0x30,		/* FC_BIND_CONTEXT */
			0xa0,		/* Ctxt flags:  via ptr, out, */
/* 96 */	0x1,		/* 1 */
			0x2,		/* 2 */
/* 98 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 100 */	NdrFcShort( 0x2 ),	/* Offset= 2 (102) */
/* 102 */	
			0x12, 0x0,	/* FC_UP */
/* 104 */	NdrFcShort( 0x4a ),	/* Offset= 74 (178) */
/* 106 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 108 */	NdrFcShort( 0xc ),	/* 12 */
/* 110 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 112 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 114 */	NdrFcShort( 0x0 ),	/* 0 */
/* 116 */	NdrFcShort( 0x0 ),	/* 0 */
/* 118 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 120 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 122 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 124 */	NdrFcShort( 0x4 ),	/* 4 */
/* 126 */	NdrFcShort( 0x4 ),	/* 4 */
/* 128 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 130 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 132 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 134 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 136 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 138 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 140 */	NdrFcShort( 0xc ),	/* 12 */
/* 142 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 144 */	NdrFcShort( 0x0 ),	/* 0 */
/* 146 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 148 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 150 */	NdrFcShort( 0xc ),	/* 12 */
/* 152 */	NdrFcShort( 0x0 ),	/* 0 */
/* 154 */	NdrFcShort( 0x2 ),	/* 2 */
/* 156 */	NdrFcShort( 0x0 ),	/* 0 */
/* 158 */	NdrFcShort( 0x0 ),	/* 0 */
/* 160 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 162 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 164 */	NdrFcShort( 0x4 ),	/* 4 */
/* 166 */	NdrFcShort( 0x4 ),	/* 4 */
/* 168 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 170 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 172 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 174 */	0x0,		/* 0 */
			NdrFcShort( 0xffbb ),	/* Offset= -69 (106) */
			0x5b,		/* FC_END */
/* 178 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 180 */	NdrFcShort( 0x8 ),	/* 8 */
/* 182 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 184 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 186 */	NdrFcShort( 0x4 ),	/* 4 */
/* 188 */	NdrFcShort( 0x4 ),	/* 4 */
/* 190 */	0x12, 0x0,	/* FC_UP */
/* 192 */	NdrFcShort( 0xffca ),	/* Offset= -54 (138) */
/* 194 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 196 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 198 */	0x30,		/* FC_BIND_CONTEXT */
			0x41,		/* Ctxt flags:  in, can't be null */
/* 200 */	0x1,		/* 1 */
			0x0,		/* 0 */
/* 202 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 204 */	NdrFcShort( 0x2 ),	/* Offset= 2 (206) */
/* 206 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 208 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 210 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x8,		/* FC_LONG */
/* 212 */	0x8,		/* Corr desc: FC_LONG */
			0x0,		/*  */
/* 214 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 216 */	NdrFcShort( 0x2 ),	/* Offset= 2 (218) */
/* 218 */	NdrFcShort( 0x4 ),	/* 4 */
/* 220 */	NdrFcShort( 0x2 ),	/* 2 */
/* 222 */	NdrFcLong( 0x1 ),	/* 1 */
/* 226 */	NdrFcShort( 0xa ),	/* Offset= 10 (236) */
/* 228 */	NdrFcLong( 0x2 ),	/* 2 */
/* 232 */	NdrFcShort( 0x48 ),	/* Offset= 72 (304) */
/* 234 */	NdrFcShort( 0xffff ),	/* Offset= -1 (233) */
/* 236 */	
			0x12, 0x0,	/* FC_UP */
/* 238 */	NdrFcShort( 0x14 ),	/* Offset= 20 (258) */
/* 240 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 242 */	NdrFcShort( 0x29 ),	/* 41 */
/* 244 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 246 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 248 */	NdrFcShort( 0x8 ),	/* 8 */
/* 250 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 252 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 254 */	NdrFcShort( 0x4e2 ),	/* 1250 */
/* 256 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 258 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 260 */	NdrFcShort( 0x5d4 ),	/* 1492 */
/* 262 */	NdrFcShort( 0x0 ),	/* 0 */
/* 264 */	NdrFcShort( 0x0 ),	/* Offset= 0 (264) */
/* 266 */	0x2,		/* FC_CHAR */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 268 */	0x0,		/* 0 */
			NdrFcShort( 0xffe3 ),	/* Offset= -29 (240) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 272 */	0x0,		/* 0 */
			NdrFcShort( 0xffdf ),	/* Offset= -33 (240) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 276 */	0x0,		/* 0 */
			NdrFcShort( 0xffdb ),	/* Offset= -37 (240) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 280 */	0x0,		/* 0 */
			NdrFcShort( 0xffd7 ),	/* Offset= -41 (240) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 284 */	0x0,		/* 0 */
			NdrFcShort( 0xffd3 ),	/* Offset= -45 (240) */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 288 */	0x0,		/* 0 */
			NdrFcShort( 0xffd5 ),	/* Offset= -43 (246) */
			0x3e,		/* FC_STRUCTPAD2 */
/* 292 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 294 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 296 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 298 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 300 */	NdrFcShort( 0xffd0 ),	/* Offset= -48 (252) */
/* 302 */	0x3e,		/* FC_STRUCTPAD2 */
			0x5b,		/* FC_END */
/* 304 */	
			0x12, 0x0,	/* FC_UP */
/* 306 */	NdrFcShort( 0x80 ),	/* Offset= 128 (434) */
/* 308 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 310 */	NdrFcShort( 0x1 ),	/* 1 */
/* 312 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 314 */	NdrFcShort( 0xc ),	/* 12 */
/* 316 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 318 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 320 */	NdrFcShort( 0x1 ),	/* 1 */
/* 322 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 324 */	NdrFcShort( 0x4 ),	/* 4 */
/* 326 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 328 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 330 */	NdrFcShort( 0xc ),	/* 12 */
/* 332 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 334 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 336 */	NdrFcShort( 0x8 ),	/* 8 */
/* 338 */	NdrFcShort( 0x8 ),	/* 8 */
/* 340 */	0x12, 0x0,	/* FC_UP */
/* 342 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (318) */
/* 344 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 346 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 348 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 350 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 352 */	NdrFcShort( 0xc ),	/* 12 */
/* 354 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 356 */	NdrFcShort( 0x2c ),	/* 44 */
/* 358 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 360 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 362 */	NdrFcShort( 0xc ),	/* 12 */
/* 364 */	NdrFcShort( 0x0 ),	/* 0 */
/* 366 */	NdrFcShort( 0x1 ),	/* 1 */
/* 368 */	NdrFcShort( 0x8 ),	/* 8 */
/* 370 */	NdrFcShort( 0x8 ),	/* 8 */
/* 372 */	0x12, 0x0,	/* FC_UP */
/* 374 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (318) */
/* 376 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 378 */	0x0,		/* 0 */
			NdrFcShort( 0xffcd ),	/* Offset= -51 (328) */
			0x5b,		/* FC_END */
/* 382 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 384 */	NdrFcShort( 0x1 ),	/* 1 */
/* 386 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 388 */	NdrFcShort( 0x38 ),	/* 56 */
/* 390 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 392 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 394 */	NdrFcShort( 0x1 ),	/* 1 */
/* 396 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 398 */	NdrFcShort( 0x44 ),	/* 68 */
/* 400 */	0x2,		/* FC_CHAR */
			0x5b,		/* FC_END */
/* 402 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 404 */	NdrFcShort( 0xc ),	/* 12 */
/* 406 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 408 */	NdrFcShort( 0x4c ),	/* 76 */
/* 410 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 412 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 414 */	NdrFcShort( 0xc ),	/* 12 */
/* 416 */	NdrFcShort( 0x0 ),	/* 0 */
/* 418 */	NdrFcShort( 0x1 ),	/* 1 */
/* 420 */	NdrFcShort( 0x8 ),	/* 8 */
/* 422 */	NdrFcShort( 0x8 ),	/* 8 */
/* 424 */	0x12, 0x0,	/* FC_UP */
/* 426 */	NdrFcShort( 0xff94 ),	/* Offset= -108 (318) */
/* 428 */	
			0x5b,		/* FC_END */

			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 430 */	0x0,		/* 0 */
			NdrFcShort( 0xff99 ),	/* Offset= -103 (328) */
			0x5b,		/* FC_END */
/* 434 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 436 */	NdrFcShort( 0x54 ),	/* 84 */
/* 438 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 440 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 442 */	NdrFcShort( 0x0 ),	/* 0 */
/* 444 */	NdrFcShort( 0x0 ),	/* 0 */
/* 446 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 448 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 450 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 452 */	NdrFcShort( 0x4 ),	/* 4 */
/* 454 */	NdrFcShort( 0x4 ),	/* 4 */
/* 456 */	0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 458 */	
			0x22,		/* FC_C_CSTRING */
			0x5c,		/* FC_PAD */
/* 460 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 462 */	NdrFcShort( 0x10 ),	/* 16 */
/* 464 */	NdrFcShort( 0x10 ),	/* 16 */
/* 466 */	0x12, 0x0,	/* FC_UP */
/* 468 */	NdrFcShort( 0xff60 ),	/* Offset= -160 (308) */
/* 470 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 472 */	NdrFcShort( 0x30 ),	/* 48 */
/* 474 */	NdrFcShort( 0x30 ),	/* 48 */
/* 476 */	0x12, 0x0,	/* FC_UP */
/* 478 */	NdrFcShort( 0xff80 ),	/* Offset= -128 (350) */
/* 480 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 482 */	NdrFcShort( 0x3c ),	/* 60 */
/* 484 */	NdrFcShort( 0x3c ),	/* 60 */
/* 486 */	0x12, 0x0,	/* FC_UP */
/* 488 */	NdrFcShort( 0xff96 ),	/* Offset= -106 (382) */
/* 490 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 492 */	NdrFcShort( 0x48 ),	/* 72 */
/* 494 */	NdrFcShort( 0x48 ),	/* 72 */
/* 496 */	0x12, 0x0,	/* FC_UP */
/* 498 */	NdrFcShort( 0xff96 ),	/* Offset= -106 (392) */
/* 500 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 502 */	NdrFcShort( 0x50 ),	/* 80 */
/* 504 */	NdrFcShort( 0x50 ),	/* 80 */
/* 506 */	0x12, 0x0,	/* FC_UP */
/* 508 */	NdrFcShort( 0xff96 ),	/* Offset= -106 (402) */
/* 510 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 512 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 514 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 516 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 518 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 520 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 522 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 524 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 526 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 528 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 530 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 532 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 534 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 536 */	NdrFcShort( 0x8 ),	/* 8 */
/* 538 */	NdrFcShort( 0x0 ),	/* 0 */
/* 540 */	NdrFcShort( 0x0 ),	/* Offset= 0 (540) */
/* 542 */	0x8,		/* FC_LONG */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 544 */	0x0,		/* 0 */
			NdrFcShort( 0xfeb1 ),	/* Offset= -335 (210) */
			0x5b,		/* FC_END */
/* 548 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 550 */	NdrFcShort( 0x2 ),	/* Offset= 2 (552) */
/* 552 */	0x30,		/* FC_BIND_CONTEXT */
			0xe0,		/* Ctxt flags:  via ptr, in, out, */
/* 554 */	0x3,		/* 3 */
			0x1,		/* 1 */
/* 556 */	
			0x11, 0x10,	/* FC_RP [pointer_deref] */
/* 558 */	NdrFcShort( 0x2 ),	/* Offset= 2 (560) */
/* 560 */	
			0x12, 0x0,	/* FC_UP */
/* 562 */	NdrFcShort( 0xffe4 ),	/* Offset= -28 (534) */
/* 564 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 566 */	NdrFcShort( 0x2 ),	/* Offset= 2 (568) */
/* 568 */	0x30,		/* FC_BIND_CONTEXT */
			0xa0,		/* Ctxt flags:  via ptr, out, */
/* 570 */	0x3,		/* 3 */
			0x1,		/* 1 */
/* 572 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 574 */	NdrFcShort( 0x2 ),	/* Offset= 2 (576) */
/* 576 */	0x30,		/* FC_BIND_CONTEXT */
			0xe1,		/* Ctxt flags:  via ptr, in, out, can't be null */
/* 578 */	0x3,		/* 3 */
			0x0,		/* 0 */
/* 580 */	0x30,		/* FC_BIND_CONTEXT */
			0x41,		/* Ctxt flags:  in, can't be null */
/* 582 */	0x3,		/* 3 */
			0x0,		/* 0 */

			0x0
        }
    };

static const NDR_RUNDOWN RundownRoutines[] = 
    {
    HCTX_rundown
    ,HCACHE_rundown
    ,HCACHE_ITER_rundown
    ,HCRED_ITER_rundown
    };


static const unsigned short ccapi_FormatStringOffsetTable[] =
    {
    0,
    16,
    38,
    64,
    102,
    136,
    158,
    180,
    206,
    228,
    254,
    284,
    310,
    336,
    364,
    390,
    416,
    440,
    440,
    450,
    480,
    506,
    528,
    554,
    570
    };


static const MIDL_STUB_DESC ccapi_StubDesc = 
    {
    (void *)& ccapi___RpcServerInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    0,
    RundownRoutines,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x10001, /* Ndr library version */
    0,
    0x6000169, /* MIDL Version 6.0.361 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };

static RPC_DISPATCH_FUNCTION ccapi_table[] =
    {
    NdrServerCall,
    NdrServerCall,
    NdrServerCall,
    NdrServerCall,
    NdrServerCall,
    NdrServerCall,
    NdrServerCall,
    NdrServerCall,
    NdrServerCall,
    NdrServerCall,
    NdrServerCall,
    NdrServerCall,
    NdrServerCall,
    NdrServerCall,
    NdrServerCall,
    NdrServerCall,
    NdrServerCall,
    ccapi_rcc_store,
    ccapi_rcc_remove_cred,
    NdrServerCall,
    NdrServerCall,
    NdrServerCall,
    NdrServerCall,
    NdrServerCall,
    NdrServerCall,
    0
    };
RPC_DISPATCH_TABLE ccapi_DispatchTable = 
    {
    25,
    ccapi_table
    };

static const SERVER_ROUTINE ccapi_ServerRoutineTable[] = 
    {
    (SERVER_ROUTINE)rcc_initialize,
    (SERVER_ROUTINE)rcc_shutdown,
    (SERVER_ROUTINE)rcc_get_change_time,
    (SERVER_ROUTINE)rcc_create,
    (SERVER_ROUTINE)rcc_open,
    (SERVER_ROUTINE)rcc_close,
    (SERVER_ROUTINE)rcc_destroy,
    (SERVER_ROUTINE)rcc_seq_fetch_NCs_begin,
    (SERVER_ROUTINE)rcc_seq_fetch_NCs_end,
    (SERVER_ROUTINE)rcc_seq_fetch_NCs_next,
    (SERVER_ROUTINE)rcc_seq_fetch_NCs,
    (SERVER_ROUTINE)rcc_get_NC_info,
    (SERVER_ROUTINE)rcc_get_name,
    (SERVER_ROUTINE)rcc_set_principal,
    (SERVER_ROUTINE)rcc_get_principal,
    (SERVER_ROUTINE)rcc_get_cred_version,
    (SERVER_ROUTINE)rcc_lock_request,
    (SERVER_ROUTINE)rcc_store,
    (SERVER_ROUTINE)rcc_remove_cred,
    (SERVER_ROUTINE)rcc_seq_fetch_creds,
    (SERVER_ROUTINE)rcc_seq_fetch_creds_begin,
    (SERVER_ROUTINE)rcc_seq_fetch_creds_end,
    (SERVER_ROUTINE)rcc_seq_fetch_creds_next,
    (SERVER_ROUTINE)Connect,
    (SERVER_ROUTINE)Shutdown
    };

static const MIDL_SERVER_INFO ccapi_ServerInfo = 
    {
    &ccapi_StubDesc,
    ccapi_ServerRoutineTable,
    __MIDL_ProcFormatString.Format,
    ccapi_FormatStringOffsetTable,
    0,
    0,
    0,
    0};
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

