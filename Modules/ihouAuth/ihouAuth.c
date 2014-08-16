#include <stdio.h>
#include <stdlib.h>
#include "ihouAuth.h"
#include "ihouAuth/des/DES.h"

int encrypt(const char *plainText,
            char *cipherText)
{
    int plainTextLength = strlen(plainText) + strlen(IHOU_PLUGIN_STRING);
    int cipherLength = 0;

    char *strPlainText = (char *)malloc(plainTextLength + 1);
    memset(strPlainText, '\0', plainTextLength + 1);
    memcpy(strPlainText, plainText, strlen(plainText));
    memcpy(strPlainText + strlen(plainText),
           IHOU_PLUGIN_STRING,
           strlen(IHOU_PLUGIN_STRING));

    cipherLength = DES_Encrypt(strPlainText, cipherText, KEY);
    cipherText[cipherLength] = '\0';
    free(strPlainText);

    return cipherLength;
}


int decrypt(const char *cipherText, int cipherTextLength, char *plainText)
{
    int plainTextLength = 0;

    if (cipherTextLength > 0)
    {
        plainTextLength = DES_Decrypt(cipherText, cipherTextLength, plainText, KEY);
    }

    return plainTextLength;
}
