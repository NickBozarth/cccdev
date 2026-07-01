#ifndef CCCDEV_SETTINGS
#define CCCDEV_SETTINGS

#include <sys/ioccom.h>

#include "alg_settings.h"


enum crypt_function {
    FUNC_UNDEFINED = 0,

    FUNC_ENCRYPT,
    FUNC_DECRYPT,
    FUNC_HASH,

    FUNC_ENUM_END,
};


enum crypt_algorithm {
    ALG_UNDEFINED = 0,


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
    ALG_DJB2_TEST,

    ALG_ENUM_END
};


struct settings {
    enum crypt_function function;
    enum crypt_algorithm algorithm;
    union algorithm_settings settings;
};


static int alg_func_agree(enum crypt_algorithm alg, enum crypt_function func) {
    switch (alg) {
    case ALG_UNDEFINED:
        return (0);
        break;
    
    case ALG_AES_GCM:
    case ALG_TWOFISH:
    case ALG_CAESAR_TEST:
    case ALG_VIGENERE_TEST:
    case ALG_RSA:
    case ALG_ECC:
    case ALG_TOY_RSA_TEST:
        return (func == FUNC_ENCRYPT || func == FUNC_DECRYPT);
        break;

    case ALG_SHA2:
    case ALG_MD5:
    case ALG_DJB2_TEST:
        return (func == FUNC_HASH);

    default:
        return (0);
        break;
    }
}



#define CCCDEV_IOCTL_GROUP 'Q'



#define CCCDEV_IOCTL_RESET_SETTINGS _IO(CCCDEV_IOCTL_GROUP, 1)
#define CCCDEV_IOCTL_SET_SETTINGS   _IOW(CCCDEV_IOCTL_GROUP, 4, struct settings)

#endif /* CCCDEV_SETTINGS */
