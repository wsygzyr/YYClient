/*
============================================================================
 Name		 : DesEbcHelper.h
 Author	     : fgtian
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : DES¼ÓÃÜEBCÄ£Ê½.
============================================================================
*/

#ifndef DESEBCHELPER_H_
#define DESEBCHELPER_H_


#ifdef __cplusplus
extern "C" 
{
#endif /* C++ */
	int DES_Encrypt(const char *src, char *dst, const char *key);
	int DES_Decrypt(const char * src, int srcLength, char * dst, const char * aKey);
#ifdef __cplusplus
} /* extern "C" */
#endif /* C++ */

//void DES_ECB_Encrypt(const char *src, char *dst, const char *key);

#endif /* __QHCR_H__ */