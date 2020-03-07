/*!
 *\file		des.c
 *\brief	des加密解密模块
 *\author	fancong
 *\date		2016年12月15日 11:42:10
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "desLib.h"

typedef unsigned char*	LPBYTE;
typedef unsigned char	BYTE;
typedef unsigned long	DWORD;
typedef unsigned int	UINT;
typedef unsigned long	ULONG;
typedef unsigned long*	ULONG_PTR;
typedef unsigned char	UInt8;
typedef unsigned int    Int32;

typedef struct _destable 
{
    unsigned long   keytab[16][2];
} DESTable;

#define ROTATE_LEFT(x, n)	rotl((x), (n))
#define ROTATE_RIGHT(x, n)	rotr((x), (n))

const  unsigned char double_shift[16] = {0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,0};

static BYTE DESParityTable[] = {0x00,0x01,0x01,0x02,0x01,0x02,0x02,0x03, 0x01,0x02,0x02,0x03,0x02,0x03,0x03,0x04};

const static DWORD Spbox[8][64] = 
{
	0x02080800,0x00080000,0x02000002,0x02080802,0x02000000,0x00080802,0x00080002,0x02000002,
	0x00080802,0x02080800,0x02080000,0x00000802,0x02000802,0x02000000,0x00000000,0x00080002,
	0x00080000,0x00000002,0x02000800,0x00080800,0x02080802,0x02080000,0x00000802,0x02000800,
	0x00000002,0x00000800,0x00080800,0x02080002,0x00000800,0x02000802,0x02080002,0x00000000,
	0x00000000,0x02080802,0x02000800,0x00080002,0x02080800,0x00080000,0x00000802,0x02000800,
	0x02080002,0x00000800,0x00080800,0x02000002,0x00080802,0x00000002,0x02000002,0x02080000,
	0x02080802,0x00080800,0x02080000,0x02000802,0x02000000,0x00000802,0x00080002,0x00000000,
	0x00080000,0x02000000,0x02000802,0x02080800,0x00000002,0x02080002,0x00000800,0x00080802,
	
	0x40108010,0x00000000,0x00108000,0x40100000,0x40000010,0x00008010,0x40008000,0x00108000,
	0x00008000,0x40100010,0x00000010,0x40008000,0x00100010,0x40108000,0x40100000,0x00000010,
	0x00100000,0x40008010,0x40100010,0x00008000,0x00108010,0x40000000,0x00000000,0x00100010,
	0x40008010,0x00108010,0x40108000,0x40000010,0x40000000,0x00100000,0x00008010,0x40108010,
	0x00100010,0x40108000,0x40008000,0x00108010,0x40108010,0x00100010,0x40000010,0x00000000,
	0x40000000,0x00008010,0x00100000,0x40100010,0x00008000,0x40000000,0x00108010,0x40008010,
	0x40108000,0x00008000,0x00000000,0x40000010,0x00000010,0x40108010,0x00108000,0x40100000,
	0x40100010,0x00100000,0x00008010,0x40008000,0x40008010,0x00000010,0x40100000,0x00108000,

	0x04000001,0x04040100,0x00000100,0x04000101,0x00040001,0x04000000,0x04000101,0x00040100,
	0x04000100,0x00040000,0x04040000,0x00000001,0x04040101,0x00000101,0x00000001,0x04040001,
	0x00000000,0x00040001,0x04040100,0x00000100,0x00000101,0x04040101,0x00040000,0x04000001,
	0x04040001,0x04000100,0x00040101,0x04040000,0x00040100,0x00000000,0x04000000,0x00040101,
	0x04040100,0x00000100,0x00000001,0x00040000,0x00000101,0x00040001,0x04040000,0x04000101,
	0x00000000,0x04040100,0x00040100,0x04040001,0x00040001,0x04000000,0x04040101,0x00000001,
	0x00040101,0x04000001,0x04000000,0x04040101,0x00040000,0x04000100,0x04000101,0x00040100,
	0x04000100,0x00000000,0x04040001,0x00000101,0x04000001,0x00040101,0x00000100,0x04040000,

	0x00401008,0x10001000,0x00000008,0x10401008,0x00000000,0x10400000,0x10001008,0x00400008,
	0x10401000,0x10000008,0x10000000,0x00001008,0x10000008,0x00401008,0x00400000,0x10000000,
	0x10400008,0x00401000,0x00001000,0x00000008,0x00401000,0x10001008,0x10400000,0x00001000,
	0x00001008,0x00000000,0x00400008,0x10401000,0x10001000,0x10400008,0x10401008,0x00400000,
	0x10400008,0x00001008,0x00400000,0x10000008,0x00401000,0x10001000,0x00000008,0x10400000,
	0x10001008,0x00000000,0x00001000,0x00400008,0x00000000,0x10400008,0x10401000,0x00001000,
	0x10000000,0x10401008,0x00401008,0x00400000,0x10401008,0x00000008,0x10001000,0x00401008,
	0x00400008,0x00401000,0x10400000,0x10001008,0x00001008,0x10000000,0x10000008,0x10401000,

	0x08000000,0x00010000,0x00000400,0x08010420,0x08010020,0x08000400,0x00010420,0x08010000,
	0x00010000,0x00000020,0x08000020,0x00010400,0x08000420,0x08010020,0x08010400,0x00000000,
	0x00010400,0x08000000,0x00010020,0x00000420,0x08000400,0x00010420,0x00000000,0x08000020,
	0x00000020,0x08000420,0x08010420,0x00010020,0x08010000,0x00000400,0x00000420,0x08010400,
	0x08010400,0x08000420,0x00010020,0x08010000,0x00010000,0x00000020,0x08000020,0x08000400,
	0x08000000,0x00010400,0x08010420,0x00000000,0x00010420,0x08000000,0x00000400,0x00010020,
	0x08000420,0x00000400,0x00000000,0x08010420,0x08010020,0x08010400,0x00000420,0x00010000,
	0x00010400,0x08010020,0x08000400,0x00000420,0x00000020,0x00010420,0x08010000,0x08000020,

	0x80000040,0x00200040,0x00000000,0x80202000,0x00200040,0x00002000,0x80002040,0x00200000,
	0x00002040,0x80202040,0x00202000,0x80000000,0x80002000,0x80000040,0x80200000,0x00202040,
	0x00200000,0x80002040,0x80200040,0x00000000,0x00002000,0x00000040,0x80202000,0x80200040,
	0x80202040,0x80200000,0x80000000,0x00002040,0x00000040,0x00202000,0x00202040,0x80002000,
	0x00002040,0x80000000,0x80002000,0x00202040,0x80202000,0x00200040,0x00000000,0x80002000,
	0x80000000,0x00002000,0x80200040,0x00200000,0x00200040,0x80202040,0x00202000,0x00000040,
	0x80202040,0x00202000,0x00200000,0x80002040,0x80000040,0x80200000,0x00202040,0x00000000,
	0x00002000,0x80000040,0x80002040,0x80202000,0x80200000,0x00002040,0x00000040,0x80200040,

	0x00004000,0x00000200,0x01000200,0x01000004,0x01004204,0x00004004,0x00004200,0x00000000,
	0x01000000,0x01000204,0x00000204,0x01004000,0x00000004,0x01004200,0x01004000,0x00000204,
	0x01000204,0x00004000,0x00004004,0x01004204,0x00000000,0x01000200,0x01000004,0x00004200,
	0x01004004,0x00004204,0x01004200,0x00000004,0x00004204,0x01004004,0x00000200,0x01000000,
	0x00004204,0x01004000,0x01004004,0x00000204,0x00004000,0x00000200,0x01000000,0x01004004,
	0x01000204,0x00004204,0x00004200,0x00000000,0x00000200,0x01000004,0x00000004,0x01000200,
	0x00000000,0x01000204,0x01000200,0x00004200,0x00000204,0x00004000,0x01004204,0x01000000,
	0x01004200,0x00000004,0x00004004,0x01004204,0x01000004,0x01004200,0x01004000,0x00004004,

	0x20800080,0x20820000,0x00020080,0x00000000,0x20020000,0x00800080,0x20800000,0x20820080,
	0x00000080,0x20000000,0x00820000,0x00020080,0x00820080,0x20020080,0x20000080,0x20800000,
	0x00020000,0x00820080,0x00800080,0x20020000,0x20820080,0x20000080,0x00000000,0x00820000,
	0x20000000,0x00800000,0x20020080,0x20800080,0x00800000,0x00020000,0x20820000,0x00000080,
	0x00800000,0x00020000,0x20000080,0x20820080,0x00020080,0x20000000,0x00000000,0x00820000,
	0x20800080,0x20020080,0x20020000,0x00800080,0x20820000,0x00000080,0x00800080,0x20020000,
	0x20820080,0x00800000,0x20800000,0x20000080,0x00820000,0x00020080,0x20020080,0x20800000,
	0x00000080,0x20820000,0x00820080,0x00000000,0x20000000,0x20800080,0x00020000,0x00820080,
};

const static DWORD Sel[8][64]=
{
	0x00000000,0x00000010,0x20000000,0x20000010,0x00010000,0x00010010,0x20010000,0x20010010,
	0x00000800,0x00000810,0x20000800,0x20000810,0x00010800,0x00010810,0x20010800,0x20010810,
	0x00000020,0x00000030,0x20000020,0x20000030,0x00010020,0x00010030,0x20010020,0x20010030,
	0x00000820,0x00000830,0x20000820,0x20000830,0x00010820,0x00010830,0x20010820,0x20010830,
	0x00080000,0x00080010,0x20080000,0x20080010,0x00090000,0x00090010,0x20090000,0x20090010,
	0x00080800,0x00080810,0x20080800,0x20080810,0x00090800,0x00090810,0x20090800,0x20090810,
	0x00080020,0x00080030,0x20080020,0x20080030,0x00090020,0x00090030,0x20090020,0x20090030,
	0x00080820,0x00080830,0x20080820,0x20080830,0x00090820,0x00090830,0x20090820,0x20090830,

	0x00000000,0x02000000,0x00002000,0x02002000,0x00200000,0x02200000,0x00202000,0x02202000,
	0x00000004,0x02000004,0x00002004,0x02002004,0x00200004,0x02200004,0x00202004,0x02202004,
	0x00000400,0x02000400,0x00002400,0x02002400,0x00200400,0x02200400,0x00202400,0x02202400,
	0x00000404,0x02000404,0x00002404,0x02002404,0x00200404,0x02200404,0x00202404,0x02202404,
	0x10000000,0x12000000,0x10002000,0x12002000,0x10200000,0x12200000,0x10202000,0x12202000,
	0x10000004,0x12000004,0x10002004,0x12002004,0x10200004,0x12200004,0x10202004,0x12202004,
	0x10000400,0x12000400,0x10002400,0x12002400,0x10200400,0x12200400,0x10202400,0x12202400,
	0x10000404,0x12000404,0x10002404,0x12002404,0x10200404,0x12200404,0x10202404,0x12202404,

	0x00000000,0x00000001,0x00040000,0x00040001,0x01000000,0x01000001,0x01040000,0x01040001,
	0x00000002,0x00000003,0x00040002,0x00040003,0x01000002,0x01000003,0x01040002,0x01040003,
	0x00000200,0x00000201,0x00040200,0x00040201,0x01000200,0x01000201,0x01040200,0x01040201,
	0x00000202,0x00000203,0x00040202,0x00040203,0x01000202,0x01000203,0x01040202,0x01040203,
	0x08000000,0x08000001,0x08040000,0x08040001,0x09000000,0x09000001,0x09040000,0x09040001,
	0x08000002,0x08000003,0x08040002,0x08040003,0x09000002,0x09000003,0x09040002,0x09040003,
	0x08000200,0x08000201,0x08040200,0x08040201,0x09000200,0x09000201,0x09040200,0x09040201,
	0x08000202,0x08000203,0x08040202,0x08040203,0x09000202,0x09000203,0x09040202,0x09040203,

	0x00000000,0x00100000,0x00000100,0x00100100,0x00000008,0x00100008,0x00000108,0x00100108,
	0x00001000,0x00101000,0x00001100,0x00101100,0x00001008,0x00101008,0x00001108,0x00101108,
	0x04000000,0x04100000,0x04000100,0x04100100,0x04000008,0x04100008,0x04000108,0x04100108,
	0x04001000,0x04101000,0x04001100,0x04101100,0x04001008,0x04101008,0x04001108,0x04101108,
	0x00020000,0x00120000,0x00020100,0x00120100,0x00020008,0x00120008,0x00020108,0x00120108,
	0x00021000,0x00121000,0x00021100,0x00121100,0x00021008,0x00121008,0x00021108,0x00121108,
	0x04020000,0x04120000,0x04020100,0x04120100,0x04020008,0x04120008,0x04020108,0x04120108,
	0x04021000,0x04121000,0x04021100,0x04121100,0x04021008,0x04121008,0x04021108,0x04121108,

	0x00000000,0x10000000,0x00010000,0x10010000,0x00000004,0x10000004,0x00010004,0x10010004,
	0x20000000,0x30000000,0x20010000,0x30010000,0x20000004,0x30000004,0x20010004,0x30010004,
	0x00100000,0x10100000,0x00110000,0x10110000,0x00100004,0x10100004,0x00110004,0x10110004,
	0x20100000,0x30100000,0x20110000,0x30110000,0x20100004,0x30100004,0x20110004,0x30110004,
	0x00001000,0x10001000,0x00011000,0x10011000,0x00001004,0x10001004,0x00011004,0x10011004,
	0x20001000,0x30001000,0x20011000,0x30011000,0x20001004,0x30001004,0x20011004,0x30011004,
	0x00101000,0x10101000,0x00111000,0x10111000,0x00101004,0x10101004,0x00111004,0x10111004,
	0x20101000,0x30101000,0x20111000,0x30111000,0x20101004,0x30101004,0x20111004,0x30111004,

	0x00000000,0x08000000,0x00000008,0x08000008,0x00000400,0x08000400,0x00000408,0x08000408,
	0x00020000,0x08020000,0x00020008,0x08020008,0x00020400,0x08020400,0x00020408,0x08020408,
	0x00000001,0x08000001,0x00000009,0x08000009,0x00000401,0x08000401,0x00000409,0x08000409,
	0x00020001,0x08020001,0x00020009,0x08020009,0x00020401,0x08020401,0x00020409,0x08020409,
	0x02000000,0x0A000000,0x02000008,0x0A000008,0x02000400,0x0A000400,0x02000408,0x0A000408,
	0x02020000,0x0A020000,0x02020008,0x0A020008,0x02020400,0x0A020400,0x02020408,0x0A020408,
	0x02000001,0x0A000001,0x02000009,0x0A000009,0x02000401,0x0A000401,0x02000409,0x0A000409,
	0x02020001,0x0A020001,0x02020009,0x0A020009,0x02020401,0x0A020401,0x02020409,0x0A020409,

	0x00000000,0x00000100,0x00080000,0x00080100,0x01000000,0x01000100,0x01080000,0x01080100,
	0x00000010,0x00000110,0x00080010,0x00080110,0x01000010,0x01000110,0x01080010,0x01080110,
	0x00200000,0x00200100,0x00280000,0x00280100,0x01200000,0x01200100,0x01280000,0x01280100,
	0x00200010,0x00200110,0x00280010,0x00280110,0x01200010,0x01200110,0x01280010,0x01280110,
	0x00000200,0x00000300,0x00080200,0x00080300,0x01000200,0x01000300,0x01080200,0x01080300,
	0x00000210,0x00000310,0x00080210,0x00080310,0x01000210,0x01000310,0x01080210,0x01080310,
	0x00200200,0x00200300,0x00280200,0x00280300,0x01200200,0x01200300,0x01280200,0x01280300,
	0x00200210,0x00200310,0x00280210,0x00280310,0x01200210,0x01200310,0x01280210,0x01280310,

	0x00000000,0x04000000,0x00040000,0x04040000,0x00000002,0x04000002,0x00040002,0x04040002,
	0x00002000,0x04002000,0x00042000,0x04042000,0x00002002,0x04002002,0x00042002,0x04042002,
	0x00000020,0x04000020,0x00040020,0x04040020,0x00000022,0x04000022,0x00040022,0x04040022,
	0x00002020,0x04002020,0x00042020,0x04042020,0x00002022,0x04002022,0x00042022,0x04042022,
	0x00000800,0x04000800,0x00040800,0x04040800,0x00000802,0x04000802,0x00040802,0x04040802,
	0x00002800,0x04002800,0x00042800,0x04042800,0x00002802,0x04002802,0x00042802,0x04042802,
	0x00000820,0x04000820,0x00040820,0x04040820,0x00000822,0x04000822,0x00040822,0x04040822,
	0x00002820,0x04002820,0x00042820,0x04042820,0x00002822,0x04002822,0x00042822,0x04042822,
};

#define F(l,r,S) {                                                          \
    work = ((DWORD *)keyin)[S  ];			zork = ((DWORD *)keyin)[S+1];   \
    work ^= r;								zork ^= r;                      \
    zork = ROTATE_RIGHT(zork, 4);                                           \
    l ^= *(DWORD *)((LPBYTE)Spbox[0] + (work      & 0xfc)); \
    l ^= *(DWORD *)((LPBYTE)Spbox[1] + (zork      & 0xfc)); \
    l ^= *(DWORD *)((LPBYTE)Spbox[2] + ((work>> 8)& 0xfc)); \
    l ^= *(DWORD *)((LPBYTE)Spbox[3] + ((zork>> 8)& 0xfc)); \
    l ^= *(DWORD *)((LPBYTE)Spbox[4] + ((work>>16)& 0xfc)); \
    l ^= *(DWORD *)((LPBYTE)Spbox[5] + ((zork>>16)& 0xfc)); \
    l ^= *(DWORD *)((LPBYTE)Spbox[6] + ((work>>24)& 0xfc)); \
    l ^= *(DWORD *)((LPBYTE)Spbox[7] + ((zork>>24)& 0xfc)); }
    
#define PERM_OP(a,b,t,n,m) ((t)=((((a)>>(n))^(b))&(m)),\
    (b)^=(t),\
    (a)^=((t)<<(n)))

#define HPERM_OP(a,t,n,m) ((t)=((((a)<<(16-(n)))^(a))&(m)),\
    (a)=(a)^(t)^(t>>(16-(n))))

#define DEBUGER_MODE	(0)
#define PRINTFMODE	    (0)

static unsigned int rotl(unsigned int value, int shift)
{
    register unsigned hibit;
    register unsigned num = value;

    shift &= 0x1f;
    while (shift--)
    {
        hibit = num & 0x80000000;
        num <<= 1;
        if (hibit)
        {
            num |= 1;
        }
    }
    return num;
}

static unsigned int rotr(unsigned int value, int shift)
{
    register unsigned lobit;
    register unsigned num = value;

    shift &= 0x1f;

    while (shift--)
    {
         lobit = num & 1;
         num >>= 1;
         if (lobit)
         {
	    	num |= 0x80000000;
	     }
    }
    return num;
}

static void deskey( DESTable *k, BYTE *key )
{
    DWORD csel,dsel,t,s, i;
    DWORD *kp = (DWORD *)k;

    csel = ((DWORD *)key)[0];										dsel = ((DWORD *)key)[1];

    PERM_OP (dsel,csel,t,4,0x0f0f0f0f);								HPERM_OP(csel,t,-2,0xcccc0000);
    HPERM_OP(dsel,t,-2,0xcccc0000);									PERM_OP (dsel,csel,t,1,0x55555555);
    PERM_OP (csel,dsel,t,8,0x00ff00ff);								PERM_OP (dsel,csel,t,1,0x55555555);
    dsel	=  (((dsel & 0x000000ff) << 16) | (dsel & 0x0000ff00) | ((dsel & 0x00ff0000) >> 16) | ((csel & 0xf0000000)>>4));
    csel	&= 0x0fffffff;
	
    for(i = 0; i < 16; i++)
    {
        DWORD temp;
        if(double_shift[i]) 
		{ 
            csel = ((csel >> 2) | (csel << 26));	dsel = ((dsel >> 2) | (dsel << 26));
        } 
		else 
		{
            csel = ((csel >> 1) | (csel << 27));	dsel = ((dsel >> 1) | (dsel << 27));
        }
        csel &= 0x0fffffff;							dsel &= 0x0fffffff;
        s =  Sel[0][ (csel      )&0x3f                ];
        s |= Sel[1][((csel >>  6)&0x03) | ((csel >>  7)&0x3c)];
        s |= Sel[2][((csel >> 13)&0x0f) | ((csel >> 14)&0x30)];
        s |= Sel[3][((csel >> 20)&0x01) | ((csel >> 21)&0x06)|((csel >> 22)&0x38)];
        t =  Sel[4][ (dsel      )&0x3f                ];
        t |= Sel[5][((dsel >>  7)&0x03) | ((dsel >>  8)&0x3c)];
        t |= Sel[6][ (dsel >> 15)&0x3f                ];
        t |= Sel[7][((dsel >> 21)&0x0f) | ((dsel >> 22)&0x30)];
        temp = ((t << 16) | (s & 0x0000ffff)) ;
        *(kp++) = ROTATE_LEFT(temp, 2);
        temp = ((s >> 16) | (t & 0xffff0000));
        *(kp++) = ROTATE_LEFT(temp, 6);
    }
}

static void des( BYTE *pbOut, BYTE *pbIn, void *keyin, int op )
{
    DWORD   left,right, work, zork;

    if( (((ULONG)(ULONG_PTR)pbIn) & (sizeof(ULONG_PTR)-1)) == 0 )
    {
        right	= ((DWORD *)pbIn)[0];					left	= ((DWORD *)pbIn)[1];
    } 
    else 
	{
        memcpy( &right, pbIn, sizeof(right) );			memcpy( &left,  pbIn+sizeof(right), sizeof(left) );
    }																//????????

	right	= ROTATE_LEFT(right, 6);					work	= right;
    right	^= left;									right	&= 0xf0f0f0f0;
    work	^= right;									left	^= right;
    
	left	= ROTATE_LEFT(left, 18);					right	= work;
    work	^= left;									work	&= 0xfff0000f;
    right	^= work;									left	^= work;
    
	left	= ROTATE_LEFT(left,14);						work	= right;
    right	^= left;									right	&= 0x33333333;
    work	^= right;									left	^= right;

	work	= ROTATE_LEFT(work, 25);					right	= work;
    work	^= left;									work	&= 0x03fc03fc;
    right	^= work;									left	^= work;

	right	= ROTATE_LEFT(right, 15);					work	= right;
	right	^= left;									right	&= 0xaaaaaaaa;
    work	^= right;									left	^= right;
	
	left	= ROTATE_LEFT(left, 3);						right	= work;
    if(op == DECRYPT)
    {
        F(left, right, 30);			F(right, left, 27);			F(left, right, 26);			F(right, left, 23);
        F(left, right, 22);			F(right, left, 20);			F(left, right, 19);			F(right, left, 16);
        F(left, right, 14);			F(right, left, 12);			F(left, right, 10);			F(right, left, 9);
        F(left, right, 6);			F(right, left, 3);			F(left, right, 2);			F(right, left, 0);
    }
    else
    {
        F(left, right, 0);			F(right, left, 2);			F(left, right, 3);			F(right, left, 6);
        F(left, right, 9);			F(right, left, 10);			F(left, right, 12);			F(right, left, 14);
        F(left, right, 16);			F(right, left, 19);			F(left, right, 20);			F(right, left, 22);
        F(left, right, 23);			F(right, left, 26);			F(left, right, 27);			F(right, left, 30);
    }
    /* Inverse permutation, also from Hoey via Outerbridge and Schneier */
    right	= ROTATE_RIGHT(right, 3);					work	= left;
    work	^= right;									work	&= 0xaaaaaaaa;
    right	^= work;									left	^= work;
    
	left	= ROTATE_RIGHT(left, 15);					work	= right;
    right	^= left;									right	&= 0x03fc03fc;
    work	^= right;									left	^= right;

    left	= ROTATE_RIGHT(left, 25);					right	= work;
    work	^= left;									work	&= 0x33333333;
    right	^= work;									left	^= work;

    right	= ROTATE_RIGHT(right, 14);					work	= right;
    right	^= left;									right	&= 0xfff0000f;
    work	^= right;									left	^= right;
    
	work	= ROTATE_RIGHT(work, 18);					right	= work;
    work	^= left;									work	&= 0xf0f0f0f0;
    right	^= work;									left	^= work;
    
	left	= ROTATE_RIGHT(left, 6);
    memcpy( pbOut, &left, sizeof(left) );				memcpy( pbOut+sizeof(left), &right, sizeof(right) );
}

