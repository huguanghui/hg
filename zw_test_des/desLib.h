/*!
 *\file		des.h
 *\brief	des���ܽ���ģ��
 *\author	fancong
 *\date		2016��12��15�� 11:42:10
 */
#ifndef __H_DES__
#define __H_DES__

#define DECRYPT (0)
#define ENCRYPT (1)

#ifdef __cplusplus
extern "C" {
#endif

/*@breif DesEncryption DES���ܽ��ܲ���
 *@param[out] Out ���ܻ��߽���֮��Ļ�����
 *@param[in]  In ��Ҫ���ܻ��߽��ܵĻ�����
 *@param[in]  datalen In�ĳ���
 *@param[in]  Key ��Կ
 *@param[in]  keylen Key��Կ����
 *@param[in]  Type ��������: 0->����  1->����
 *@return �ɹ�����1 ʧ�ܷ���0
 */
int DesEncryption(char * Out, char * In, long datalen, const char * Key, int keylen, int Type);

#ifdef __cplusplus
}
#endif

#endif // end of __H_DES__
