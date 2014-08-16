#ifndef _BASE64_
#define _BASE64_

#ifndef uint32_t
#define uint32_t unsigned int
#endif

char *base64_encode(const unsigned char *data,
                    int input_length,
                    int *output_length) ;


unsigned char *base64_decode(const char *data,
                             int input_length,
                             int *output_length) ;

void build_decoding_table();


void base64_cleanup();

#endif /* _BASE64_ */