#ifndef __INC_GTYPES_H
#define __INC_GTYPES_H

//----------�궨����������---------------------------------
typedef	unsigned char	UCHAR;
typedef unsigned short	USHORT;
typedef	unsigned int	UINT;
typedef unsigned long	ULONG;
typedef unsigned long long	ULONGLONG;

typedef	int		BOOL;
typedef	int		STATUS; //��������״̬
typedef	void	VOID;

typedef	char		INT8;			
typedef	short		INT16;
typedef	int			INT32;	

typedef	unsigned char	UINT8;
typedef	unsigned short	UINT16;
typedef	unsigned int	UINT32;
typedef unsigned long long UINT64;

//------------�궨�峣��-----------------------------------
#define TRUE_1			0x55			//��BOOL���� ������==��!=�����ж�
#define FALSE_2			0xaa			//��BOOL���� ������==��!=�����ж�

//#define NULL			((void *) 0)	//��ʾ�ṹ��ָ��Ϊ��

#define OK				0				//��STATUS����
#define ERROR			(-1)			//��STATUS����

/* timeout defines */
#define NO_WAIT			0				//��ѯ��ʽ
#define WAIT_FOREVER	(-1)			//������ʽ

//----------�궨��洢����---------------------------------
#define FAST	register //��volatile�෴
#define IMPORT	extern
#define LOCAL	static

//----------���ú���ָ������------------------------------
typedef int 		(*FUNCPTR) (void);			/* ptr to function returning int */
typedef void 		(*VOIDFUNCPTR) (void);		/* ptr to function returning void */
typedef double 		(*DBLFUNCPTR) (void);		/* ptr to function returning double*/
typedef float 		(*FLTFUNCPTR) (void);		/* ptr to function returning float */
typedef int 		(*INTFUNCPTR) (int a);	/* ptr to function returning int */


//----------�궨����������---------------------------------
typedef	unsigned char	BYTE;
typedef unsigned short	WORD;
typedef unsigned int	DWORD;

#define MSB(x)	((UINT8)(((x) >> 8) & 0xff))	  /* most signif byte of 2-byte integer */
#define LSB(x)	((UINT8)((x) & 0xff))		  /* least signif byte of 2-byte integer*/
#define MSW(x) ((UINT16)(((x) >> 16) & 0xffff)) /* most signif word of 2-word integer */
#define LSW(x) ((UINT16)((x) & 0xffff)) 		  /* least signif byte of 2-word integer*/

#define MAKEUINT16(a, b)      ((UINT16)((UINT16)(((UINT16)((UINT8)((a)&0xff))) << 8) | ((UINT8)((b)&0xff))))
#define MAKEINT16(a, b)      ((INT16)((UINT16)(((UINT16)((UINT8)((a)&0xff))) << 8) | ((UINT8)((b)&0xff))))
#define MAKEUINT32(a, b, c, d)   ((UINT32)((((UINT32)((UINT8)((a)&0xff))) << 24) | (((UINT32)((UINT8)((b)&0xff))) << 16) | (((UINT32)((UINT8)((c)&0xff))) << 8) | ((UINT8)((d)&0xff))))
#define MAKEINT32(a, b, c, d)   ((INT32)((((UINT32)((UINT8)((a)&0xff))) << 24) | (((UINT32)((UINT8)((b)&0xff))) << 16) | (((UINT32)((UINT8)((c)&0xff))) << 8) | ((UINT8)((d)&0xff))))

#define LLSB(x)	((x) & 0xff)		/* 32bit word byte/word swap macros */
#define LNLSB(x) (((x) >> 8) & 0xff)
#define LNMSB(x) (((x) >> 16) & 0xff)
#define LMSB(x)	 (((x) >> 24) & 0xff)
#define LONGSWAP(x) ((LLSB(x) << 24) | \
	(LNLSB(x) << 16)| \
	(LNMSB(x) << 8) | \
	(LMSB(x)))

#endif //__INC_GTYPES_H