// initial permutation IP
const static char IP_Table[64] = {
	58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17,  9, 1, 59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7
};

// final permutation IP^-1 
const static char IPR_Table[64] = {
	40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41,  9, 49, 17, 57, 25
};

// expansion operation matrix
static const char E_Table[48] = {
	32,  1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
	 8,  9, 10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32,  1
};

// 32-bit permutation function P used on the output of the S-boxes 
const static char P_Table[32] = {
	16, 7, 20, 21, 29, 12, 28, 17, 1,  15, 23, 26, 5,  18, 31, 10,
	2,  8, 24, 14, 32, 27, 3,  9,  19, 13, 30, 6,  22, 11, 4,  25
};

// permuted choice table (key) 
const static char PC1_Table[56] = {
	57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4
};

// permuted choice key (table) 
const static char PC2_Table[48] = {
	14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2,
	41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};

// number left rotations of pc1 
const static char LOOP_Table[16] = {
	1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1
};

// The (in)famous S-boxes 
const static char S_Box[8][4][16] = {
	// S1 
	14,	 4,	13,	 1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
	 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
	 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
    15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,
	// S2 
    15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
	 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
	 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
    13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,
	// S3 
    10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
	13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
	13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
     1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,
	// S4 
     7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
	13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
	10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
     3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,
	// S5 
     2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
	14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
	 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
    11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,
	// S6 
    12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
	10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
	 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
     4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,
	// S7 
     4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
	13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
	 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
     6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,
	// S8 
    13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
	 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
	 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
     2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
};

