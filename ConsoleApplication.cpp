// ConsoleApplication.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define RSA         0
#define RSA_2048 1
#define AES         2
#define DEBUG RSA_2048

#if (DEBUG == RSA)
#include "rsa.h"
//RSA示例
#define MES_LENGTH 10
int main(int argc, char** argv)
{
    struct public_key_class pub[1];
    struct private_key_class priv[1];
    rsa_gen_keys(pub, priv, PRIME_SOURCE_FILE);

    printf("Private Key:\n Modulus: %lld\n Exponent: %lld\n", (long long)priv->modulus, (long long)priv->exponent);
    printf("Public Key:\n Modulus: %lld\n Exponent: %lld\n", (long long)pub->modulus, (long long)pub->exponent);

    char message[MES_LENGTH] = { 45,12,11,0,87,67,2,134,234,10 };
    int i;

    printf("Original:\n");
    for (i = 0; i < MES_LENGTH; i++) {
        printf("%lld\n", (long long)message[i]);
    }

    long long* encrypted = rsa_encrypt(message, MES_LENGTH, pub);
    if (!encrypted) {
        fprintf(stderr, "Error in encryption!\n");
        return 1;
    }
    printf("Encrypted:\n");
    for (i = 0; i < MES_LENGTH; i++) {
        printf("%lld\n", (long long)encrypted[i]);
    }

    char* decrypted = rsa_decrypt(encrypted, 8 * MES_LENGTH, priv);
    if (!decrypted) {
        fprintf(stderr, "Error in decryption!\n");
        return 1;
    }
    printf("Decrypted:\n");
    for (i = 0; i < MES_LENGTH; i++) {
        printf("%lld\n", (long long)decrypted[i]);
    }

    printf("\n");
    free(encrypted);
    free(decrypted);
    return 0;
}

#elif (DEBUG == RSA_2048)
#include "keys.h"
#include "rsa2048.h"
#include "bignum.h"

#include"base64.h"


/*
 * RSA2048 encrypt and decrypt
 * include rsa.c/bignum.c/rsa.h/bignum.h/keys.h
 */
#define MSG_LEN 128
int RSA2048(void) {
    int ret;
    rsa_pk_t pk = { 0 };
    rsa_sk_t sk = { 0 };
    uint8_t output[2048];

    // message to encrypt
    uint8_t input[256] = { 0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00, 0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00,0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00, 0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00,
        0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00, 0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00,0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00, 0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00,
        0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00, 0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00,0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00, 0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00,
        0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00, 0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00,0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00, 0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00,
//        0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00, 0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00,0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00, 0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00,
//        0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00, 0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00,0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00, 0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00,
//        0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00, 0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00,0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00, 0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00,
//       0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00, 0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00,0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x00, 0x21,0x55,0x53,0x53,0x53,0x53,0x11,0x02,
    };

    unsigned char msg[256];
    uint32_t outputLen, msg_len;
    uint8_t  inputLen;


    // copy keys.h message about public key and private key to the flash RAM
    pk.bits = KEY_M_BITS;
    memcpy(&pk.modulus[RSA_MAX_MODULUS_LEN - sizeof(key_m)], key_m, sizeof(key_m));
    memcpy(&pk.exponent[RSA_MAX_MODULUS_LEN - sizeof(key_e)], key_e, sizeof(key_e));
    sk.bits = KEY_M_BITS;
    memcpy(&sk.modulus[RSA_MAX_MODULUS_LEN - sizeof(key_m)], key_m, sizeof(key_m));
    memcpy(&sk.public_exponet[RSA_MAX_MODULUS_LEN - sizeof(key_e)], key_e, sizeof(key_e));
    memcpy(&sk.exponent[RSA_MAX_MODULUS_LEN - sizeof(key_pe)], key_pe, sizeof(key_pe));
    memcpy(&sk.prime1[RSA_MAX_PRIME_LEN - sizeof(key_p1)], key_p1, sizeof(key_p1));
    memcpy(&sk.prime2[RSA_MAX_PRIME_LEN - sizeof(key_p2)], key_p2, sizeof(key_p2));
    memcpy(&sk.prime_exponent1[RSA_MAX_PRIME_LEN - sizeof(key_e1)], key_e1, sizeof(key_e1));
    memcpy(&sk.prime_exponent2[RSA_MAX_PRIME_LEN - sizeof(key_e2)], key_e2, sizeof(key_e2));
    memcpy(&sk.coefficient[RSA_MAX_PRIME_LEN - sizeof(key_c)], key_c, sizeof(key_c));

    //inputLen = strlen((const char*)input);
    inputLen = MSG_LEN;
    // public key encrypt
 //   rsa_public_encrypt(output, &outputLen, input, inputLen, &pk);

    // private key decrypt
//    rsa_private_decrypt(msg, &msg_len, output, outputLen, &sk);

    // private key encrypt
    rsa_private_encrypt(output, &outputLen, input, inputLen, &sk);
    printf("加密前：\n");
    for (int i = 0; i < inputLen; i++)
    {
        printf("%02x", input[i]);
    }
    printf("\n");
    printf("加密后：\n");
    for (int i = 0; i < outputLen; i++)
    {
        printf("%02x", output[i]);
    }
    printf("\n");

    // public key decrypted
    rsa_public_decrypt(msg, &msg_len, output, outputLen, &pk);
    printf("解密前：\n");
    for (int i = 0; i < outputLen; i++)
    {
        printf("%02x", output[i]);
    }
    printf("\n");
    printf("解密后：\n");
    for (int i = 0; i < msg_len; i++)
    {
        printf("%02x", msg[i]);
    }
    printf("\n");




    return 0;
}
/* RSA2048 function ended */

