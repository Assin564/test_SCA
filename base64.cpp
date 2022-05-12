#include "base64.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static const char base64[64] =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'
};
/*
 *  ��base64�ı���ֵ��Ϊ��������ԭindex������.����ת����
 *  ����,A~Z��a~z��0~9��+��/��64������
 *  ��64�����ţ�ASCII����Ϊbase64_back[]������,����base64[]�е�������Ϊbase64_back[]������ֵ
 *  ���,���ڱ�����'=(ASCIIΪ61)'���ɲ���0ת���ɵģ���������ת������index=61��λ��,ֵΪ0
 *  ����indexֵ��Ϊ-1
*/
static const char base64_back[128] =
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1,  0, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
};

void show_base64(const char* buf, int len);
void base64_encrypt(const char* pbuf, char* cbuf);
void base64_decrypt(const char* cbuf, char* pbuf);


void base64_encrypt_text(const char* pbuf, int plen)
{
    int clen = (plen % 3) ? (plen / 3 + 1) : (plen / 3);

    char* buf = (char*)malloc(clen * 3);
    char* cbuf = (char*)malloc(clen * 4);
    if (NULL == cbuf || NULL == buf) {
        exit(EXIT_FAILURE);
    }
    memset(cbuf, 0, clen * 4);
    memset(buf, 0, clen * 3);
    memcpy(buf, pbuf, plen);

    //����ת��
    for (int i = 0; i < clen; i++) {
        base64_encrypt(&buf[i * 3], &cbuf[i * 4]);
    }
    //���ڶ���0������ͳһ��base6_encrypt()��������Ҫ��ĩβ��'A'��������Ϊ'='
    if (plen % 3 == 2) {//ֻ��һ���ֽڣ���Ӧһ��'='
        cbuf[clen * 4 - 1] = '=';
    }
    else if (plen % 3 == 1) {//�������ֽ����Ӧ����'='
        cbuf[clen * 4 - 1] = cbuf[clen * 4 - 2] = '=';
    }
    show_base64(cbuf, clen * 4);
    free(buf);
    free(cbuf);
}


void base64_decrypt_text(const char* cbuf, int clen)
{
    int plen = clen / 4;

    char* pbuf = (char*)malloc(plen * 3);

    if (NULL == pbuf) {
        exit(EXIT_FAILURE);
    }
    memset(pbuf, 0, plen * 3);

    for (int i = 0; i < plen; i++) {
        base64_decrypt(&cbuf[i * 4], &pbuf[i * 3]);
    }
    show_base64(pbuf, plen * 3);
    free(pbuf);
}
void base64_encrypt(const char* pbuf, char* cbuf)
{
    int temp = (pbuf[0] << 16) | (pbuf[1] << 8) | (pbuf[2] << 0);
    for (int i = 0; i < 4; i++) {
        int index = (temp >> (18 - i * 6)) & 0x3F;
        cbuf[i] = base64[index];
    }
}
void base64_decrypt(const char* cbuf, char* pbuf)
{
    int temp = 0;
    for (int i = 0; i < 4; i++) {//�������������ݱ������ԭindex��������λ���
        temp |= base64_back[cbuf[i]] << (18 - 6 * i);//temp�ĸ�1byteδʹ��
    }
    for (int i = 0; i < 3; i++) {//��λ���õ���temp�ĵ���byte�ֱ�Ϊԭ������byte
        pbuf[i] = (temp >> (16 - i * 8)) & 0XFF;
    }
}
void show_base64(const char* buf, int len)
{
    for (int i = 0; i < len; i++) {
        printf("%c", buf[i]);
    }
    printf("\n");
}