//////////////////////////////////////////////////////////////////////////
#define true    (1)
#define false   (0)

typedef char    (*PSubKey)[16][48];

//////////////////////////////////////////////////////////////////////////

static void SDES(char Out[8], char In[8], const PSubKey pSubKey, int Type);//标志DES加解密
static void SetKey(const char* Key, int len);// 设置密钥
static void SetSubKey(PSubKey pSubKey, const char Key[8]);// 设置子密钥
static void F_func(char In[32], const char Ki[48]);// f 函数
static void S_func(char Out[32], const char In[48]);// S 函数
static void Transform(char *Out, char *In, const char *Table, int len);// 变换
static void Xor(char *InA, const char *InB, int len);// 异或
static void RotateL(char *In, int len, int loop);// 循环左移
static void ByteToBit(char *Out, const char *In, int bits);// 字节组转换成位组
static void BitToByte(char *Out, const char *In, int bits);// 位组转换成字节组

//////////////////////////////////////////////////////////////////////////

static char SubKey[2][16][48];      // 16圈子密钥
static char Is3DES;                 // 3次DES标志
static char Tmp[256] = {0};
static char deskey2[16] = {0};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Code starts from Line 130
//////////////////////////////////////////////////////////////////////////

static void SetKey(const char* Key, int len)
{
	memset(deskey2, 0, 16);
	memcpy(deskey2, Key, len>16?16:len);
	SetSubKey(&SubKey[0], &deskey2[0]);
	Is3DES = len > 8 ? (SetSubKey(&SubKey[1], &deskey2[8]), true) : false;
}

