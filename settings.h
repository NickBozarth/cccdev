#include "alg_settings.h"


enum crypt_function {
    FUN_UNDEFINED,
    FUN_ENCRYPT,
    FUN_DECRYPT,
    FUN_HASH,
};


enum crypt_algorithm {
    ALG_UNDEFINED,


    /* 
     * USE WITH FUN_ENCRYPT OR FUN_DECRYPT OPTIONS
     * 
     * Symmetric encryption algorithms
     * Use the same key to encrypt and decrpyt
     */
    ALG_AES_GCM,
    ALG_TWOFISH,
    /* TEST FUNCTION! DO NOT USE IN PROD */
    ALG_CAESAR_TEST,
    /* TEST FUNCTION! DO NOT USE IN PROD */
    ALG_VIGENERE_TEST,


    /*
     * USE WITH FUN_ENCRYPT OR FUN_DECRYPT OPTIONS
     *
     * Asymmetric encryption algorithms
     * Use different keys derived from one another to encrypt/decrpyt
     */
    ALG_RSA,
    ALG_ECC,
    /* TEST FUNCTION! DO NOT USE IN PROD */
    ALG_TOY_RSA_TEST,


    /*
     * USE WITH FUN_HASH
     *
     * Hashing algorithms
     * Digests data into a unique* predictable hash
     */
    ALG_SHA2,
    ALG_MD5,
    /* TEST FUNCTION! DO NOT USE IN PROD */
    ALG_DJB2_TEST
};


struct settings {
    enum crypt_function function;
    enum crypt_algorithm algorithm;
    union algorithm_settings settings;
};
