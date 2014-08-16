/*
============================================================================
 Name		 : DesEbcHelper.cpp
 Author	     : fgtian
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : DESº”√‹EBCƒ£ Ω.
============================================================================
*/
#include "DES.h"

#include "string.h"
#include "stdlib.h"
#include "base64.h"

#include "des_locl.h"
#include "spr.h"

extern void DES_set_key_unchecked(const_DES_cblock *key, DES_key_schedule *schedule);
void DES_encrypt1(DES_LONG *data, DES_key_schedule *ks, int enc)
	{
	register DES_LONG l,r,t,u;
#ifdef DES_PTR
	register const unsigned char *des_SP=(const unsigned char *)DES_SPtrans;
#endif
#ifndef DES_UNROLL
	register int i;
#endif
	register DES_LONG *s;

	r=data[0];
	l=data[1];

	IP(r,l);
	/* Things have been modified so that the initial rotate is
	 * done outside the loop.  This required the
	 * DES_SPtrans values in sp.h to be rotated 1 bit to the right.
	 * One perl script later and things have a 5% speed up on a sparc2.
	 * Thanks to Richard Outerbridge <71755.204@CompuServe.COM>
	 * for pointing this out. */
	/* clear the top bits on machines with 8byte longs */
	/* shift left by 2 */
	r=ROTATE(r,29)&0xffffffffL;
	l=ROTATE(l,29)&0xffffffffL;

	s=ks->ks->deslong;
	/* I don't know if it is worth the effort of loop unrolling the
	 * inner loop */
	if (enc)
		{
#ifdef DES_UNROLL
		D_ENCRYPT(l,r, 0); /*  1 */
		D_ENCRYPT(r,l, 2); /*  2 */
		D_ENCRYPT(l,r, 4); /*  3 */
		D_ENCRYPT(r,l, 6); /*  4 */
		D_ENCRYPT(l,r, 8); /*  5 */
		D_ENCRYPT(r,l,10); /*  6 */
		D_ENCRYPT(l,r,12); /*  7 */
		D_ENCRYPT(r,l,14); /*  8 */
		D_ENCRYPT(l,r,16); /*  9 */
		D_ENCRYPT(r,l,18); /*  10 */
		D_ENCRYPT(l,r,20); /*  11 */
		D_ENCRYPT(r,l,22); /*  12 */
		D_ENCRYPT(l,r,24); /*  13 */
		D_ENCRYPT(r,l,26); /*  14 */
		D_ENCRYPT(l,r,28); /*  15 */
		D_ENCRYPT(r,l,30); /*  16 */
#else
		for (i=0; i<32; i+=8)
			{
			D_ENCRYPT(l,r,i+0); /*  1 */
			D_ENCRYPT(r,l,i+2); /*  2 */
			D_ENCRYPT(l,r,i+4); /*  3 */
			D_ENCRYPT(r,l,i+6); /*  4 */
			}
#endif
		}
	else
		{
#ifdef DES_UNROLL
		D_ENCRYPT(l,r,30); /* 16 */
		D_ENCRYPT(r,l,28); /* 15 */
		D_ENCRYPT(l,r,26); /* 14 */
		D_ENCRYPT(r,l,24); /* 13 */
		D_ENCRYPT(l,r,22); /* 12 */
		D_ENCRYPT(r,l,20); /* 11 */
		D_ENCRYPT(l,r,18); /* 10 */
		D_ENCRYPT(r,l,16); /*  9 */
		D_ENCRYPT(l,r,14); /*  8 */
		D_ENCRYPT(r,l,12); /*  7 */
		D_ENCRYPT(l,r,10); /*  6 */
		D_ENCRYPT(r,l, 8); /*  5 */
		D_ENCRYPT(l,r, 6); /*  4 */
		D_ENCRYPT(r,l, 4); /*  3 */
		D_ENCRYPT(l,r, 2); /*  2 */
		D_ENCRYPT(r,l, 0); /*  1 */
#else
		for (i=30; i>0; i-=8)
			{
			D_ENCRYPT(l,r,i-0); /* 16 */
			D_ENCRYPT(r,l,i-2); /* 15 */
			D_ENCRYPT(l,r,i-4); /* 14 */
			D_ENCRYPT(r,l,i-6); /* 13 */
			}
#endif
		}

	/* rotate and clear the top bits on machines with 8byte longs */
	l=ROTATE(l,3)&0xffffffffL;
	r=ROTATE(r,3)&0xffffffffL;

	FP(r,l);
	data[0]=l;
	data[1]=r;
	l=r=t=u=0;
	}


