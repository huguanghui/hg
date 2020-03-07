#ifndef _STD_DES_H_
#define _STD_DES_H_

#define ENCRYPTION_MODE 1
#define DECRYPTION_MODE 0

typedef struct {
	unsigned char k[8];
	unsigned char c[4];
	unsigned char d[4];
} key_set;

#ifdef __cplusplus
extern "C" {
#endif
void generate_key(unsigned char* key);
void generate_sub_keys(unsigned char* main_key, key_set* key_sets);
void process_message(unsigned char* message_piece, unsigned char* processed_piece, key_set* key_sets, int mode);
/*@breif StdDesEncryption 标准DES加密解密操作(不支持3DES)
 *@param[in]  In 需要加密或者解密的缓冲区(8byte对齐)
 *@param[out] Out 加密或者解密之后的缓冲区(8byte对齐)
 *@param[in]  datalen In的长度(8对齐)
 *@param[in]  Key 密钥(8byte)
 *@param[in]  Type 操作类型: 0->解密  1->加密
 *@return 成功返回0 失败返回1
 */
int StdDesEncryption(unsigned char *In, unsigned char * Out, int datalen, const unsigned char *Key, int Type);

#ifdef __cplusplus
}
#endif

#endif

