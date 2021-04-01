/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 3.02.88 */
/* at Fri Oct 11 08:32:45 2002
 */
/* Compiler settings for C:\PRJ\OPCToolbox\SOProxy\SOProxy.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_ISoftingOPCWatch = {0x8C727A71,0xAA9D,0x4415,{0xB4,0x9F,0x87,0xEF,0xD8,0x50,0x4D,0xBC}};


const IID IID_ISoftingOPCTrace = {0xB5C68CC6,0xE339,0x44ce,{0x82,0xDF,0x43,0x29,0x3D,0x97,0x08,0xED}};


const IID LIBID_SOProxy = {0xFC1DF082,0x1EC5,0x43ba,{0x8A,0x50,0x63,0x4D,0xDC,0x89,0x66,0x1E}};


#ifdef __cplusplus
}
#endif