void DES_ecb_encrypt(const_DES_cblock *input, DES_cblock *output,
					 DES_key_schedule *ks, int enc)
{
	register DES_LONG l;
	DES_LONG ll[2];
	const unsigned char *in = &(*input)[0];
	unsigned char *out = &(*output)[0];

	c2l(in,l); ll[0]=l;
	c2l(in,l); ll[1]=l;
	DES_encrypt1(ll,ks,enc);
	l=ll[0]; 
	l2c(l,out);
	l=ll[1]; 
	l2c(l,out);
	l=ll[0]=ll[1]=0;
}


int DES_Encrypt(const char *src, char *dst, const char *aKey)
{
	des_key_schedule ks;
	int len, i;
	unsigned char key[8] = {0};
	char tmp[8] = {0};
	int length = 0;
	int padding = 0;
	char *plainText = NULL;

	// add by xianli 
	int base64DataLen = 0;
	char *base64DataBuf = NULL;
	
	for (i = 0; i < 8; i++)
	{
		key[i] = aKey[i];
	}
	
	length = strlen(src);
	padding = 8 - length % 8;
	len = length + padding;
	plainText = (char *)malloc(len + 1);
	memset(plainText, 0, len + 1);
	memcpy(plainText, src, length);
	
	for (i = 0; i < padding; i++)
	{
		plainText[i + strlen(src)] = padding;
	}
	
	
	
	DES_set_key_unchecked(&key,&ks);
	
	length = 0;
	for (i = 0; i < len; i++)
	{
		if (i % 8 == 0 && i > 0)
		{
			DES_ecb_encrypt((const_DES_cblock*)tmp,(const_DES_cblock*)tmp,&ks,DES_ENCRYPT);
			memcpy(dst + length, tmp, 8);
			length += 8;
		}
		tmp[i % 8] = plainText[i];
	}
	
	DES_ecb_encrypt((const_DES_cblock*)tmp,(const_DES_cblock*)tmp,&ks,DES_ENCRYPT);
	memcpy(dst + length, tmp, 8);

	// add by xianli
	base64DataBuf = base64_encode(dst, len, &base64DataLen);
	memcpy(dst, base64DataBuf, base64DataLen);
	free(base64DataBuf);
	
	free(plainText);

	return base64DataLen;
	
	// return len;
}


int DES_Decrypt(const char * src, int srcLength, char * dst, const char * aKey)
{
	des_key_schedule ks;
	int len, i;
	unsigned char key[8] = {0};
	int length = 0;
	char message[8] = {0};
	int padding = 0;

	int base64DecodeDataLen = 0;
	char *base64DecodeDataBuf = NULL;

	base64DecodeDataBuf = base64_decode(src, srcLength, &base64DecodeDataLen);

	for (i = 0; i < 8; i++)
	{
		key[i] = aKey[i];
	}

	// len = srcLength;
	len = base64DecodeDataLen;
	len = len - len % 8;

	DES_set_key_unchecked(&key, &ks);
	
	for (i = 0; i < len; i++)
	{
		if (i % 8 == 0 && i > 0)
		{
			DES_ecb_encrypt((const_DES_cblock*)message,(const_DES_cblock*)message,&ks,DES_DECRYPT);
			memcpy(dst + length, message, 8);
			length += 8;
		}
		// message[i % 8] = src[i];
		message[i % 8] = base64DecodeDataBuf[i];
	}
	
	free(base64DecodeDataBuf);
	DES_ecb_encrypt((const_DES_cblock*)message,(const_DES_cblock*)message,&ks,DES_DECRYPT);
	memcpy(dst + length, message, 8);

	padding = dst[len-1];
	len -= padding;
	dst[len] = 0;
	

	return len;
}
