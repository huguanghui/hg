/*!
 *\file     base64Lib.h
 *\brief	base64¼ÓÃÜ½âÃÜÄ£¿é
 */
#ifndef __H_BASE64_LIB__
#define __H_BASE64_LIB__

#ifdef __cplusplus
extern "C" {
#endif

char * base64Encode(const unsigned char *bindata, char *base64, int binlength);

int base64Decode(const char *base64, unsigned char *bindata);

#ifdef __cplusplus
}
#endif

#endif // end of __H_BASE64_LIB__