static void SDES(char Out[8], char In[8], const PSubKey pSubKey, int Type)
{
	int i = 0;
    static char M[64], tmp[32], *Li=&M[0], *Ri=&M[32];
    ByteToBit(M, In, 64);
    Transform(M, M, IP_Table, 64);
    if( Type == 0 )
    {
        for( i=0; i<16; ++i)
        {
            memcpy(tmp, Ri, 32);
            F_func(Ri, (*pSubKey)[i]);
            Xor(Ri, Li, 32);
            memcpy(Li, tmp, 32);
        }
    }
    else
    {
        for( i=15; i>=0; --i)
        {
            memcpy(tmp, Li, 32);
            F_func(Li, (*pSubKey)[i]);
            Xor(Li, Ri, 32);
            memcpy(Ri, tmp, 32);
        }
	}
    Transform(M, M, IPR_Table, 64);
    BitToByte(Out, M, 64);
}

static void SetSubKey(PSubKey pSubKey, const char Key[8])
{
	int i;
    static char K[64], *KL=&K[0], *KR=&K[28];
    ByteToBit(K, Key, 64);
    Transform(K, K, PC1_Table, 56);
    for( i=0; i<16; ++i) 
    {
        RotateL(KL, 28, LOOP_Table[i]);
        RotateL(KR, 28, LOOP_Table[i]);
        Transform((*pSubKey)[i], K, PC2_Table, 48);
    }
}

