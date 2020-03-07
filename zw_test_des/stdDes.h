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
/*@breif StdDesEncryption ��׼DES���ܽ��ܲ���(��֧��3DES)
 *@param[in]  In ��Ҫ���ܻ��߽��ܵĻ�����(8byte����)
 *@param[out] Out ���ܻ��߽���֮��Ļ�����(8byte����)
 *@param[in]  datalen In�ĳ���(8����)
 *@param[in]  Key ��Կ(8byte)
 *@param[in]  Type ��������: 0->����  1->����
 *@return �ɹ�����0 ʧ�ܷ���1
 */
int StdDesEncryption(unsigned char *In, unsigned char * Out, int datalen, const unsigned char *Key, int Type);

#ifdef __cplusplus
}
#endif

#endif

