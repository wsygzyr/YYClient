#ifndef IHOUAUTH_H
#define IHOUAUTH_H

#ifdef __cplusplus
extern "C"
{
#endif

#define KEY "12011911"
#define IHOU_PLUGIN_STRING "ihou plugin"

int encrypt(const char* plainText, char* cipherText);

int decrypt(const char* cipherText, int cipherTextLength, char* plainText);

#ifdef __cplusplus
}
#endif

#endif