static void F_func(char In[32], const char Ki[48])
{
    static char MR[48];
    Transform(MR, In, E_Table, 48);
    Xor(MR, Ki, 48);
    S_func(In, MR);
    Transform(In, In, P_Table, 32);
}

static void S_func(char Out[32], const char In[48])
{
	char i=0,j=0,k=0;
    for(i=0 ;i<8; ++i,In+=6,Out+=4) 
    {
        j = (In[0]<<1) + In[5];
        k = (In[1]<<3) + (In[2]<<2) + (In[3]<<1) + In[4];
		ByteToBit(Out, &S_Box[i][j][k], 4);
    }
}

static void inline Transform(char *Out, char *In, const char *Table, int len)
{
    int i = 0;
    for(i=0; i<len; ++i)
    {
        Tmp[i] = In[ Table[i]-1 ];
    }
    memcpy(Out, Tmp, len);
}

static void inline Xor(char *InA, const char *InB, int len)
{
    int i = 0;
    for( i=0; i<len; ++i)
    {
        InA[i] ^= InB[i];
    }
}

static void inline RotateL(char *In, int len, int loop)
{
    memcpy(Tmp, In, loop);
    memcpy(In, In+loop, len-loop);
    memcpy(In+len-loop, Tmp, loop);
}

static void inline ByteToBit(char *Out, const char *In, int bits)
{
    int i = 0;
    for(i=0; i<bits; ++i)
    {
        Out[i] = (In[i>>3]>>(i&7)) & 1;
    }
}