int main(int argc, char const* argv[])
{
    clock_t start, finish;
    double  duration;



    start = clock();    // init start time
    RSA2048();
    finish = clock();   // print end time
    duration = (double)(finish - start) / CLOCKS_PER_SEC;   // print encrypt and decrypt time
    printf("%f seconds\n", duration);
    return 0;
}

#elif (DEBUG == AES)
#include "aes.h"
//AES 示例
int main()
{
    char plain[1024] = {
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F', 
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','1','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','2','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','1','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','2','0','E','F',
          '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','1','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','2','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','1','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','2','0','E','F',
          '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','1','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','2','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','1','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','2','0','E','F',
          '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','1','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','2','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','1','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','2','0','E','F',
          '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','1','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','2','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','1','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','2','0','E','F',
          '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','1','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','2','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','1','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','2','0','E','F',
          '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','1','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','2','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','1','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','2','0','E','F',
          '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','1','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','2','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','K','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','1','0','E','F',
        '0','1','2','3','4','5','6','7','8',0,'A','B','2',0xff,0,0,
    };
    int len = 1024;
    char key[16] = { '0','1','2','3','4','5','6','7',1,2,0,'B','C','D','E','F' };
    char key1[16] = { '0','1','2','3','4','5','6','7',1,1,0,'B','C','D','E','F' };
    char ciphertext[16] = { 0 };

    printf("明文：\n");
    for (int i = 0; i < len; i++)
    {
        printf("%02x", plain[i] & 0xff);
    }
    printf("\n密钥：\n");
    for (int i = 0; i < 16; i++)
    {
        printf("%02x", key[i] & 0xff);
    }
    printf("\n");
    // 加密, 其中plain是明文字符数组， len是长度， key是密钥
    aes(plain, len, key);
    printf("密文：\n");
    for (int i = 0; i < len; i++)
    {
        printf("%02x", plain[i] & 0xff);
    }
    printf("\n");
    //解密，其中ciphertext是密文字符数组， len是长度， key是密钥
    deAes(plain, len, key);
    printf("明文：\n");
    for (int i = 0; i < len; i++)
    {
        printf("%02x", plain[i] & 0xff);
    }


    while (1)
    {
        ;
    }
}
#endif