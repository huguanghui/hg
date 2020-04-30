/*!
 *\file		des.h
 *\brief	des加密解密模块
 *\author	fancong
 *\date		2016年12月15日 11:42:10
 */
#ifndef __H_DES__
#define __H_DES__

#define DECRYPT (0)
#define ENCRYPT (1)

#ifdef __cplusplus
extern "C" {
#endif

/*@breif DesEncryption DES加密解密操作
 *@param[out] Out 加密或者解密之后的缓冲区
 *@param[in]  In 需要加密或者解密的缓冲区
 *@param[in]  datalen In的长度
 *@param[in]  Key 密钥
 *@param[in]  keylen Key密钥长度
 *@param[in]  Type 操作类型: 0->解密  1->加密
 *@return 成功返回1 失败返回0
 */
int DesEncryption(char * Out, char * In, long datalen, const char * Key, int keylen, int Type);

#ifdef __cplusplus
}
#endif

#endif // end of __H_DES__