static void inline BitToByte(char *Out, const char *In, int bits)
{
	int i = 0;
	
    memset(Out, 0, bits>>3);
    for(i=0; i<bits; ++i)
    {
        Out[i>>3] |= In[i]<<(i&7);
    }
}

/*@breif DesEncryption DES加密解密操作
 *@param[out] Out 加密或者解密之后的缓冲区
 *@param[in]  In 需要加密或者解密的缓冲区
 *@param[in]  datalen In的长度
 *@param[in]  Key 密钥
 *@param[in]  keylen Key密钥长度
 *@param[in]  Type 操作类型: 0->解密  1->加密
 *@return 成功返回1 失败返回0
 */
int DesEncryption(char *Out, char *In, long datalen, const char *Key, int keylen, int Type)
{
    long i = 0, j = 0;

    if(!(Out && In && Key && (datalen=(datalen+7)&0xfffffff8)))
    {
        return false;
    }
    
    SetKey(Key, keylen);
    if(!Is3DES) // 1次DES
    {
        for( i=0,j=datalen>>3; i<j; ++i,Out+=8,In+=8)
        {
            SDES(Out, In, &SubKey[0], Type);
        }
    }
    else        // // 3次DES 加密:加(key0)-解(key1)-加(key0) 解密::解(key0)-加(key1)-解(key0)
    {
        for(i=0,j=datalen>>3; i<j; ++i,Out+=8,In+=8) 
        {
            SDES(Out, In,  &SubKey[0], Type);
            SDES(Out, Out, &SubKey[1], !Type);
            SDES(Out, Out, &SubKey[0], Type);
        }
    }
    return true;
}

#if 0

int main()
{
    char key[]={0,2,0,0,9,3,5,1,9,8,0,0,9,1,7},buf[255];
    char str[]="Welcome to My 3-DES Test! --WangJunchuan\n";

    memset(buf, 0, sizeof(buf));
    strcpy(buf, str);
    puts("Before encrypting: \n");
    int i = 0;
    for (i=0; i<strlen(str); i++)
    {
        printf("%#x ", buf[i]);
    }
    printf("\n");

    Des_Go(buf, buf, sizeof(str), key, sizeof(key), ENCRYPT);
    printf("After encrypting: \n");
    for (i=0; i<strlen(str); i++)
    {
        printf("%#x ", buf[i]);
    }
    printf("\n");

    printf("\n");

    Des_Go(buf, buf, sizeof(str), key, sizeof(key), DECRYPT);
    
    printf("After decrypting: \n");
    for (i=0; i<strlen(str); i++)
    {
        printf("%#x ", buf[i]);
    }
    printf("\n");

    return 0;
}

#endif
